#pragma once
#include "NavmeshHandler.h"

class AStarSolver
{
private:
	NavmeshHandler* navmesh = nullptr;

	Grid* startGrid = nullptr;
	Grid* currentGrid = nullptr;
	Grid* nextGrid = nullptr;
	Grid* endGrid = nullptr;

	std::vector<Node*> touchedNodes;

	void Reset();
	vector<Node*> ReconstructPath(map<Node*, Node*> cameFrom, Node* initialNode);
	float GetEstimatedCost(Node* source, Node* destination);
	float GetActualCost(Node* source, Node* destination);
public:
	AStarSolver();
	~AStarSolver();
	vector<Node*> FindPath(XMFLOAT3 start, XMFLOAT3 end);
	Grid* GetCurrentGrid(XMFLOAT3 start);
};

