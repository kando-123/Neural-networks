#include "interface.h"
#include <iostream>
#include <list>
#include <sstream>
#include <string>

/** Reads a string delimited with quotation marks from std::cin.
* @param sentence the reference to the std::string variable which the string of characters should be written to
*/
void myInterface::readSentence(std::string& sentence)
{
	while (std::cin.peek() == ' ' or std::cin.peek() == '\n' or std::cin.peek() == '\t')
		std::cin.get();
	if (std::cin.peek() == '\"')
	{
		std::cin.get();
		getline(std::cin, sentence, '\"');
	}
	else
		std::cin >> sentence;
}

/** Reads a name for a network and checks whether it will be unique.
* @param name the std::string variable which the name should be written to
*/
void myInterface::readUniqueName(std::string& name)
{
	bool occurred;
	do {
		std::cin >> name;
		occurred = false;
		for (const auto& n : allNetworks)
			if (name == n.name)
			{
				occurred = true;
				break;
			}
		if (occurred)
			std::cout << "A network with such name already exists. Enter a unique name: ";
	} while (occurred);
}

/** Reads a command from std::cin and calls appropriate function.
*/
void myInterface::execute()
{
	std::cout << "> ";
	std::string command;
	std::cin >> command;
	if (command == "net.make")
		net_make();
	else if (command == "net.print")
		net_print();
	else if (command == "net.remove")
		net_remove();
	else if (command == "net.read")
		net_read();
	else if (command == "net.save")
		net_save();
	else if (command == "net.save.as")
	{
		try
		{
			net_save_as();
		}
		catch (std::exception exc)
		{
			std::cout << exc.what() << std::endl;
		}
	}
	else if (command == "net.set.source")
		net_set_source();
	else if (command == "net.test")
		net_test();
	else if (command == "net.train")
		net_train();
	else if (command == "net.compute")
		net_compute();
	else if (command == "list.networks")
		list_networks();
	else if (command == "list.sources")
		list_sources();
	else if (command == "list.sets")
		list_sets();
	else if (command == "set.read")
	{
		try
		{
			set_read();
		}
		catch (std::exception exc)
		{
			std::cout << exc.what() << std::endl;
		}
	}
	else if (command == "set.remove")
		set_remove();
	else if (command == "help")
		help();
	else if (command == "end")
		throw finish();
	std::cout << std::endl;
}

/** Reads a layout vector and creates a network according to the layout. Reads the name for the network.
*/
void myInterface::net_make()
{
	size_t networkSize;
	while (not (std::cin >> networkSize) or networkSize == 0)
		std::cout << "Enter a valid value. Error at: network size." << std::endl;
	std::vector<size_t> layerSizes(networkSize);
	for (size_t l = 0; l < networkSize; ++l)
		while (not (std::cin >> layerSizes[l]) or layerSizes[l] == 0)
			std::cout << "Enter a valid value. Error at: layer " << l << " size." << std::endl;
	allNetworks.push_back(net_entity(layerSizes));
	std::string networkName;
	readUniqueName(networkName);
	allNetworks.back().name = networkName;
	std::cout << "Network " << networkName << " has been created." << std::endl;
}

/** Prints information about the network.
*/
void myInterface::net_print()
{
	std::string networkName;
	std::cin >> networkName;
	bool printed = false;
	for (std::list<net_entity>::iterator it = allNetworks.begin();
		it != allNetworks.end(); ++it)
		if (it->name == networkName)
		{
			std::cout << std::endl;
			it->network.printNet();
			printed = true;
			break;
		}
	if (not printed)
		std::cout << "No such network was found." << std::endl;
}

/** From the list of networks removes the network with the name read.
*/
void myInterface::net_remove()
{
	std::string networkName;
	std::cin >> networkName;
	bool removed = false;
	for (std::list<net_entity>::iterator it = allNetworks.begin();
		it != allNetworks.end(); ++it)
		if (it->name == networkName)
		{
			allNetworks.erase(it);
			removed = true;
			break;
		}
	if (removed)
		std::cout << "Network " << networkName << " has been removed." << std::endl;
	else
		std::cout << "No such network was found." << std::endl;
}

/** Reads a network from a file.
*/
void myInterface::net_read()
{
	std::string path, networkName;
	readSentence(path);
	allNetworks.push_back(net_entity());
	allNetworks.back().sourcefile = path;
	readUniqueName(networkName);
	allNetworks.back().name = networkName;
	bool success = false;
	try
	{
		allNetworks.back().network.read(path);
		success = true;
	}
	catch (std::exception exc)
	{
		std::cerr << exc.what() << std::endl;
		allNetworks.pop_back();
	}
	if (success)
		std::cout << "Network " << networkName << " has been successfully read"
		<< " from \"" << path << "\"." << std::endl;
}

/** Saves the network to the associated path.
*/
void myInterface::net_save()
{
	std::string networkName;
	std::cin >> networkName;
	std::list<net_entity>::iterator net;
	for (net = allNetworks.begin(); net != allNetworks.end(); ++net)
		if (net->name == networkName)
			break;
	if (net == allNetworks.end())
		std::cout << "No such network was found." << std::endl;
	else
	{
		if (net->sourcefile == "")
			std::cout << "Net " << networkName << " has not been assigned a sourcefile." << std::endl
			<< "Try calling \"net.save.as\" or \"net.set.source\"." << std::endl;
		else
		{
			if (extension(net->sourcefile) == ".lay")
				net->network.saveLayout(net->sourcefile);
			else if (extension(net->sourcefile) == ".net")
				net->network.saveNetwork(net->sourcefile);
			else
				std::cout << "Incorrect extension." << std::endl;
		}
	}
}

/** Saves the network to the path read.
*/
void myInterface::net_save_as()
{
	std::string networkName, path, type;
	std::cin >> networkName;
	readSentence(path);
	type = extension(path);
	if (type != ".lay" and type != ".net")
		throw bad_extension(filetype::net);
	std::list<net_entity>::iterator net;
	for (net = allNetworks.begin(); net != allNetworks.end(); ++net)
		if (net->name == networkName)
			break;
	if (net == allNetworks.end())
		std::cout << "No such network was found." << std::endl;
	else
	{
		if (net->sourcefile == "")
			net->sourcefile = path;
		bool success = false;
		try
		{
			if (type == ".lay")
				net->network.saveLayout(path);
			else
				net->network.saveNetwork(path);
			success = true;
		}
		catch (std::exception exc)
		{
			std::cout << exc.what() << std::endl;
		}
		if (success)
			std::cout << "The network has been successfully saved." << std::endl;
	}
}

/** Sets the associated path for the network.
*/
void myInterface::net_set_source()
{
	std::string networkName, sourcefile, type;
	std::cin >> networkName;
	readSentence(sourcefile);
	type = extension(sourcefile);
	if (type != ".lay" and type != ".net")
		std::cout << "Invalid extension. Acceptable are: \".lay\" and \".net\".";
	else
	{
		std::list<net_entity>::iterator net;
		for (net = allNetworks.begin(); net != allNetworks.end(); ++net)
			if (net->name == networkName)
				break;
		if (net == allNetworks.end())
			std::cout << "No such network was found." << std::endl;
		else
		{
			net->sourcefile = sourcefile;
			std::cout << "The source file has been successfully set." << std::endl;
		}
	}
}

/** Tests the network with the data set.
*/
void myInterface::net_test()
{
	std::string networkName, setName;
	std::cin >> networkName >> setName;
	std::list<net_entity>::iterator net;
	for (net = allNetworks.begin(); net != allNetworks.end(); ++net)
		if (net->name == networkName)
			break;
	if (net == allNetworks.end())
		std::cout << "No such network was found." << std::endl;
	else
	{
		std::list<set_entity>::iterator set;
		for (set = allSets.begin(); set != allSets.end(); ++set)
			if (set->name == setName)
				break;
		if (set == allSets.end())
			std::cout << "No such set was found." << std::endl;
		else
		{
			double error = net->network.testSet(set->set);
			std::cout << "Net " << networkName << " has been tested with set " << setName << ". "
				"The root mean square error is equal to: " << error << std::endl;
		}
	}
}

/** Trains the network with the data set.
*/
void myInterface::net_train()
{
	std::string networkName, setName;
	std::cin >> networkName >> setName;
	std::list<net_entity>::iterator net;
	std::list<set_entity>::iterator set;
	for (net = allNetworks.begin(); net != allNetworks.end(); ++net)
		if (net->name == networkName)
			break;
	if (net == allNetworks.end())
		std::cout << "No such network was found." << std::endl;
	else
	{
		for (set = allSets.begin(); set != allSets.end(); ++set)
			if (set->name == setName)
				break;
		if (set == allSets.end())
			std::cout << "No such set was found." << std::endl;
		else
		{
			bool success = true;
			try
			{
				net->network.trainSet(set->set);
			}
			catch (incompatible_vectors)
			{
				std::cerr << "Training set " << setName << " does not match network "
					<< networkName << "." << std::endl;
				success = false;
			}
			if (success)
				std::cout << "Network " << networkName << " has been successfully trained with "
				<< "set " << setName << "." << std::endl;
		}
	}
}

/** Makes the network compute outputs for given inputs.
*/
void myInterface::net_compute()
{
	std::string networkName;
	std::cin >> networkName;
	std::list<net_entity>::iterator net;
	for (net = allNetworks.begin(); net != allNetworks.end(); ++net)
		if (net->name == networkName)
			break;
	if (net == allNetworks.end())
		std::cout << "No such network was found." << std::endl;
	else
	{
		if (net->network.empty())
			std::cout << "Network " << networkName << " is empty." << std::endl;
		else
		{
			bool success = true;
			size_t size = net->network.inputSize();
			std::vector<double> values(size);
			for (size_t i = 0; i < size; ++i)
				if (not (std::cin >> values[i]))
				{
					std::cerr << "The input is wrong." << std::endl;
					success = false;
					break;
				}
			if (success)
			{
				bool moreSuccess = false;
				try
				{
					net->network.propagate(values);
					moreSuccess = true;
				}
				catch (std::exception exc)
				{
					std::cerr << exc.what() << std::endl;
				}
				if (moreSuccess)
				{
					net->network.getResults(values);
					std::cout << "Network " << networkName << " has computed the outputs as:" << std::endl;
					for (size_t i = 0; i < values.size(); ++i)
						std::cout << "output[" << i << "] = " << values[i] << std::endl;
				}
			}
		}
	}
}

/** Prints the names of the networks read.
*/
void myInterface::list_networks()
{
	if (allNetworks.empty())
		std::cout << "No networks are there." << std::endl;
	else
	{
		std::cout << "Networks:" << std::endl;
		for (std::list<net_entity>::iterator it = allNetworks.begin();
			it != allNetworks.end(); ++it)
			std::cout << " + " << it->name << std::endl;
	}
}

/** Prints the path associated with the networks.
*/
void myInterface::list_sources()
{
	if (allNetworks.empty())
		std::cout << "No networks are there." << std::endl;
	else
	{
		std::cout << "Networks:" << std::endl;
		for (std::list<net_entity>::iterator it = allNetworks.begin();
			it != allNetworks.end(); ++it)
			std::cout << " + " << it->name << (it->sourcefile == "" ?
				" (no sourcefile)" : " \"" + it->sourcefile + "\"")
			<< std::endl;
	}
}

/** Prints the names and sizes of the sets read.
*/
void myInterface::list_sets()
{
	if (allSets.empty())
		std::cout << "No set are there." << std::endl;
	else
	{
		std::cout << "Sets:" << std::endl;
		for (std::list<set_entity>::iterator it = allSets.begin();
			it != allSets.end(); ++it)
			std::cout << " + " << it->name << "\tsize = " << it->set.size() << std::endl;
	}
}

/** Reads sets from the path and calls it uniquely.
*/
void myInterface::set_read()
{
	std::string path, setName;
	readSentence(path);
	allSets.push_back(set_entity());
	bool success = false;
	try
	{
		allSets.back().set.read(path);
		success = true;
	}
	catch (...)
	{
		allSets.pop_back();
		throw;
	}
	bool occurred;
	do {
		std::cin >> setName;
		occurred = false;
		for (const auto& s : allNetworks)
			if (setName == s.name)
			{
				occurred = true;
				break;
			}
		if (occurred)
			std::cout << "A set with such name already exists. Enter a new, unique name: ";
	} while (occurred);
	allSets.back().name = setName;
	if (success)
		std::cout << "Set " << setName << " has been successfully read from "
		<< "\"" << path << "\"; it contains " << allSets.back().set.size()
		<< " records." << std::endl;
}

/** Removes the set from the list.
*/
void myInterface::set_remove()
{
	std::string setName;
	std::cin >> setName;
	bool removed = false;
	for (std::list<set_entity>::iterator it = allSets.begin();
		it != allSets.end(); ++it)
		if (it->name == setName)
		{
			allSets.erase(it);
			removed = true;
			break;
		}
	if (removed)
		std::cout << "Set " << setName << " has been removed." << std::endl;
	else
		std::cout << "No such set was found." << std::endl;
}


/** Prints the help.
*/
void myInterface::help()
{
	std::cout << R"(Commands
 * net.make       number_of_layers layers_sizes net_name .. adds a new network
 * net.print      net_name ................................ prints the network
 * net.remove     net_name ................................ deletes the network
 * net.read       path net_name ........................... reads a network from the file
 * net.save       net_name ................................ saves the network at its assigned source file if provided
 * net.save.as    net_name path ........................... saves the network at the path provided
 * net.set.source net_name path ........................... sets the network source file
 * net.test       net_name set_name ....................... tests the network with the set and tells the RMS error
 * net.train      net_name set_name ....................... trains the network with the set
 * net.compute    name inputs ............................. computes output for given inputs
 * set.read       path name ............................... reads a set from the path
 * set.remove     set_name ................................ removes the set
 * list.networks  ......................................... prints names of all networks
 * list.sources   ......................................... prints names and source files of all networks
 * end            ......................................... finishes the program
)";
}
