#pragma once

#include "DXCore.h"
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
#include "Terrain.h"
#include "TerrainMaterial.h"
#include "Water.h"
#include "WaterMaterial.h"
#include "SceneLoader.h"
#include "soloud.h"
#include "soloud_thread.h"
#include "soloud_wav.h"

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

	// Initialization methods
	/*
	void LoadShaders();
	void LoadDefaultMeshes();
	void LoadDefaultTextures();
	void LoadDefaultMaterials();
	*/

	//void BuildDefaultEntity(string entityName, string objName, Entity* e);

	// Scene generation methods
	//Utility::MESH_TYPE AutoLoadOBJMTL(string name);
	//void LoadScene(string sceneName = "scene");

	// Wrappers for DirectX shaders to provide simplified functionality
	/*
	map<string, SimpleVertexShader*> vertexShadersMap;
	map<string, SimplePixelShader*> pixelShadersMap;
	*/

	// Keeps track of the old mouse position for determining how far the mouse moved in a single frame
	POINT prevMousePos;

	//maps representing meshes, materials, and textures currently being used in the scene
	/*
	map<string, bool> utilizedMeshesMap;
	map<string, bool> utilizedMaterialsMap;
	map<string, bool> utilizedTexturesMap;

	map<string, Mesh*> defaultMeshesMap;
	map<string, Mesh*> generatedMeshesMap;

	map<string, ID3D11ShaderResourceView*> defaultTexturesMap;
	map<string, ID3D11ShaderResourceView*> generatedTexturesMap;

	map<string, Material*> defaultMaterialsMap;
	map<string, Material*> generatedMaterialsMap;

	map<string, Entity*> sceneEntitiesMap;
	vector<Entity*> sceneEntities;
	*/

	SceneLoader* sceneLoader;

	Camera* camera;

	ID3D11SamplerState* sampler;

	ID3D11ShaderResourceView* skySRV;
	ID3D11RasterizerState* skyRasterState;
	ID3D11DepthStencilState* skyDepthState;

	Renderer* renderer;

	//terrain example stuff
	Terrain* terrain;
	Water* water;

	//testing
	Light* testLight;

	//audio
	SoLoud::Soloud sLoud;
	SoLoud::Wav testAudio1;
};

