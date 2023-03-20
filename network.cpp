#include "network.h"
#include <cstring>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

/** Prints information about the network.
*/
void myNetwork::printNet()
{
	if (networkBody.empty())
	{
		std::cout << std::endl << "The network is empty." << std::endl;
		return;
	}
	std::cout << "The network consists of " << networkBody.size() << " layers." << std::endl;
	for (size_t l = 0; l < networkBody.size(); ++l)
	{
		std::cout << "Layer " << l << " has " << networkBody[l].size() << " neurons (including the bias)." << std::endl;
		for (size_t n = 0; n < networkBody[l].size(); ++n)
		{
			std::cout << "    Neuron " << n << " has the weights: " << std::endl;
			networkBody[l][n].printWeights();
		}
	}
	std::cout << std::endl << "The outputs of the neurons are:" << std::endl;
	for (size_t l = 0; l < networkBody.size(); ++l)
	{
		std::cout << "In layer " << l << " (including the bias):" << std::endl;
		for (size_t n = 0; n < networkBody[l].size(); ++n)
		{
			std::cout << "    Neuron " << n << " has the output "
				<< networkBody[l][n].getOutput() << "." << std::endl;
		}
	}
	std::cout << std::endl;
}

/** Performs propagation.
* @param inputs the vector of input values
*/
void myNetwork::propagate(std::vector<double> inputs)
{
	if (inputs.size() != networkBody[0].size() - 1)
		throw incompatible_vectors();
	for (size_t i = 0; i < inputs.size(); ++i)
		networkBody[0][i].setOutput(inputs[i]);
	for (size_t l = 1; l < networkBody.size(); ++l)
		for (size_t n = 0; n < networkBody[l].size() - 1; ++n)
			networkBody[l][n].computeOutput(networkBody[l - 1]);
}

/** Performs backpropagation.
* @param targets the vector of target output values
*/
void myNetwork::backpropagate(std::vector<double> targets)
{
	if (targets.size() != networkBody.back().size() - 1)
		throw incompatible_vectors();
	for (size_t n = 0; n < networkBody.back().size() - 1; ++n)
		networkBody.back()[n].computeOutputGradient(targets[n]);
	for (size_t l = networkBody.size() - 2; l > 0; --l)
		for (size_t n = 0; n < networkBody[l].size() - 1; ++n)
			networkBody[l][n].computeHiddenGradient(networkBody[l + 1]);
	for (size_t l = networkBody.size() - 1; l > 0; --l)
		for (size_t n = 0; n < networkBody[l].size() - 1; ++n)
			networkBody[l][n].improveInputWeights(networkBody[l - 1]);
}

/** Saves the current outputs.
* @param results the vector to which the values should be saved
*/
void myNetwork::getResults(std::vector<double>& results)
{
	results.clear();
	for (size_t n = 0; n < networkBody.back().size() - 1; ++n)
		results.push_back(networkBody.back()[n].getOutput());
	results.shrink_to_fit();
}

/** Prints the outputs.
*/
void myNetwork::printOutputs()
{
	std::cout << "The outputs are: " << std::endl;
	for (size_t n = 0; n < networkBody.back().size() - 1; ++n)
		std::cout << "[" << n << "] " << networkBody.back()[n].getOutput() << std::endl;
}

/** Calculates the aggregate square error for current outputs and the target output values.
* @param targets the vector of target output values for the inputs for which the current outputs have been calculated
*/
double myNetwork::AggregateSquareError(const std::vector<double>& targets)
{
	if (targets.size() != networkBody.back().size() - 1)
		throw incompatible_vectors();
	double error, totalError = 0.0;
	for (size_t i = 0; i < targets.size(); ++i)
	{
		error = (networkBody.back()[i].getOutput() - targets[i]);
		totalError += error * error;
	}
	return totalError;
}

/** Trains the network with the record.
* @param record the record on which the network shall be trained
*/
void myNetwork::trainRecord(const myDataRecord& record)
{
	propagate(record.inputValues);
	backpropagate(record.targetValues);
	if (rand() % 10 == 0)
		std::cout << ".";
}

/** Trains the network on the data set.
* @param set the set on which the network shall be trained
*/
void myNetwork::trainSet(const myDataSet& set)
{
	for (const auto& record : *(set.dataRef()))
		trainRecord(record);
}

/** Trains the network on the data set.
* @param set the set on which the network shall be tested
*/
double myNetwork::testSet(const myDataSet& set)
{
	if (set.empty())
		throw empty_set();
	if (set.inputSize() != networkBody.front().size() - 1 or
		set.outputSize() != networkBody.back().size() - 1)
		throw incompatible_vectors();
	double error = 0.0;
	for (const auto& record : *(set.dataRef()))
	{
		propagate(record.inputValues);
		error += AggregateSquareError(record.targetValues);
		if (rand() % 10 == 0)
			std::cout << ".";
	}
	return sqrt(error / set.size() / set.outputSize());
}

/** Creates the network according to the layout.
* @param layout the vector defining the network's structure
*/
void myNetwork::create(const std::vector<size_t>& layout)
{
	for (size_t l = 0; l < layout.size(); ++l)
	{
		networkBody.push_back(std::vector<myNeuron>());
		for (size_t n = 0; n <= layout[l]; ++n)
			networkBody.back().push_back(myNeuron(l == 0 or n == layout[l] ? 0 : layout[l - 1] + 1, n));
		networkBody.back().shrink_to_fit();
	}
	networkBody.shrink_to_fit();
}

/** Reads the network from the path.
* @param path the path from which the network shall be read
*/
void myNetwork::read(std::string path)
{
	std::ifstream source;
	std::string type = extension(path);
	if (type == ".lay" or type == ".net")
		source.open(path, std::ios::in);
	else
		throw bad_extension(filetype::net);
	if (not source.good())
	{
		source.close();
		throw no_file();
	}
	size_t networkSize;
	if (not (source >> networkSize))
	{
		source.close();
		throw incomplete_contents();
	}
	if (networkSize == 0)
	{
		source.close();
		throw incorrect_contents();
	}
	std::vector<size_t> layerSizes(networkSize);
	for (size_t l = 0; l < networkSize; ++l)
	{
		if (not (source >> layerSizes[l]))
		{
			source.close();
			throw incomplete_contents();
		}
		if (layerSizes[l] == 0)
		{
			source.close();
			throw incorrect_contents();
		}
	}
	create(layerSizes);
	layerSizes.clear();
	if (type == ".lay")
	{
		source.close();
		return;
	}
	std::vector<double> weights;
	for (size_t l = 1; l < networkBody.size(); ++l)
	{
		weights.resize(networkBody[l - 1].size());
		for (size_t n = 0; n < networkBody[l].size() - 1; ++n)
		{
			for (size_t w = 0; w < networkBody[l - 1].size(); ++w)
				if (not (source >> weights[w]))
				{
					source.close();
					throw incomplete_contents();
				}
			networkBody[l][n].setInputWeights(weights);
		}
	}
	source.close();
}

/** Saves the layout of the network on the path given.
* @param path the path on which the network shall be saved
*/
void myNetwork::saveLayout(std::string path)
{
	std::ofstream file;
	file.open(path, std::ios::out);
	if (not file.good())
	{
		file.close();
		throw bad_path();
	}
	file << networkBody.size() << '\n';
	for (size_t l = 0; l < networkBody.size(); ++l)
		file << networkBody[l].size() - 1 << ' ';
	file.close();
}

/** Saves the network (with the weights) on the path given.
* @param path the path on which the network shall be saved
*/
void myNetwork::saveNetwork(std::string path)
{
	std::ofstream file;
	file.open(path, std::ios::out);
	if (not file.good())
	{
		file.close();
		throw bad_path();
	}
	file << networkBody.size() << '\n';
	for (size_t l = 0; l < networkBody.size(); ++l)
		file << networkBody[l].size() - 1 << ' ';
	file << '\n' << '\n';
	for (size_t l = 1; l < networkBody.size(); ++l)
	{
		for (size_t n = 0; n < networkBody[l].size() - 1; ++n)
		{
			for (size_t w = 0; w < networkBody[l - 1].size(); ++w)
				file << networkBody[l][n].getWeight(w) << ' ';
			file << '\n';
		}
		file << '\n';
	}
	file.close();
}

const char* bad_extension::what()
{
	
	if (type == filetype::net)
		return "Invalid extension. Acceptable are \".lay\" and \".net\".";
	else 
		return "Invalid extension. Acceptable is \".set\".";
}

const char* no_file::what()
{
	return "No file was found under the path given.";
}

const char* incorrect_contents::what()
{
	return "The source file contains incorrect data.";
}

const char* incomplete_contents::what()
{
	return "The source file contains incomplete data.";
}

const char* bad_path::what()
{
	return "The path is incorrect.";
}

const char* incompatible_vectors::what()
{
	return "The vectors are incompatible.";
}

const char* empty_set::what()
{
	return "The set is empty.";
}
