/**@file*/

#pragma once
#include "network.h"
#include <exception>
#include <string>
#include <vector>

std::string extension(std::string path, char delimiter = '.');

struct myDataRecord
{
	std::vector<double> inputValues, targetValues;
	myDataRecord(size_t _i, size_t _o) : inputValues(_i), targetValues(_o) {}
};

class myDataSet
{
	std::vector<myDataRecord> dataBase;
public:
	myDataSet() {};
	myDataSet(std::string path) { read(path); };
	void read(std::string path);
	void printData();
	void clear() { dataBase.clear(); }
	auto dataRef() const { return &dataBase; }
	size_t size() const { return dataBase.size(); }
	size_t inputSize() const;
	size_t outputSize() const;
	bool empty() const { return dataBase.empty(); }
};
