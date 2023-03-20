#include "neuron.h"
#include <cassert>
#include <iostream>

/** Returns the derivative of the transfer function.
* @param arg the argument of the function
*/
double myNeuron::transferDerivative(double arg)
{
	arg = tanh(arg);
	return 1 - arg * arg;
}

/** Constructor
* @param inputsNumber the number of input weights
* @param _index 
*/
myNeuron::myNeuron(size_t inputsNumber, size_t _index) : index(_index)
{
	for (size_t c = 0; c < inputsNumber; ++c)
		inputWeights.push_back(myConnection());
	inputWeights.shrink_to_fit();
	outputValue = 1.0;
	gradientValue = 0.0;
}

/** Computes the output of the neuron.
* @param prevLayer reference to the previous layer
*/
void myNeuron::computeOutput(const std::vector<myNeuron>& prevLayer)
{
	double sum = 0.0;
	for (size_t n = 0; n < prevLayer.size(); ++n)
		sum += prevLayer[n].getOutput() * inputWeights[n].weight;
	outputValue = transfer(sum);
}

/** Computes the gradient for the target value according to the formula for the output layer.
* @param target the target value
*/
void myNeuron::computeOutputGradient(double target)
{
	gradientValue = (target - outputValue) * transferDerivative(outputValue);
}

/** Computes the gradient for the target value according to the formula for the hidden layers.
* @param target the target value
*/
void myNeuron::computeHiddenGradient(const std::vector<myNeuron>& nextLayer)
{
	double sum = 0.0;
	for (size_t n = 0; n < nextLayer.size() - 1; ++n)
		sum += nextLayer[n].inputWeights[index].weight * nextLayer[n].gradientValue;
	gradientValue = sum * transferDerivative(outputValue);
}

/** Improves the input weights.
* @param prevLayer reference to the previous layer
*/
void myNeuron::improveInputWeights(const std::vector<myNeuron>& prevLayer)
{
	double oldWeightDifference, newWeightDifference;
	for (size_t n = 0; n < prevLayer.size(); ++n)
	{
		oldWeightDifference = inputWeights[n].weightDifference;
		newWeightDifference =
			learningRate * prevLayer[n].getOutput() * gradientValue
			+ momentum * oldWeightDifference;
		inputWeights[n].weightDifference = newWeightDifference;
		inputWeights[n].weight += newWeightDifference;
	}
}

/** Returns the value of the weight to the neuron.
* @param initial the index of the initial neuron of the weight
*/
double myNeuron::getWeight(size_t initial) const
{
	if (initial >= inputWeights.size())
		throw out_of_range();
	else
		return inputWeights[initial].weight;
}

/** Sets the values of the input weights.
*/
void myNeuron::setInputWeights(const std::vector<double>& weights)
{
	assert(weights.size() == inputWeights.size());
	for (size_t w = 0; w < inputWeights.size(); ++w)
		inputWeights[w].weight = weights[w];
}

/** Prints the input weights.
*/
void myNeuron::printWeights()
{
	if (inputWeights.empty())
		std::cout << "        The neuron has no weights." << std::endl;
	for (size_t w = 0; w < inputWeights.size(); ++w)
		std::cout << "        Weight " << w << " has the value: " << inputWeights[w].weight << "." << std::endl;
}
