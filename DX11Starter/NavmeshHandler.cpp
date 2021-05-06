#include "pch.h"
#include "NavmeshHandler.h"

NavmeshHandler* NavmeshHandler::instance = nullptr;

NavmeshHandler::NavmeshHandler()
{

}

NavmeshHandler::~NavmeshHandler()
{
	for (size_t i = 0; i < Grids.size(); i++)
	{
		delete Grids[i];
	}
}

bool NavmeshHandler::SetupInstance()
{
	if (instance == nullptr) {
		instance = new NavmeshHandler();
		return true;
	}
	return false;
}

NavmeshHandler* NavmeshHandler::GetInstance()
{
	return instance;
}

bool NavmeshHandler::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
}

vector<Grid*>& NavmeshHandler::GetGrids()
{
	return Grids;
}

Grid* NavmeshHandler::GetGrid(unsigned int gridID)
{
	if (GridsMap.count(gridID)) return GridsMap[gridID];
	return nullptr;
}

vector<unsigned int>& NavmeshHandler::GetGridConnections(unsigned int gridID)
{
	assert(GridConnections.count(gridID));
	return GridConnections[gridID];
}

void NavmeshHandler::ClearGrids()
{
	for (size_t i = 0; i < Grids.size(); i++)
	{
		delete Grids[i];
	}
	Grids.clear();
	GridsMap.clear();
	GridConnections.clear();
}

Grid* NavmeshHandler::AddGrid(GridDescription desc, bool automaticLinearGridConnections)
{
	if (!GridsMap.count(desc.gridID)) {
		if ((desc.position.x == 0.0f && desc.position.y == 0.0f && desc.position.z == 0.0f) ||
			(desc.size.x == 0.0f && desc.size.y == 0.0f && desc.size.z == 0.0f) ||
			(desc.nodeSpacing == 0.0f))
			return nullptr;

		Grid* newGrid = new Grid(desc.gridID, desc.position, desc.size, desc.nodeSpacing);
		newGrid->CreateGrid();
		GridsMap.insert({ desc.gridID, newGrid });
		Grids.push_back(newGrid);

		if (desc.gridConnectionIDs != nullptr && desc.gridConnectionsCount != 0) {
			for (size_t i = 0; i < desc.gridConnectionsCount; i++)
			{
				AddGridConnection(desc.gridID, desc.gridConnectionIDs[i]);
			}
		}
		else if (automaticLinearGridConnections && desc.gridConnectionIDs == nullptr && Grids.size() > 1) {
			AddGridConnection(desc.gridID, previousGridID);
		}
		previousGridID = desc.gridID;

		return newGrid;
	}
	return nullptr;
}

void NavmeshHandler::AddGridConnection(unsigned int gridID_A, unsigned int gridID_B)
{
	if (!GridConnections.count(gridID_A)) GridConnections.insert({ gridID_A, vector<unsigned int>() });
	GridConnections[gridID_A].push_back(gridID_B);
	if (!GridConnections.count(gridID_B)) GridConnections.insert({ gridID_B, vector<unsigned int>() });
	GridConnections[gridID_B].push_back(gridID_A);
}

Grid* NavmeshHandler::GetGridAtPosition(XMFLOAT3 position)
{
	if (Grids.size() == 0) return nullptr;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR gridStart;
	XMVECTOR gridEnd;
	Grid* closest = Grids[0];
	float closestNodeDist = XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&closest->FindNearestNode(position)->GetPos()), pos)).m128_f32[0];
	for (size_t i = 1; i < Grids.size(); i++)
	{
		gridStart = XMLoadFloat3(&Grids[i]->GetGridStart());
		gridEnd = XMVectorAdd(gridStart, XMLoadFloat3(&Grids[i]->GetGridSize()));
		if ((pos.m128_f32[0] >= gridStart.m128_f32[0]) && (pos.m128_f32[0] <= gridEnd.m128_f32[0]) &&
			(pos.m128_f32[1] >= gridStart.m128_f32[1]) && (pos.m128_f32[1] <= gridEnd.m128_f32[1]) &&
			(pos.m128_f32[2] >= gridStart.m128_f32[2]) && (pos.m128_f32[2] <= gridEnd.m128_f32[2])) {
			return Grids[i];
		}
		else if(XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&Grids[i]->FindNearestNode(position)->GetPos()), pos)).m128_f32[0] < closestNodeDist) {
			closestNodeDist = XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&Grids[i]->FindNearestNode(position)->GetPos()), pos)).m128_f32[0];
			closest = Grids[i];
		}
	}
	return closest;
}

Grid* NavmeshHandler::GetAdjacentGrid(unsigned int currentGridID, XMFLOAT3 position)
{
	if(!GridsMap.count(currentGridID) || !GridConnections.count(currentGridID)) return nullptr;

	XMVECTOR pos = XMLoadFloat3(&position);

	vector<unsigned int> connections = GetGridConnections(currentGridID);

	Grid* closest = GridsMap[connections[0]];
	float closestNodeDist = XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&closest->FindNearestNode(position)->GetPos()), pos)).m128_f32[0];
	float testVal;
	for (size_t i = 1; i < connections.size(); i++)
	{
		testVal = XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&GridsMap[connections[i]]->FindNearestNode(position)->GetPos()), pos)).m128_f32[0];
		if (testVal < closestNodeDist) {
			closestNodeDist = testVal;
			closest = GridsMap[connections[i]];
		}
	}

	return closest;
}
