/**@file*/

#include "interface.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>

double myNeuron::learningRate = 0.01;
double myNeuron::momentum = 0.5;

int main()
{
	srand(time(NULL));
	myInterface interface;
	while (true)
	{
		try
		{
			interface.execute();
		}
		catch (finish)
		{
			break;
		}
		catch (...)
		{
			std::cerr << "An unexpected problem has been encountered." << std::endl;
		}
	}
	return 0;
}
