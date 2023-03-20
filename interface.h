/**@file*/

#pragma once
#include "network.h"
#include <array>
#include <list>

class finish {};

struct net_entity
{
	myNetwork network;
	std::string name, sourcefile;
	net_entity() : network(), name(""), sourcefile("") {}
	net_entity(std::vector<size_t> layout)
		: network(layout), name(""), sourcefile("") {}
	net_entity(std::string _name) : name(_name), sourcefile("") {}
};

struct set_entity
{
	myDataSet set;
	std::string name;
};

class myInterface
{
	std::list<net_entity> allNetworks; 
	std::list<set_entity> allSets;
	void readSentence(std::string& sentence);
	void readUniqueName(std::string& name);

public:
	void execute();

private:
	void net_make();
	void net_print();
	void net_remove();
	void net_read();
	void net_save();
	void net_save_as();
	void net_set_source();
	void net_test();
	void net_train();
	void net_compute();
	void list_networks();
	void list_sources();
	void list_sets();
	void set_read();
	void set_remove();
	void help();
};