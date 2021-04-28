#include "pch.h"
#include "Grid.h"

Grid::Grid()
{
}

Grid::Grid(DirectX::XMFLOAT3 start, DirectX::XMFLOAT2 size, float spacing)
{
	gridStartPosition = start;
	gridSize = size;
	nodeSpacing = spacing;
}

Grid::~Grid()
{
}

void Grid::CreateGrid()
{
	numberOfRows = round(gridSize.x / (nodeSpacing * 2));
	numberOfColumns = round(gridSize.y / (nodeSpacing * 2));

	grid = std::vector<std::vector<Node>>(numberOfRows, std::vector<Node>(numberOfColumns));
	bool obstruction;

	for (int z = 0; z < numberOfColumns; z++)
	{
		for (int x = 0; x < numberOfRows; x++)
		{
			XMFLOAT3 nodePos = XMFLOAT3(gridStartPosition.x + (x * nodeSpacing * 2 + nodeSpacing), gridStartPosition.y, gridStartPosition.z + (z * nodeSpacing * 2 + nodeSpacing));
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
					if (hit->GetName().find("Floor") == std::string::npos)
						obstruction = true;
				}
			}
			else
			{
				// The ray didn't hit anything so just assign a default y value and make it an obstruction
				nodePos.y = 0.0f;
				obstruction = true;
			}

			grid[x][z] = Node(x, z, nodePos, obstruction);

			//------
			// Debug line shit
			//------
			/*
			// Create the world matrix for the debug line
			XMFLOAT4X4 wm;
			XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));

			// Create debug line
			DebugLines* dl = new DebugLines("TestRay", 0, false);
			XMFLOAT3 c;
			if (obstruction)
				c = XMFLOAT3(1.0f, 0.0f, 0.0f);
			else
				c = XMFLOAT3(0.0f, 1.0f, 0.0f);
			
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
			delete[] rayPoints;*/
		}
	}
}

Node* Grid::FindNearestNode(DirectX::XMFLOAT3 position)
{
	int x;
	int z;
	float diffx;
	float diffz;

	if ((position.x > 0 && gridStartPosition.x < 0) || (position.x < 0 && gridStartPosition.x > 0))
		diffx = fabs(position.x) + fabs(gridStartPosition.x);
	else
		diffx = fabs(position.x) - fabs(gridStartPosition.x);

	if ((position.z > 0 && gridStartPosition.z < 0) || (position.z < 0 && gridStartPosition.z > 0))
		diffz = fabs(position.z) + fabs(gridStartPosition.z);
	else
		diffz = fabs(position.z) - fabs(gridStartPosition.z);

	x = round(diffx / (nodeSpacing * 2));
	z = round(diffz / (nodeSpacing * 2));

	if (x < 0)
		x = 0;
	else if (x >= numberOfRows)
		x = numberOfRows - 1;

	if (z < 0)
		z = 0;
	else if (z >= numberOfColumns)
		z = numberOfColumns - 1;

	return &grid[x][z];
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

std::vector<Node*> Grid::GetUnobstructedMoves(Node* node)
{
	std::vector<Node*> possibleMoves = GetAdjacentNodes(node);
	std::vector<Node*> unobstructedMoves;

	for (auto& move : possibleMoves)
	{
		if (!move->IsObstruction())
			unobstructedMoves.push_back(move);
	}

	return unobstructedMoves;
}

void Grid::ResetAStar()
{
	// TODO: Have Grid keep track of the "touched" nodes so we only need to reset those instead of looping through everything.
}