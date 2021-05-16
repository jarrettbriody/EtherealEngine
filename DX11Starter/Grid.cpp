#include "pch.h"
#include "Grid.h"

Grid::Grid()
{
}

Grid::Grid(unsigned int ID, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 size, float spacing)
{
	gridStartPosition = start;
	gridSize = size;
	nodeSpacing = spacing;
	gridID = ID;
}

Grid::~Grid()
{
}

void Grid::CreateGrid()
{
	numberOfRows = (int)(gridSize.x / nodeSpacing);
	numberOfColumns = (int)(gridSize.z / nodeSpacing);

	grid = std::vector<std::vector<Node>>(numberOfRows, std::vector<Node>(numberOfColumns));
	bool obstruction;

	for (int z = 0; z < numberOfColumns; z++)
	{
		for (int x = 0; x < numberOfRows; x++)
		{
			XMFLOAT3 nodePos = XMFLOAT3(gridStartPosition.x + (x * nodeSpacing), gridStartPosition.y, gridStartPosition.z + (z * nodeSpacing));
			obstruction = false;

			// Update physics
			Config::DynamicsWorld->updateAabbs();
			Config::DynamicsWorld->computeOverlappingPairs();

			btVector3 from(nodePos.x, nodePos.y, nodePos.z);
			btVector3 to(nodePos.x, -100.f, nodePos.z);

			// Create variable to store the ray hit and set flags
			btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
			closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

			Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

			if (closestResult.hasHit())
			{
				nodePos.y = closestResult.m_hitPointWorld.getY();
				PhysicsWrapper* wrapper = (PhysicsWrapper*)closestResult.m_collisionObject->getUserPointer();

				if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
					Entity* hit = (Entity*)wrapper->objectPointer;
					//printf("Raycast Hit: %s\n", hit->GetName().c_str());

					// If our raycast didn't hit a floor then we probably can't move here
					if (!hit->HasTag("navmesh"))
						obstruction = true;
				}
			}
			else
			{
				// The ray didn't hit anything so just assign a default y value and make it an obstruction
				nodePos.y = 0.0f;
				obstruction = true;
			}

			grid[x][z] = Node(gridID, x, z, nodePos, obstruction);
			gridEndPosition = nodePos;

			//------
			// Debug line shit
			//------
			// Create the world matrix for the debug line
			/*
			XMFLOAT4X4 wm;
			XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));

			// Create debug line
			DebugLines* dl = new DebugLines("TestRay", 0, false);
			XMFLOAT3 c;
			if (obstruction)
				c = XMFLOAT3(1.0f, 0.0f, 0.0f);
			else
				c = XMFLOAT3(0.0f, 1.0f, 0.0f);
			
			if (true)
			{
				dl->color = c;
				dl->worldMatrix = wm;

				XMFLOAT3 start = XMFLOAT3(nodePos.x, gridStartPosition.y, nodePos.z);

				// Draw the debug line to show the raycast
				XMFLOAT3* rayPoints = new XMFLOAT3[8];
				rayPoints[0] = start;
				rayPoints[1] = start;
				rayPoints[2] = start;
				rayPoints[3] = start;
				rayPoints[4] = nodePos;
				rayPoints[5] = nodePos;
				rayPoints[6] = nodePos;
				rayPoints[7] = nodePos;
				dl->GenerateCuboidVertexBuffer(rayPoints, 8);
				delete[] rayPoints;
			}
			*/
		}
	}
	gridSize = XMFLOAT3(gridEndPosition.x - gridStartPosition.x, gridSize.y, gridEndPosition.z - gridStartPosition.x);
}

Node* Grid::FindNearestNode(DirectX::XMFLOAT3 position)
{
	unsigned int xIndex = 0;
	unsigned int zIndex = 0;

	if (position.x > gridStartPosition.x && position.x < gridEndPosition.x) {
		xIndex = (unsigned int)((position.x - gridStartPosition.x) / nodeSpacing);
	}
	if (position.z > gridStartPosition.z && position.z < gridEndPosition.z) {
		zIndex = (unsigned int)((position.z - gridStartPosition.z) / nodeSpacing);
	}

	if (position.x <= gridStartPosition.x) {
		xIndex = 0;
	}
	else if (position.x >= gridEndPosition.x) {
		xIndex = numberOfRows - 1;
	}

	if (position.z <= gridStartPosition.z) {
		zIndex = 0;
	}
	else if (position.z >= gridEndPosition.z) {
		zIndex = numberOfColumns - 1;
	}

	return &grid[xIndex][zIndex];
}

Node* Grid::GetNode(int row, int col)
{
	if (row >= numberOfRows || col >= numberOfColumns || row < 0 || col < 0)
		throw std::out_of_range("Grid::GetNode() : Out of range");

	return &grid[row][col];
}

std::vector<Node*> Grid::GetAdjacentNodes(Node* node)
{
	std::vector<Node*> adjacent;
	int row = node->GetRow();
	int col = node->GetCol();

	//
	//
	// 	   CRASH HERE, ACCESS VIOLATION INTO THE GRID
	//
	//

	if (row + 1 < numberOfRows)
	{
		adjacent.push_back(&grid[row + 1][col]);
	}
	if (row + 1 < numberOfRows && col + 1 < numberOfColumns)
	{
		adjacent.push_back(&grid[row + 1][col + 1]);
	}
	if (row + 1 < numberOfRows && col - 1 >= 0)
	{
		adjacent.push_back(&grid[row + 1][col - 1]);
	}
	if (row - 1 >= 0)
	{
		adjacent.push_back(&grid[row - 1][col]);
	}
	if (row - 1 >= 0 && col - 1 >= 0)
	{
		adjacent.push_back(&grid[row - 1][col - 1]);
	}
	if (row - 1 >= 0 && col + 1 < numberOfColumns)
	{
		adjacent.push_back(&grid[row - 1][col + 1]);
	}
	if (col + 1 < numberOfColumns)
	{
		adjacent.push_back(&grid[row][col + 1]);
	}
	if (col - 1 >= 0)
	{
		adjacent.push_back(&grid[row][col - 1]);
	}

	return adjacent;
}

void Grid::GetUnobstructedMoves(Node* node, vector<Node*>& vec, int& totalAdjacents)
{
	std::vector<Node*> possibleMoves = GetAdjacentNodes(node);
	totalAdjacents = possibleMoves.size();
	//std::vector<Node*> unobstructedMoves;

	for (auto& move : possibleMoves)
	{
		if (!move->IsObstruction())
			vec.push_back(move);
	}

	//return unobstructedMoves;
}

/*
void Grid::ResetAStar()
{
	for (auto& node : touchedNodes)
	{
		node->SetCostSoFar(0.0f);
		node->SetEstimatedTotalCost(0.0f);
	}

	touchedNodes.clear();
}

std::list<Node*> Grid::ReconstructPath(std::map<Node*, Node*> cameFrom, Node* initialNode)
{
	Node* current = initialNode;
	std::list<Node*> finalPath;
	finalPath.push_back(current);

	while (cameFrom.find(current) != cameFrom.end())
	{
		current = cameFrom[current];
		finalPath.push_back(current);
	}

	finalPath.reverse();

	return finalPath;
}

std::list<Node*> Grid::FindPath(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	ResetAStar();
	std::vector<Node*> closedNodes;
	PriorityQueue openNodes;
	std::map<Node*, Node*> cameFrom;

	Node* firstNode = FindNearestNode(start);
	Node* target = FindNearestNode(end);
	
	firstNode->SetEstimatedTotalCost(GetEstimatedCost(firstNode, target));
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

		std::vector<Node*> possibleMoves = GetUnobstructedMoves(currentNode);

		for (auto& move : possibleMoves)
		{
			if (std::find(closedNodes.begin(), closedNodes.end(), move) != closedNodes.end())
				continue;

			float costToMove = currentNode->GetCostSoFar() + GetActualCost(currentNode, move);

			if (move->GetCostSoFar() == 0 || move->GetCostSoFar() > costToMove)
			{
				cameFrom[move] = currentNode;

				move->SetCostSoFar(costToMove);
				move->SetEstimatedTotalCost(move->GetCostSoFar() + GetEstimatedCost(move, target));
				touchedNodes.push_back(move);

				if (!openNodes.Contains(move))
				{
					openNodes.Add(move);
				}
			}
		}
	}

	return std::list<Node*>();
}

float Grid::GetEstimatedCost(Node* source, Node* destination)
{
	return abs(destination->GetRow() - source->GetRow()) + abs(destination->GetCol() - source->GetCol());
}

float Grid::GetActualCost(Node* source, Node* destination)
{
	float xDiff = source->GetPos().x - destination->GetPos().x;
	float yDiff = source->GetPos().y - destination->GetPos().y;
	float zDiff = source->GetPos().z - destination->GetPos().z;

	return sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);
}
*/

XMFLOAT3 Grid::GetGridStart()
{
	return gridStartPosition;
}

XMFLOAT3 Grid::GetGridSize()
{
	return gridSize;
}

unsigned int Grid::GetGridID()
{
	return gridID;
}

float Grid::GetNodeSpacing()
{
	return nodeSpacing;
}
