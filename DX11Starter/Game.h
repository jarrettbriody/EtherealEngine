#pragma once

#include "pch.h"
#include "DXCore.h"
#include "Config.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "Renderer.h"
#include "Utility.h"
//#include "Terrain.h"
//#include "TerrainMaterial.h"
//#include "Water.h"
//#include "WaterMaterial.h"
#include "SceneLoader.h"
#include "DebugLines.h"
#include "ScriptManager.h"
#include "Scripts.h"
#include "DebugLines.h"
#include "DecalHandler.h"

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

	void PhysicsStep(float deltaTime);
	void EnforcePhysics();
	void AudioStep();
	void DrawSky();

	void FmodErrorCheck(FMOD_RESULT result); // Define it here because current file structure wont let me put it in utility
	void GarbageCollect();

private:
	// Keeps track of the old mouse position for determining how far the mouse moved in a single frame
	ID3D11ShaderResourceView* skySRV = nullptr;
	ID3D11RasterizerState* skyRasterState = nullptr;
	ID3D11DepthStencilState* skyDepthState = nullptr;

	Camera* EECamera = nullptr;
	Renderer* EERenderer = nullptr;
	SceneLoader* EESceneLoader = nullptr;
	MemoryAllocator* EEMemoryAllocator = nullptr;
	DecalHandler* EEDecalHandler = nullptr;
	
	//testing
	Light* testLight = nullptr;

	// Physics
	btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadphase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;

	// DebugLines* physicsDraw;
	
	// Audio
	FMOD_RESULT fmodResult = FMOD_RESULT();
	FMOD::System* fmodSystem = nullptr;

	FMOD::Sound* backgroundMusic = nullptr;
	FMOD::Sound* sound[12];

	FMOD::Channel* musicChannel = 0;
	FMOD::Channel* channel[12];
	FMOD::ChannelGroup* masterGroup = nullptr;
	FMOD::ChannelGroup* sfxGroup = nullptr;

	bool isPlaying = 0;

	FMOD_VECTOR listener_pos = FMOD_VECTOR();
	//FMOD_VECTOR listener_vel; // If we want a doppler effect
	FMOD_VECTOR listener_forward = FMOD_VECTOR();
	FMOD_VECTOR listener_up = FMOD_VECTOR();
};
