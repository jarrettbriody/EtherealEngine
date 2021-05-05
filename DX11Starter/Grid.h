#pragma once
#include "pch.h"
#include "Node.h"
#include "Entity.h"
#include "DebugLines.h"
#include "PriorityQueue.h"

class Grid
{
private:
	DirectX::XMFLOAT3 gridStartPosition;
	DirectX::XMFLOAT3 gridEndPosition;
	DirectX::XMFLOAT3 gridSize;
	float nodeSpacing;
	int numberOfRows;
	int numberOfColumns;
	std::vector<Node*> touchedNodes;
	std::vector<std::vector<Node>> grid;
	unsigned int gridID = 0;
public:
	Grid();
	Grid(unsigned int ID, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 size, float spacing);
	~Grid();

	void CreateGrid();
	Node* FindNearestNode(DirectX::XMFLOAT3 position);
	Node* GetNode(int row, int col);
	std::vector<Node*> GetAdjacentNodes(Node* node);
	void GetUnobstructedMoves(Node* node, vector<Node*>& vec, int& totalAdjacents);
	//void ResetAStar();
	//std::list<Node*> ReconstructPath(std::map<Node*, Node*> cameFrom, Node* initialNode);
	//std::list<Node*> FindPath(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);
	//float GetEstimatedCost(Node* source, Node* destination);
	//float GetActualCost(Node* source, Node* destination);

	XMFLOAT3 GetGridStart();
	XMFLOAT3 GetGridSize();
	unsigned int GetGridID();
	float GetNodeSpacing();
};