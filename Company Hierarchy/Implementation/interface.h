#pragma once
#include <iostream>
#include <string>
#include "Tree.h"

using std::cout;
using std::string;

class Hierarchy
{
public:
    const string BOSS = "Uspeshnia";

    struct Relation {
        string manager;
        string subordinate;

        bool valid() { return manager != ""; }
        Relation(const string& m = "", const string& s = "") : manager(m), subordinate(s) {}
    };

public:
    Hierarchy(Hierarchy&& r) noexcept;
    Hierarchy(const Hierarchy& r);
    Hierarchy(const string& data);
    ~Hierarchy() noexcept;
    void operator=(const Hierarchy&) = delete;

    string print()const;

    int longest_chain() const;
    bool find(const string& name) const;
    int num_employees() const;
    int num_overloaded(int level = 20) const;

    string manager(const string& name) const;
    int num_subordinates(const string& name) const;
    unsigned long getSalary(const string& who) const;

    bool fire(const string& who);
    bool hire(const string& who, const string& boss);
    bool insert(const string& who, const string& boss);

    void incorporate();
    void modernize();

    Hierarchy join(const Hierarchy& right) const;

    // By given string extracts the text from position i to the first '\n' and splits it into manager and subordinate
    Relation extractPair(const string& data, int& i);

private:

    Tree hierarchyTree;
};

