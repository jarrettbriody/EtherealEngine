#pragma once
#include "pch.h"
#include "Node.h"

class PriorityQueue
{
private:
	std::list<Node*> queue;
public:
	PriorityQueue();
	~PriorityQueue();

	void Add(Node* node);
	Node* Pop();
	void Remove(Node* node);
	int Count();
	bool Contains(Node* node);
	bool IsEmpty();
	Node* Get(int index);
	int IndexOf(Node* node);
	std::list<Node*> GetQueue();
};