/**@file*/

#pragma once
#include <exception>
#include <string>
#include <vector>

class out_of_range : public std::exception
{
	const char* what() { return "An attempt of accessing an element that is out of range."; }
};

struct myConnection
{
	double weight, weightDifference;
	myConnection() { weight = random(); weightDifference = 0.0; }
private:
	double random() { return rand() / double(RAND_MAX) * (rand() % 2 ? -1 : +1); }
};

class myNeuron
{
	static double learningRate, momentum;

	double outputValue, gradientValue;
	std::vector<myConnection> inputWeights;
	size_t index;

	double transfer(double arg) { return tanh(arg); }
	double transferDerivative(double arg);

public:
	myNeuron(size_t inputsNumber, size_t _index);
	
	void setOutput(double value) { outputValue = value; }
	double getOutput() const { return outputValue; }
	
	void computeOutput(const std::vector<myNeuron>& prevLayer);
	void computeOutputGradient(double target);
	void computeHiddenGradient(const std::vector<myNeuron>& nextLayer);
	void improveInputWeights(const std::vector<myNeuron>& prevLayer);
	
	double getWeight(size_t initial) const;
	void setInputWeights(const std::vector<double>& weights);
	void printWeights();
};
