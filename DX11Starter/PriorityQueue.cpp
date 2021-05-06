#include "pch.h"
#include "PriorityQueue.h"

PriorityQueue::PriorityQueue()
{
}

PriorityQueue::~PriorityQueue()
{
}

void PriorityQueue::Add(Node* node)
{
	bool found = false;

	if (queue.empty())
		queue.push_back(node);
	else
	{
		for (std::list<Node*>::iterator it = queue.begin(); it != queue.end(); ++it)
		{
			if ((*it)->GetEstimatedTotalCost() >= node->GetEstimatedTotalCost())
			{
				queue.insert(it, node);
				found = true;
				break;
			}
		}
		if (!found)
		{
			queue.push_back(node);
		}
	}
}

Node* PriorityQueue::Pop()
{
	Node* first = queue.front();
	queue.pop_front();
	return first;
}

void PriorityQueue::Remove(Node* node)
{
	queue.remove(node);
}

int PriorityQueue::Count()
{
	return queue.size();
}

bool PriorityQueue::Contains(Node* node)
{
	return (std::find(queue.begin(), queue.end(), node) != queue.end());
}

bool PriorityQueue::IsEmpty()
{
	return queue.empty();
}

Node* PriorityQueue::Get(int index)
{
	auto it = std::next(queue.begin(), index);

	return *it;
}

int PriorityQueue::IndexOf(Node* node)
{
	int index = 0;

	for (std::list<Node*>::iterator it = queue.begin(); it != queue.end(); ++it)
	{
		if ((*it)->Equals(*node))
			return index;

		index++;
	}

	return -1;
}

std::list<Node*> PriorityQueue::GetQueue()
{
	return queue;
}