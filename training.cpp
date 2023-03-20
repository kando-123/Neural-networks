#include "training.h"
#include <fstream>
#include <iostream>

/** Returns the substring from the last occurrence of the delimiter.
* @param path the string from which the extension shall be extracted
* @param delimiter the initial characted of the extension
* @return the substring from the last occurrence of the delimiter
*/
std::string extension(std::string path, char delimiter)
{
	size_t point = path.find_last_of(delimiter);
	if (point == std::string::npos)
		return std::string(1, delimiter);
	else
		return path.substr(point);
}

/** Reads a set from the path.
*/
void myDataSet::read(std::string path)
{
	std::ifstream source;
	std::string type = extension(path);
	unsigned long int inputsNumber, outputsNumber;
	if (type == ".set")
		source.open(path, std::ios::in);
	else
		throw bad_extension(filetype::set);
	if (not source.good())
	{
		source.close();
		throw no_file();
	}
	if (not (source >> inputsNumber >> outputsNumber))
		throw incomplete_contents();
	if (inputsNumber == 0 or outputsNumber == 0)
	{
		source.close();
		throw incorrect_contents();
	}
	while (true)
	{
		dataBase.push_back(myDataRecord(inputsNumber, outputsNumber));
		for (size_t i = 0; i < inputsNumber; ++i)
			if (not (source >> dataBase.back().inputValues[i]))
			{
				dataBase.pop_back();
				if (dataBase.empty())
					throw incomplete_contents();
				else
					return;
			}
		for (size_t i = 0; i < outputsNumber; ++i)
			if (not (source >> dataBase.back().targetValues[i]))
			{
				dataBase.pop_back();
				if (dataBase.empty())
					throw incomplete_contents();
				else
					return;
			}
	}
}

/** Prints the contents of the set.
*/
void myDataSet::printData()
{
	if (dataBase.empty())
		std::cout << "No data have been read." << std::endl;
	else
	{
		for (const auto& record : dataBase)
		{
			std::cout << "inputs: ";
			for (const auto& input : record.inputValues)
				std::cout << input << " ";
			std::cout << "outputs: ";
			for (const auto& output : record.targetValues)
				std::cout << output << " ";
			std::cout << std::endl;
		}
	}
}

/** Returns the size of the input vectors.
*/
size_t myDataSet::inputSize() const
{
	if (dataBase.empty())
		throw empty_set();
	return dataBase[0].inputValues.size();
}

/** Returns the size of the output vectors.
*/
size_t myDataSet::outputSize() const
{
	if (dataBase.empty())
		throw empty_set();
	return dataBase[0].targetValues.size();
}
