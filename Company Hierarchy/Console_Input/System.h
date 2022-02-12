#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <forward_list>
#include <exception>
#include "../HomeWork2/interface.h"
#include "../HW2_ConsoleInput/HashTable.h"

using std::string;
using std::cin;
using std::cout;

class System
{
public:
	System();

	void Run();
private:
	void operation(const string& input);
	void help();

	void find(const string& input);
	void num_subordinates(const string& input);
	void manager(const string& input);
	void num_employees(const string& branchName);
	void overloaded(const string& branchName);
	void join(const string& input);
	void fire(const string& input);
	void hire(const string& input);
	void salary(const string& input);
	void incorporate(const string& branchName);
	void modernize(const string& branchName);
	void longest_chain(const string& branchName);

	void load(const string& input);
	void loadFromFile(const string& firmName, const string& fileName, bool newBranch);
	bool saveToFile(Data*& toSave, const string& fileName);
	bool save(const string& input);
	void saveOnExit(Data*& data);

	bool valdidName(const string& objectName);
	bool vallidFileName(const string& fileName);
	string clearWhiteSpace(const string& input);
	string extract(const string& input, char delimiter, int& pos);


	const string BOSS = "Uspeshnia";
	HashMap branches;
};

