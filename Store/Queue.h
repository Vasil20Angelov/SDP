#pragma once
#include <iostream>
#include <exception>

template <class T>
class Queue
{
public:
	Queue()
	{
		first = nullptr;
		last = nullptr;
	}

	~Queue()
	{
		while (!empty())
			pop();
	}

	void push(const T& data)
	{
		Node* node;
		try {
			node = new Node(data);
		}
		catch (std::bad_alloc)
		{
			std::cout << "Not enough memory!\n";
			throw;
		}

		if (!first)
		{
			first = node;
			last = first;
		}
		else
		{
			last->next = node;
			last = node;
		}
	}

	void pop()
	{
		if (empty())
			throw std::logic_error("Pop from an empty queue!\n");

		Node* temp = first;
		first = first->next;

		if (!first)
			last = nullptr;

		delete temp;
	}

	T& front()
	{
		if (empty())
			throw std::logic_error("Get an element from empty list!\n");

		return first->data;
	}

	const T& front() const
	{
		if (empty())
			throw std::logic_error("Get an element from empty list!\n");

		return first->data;
	}

	bool empty() const
	{
		return first == nullptr;
	}

private:
	struct Node
	{
		T data;
		Node* next;
		Node(const T& value) : data(value), next(nullptr) {};
	};

	Node* first;
	Node* last;
};