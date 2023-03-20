/**@file*/

#pragma once
#include "neuron.h"
#include "training.h"
#include <list>
#include <string>
#include <vector>

enum class filetype { net, set };
class bad_extension : public std::exception
{
	filetype type;
public:
	bad_extension(filetype _type) : type(_type) {}
	const char* what();
};
class no_file              : public std::exception { const char* what(); };
class incorrect_contents   : public std::exception { const char* what(); };
class incomplete_contents  : public std::exception { const char* what(); };
class bad_path             : public std::exception { const char* what(); };
class incompatible_vectors : public std::exception { const char* what(); };
class empty_set            : public std::exception { const char* what(); };

struct myDataRecord;
class myDataSet;

class myNetwork
{
	std::vector<std::vector<myNeuron>> networkBody;
	double AggregateSquareError(const std::vector<double>& targets);

public:
	myNetwork() {}
	myNetwork(const std::vector<size_t>& layout) { create(layout); }
	void printNet();

	void propagate(std::vector<double> inputs);
	void backpropagate(std::vector<double> targets);
	
	void getResults(std::vector<double>& results);
	void printOutputs();
	
	void trainRecord(const myDataRecord& record);
	void trainSet(const myDataSet& set);
	double testSet(const myDataSet& set);
	
	bool empty() { return networkBody.empty(); }
	void create(const std::vector<size_t>& layout);
	void clear() { networkBody.clear(); }
	
	void read(std::string path);
	void saveLayout(std::string path);
	void saveNetwork(std::string path);

	size_t inputSize() const { return (networkBody.empty() ? 0 : networkBody.front().size() - 1); }
};
