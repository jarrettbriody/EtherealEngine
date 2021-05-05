#pragma once
#include "Config.h"
#include "Grid.h"

using namespace DirectX;
using namespace std;

struct GridDescription {
	unsigned int gridID = 0;
	XMFLOAT3 position = ZERO_VECTOR3;
	XMFLOAT3 size = ZERO_VECTOR3;
	float nodeSpacing = 0.0f;
	unsigned int* gridConnectionIDs = nullptr;
	unsigned int gridConnectionsCount = 0;
};

class NavmeshHandler
{
private:
	static NavmeshHandler* instance;

	unsigned int previousGridID = 0;

	vector<Grid*> Grids;
	map<unsigned int, Grid*> GridsMap;
	map<unsigned int, vector<unsigned int>> GridConnections;

	NavmeshHandler();
	~NavmeshHandler();
public:
	static bool SetupInstance();
	static NavmeshHandler* GetInstance();
	static bool DestroyInstance();

	vector<Grid*>& GetGrids();
	Grid* GetGrid(unsigned int gridID);
	vector<unsigned int>& GetGridConnections(unsigned int gridID);
	void ClearGrids();
	Grid* AddGrid(GridDescription desc, bool automaticLinearGridConnections = false);
	void AddGridConnection(unsigned int gridID_A, unsigned int gridID_B);

	Grid* GetGridAtPosition(XMFLOAT3 position);
	Grid* GetAdjacentGrid(unsigned int currentGridID, XMFLOAT3 position);
};