#pragma once
#include <iostream>
#include <exception>

template <class T>
struct Node
{
	T data;
	Node* next;
	Node() { next = nullptr; }
	Node(const T& value, Node* node) : data(value), next(node) {};
};

/// <summary>
/// Despite the name, it isn't exactly the STL data sturcture std::list/forward_list. Implemented is what i need
/// We can iterate trough the list only from the first node to the last as it is single-linked list.
/// There is a push_back method so when i add elements and then search elements with the same key,
///		iterating from the beginning, the first found element will be the element added first in the list.
///	For example: adding 2 clients on the same minute, that can (and have to) wait the same time, the client that will leave first will be the first added.
/// 
/// All methods in this List can be executed for O(1) time
/// </summary>
template <class T>
class List 
{
public:

	List()
	{
		head = nullptr;
		tail = nullptr;
		before_head = nullptr;
	}

	~List()
	{
		while (!empty())
			pop_front();

		delete before_head;
	}

	void push_front(const T& element)
	{
		Node* node;
		try {
			node = new Node(element, head);
		}
		catch (std::bad_alloc) {
			std::cout << "Not enough memory!" << std::endl;
			throw;
		}

		if (empty())
			tail = node;
		
		head = node;
		before_head->next = head;
	}

	void push_back(const T& element)
	{
		Node<T>* node;
		try {
			node = new Node<T>(element, nullptr);
		}
		catch (std::bad_alloc) {
			std::cout << "Not enough memory!" << std::endl;
			throw;
		}

		if (empty())
		{
			head = node;
			tail = node;

			try {
				before_head = new Node<T>;
			}
			catch (std::bad_alloc) {
				std::cout << "Not enough memory!" << std::endl;
				throw;
			}

			before_head->next = head;
		}
		else
		{
			tail->next = node;
			tail = node;
		}
	}

	void pop_front()
	{
		if (empty())
			throw std::logic_error("Pop from an empty list!");

		if (head == tail)
		{
			delete head;
			delete before_head;

			head = nullptr;
			tail = nullptr;
			before_head = nullptr;

			return;
		}

		Node<T>* temp = head;
		head = head->next;
		delete temp;

		before_head->next = head;
	}

	Node<T>*& front_node()
	{
		return head;
	}

	Node<T>*& pre_head()
	{
		return before_head;
	}

	Node<T>*& next(Node<T>*& node)
	{
		return node->next;
	}

	Node<T>*& erase_after(Node<T>*& node)
	{
		if (!node->next)
			throw std::logic_error("Deleting non-existing element!");

		if (node->next == head)
		{
			pop_front();
			node->next = head;
		}
		else if (node->next == tail)
		{
			delete tail;
			node->next = nullptr;
			tail = node;
		}
		else
		{
			Node<T>* temp = node->next;
			node->next = temp->next;
			delete temp;
		}

		return node->next;
	}

	bool empty() const
	{
		return head == nullptr;
	}

private:

	Node<T>* before_head;
	Node<T>* head;
	Node<T>* tail;
};