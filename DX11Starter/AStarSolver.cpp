#include "pch.h"
#include "AStarSolver.h"

void AStarSolver::Reset()
{
	for (auto& node : touchedNodes)
	{
		node->SetCostSoFar(0.0f);
		node->SetEstimatedTotalCost(0.0f);
	}

	touchedNodes.clear();
}

vector<Node*> AStarSolver::ReconstructPath(map<Node*, Node*> cameFrom, Node* initialNode)
{
	Node* current = initialNode;
	vector<Node*> finalPath;
	finalPath.push_back(current);

	while (cameFrom.find(current) != cameFrom.end())
	{
		current = cameFrom[current];
		finalPath.push_back(current);
	}

	std::reverse(finalPath.begin(), finalPath.end());

	return finalPath;
}

float AStarSolver::GetEstimatedCost(Node* source, Node* destination)
{
	return abs(destination->GetRow() - source->GetRow()) + abs(destination->GetCol() - source->GetCol());
}

float AStarSolver::GetActualCost(Node* source, Node* destination)
{
	float xDiff = source->GetPos().x - destination->GetPos().x;
	float yDiff = source->GetPos().y - destination->GetPos().y;
	float zDiff = source->GetPos().z - destination->GetPos().z;

	return sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);
}

AStarSolver::AStarSolver()
{
	navmesh = NavmeshHandler::GetInstance();
}

AStarSolver::~AStarSolver()
{
}

vector<Node*> AStarSolver::FindPath(XMFLOAT3 start, XMFLOAT3 end)
{
	Reset();
	std::vector<Node*> closedNodes;
	PriorityQueue openNodes;
	std::map<Node*, Node*> cameFrom;

	startGrid, currentGrid = navmesh->GetGridAtPosition(start);
	endGrid = navmesh->GetGridAtPosition(end);

	Node* firstNode = startGrid->FindNearestNode(start);
	Node* target = endGrid->FindNearestNode(end);

	firstNode->SetEstimatedTotalCost((startGrid == endGrid) ? GetEstimatedCost(firstNode, target) : GetEstimatedCost(firstNode, startGrid->FindNearestNode(end)));
	touchedNodes.push_back(firstNode);

	openNodes.Add(firstNode);

	while (!openNodes.IsEmpty())
	{
		Node* currentNode = openNodes.Pop();

		if (currentNode == target)
		{
			return ReconstructPath(cameFrom, currentNode);
		}

		closedNodes.push_back(currentNode);

		int totalAdjacents = 0;
		std::vector<Node*> possibleMoves;
		currentGrid->GetUnobstructedMoves(currentNode, possibleMoves, totalAdjacents);
		if (totalAdjacents < 4) {
			XMFLOAT3 currentPos = currentNode->GetPos();
			nextGrid = navmesh->GetAdjacentGrid(currentGrid->GetGridID(), currentPos);
			Node* potentialNewNode = nextGrid->FindNearestNode(currentPos);
			XMFLOAT3 nearestNewNodePos = potentialNewNode->GetPos();
			if ((endGrid == nextGrid) || 
				XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&nearestNewNodePos), XMLoadFloat3(&currentPos))).m128_f32[0] < (currentGrid->GetNodeSpacing() * nextGrid->GetNodeSpacing())) {
				currentGrid = nextGrid;
				currentGrid->GetUnobstructedMoves(potentialNewNode, possibleMoves, totalAdjacents);
			}
		}

		for (auto& move : possibleMoves)
		{
			if (std::find(closedNodes.begin(), closedNodes.end(), move) != closedNodes.end())
				continue;

			float costToMove = currentNode->GetCostSoFar() + GetActualCost(currentNode, move);

			if (move->GetCostSoFar() == 0 || move->GetCostSoFar() > costToMove)
			{
				cameFrom[move] = currentNode;

				move->SetCostSoFar(costToMove);
				move->SetEstimatedTotalCost(move->GetCostSoFar() + ((currentGrid == endGrid) ? GetEstimatedCost(move, target) : GetEstimatedCost(move, currentGrid->FindNearestNode(end))));
				touchedNodes.push_back(move);

				if (!openNodes.Contains(move))
				{
					openNodes.Add(move);
				}
			}
		}
	}

	return std::vector<Node*>();
}
