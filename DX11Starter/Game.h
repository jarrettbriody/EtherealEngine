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
#include "fmod.hpp"
#include "fmod_errors.h"

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

	void PhysicsStep();
	void AudioStep();
	void DrawSky();

	void FmodErrorCheck(FMOD_RESULT result); // Define it here because current file structure wont let me put it in utility

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

	// Physics
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	// Audio
	FMOD_RESULT fmodResult;
	FMOD::System* fmodSystem = NULL;

	FMOD::Sound* backgroundMusic;
	FMOD::Sound* sound[12];

	FMOD::Channel* musicChannel = 0;
	FMOD::Channel* channel[12];
	FMOD::ChannelGroup* masterGroup, * sfxGroup;

	bool isPlaying = 0;

	FMOD_VECTOR listener_pos;
	//FMOD_VECTOR listener_vel; // If we want a doppler effect
	FMOD_VECTOR listener_forward;
	FMOD_VECTOR listener_up;
};

