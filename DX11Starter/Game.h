#pragma once

#include "DXCore.h"
#include "pch.h"
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

using namespace std;

//#define BT_EULER_DEFAULT_ZYX = true;

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
	void AudioStep();
	void DrawSky();

	void FmodErrorCheck(FMOD_RESULT result); // Define it here because current file structure wont let me put it in utility
	void GarbageCollect();

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
	MemoryAllocator* EEMemoryAllocator = nullptr;

	ScriptManager* barrel;

	//terrain example stuff
	//Terrain* terrain;
	//Water* water;
	
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

