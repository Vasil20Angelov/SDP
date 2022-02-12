#pragma once
#include "interface.h"

Hierarchy::Hierarchy(Hierarchy&& r) noexcept
{
	this->hierarchyTree = r.hierarchyTree;
}

Hierarchy::Hierarchy(const Hierarchy& r)
{
	this->hierarchyTree = Tree(r.hierarchyTree);
}

Hierarchy::Hierarchy(const string& data)
{
	if (data == "")
		return;

	if (hierarchyTree.treeSize() != 0) // If there is already a tree in this instance -> remove it
	{
		hierarchyTree.clear();
		cout << "Data from the current Hierarchy has been removed!\n";
	}

	int i = 0;
	Relation rel;

	rel = extractPair(data, i); // Get 1 relation from the data
	if (!rel.valid() || rel.manager != BOSS)
		throw std::invalid_argument("Invalid arguments!");

	hierarchyTree = Tree(rel.manager); // First add "Uspeshnia"

	try {
		if (!hierarchyTree.insert(rel.manager, rel.subordinate)) // Catches the cases where the tree initialization fails or can't add the first worker 
		{
			hierarchyTree.clear();
			//cout << "Hirarchy has not been created!\n";
			throw std::invalid_argument("Invalid arguments!");
		}
	}
	catch (...) {
		hierarchyTree.clear();
		throw;
	}

	// Repeat it until the end of the given string or until an error is found
	while (i < data.size() - 1)
	{
		try {
			rel = extractPair(data, i);

			if (!rel.valid() || !hierarchyTree.insert(rel.manager, rel.subordinate))
			{
				hierarchyTree.clear();
				throw std::invalid_argument("Invalid arguments!");
			}
		}
		catch(...) {
			hierarchyTree.clear();
			throw;
		}
	}
}

Hierarchy::~Hierarchy() noexcept
{
	hierarchyTree.clear();
}

string Hierarchy::print() const
{
    return hierarchyTree.toString();
}

int Hierarchy::longest_chain() const
{
	return hierarchyTree.depth();
}

bool Hierarchy::find(const string& name) const
{
    return hierarchyTree.find(name);
}

int Hierarchy::num_employees() const
{
	return hierarchyTree.treeSize();
}

int Hierarchy::num_overloaded(int level) const
{
	return hierarchyTree.overloaded(level);
}

string Hierarchy::manager(const string& name) const
{
	return hierarchyTree.getParentData(name);
}

int Hierarchy::num_subordinates(const string& name) const
{
	return hierarchyTree.directHeirs(name);
}

unsigned long Hierarchy::getSalary(const string& who) const
{
	return hierarchyTree.getSalary(who);
}

bool Hierarchy::fire(const string& who)
{
    return hierarchyTree.remove(who);
}

bool Hierarchy::hire(const string& who, const string& boss)
{
	try { 
		if (hierarchyTree.moveNode(boss, who))
			return true;
	}
	catch (bad_alloc) {
		cout << "Not enough memory!\n";
	}
	catch (...) {
		cout << "Unknown error!\n";
	}

	return false;
}

bool Hierarchy::insert(const string& who, const string& boss)
{
	try {
		if (hierarchyTree.insert(boss, who))
			return true;
	}
	catch (bad_alloc) {
		cout << "Not enough memory!\n";
	}
	catch (...) {
		cout << "Unknown error!\n";
	}

	return false;
}

void Hierarchy::incorporate()
{
	hierarchyTree.incorporate();
}

void Hierarchy::modernize()
{
	hierarchyTree.modernize();
}

Hierarchy Hierarchy::join(const Hierarchy& right) const
{
	Hierarchy h("");
	h.hierarchyTree = (this->hierarchyTree.join(right.hierarchyTree));
	return h;
}

Hierarchy::Relation Hierarchy::extractPair(const string& data, int& i)
{
	string manager;
	string subordinate;

	// Skip white space
	while (i <= data.size() && data[i] == ' ')
		++i;

	// Get manager's name
	while (i < data.size() && data[i] != ' ' && data[i] != '-')
	{
		if (data[i] == '\n')
			return Relation();

		manager += data[i];
		++i;
	}

	// Skip the white space between the name of the manager and the '-'
	while (i < data.size() && data[i] == ' ')
		++i;

	//assert(!(i == data.size() || data[i] != '-'));
	if (i == data.size() || data[i] != '-')
		return Relation();

	// Skip the white space between the '-' and the name of the worker 
	do {
		++i;
	} while (i < data.size() && data[i] == ' ');

	// Get worker's name
	while (i < data.size() && data[i] != '\n')
	{
		if (data[i] == ' ')
		{
			while (i < data.size() && data[i] == ' ')		
				++i;

			if ((i < data.size() && data[i] != '\n') || i == data.size())
				return Relation();

			break;
		}
		
		if( data[i] == '-')
			return Relation();

		subordinate += data[i];
		++i;
	}

	++i; // to skip the last element in the line: '\n'

	return Relation(manager, subordinate);
}
