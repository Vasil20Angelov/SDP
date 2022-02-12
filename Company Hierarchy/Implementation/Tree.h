#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <list>
#include <vector>
#include <exception>

using std::string;
using std::queue;
using std::list;
using std::vector;
using std::bad_alloc;

const int SALARY_FOR_DIRECT_SUBORDINATE = 500;
const int SALARY_FOR_INDIRECT_SUBORDINATE = 50;

class Tree 
{
private:
	struct Node {
		string data;
		list<Node*> children;
		int childrenCount;
		unsigned long salary;

		Node(const string& d = "")
		{
			data = d;
			childrenCount = 0;
			salary = 0;
		}
	};

public:
	Tree()
	{
		root = nullptr;
	}

	Tree(const string& data)
	{
		root = new (std::nothrow) Node(data);
		if (!root)
			std::cout << "Memory allocation has failed!\n";
	}

	Tree(const Tree& other)
	{
		try {
			root = copy(other.root);
		}
		catch (bad_alloc) {
			clear();
			std::cout << "Memory allocation has failed!\n";
		}

	}

	Tree& operator=(const Tree& other)
	{
		if (other.root == nullptr)
			return *this;

		if (this != &other)
		{
			clear();
			try {
				root = copy(other.root);	
			}
			catch (bad_alloc) {
				clear();
				std::cout << "Memory allocation has failed!\n";
			}
		}

		return *this;
	}

	~Tree()
	{
		clear();
	}

	// Returns the name of the parent (manager) by given node name (subordinate). Empty string if not found
	const string getParentData(const string& childData) const
	{
		const Node* parentNode = findNode(childData, true);
		return parentNode ? parentNode->data : "";
	}

	// Returns true is a node is find by given name, false if it's not found in the tree
	bool find(const string& data) const
	{
		if (!root || !findNode(data, false))
			return false;

		return true;
	}

	// Returns true if a new node (subordinate) is successfully added to the tree
	bool insert(const string& parentData, const string& childData)
	{
		Node* node = findNode(parentData, false);

		// If the node where should be attached the new node is not found 
		// or there is an existing node with the new data - cancel the operation
		if (!node || findNode(childData, false))
			return false;

		Node* new_node = new (std::nothrow) Node(childData);
		if (!new_node) // If memory allocation fails
			throw bad_alloc();
		
		try {
			node->children.push_back(new_node);
		}
		catch (...)
		{
			std::cout << "FAIL: Memory allocation\n";
			delete new_node;
			throw bad_alloc();
		}

		++node->childrenCount;
		setAllSalaries(root);
		return true;
	}

	// Returns true if a new node (subordinate) is successfully added or an existing one is moved
	bool moveNode(const string& parentData, const string& childData)
	{
		Node* currParent = findNode(childData, true); // Find the current parent of the node we want to move
		if (!currParent) // If not found then the node we want to move doesn't exist and we add it as a child of other node
			return insert(parentData, childData);

		Node* newParent = findNode(parentData, false); // Find the node to which we want to move a node
		if (!newParent)
			return false;

		Node* toMove = extractNode(currParent, childData); // Gets the node we want to move and removes it from the children list of it's current parent

		--currParent->childrenCount;
		++newParent->childrenCount;

		newParent->children.push_back(toMove); // Attaches the node to it's new parent
		setAllSalaries(root);
	}

	// Returns true if a node (subordinate) is deleted from the tree
	bool remove(const string& data)
	{
		Node* parent = findNode(data, true); // Find the parent of the node we want to remove
		if (!parent)
			return false;

		Node* toDel = extractNode(parent, data); // Find the node and remove it from it's current parent children's list
		removeNode(toDel, parent); // Deletes the node and moves all it's children to it's previous parent
		return true;
	}

	// Returns the size of the tree
	const int treeSize() const
	{
		if (!root)
			return 0;

		int count = 1;
		findTreeSize(root, count);
		return  count;
	}

	// Returns the depth of the tree
	const int depth() const
	{
		if (!root)
			return 0;

		return 1 + findDepth(root);
	}

	// Returns the count of all children (subordinates) of a given node name (manager)
	const int directHeirs(const string& data) const
	{
		const Node* node = findNode(data, false);
		if (!node)
			return -1;

		return node->childrenCount;
	}

	// Returns the salary of a subordinate
	const unsigned long getSalary(const string& data) const
	{
		const Node* node = findNode(data, false);
		return node != nullptr ? node->salary : -1;
	}

	// Returns the count of all subordinates that have total count of direct and indirect subordinates grater than given number
	const int overloaded(int factor) const
	{
		if (!root)
			return 0;

		int count = 0;
		OverloadedCount(root, factor, count);
		return count;
	}

	// Returns the tree represented as a string
	const string toString() const
	{
		if (!root)
			return "";

		if (root->childrenCount == 0)
			return root->data + '\n';

		queue<const Node*> wave;
		wave.push(root);

		string output = "";
		int onThisLevel = root->childrenCount, onNextLevel = 0, count = 0;
		vector<string> arrayOfRels(onThisLevel);

		// Traversing by levels
		while (!wave.empty())
		{
			// Push elements in the queue until all nodes on this level are traversed
			if (onThisLevel == count)
			{
				// Sort lexicographically
				std::sort(arrayOfRels.begin(), arrayOfRels.end());
				for (const string& str : arrayOfRels)
					output += str;

				arrayOfRels.resize(onNextLevel);
				onThisLevel = onNextLevel;
				onNextLevel = 0;
				count = 0;
			}

			const Node* current = wave.front();
			wave.pop();

			for (const Node* node : current->children)
			{
				wave.push(node);
				arrayOfRels[count] = current->data + "-" + node->data + "\n"; // In each cell is saved parent-child relation
				onNextLevel += node->childrenCount;
				count++;
			}
		}

		return output;
	}

	// Incorporates the tree and recalculates the salaries of the subordinates
	void incorporate()
	{
		if (!root)
			return;

		incorporateRec(root);
		setAllSalaries(root);
	}

	// Modernizes the tree and recalculates the salaries of the subordinates
	void modernize()
	{
		if (!root)
			return;

		int level = 0;
		modernizeRec(root, level);
		setAllSalaries(root);
	}

	// By given 2 trees, creates and returns their unification
	const Tree join(const Tree& other) const
	{
		if (root->data != other.root->data)
			return Tree();

		try {
			Tree result(root->data);
			queue<Node*> wave1;
			queue<Node*> wave2;
			wave1.push(root);
			wave2.push(other.root);

			int onThisLevel1 = root->childrenCount;
			int onThisLevel2 = other.root->childrenCount;

			// Traversing both trees by levels simultaneously
			while (!wave1.empty() || !wave2.empty())
			{
				int index = 0;
				vector<std::pair<string, string>> relations(onThisLevel1 + onThisLevel2);

				// Checks if the current wave of nodes breaks the unification. If not - fills the vector and loads next wave
				if (!joinHelper(other, wave1, relations, onThisLevel1, index) ||
					!joinHelper(*this, wave2, relations, onThisLevel2, index))
				{
					result.clear();
					return Tree();
				}

				// Sort the vector lexicographically. It is needed in cases where 1 node has 2 different parents on the same level
				std::sort(relations.begin(), relations.end());

				result.insert(relations[0].first, relations[0].second);
				for (int i = 1; i < index; ++i)
				{
					if (relations[i] != relations[i - 1])
						result.insert(relations[i].first, relations[i].second);
				}
			}

			return result;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return Tree();
		}
	}

	// Deletes the tree
	void clear()
	{
		if (root)
			clear(root);
	}

private:
	// Returns the searched node if found and nullptr if it's not found
	// If searchedParent is true then we search the parent of a node
	Node* findNode(const string& data, bool searchParent) const
	{
		// Searching starts from the root
		if (!root)
			return nullptr;

		if (data == root->data)
			return searchParent ? nullptr : root;

		// Using BFS traversal to find the searched node

		Node* current;
		queue<Node*> wave;
		wave.push(root);

		while (!wave.empty())
		{
			current = wave.front();
			wave.pop();

			for (Node*& node : current->children)
			{
				if (data == node->data)
					return searchParent ? current : node;
			
				wave.push(node);
			}
		}

		return nullptr;
	}

	// Searches a node, but the searching starts from a given node
	bool findByNode(const Node* from, const string& searched) const
	{
		// Using DFS traversal
		if (searched == from->data)
			return true;

		for (const Node* node : from->children)
			return findByNode(node, searched);

		return false;
	}

	// Finds, deletes a node from it's parent's children list and returns it
	Node*& extractNode(Node*& parentNode, const string& childData)
	{
		list<Node*>::iterator it = parentNode->children.begin();
		while ((*it)->data != childData) {
			++it;
		}

		Node* toExtract = *(it);
		parentNode->children.erase(it);
		
		return toExtract;
	}

	// Deletes a node and attaches all his children to it's parent.
	void removeNode(Node*& toDel, Node*& parentNode)
	{
		while (!toDel->children.empty())
		{
			parentNode->children.push_back(toDel->children.front());
			toDel->children.pop_front();
		}

		parentNode->childrenCount += toDel->childrenCount - 1;
		delete toDel;
		toDel = nullptr;
		setAllSalaries(root);
	}

	// Finds the size of a subtree with root - a given node
	void findTreeSize(const Node* node, int& count) const
	{
		count += node->childrenCount;
		for (Node* child : node->children)
			findTreeSize(child, count);
	}

	// Finds the depth of a subtree with root - a given node
	int findDepth(const Node* node) const
	{
		// Using DFS traversal
		int maxDepth = 0;
		int curDepth = 0;
		for (const Node* n : node->children)
		{
			curDepth = 1 + findDepth(n);
			if (maxDepth < curDepth)
				maxDepth = curDepth;
		}

		return maxDepth;
	}

	// Sets the salaries of all nodes (subordinates)
	unsigned long setAllSalaries(Node*& node)
	{
		node->salary = 0;
		for (Node*& n : node->children)
		{
			node->salary += (setAllSalaries(n) - SALARY_FOR_DIRECT_SUBORDINATE * n->childrenCount) +
				(n->childrenCount * SALARY_FOR_INDIRECT_SUBORDINATE) + SALARY_FOR_DIRECT_SUBORDINATE;
		}

		return node->salary;
	}

	// Finds the count of all overloaded nodes (subordinates)
	int OverloadedCount(const Node* node, int factor, int& count) const
	{
		int heirs = 0;
		for (const Node* child : node->children)		
			heirs += 1 + OverloadedCount(child, factor, count);
		
		if (heirs > factor)
			++count;

		return heirs;
	}

	// Incorporates the tree recursively
	void incorporateRec(Node*& node)
	{
		// First go to the bottom of the recursion
		list<Node*>::iterator it = node->children.begin();
		for (it; it != node->children.end(); ++it)
			incorporateRec(*it);
		
		// When going back to the top, if a node has atleast 2 children then incorporate it
		if (node->childrenCount > 1)
		{
			// Find the node with the biggest salary
			list<Node*>::iterator it;
			list<Node*>::iterator max = node->children.begin();
			for (it = next(node->children.begin()); it != node->children.end(); ++it)
			{
				// In case more than 1 node have the maximum salary - get the one with the smallest name lexicographically
				if ( ((*max)->salary < (*it)->salary) || 
					(((*max)->salary == (*it)->salary)) && ((*it)->data < (*max)->data) )
						max = it;
			}
			
			// Move all children of the node that is being incorporated to the node that was found in the previous step
			it = node->children.begin();
			while ( it != node->children.end() )
			{
				if (it == max) {
				    ++it;
					continue;
				}

				Node* temp = *it;
				it = node->children.erase(it);
				(*max)->children.push_back(temp);
			}

			(*max)->childrenCount += node->childrenCount - 1;
			node->childrenCount = 1; // At the end that node has only 1 child -> the maximum node
		}
	}

	// Modernizes the tree recursively
	bool modernizeRec(Node*& node, int& level)
	{
		// First go the the bottom of the recursion
		list<Node*>::iterator child = node->children.begin();
		while( child != node->children.end())
		{
			level++; // Increase the level each time when going down
			if (modernizeRec(*child, level)) // If it's true then the child is on odd level and has more than 2 children
			{
				// Move all children of "child" (the node where modernization is executing) to "node" (it's parent)
				while (!(*child)->children.empty())
				{
					node->children.push_front((*child)->children.front());
					(*child)->children.pop_front();
				}

				node->childrenCount += (*child)->childrenCount - 1;
				Node* toDel = *child;
				child = node->children.erase(child);
				delete toDel; // Delete it
			}
			else
				++child;
		}

		// Return True -> the node must be modernized; False -> skip it
		level--;
		if (level % 2 == 0 && node->childrenCount != 0)
			return true;

		return false;
	}

	// Traverses the nodes on the current level and checks for relations X->...->Y in tree1 and Y->...->X in tree2
	bool joinHelper(const Tree& other, queue<Node*>& wave, vector<std::pair<string, string>>& relations, int& onThisLevel, int& index) const
	{
		// Traverse all nodes on this level
		int count = 0, onNextLevel = 0;
		while (count < onThisLevel)
		{
			Node* node = wave.front();
			wave.pop();
			for (Node* child : node->children)
			{
				// Check if "child" is a manager (direct or indirect) of "node" in the other tree
				Node* checkNode = other.findNode(child->data, false);
				if (checkNode && other.findByNode(checkNode, node->data))
					return false;

				relations[index] = std::make_pair(node->data, child->data); // Add in the array 1 relation

				// If the "child" has 0 children then it shouldn't be added in the queue as it cannot make any manager-subordinate relations as a manager
				if (child->childrenCount > 0) {
					wave.push(child);
					onNextLevel += child->childrenCount;
				}

				++count;
				++index;
			}
		}

		onThisLevel = onNextLevel;
		return true;
	}

	Node* copy(const Node* node)
	{
		Node* new_node = new (std::nothrow) Node(node->data);
		if (!new_node)
			throw bad_alloc();

		new_node->salary = node->salary;
		new_node->childrenCount = node->childrenCount;
		for (const Node* child : node->children)
		{
			Node* new_child = copy(child);
			new_node->children.push_back(new_child);
		}

		return new_node;
	}

	void clear(Node*& node)
	{
		for (Node*& n : node->children)
			clear(n);

		delete node;
		node = nullptr;
	}

private:
	Node* root;
};