#pragma once

#include "DXCore.h"
#include "Config.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "Renderer.h"
#include <regex>
#include <iostream>
#include <map>
#include "Utility.h"
#include <atlbase.h>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
//#include "Terrain.h"
//#include "TerrainMaterial.h"
//#include "Water.h"
//#include "WaterMaterial.h"
#include "SceneLoader.h"
#include "DebugLines.h"

using namespace std;

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	void DrawSky();

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	// Keeps track of the old mouse position for determining how far the mouse moved in a single frame
	POINT prevMousePos;

	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* skyRasterState;
	ID3D11DepthStencilState* skyDepthState;

	Camera* EECamera = nullptr;
	Renderer* EERenderer = nullptr;
	SceneLoader* EESceneLoader = nullptr;

	//terrain example stuff
	//Terrain* terrain;
	//Water* water;
	
	//testing
	Light* testLight;
};

