#include "pch.h"
#include "Node.h"

Node::Node()
{
}

Node::Node(int gridRow, int gridCol, DirectX::XMFLOAT3 worldPos, bool obs)
{
    row = gridRow;
    col = gridCol;
    pos = worldPos;
    obstruction = obs;
    costSoFar = 0.0f;
    estimatedTotalCost = 0.0f;
}

Node::~Node()
{
}

int Node::GetRow()
{
    return row;
}

int Node::GetCol()
{
    return col;
}

float Node::GetEstimatedTotalCost()
{
    return estimatedTotalCost;
}

float Node::GetCostSoFar()
{
    return costSoFar;
}

DirectX::XMFLOAT3 Node::GetPos()
{
    return pos;
}

bool Node::IsObstruction()
{
    return obstruction;
}

bool Node::Equals(Node node)
{
    DirectX::XMFLOAT3 nodePos = node.GetPos();
    
    if (nodePos.x == pos.x && nodePos.y == pos.y && nodePos.z == pos.z)
        return true;
    
    return false;
}

void Node::SetEstimatedTotalCost(float cost)
{
    estimatedTotalCost = cost;
}

void Node::SetCostSoFar(float cost)
{
    costSoFar = cost;
}