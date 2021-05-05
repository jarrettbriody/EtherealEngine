#pragma once
#include "pch.h"

class Node
{
private:
	int row;
	int col;
	float costSoFar; //Cost to reach this grid from START poistion (for A-Star)
	float estimatedTotalCost; //Cost to reach from START to END, using heuristic (for A-Star)
	DirectX::XMFLOAT3 pos;
	bool obstruction;
	unsigned int gridID = 0;
public:
	Node();
	Node(unsigned int gridID, int gridRow, int gridCol, DirectX::XMFLOAT3 worldPos, bool obs);
	~Node();

	unsigned int GetGridID();
	int GetRow();
	int GetCol();
	float GetEstimatedTotalCost();
	float GetCostSoFar();
	DirectX::XMFLOAT3 GetPos();
	bool IsObstruction();
	bool Equals(Node node);
	void SetEstimatedTotalCost(float cost);
	void SetCostSoFar(float cost);
};