#include "Game.h"
#include "Vertex.h"

using namespace DirectX;

Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,					// The application's handle
		"Small Shooting Arena",	   	// Text for the window's title bar
		1600,						// Width of the window's client area
		900,						// Height of the window's client area
		true)						// Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

Game::~Game()
{
	delete EESceneLoader;

	Config::Sampler->Release();

	skySRV->Release();
	skyDepthState->Release();
	skyRasterState->Release();

	// FMOD
	sound[0]->release(); // For now just release the one sound we have assigned
	backgroundMusic->release();
	sfxGroup->release();
	fmodSystem->close();
	fmodSystem->release();
	//delete terrain;
	//delete water;

	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
	delete solver;
	delete dynamicsWorld;


	//delete EECamera;
	delete EERenderer;

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		delete ScriptManager::scriptFunctions[i];
	}

	//delete barrel;//(Barrel*)
}

void Game::Init()
{
	//dont delete this, its for finding mem leaks
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(211524);
	//_CrtSetBreakAlloc(56580);

	// Physics -----------------

	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new  btCollisionDispatcher(collisionConfiguration);
	broadphase = new  btDbvtBroadphase();
	solver = new  btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -3.0f, 0));

	Config::Device = device;
	Config::Context = context;

	EECamera = new Camera();
	EECamera->UpdateProjectionMatrix(width, height);

	DirectX::CreateDDSTextureFromFile(device, L"../../Assets/Textures/SunnyCubeMap.dds", 0, &skySRV);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &Config::Sampler);

	D3D11_RASTERIZER_DESC skyRD = {};
	skyRD.CullMode = D3D11_CULL_FRONT;
	skyRD.FillMode = D3D11_FILL_SOLID;
	skyRD.DepthClipEnable = true;
	device->CreateRasterizerState(&skyRD, &skyRasterState);

	D3D11_DEPTH_STENCIL_DESC skyDS = {};
	skyDS.DepthEnable = true;
	skyDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	skyDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&skyDS, &skyDepthState);

	EEMemoryAllocator = new MemoryAllocator(Config::MemoryAllocatorSize, Config::MemoryAllocatorAlignment);
	EEMemoryAllocator->CreatePool(Utility::ENTITY_POOL, Config::MemoryAllocatorEntityPoolSize, sizeof(Entity));

	EESceneLoader = new SceneLoader(EEMemoryAllocator, dynamicsWorld);

	cout << sizeof(Entity) << endl;
	cout << sizeof(map<string, Material*>) << endl;

	EESceneLoader->LoadShaders();

	EESceneLoader->LoadDefaultMeshes();
	EESceneLoader->LoadDefaultTextures();
	EESceneLoader->LoadDefaultMaterials();

	EESceneLoader->LoadScene("ArenaV2");
	//EESceneLoader->LoadScene("Tutorial");

	EESceneLoader->sceneEntitiesMap["barrel_1"]->isCollisionStatic = false;
	EESceneLoader->sceneEntitiesMap["barrel_1 (2)"]->isCollisionStatic = false;

	ScriptManager::sceneEntitiesMap = &EESceneLoader->sceneEntitiesMap;
	ScriptManager::sceneEntities = &EESceneLoader->sceneEntities;
	ScriptManager::sceneLoader = EESceneLoader;

	//test area --------------------------
	/*
	CUBE ALWAYS TRIES TO APPLY GRASS MATERIAL BECAUSE OF GROUND NEED TO FIX

	Entity* cube1;
	cube1 = new Entity("cube1", defaultMeshesMap["Cube"]);
	cube1->AddMaterial(defaultMaterialsMap["DEFAULT"]);
	cube1->AddMaterialNameToMesh("DEFAULT");
	cube1->SetPosition(8.0f, 8.0f, 8.0f);
	cube1->SetRotation(DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30));
	cube1->SetScale(1.0f, 2.0f, 1.0f);
	sceneEntitiesMap.insert({ "cube1", cube1 });
	sceneEntities.push_back(cube1);

	Entity* cube2;
	cube2 = new Entity("cube2", defaultMeshesMap["Cube"]);
	cube2->AddMaterial(defaultMaterialsMap["DEFAULT"]);
	cube2->AddMaterialNameToMesh("DEFAULT");
	cube2->SetPosition(1.0f, 1.0f, 1.0f);
	cube2->SetRotation(DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30));
	cube2->SetScale(1.0f, 1.0f, 1.0f);
	sceneEntitiesMap.insert({ "cube2", cube2 });
	sceneEntities.push_back(cube2);

	cube1->AddChildEntity(cube2);

	cube1->CalcWorldMatrix();

	*/


	Entity* sphere1;
	sphere1 = new Entity("sphere1", EESceneLoader->defaultMeshesMap["Sphere"]);
	sphere1->AddMaterial(EESceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere1->AddMaterialNameToMesh("DEFAULT");
	sphere1->SetPosition(8.0f, 8.0f, 8.0f);
	sphere1->SetRotation(DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30));
	sphere1->SetScale(1.0f, 2.0f, 1.0f);
	sphere1->InitRigidBody(dynamicsWorld);
	EESceneLoader->sceneEntitiesMap.insert({ "sphere1", sphere1 });
	EESceneLoader->sceneEntities.push_back(sphere1);

	Entity* sphere2;
	sphere2 = new Entity("sphere2", EESceneLoader->defaultMeshesMap["Sphere"]);
	sphere2->AddMaterial(EESceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere2->AddMaterialNameToMesh("DEFAULT");
	sphere2->SetPosition(2.0f, 2.0f, 2.0f);
	sphere2->SetRotation(0.0f, 0.0f, 0.0f);
	sphere2->SetScale(1.0f, 1.0f, 2.0f);
	sphere2->InitRigidBody(dynamicsWorld);
	EESceneLoader->sceneEntitiesMap.insert({ "sphere2", sphere2 });
	EESceneLoader->sceneEntities.push_back(sphere2);

	sphere1->AddChildEntity(sphere2);

	Entity* sphere3;
	sphere3 = new Entity("sphere3", EESceneLoader->defaultMeshesMap["Sphere"]);
	sphere3->AddMaterial(EESceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere3->AddMaterialNameToMesh("DEFAULT");
	sphere3->SetPosition(2.0f, 2.0f, 0.0f);
	sphere3->SetRotation(0.0f, 0.0f, 90.0f);
	sphere3->SetScale(1.0f, 1.0f, 1.0f);
	sphere3->InitRigidBody(dynamicsWorld);
	EESceneLoader->sceneEntitiesMap.insert({ "sphere3", sphere3 });
	EESceneLoader->sceneEntities.push_back(sphere3);

	sphere2->AddChildEntity(sphere3);

	sphere1->CalcWorldMatrix();

	prevMousePos.x = 0;
	prevMousePos.y = 0;

	Light* dLight = new Light;
	dLight->Type = LIGHT_TYPE_DIR;
	XMFLOAT3 c = XMFLOAT3(1.0f, 244.0f / 255.0f, 214.0f / 255.0f);
	dLight->Color = c;
	XMFLOAT3 d = XMFLOAT3(0.5f, -1.0f, 1.0f);
	dLight->Direction = d;
	dLight->Intensity = 1.f;

	/*testLight = new Light;
	testLight->Type = LIGHT_TYPE_SPOT;
	testLight->Direction = camera->direction;
	testLight->Intensity = 5.f;
	testLight->Position = XMFLOAT3(-3.2f, 2.f, -5.f);
	testLight->Color = XMFLOAT3(1.f, 1.f, 1.f);
	testLight->Range = 10.f;
	testLight->SpotFalloff = 20.f;*/

	EERenderer = new Renderer(device, context, swapChain, backBufferRTV, depthStencilView, width, height);
	EERenderer->AddCamera("main", EECamera);
	EERenderer->EnableCamera("main");
	EERenderer->SetShadowVertexShader(EESceneLoader->vertexShadersMap["Shadow"]);
	EERenderer->SetDebugLineVertexShader(EESceneLoader->vertexShadersMap["DebugLine"]);
	EERenderer->SetDebugLinePixelShader(EESceneLoader->pixelShadersMap["DebugLine"]);
	EERenderer->SetEntities(&(EESceneLoader->sceneEntities));
	EERenderer->AddLight("Sun", dLight);
	//EERenderer->AddLight("testLight", testLight);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["DEFAULT"]);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Normal"]);
	EERenderer->SetShadowMapResolution(4096);
	EERenderer->InitShadows();
	
	Entity* e;
	for (size_t i = 0; i < EESceneLoader->sceneEntities.size(); i++)
	{
		e = EESceneLoader->sceneEntities[i];
		EERenderer->AddRenderObject(e, e->GetMesh(), e->GetMaterial(e->GetMeshMaterialName()));
	}

	ScriptManager::renderer = EERenderer;

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//terrain -----------------
	/*
	terrain = new Terrain(device, "../../Assets/valley.raw16", 513, 513, 1.0f, 50.0f, 1.0f);
	Entity* terrainEntity = new Entity("Terrain", dynamicsWorld, terrain);
	terrainEntity->AddMaterial(EESceneLoader->defaultMaterialsMap["Terrain"]);
	terrainEntity->AddMaterialNameToMesh("Terrain");
	terrainEntity->SetPosition(0.f, -10.f, 0.f);
	terrainEntity->SetRotation(0.f, 0.f, 0.f);
	terrainEntity->SetScale(1.0f, 1.0f, 1.0f);
	EESceneLoader->sceneEntitiesMap.insert({ "Terrain", terrainEntity });
	EESceneLoader->sceneEntities.push_back(terrainEntity);
	terrainEntity->CalcWorldMatrix();

	water = new Water(0.0002f, device, 513, 513, 1.f, 1.f, 1.f, EESceneLoader->pixelShadersMap["Water"]);
	water->SetOffsets(0.2f, 0.1f, 0.1f, 0.2f);
	Entity* waterEntity = new Entity("Water", dynamicsWorld, water->terrain);
	waterEntity->AddMaterial(EESceneLoader->defaultMaterialsMap["Water"]);
	waterEntity->AddMaterialNameToMesh("Water");
	waterEntity->SetPosition(0.f, -3.f, 0.f);
	waterEntity->SetRotation(0.f, 0.f, 0.f);
	waterEntity->SetScale(1.f, 1.f, 1.f);
	EESceneLoader->sceneEntitiesMap.insert({ "Water", waterEntity });
	EESceneLoader->sceneEntities.push_back(waterEntity);
	waterEntity->CalcWorldMatrix();
	*/

	// Audio -----------------

	// Basic set-up for sound

	fmodResult = FMOD::System_Create(&fmodSystem); // Create the Studio System object
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
		exit(-1);
	}

	fmodResult = fmodSystem->init(32, FMOD_INIT_NORMAL, 0); // Initialize FMOD with 32 max channels
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
		exit(-1);
	}

	// Test to see if 3D/2D audio works - EXAMPLE CODE

	fmodResult = fmodSystem->createSound("../../Assets/Audio/CityofDawn.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF | FMOD_LOOP_NORMAL, 0, &backgroundMusic); // Create a 3D/Looping sound with linear roll off
	FmodErrorCheck(fmodResult);

	fmodResult = fmodSystem->createSound("../../Assets/Audio/wow.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound[0]); // Create a non-looping 2D sound in the first slot
	FmodErrorCheck(fmodResult);

	fmodResult = fmodSystem->createChannelGroup("SFX Group", &sfxGroup); // Create a channel group for sound effects
	FmodErrorCheck(fmodResult);

	fmodResult = fmodSystem->getMasterChannelGroup(&masterGroup); // Assign masterGroup as the master channel
	FmodErrorCheck(fmodResult);

	// Add the SFX group as a child of the master group as an example. Technically doesn't need to be done because the master group already controls everything
	fmodResult = masterGroup->addGroup(sfxGroup); 
	FmodErrorCheck(fmodResult);

	fmodResult = fmodSystem->playSound(backgroundMusic, 0, false, &musicChannel); // Start playing the 3D sound
	FmodErrorCheck(fmodResult);

	FMOD_VECTOR pos = { 1.0f, 1.0f, 1.0f };
	FMOD_VECTOR vel = { 0, 0, 0 };

	// Set the 3D values for the channel
	musicChannel->set3DAttributes(&pos, &vel);
	musicChannel->set3DMinMaxDistance(0, 15.0f);
  
	barrel = new TestScript();
	barrel->Setup("barrel_1", EESceneLoader->sceneEntitiesMap["barrel_1"]);

	/*
	Entity* fpsController = new Entity("FPSController");
	fpsController->SetPosition(0.0f, 5.0f, -10.0f);
	EESceneLoader->AddEntity(fpsController);
	ScriptManager* playerScript = new FPSController();
	playerScript->Setup("FPSController", EESceneLoader->sceneEntitiesMap["FPSController"]);
	*/

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		sf->CallInit();
	}

	//cout << sizeof(Entity);
}

void Game::OnResize()
{
	DXCore::OnResize();
	EECamera->UpdateProjectionMatrix(width, height);
}

void Game::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	GarbageCollect();

	// Play the 2D sound only if the channel group is not playing something
	sfxGroup->isPlaying(&isPlaying);
	if (GetAsyncKeyState('P') & 0x8000 && !isPlaying) {
		fmodResult = fmodSystem->playSound(sound[0], sfxGroup, false, 0); // Play the sound using any channel in the sfx group (free channels are used first)
		FmodErrorCheck(fmodResult);
	}

	// Mute/unmute the master group
	if (GetAsyncKeyState('M') & 0x8000)
	{
		bool mute = true;
		masterGroup->getMute(&mute);
		masterGroup->setMute(!mute);
	}

	EECamera->Update();
	//water->Update();
	
	for (size_t i = 0; i < DebugLines::debugLines.size(); i++)
	{
		DebugLines* dbl = DebugLines::debugLines[i];
		if (dbl->willUpdate) {
			dbl->worldMatrix = EESceneLoader->sceneEntitiesMap[dbl->entityName]->GetCollider(dbl->colliderID)->GetWorldMatrix();
		}
	}

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		sf->CallUpdate();
	}

	AudioStep();
	PhysicsStep(deltaTime);

	/*if (!GetAsyncKeyState(VK_CONTROL))
	{
		testLight->Position = camera->position;
		testLight->Direction = camera->direction;
	}*/
}

void Game::PhysicsStep(float deltaTime)
{
	btScalar xRot = 0.0f;
	btScalar yRot = 0.0f;
	btScalar zRot = 0.0f;
	btTransform transform;
	for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);

		transform = body->getWorldTransform();
		XMFLOAT3 pos = EESceneLoader->sceneEntities[i]->GetPosition();
		XMFLOAT4 rot = EESceneLoader->sceneEntities[i]->GetRotationQuaternion();
		//transform.setIdentity();
		transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

		//btQuaternion qx = btQuaternion(btVector3(1.0f, 0.0f, 0.0f), rot.x);
		//btQuaternion qy = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), rot.y);
		//btQuaternion qz = btQuaternion(btVector3(0.0f, 0.0f, 1.0f), rot.z);
		//btQuaternion res = qz * qy * qx;

		btQuaternion res = btQuaternion(rot.x, rot.y, rot.z, rot.w);
		transform.setRotation(res);

		body->getMotionState()->setWorldTransform(transform);

		dynamicsWorld->stepSimulation(deltaTime * 0.5f);

		body->getMotionState()->getWorldTransform(transform);


		/*
		btQuaternion q = transform.getRotation();
		btScalar w = q.getW();
		btScalar x = q.getX();
		btScalar y = q.getY();
		btScalar z = q.getZ();

		double sinr_cosp = 2 * (w * x + y * z);
		double cosr_cosp = 1 - 2 * (x * x + y * y);
		xRot = std::atan2(sinr_cosp, cosr_cosp);

		double sinp = 2 * (w * y - z * x);
		if (std::abs(sinp) >= 1)
			yRot = std::copysign(DirectX::XM_PI / 2, sinp); // use 90 degrees if out of range
		else
			yRot = std::asin(sinp);

		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		zRot = std::atan2(siny_cosp, cosy_cosp);
		*/

		//transform.getRotation().getEulerZYX(zRot, yRot, xRot);
		/*
		if (i == 19) {
			cout << yRot << endl;
		}
		*/
		btQuaternion q = transform.getRotation();
		EESceneLoader->sceneEntities[i]->SetPosition(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
		EESceneLoader->sceneEntities[i]->SetRotation(XMFLOAT4(q.getX(), q.getY(), q.getZ(), q.getW()));
		EESceneLoader->sceneEntities[i]->CalcWorldMatrix();
	}

	//EESceneLoader->sceneEntities[0]->GetRBody()->setLinearVelocity(btVector3(0.0f, EESceneLoader->sceneEntities[0]->GetRBody()->getLinearVelocity().getY(), 0.0f));
}

void Game::AudioStep()
{
	// Set our listener position as the camera's position for now
	listener_pos.x = EECamera->position.x;
	listener_pos.y = EECamera->position.y;
	listener_pos.z = EECamera->position.z;

	// Set the listener forward to the camera's forward
	listener_forward.x = EECamera->direction.x;
	listener_forward.y = EECamera->direction.y;
	listener_forward.z = EECamera->direction.z;

	// Set the listener up to the camera's up
	XMFLOAT3 yAxis = Y_AXIS;
	listener_up.x = yAxis.x;
	listener_up.y = yAxis.y;
	listener_up.z = yAxis.z;

	//printf("Listener forward = x: %f y: %f z: %f \n", listener_forward.x, listener_forward.y, listener_forward.z);

	fmodSystem->set3DListenerAttributes(0, &listener_pos, 0, &listener_forward, &listener_up); // Update 'ears'
	fmodSystem->update();
}

void Game::Draw(float deltaTime, float totalTime)
{
	EERenderer->ClearFrame();

	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["DEFAULT"]);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Normal"]);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Water"]);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Terrain"]);

	EERenderer->RenderShadowMap();

	EERenderer->RenderFrame();

	DrawSky();

	EERenderer->PresentFrame();
}


void Game::DrawSky() {
	ID3D11Buffer* vb = EESceneLoader->defaultMeshesMap["Cube"]->GetVertexBuffer();
	ID3D11Buffer* ib = EESceneLoader->defaultMeshesMap["Cube"]->GetIndexBuffer();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	EESceneLoader->vertexShadersMap["Sky"]->SetMatrix4x4("view", EECamera->GetViewMatrix());
	EESceneLoader->vertexShadersMap["Sky"]->SetMatrix4x4("projection", EECamera->GetProjMatrix());
	EESceneLoader->vertexShadersMap["Sky"]->CopyAllBufferData();
	EESceneLoader->vertexShadersMap["Sky"]->SetShader();

	EESceneLoader->pixelShadersMap["Sky"]->SetShaderResourceView("Sky", skySRV);
	EESceneLoader->pixelShadersMap["Sky"]->SetSamplerState("BasicSampler", Config::Sampler);
	EESceneLoader->pixelShadersMap["Sky"]->SetShader();

	context->RSSetState(skyRasterState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	context->DrawIndexed(EESceneLoader->defaultMeshesMap["Cube"]->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

void Game::GarbageCollect()
{
	int start = EESceneLoader->sceneEntities.size() - 1;
	for (int i = start; i >= 0; i--)
	{
		Entity* e = EESceneLoader->sceneEntities[i];
		if (e->destroyed) {
			string name = e->GetName();
			EESceneLoader->sceneEntitiesMap.erase(name);
			EESceneLoader->sceneEntities.erase(EESceneLoader->sceneEntities.begin() + i);
			delete e;

			vector<ScriptManager*> scriptFuncs = ScriptManager::scriptFunctionsMap[name];
			for (size_t j = scriptFuncs.size() - 1; j >= 0; j--)
			{
				scriptFuncs[j]->destroyed = true;
			}
			ScriptManager::scriptFunctionsMap.erase(name);
		}
	}

	start = ScriptManager::scriptFunctions.size() - 1;
	for (int i = start; i >= 0; i--)
	{
		ScriptManager* s = ScriptManager::scriptFunctions[i];
		if (s->destroyed) {
			ScriptManager::scriptFunctions.erase(ScriptManager::scriptFunctions.begin() + i);
			delete s;
		}
	}
}

#pragma region Mouse Input
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if (!sf->inputEnabled) continue;
		sf->CallOnMouseDown(buttonState, x, y);
	}

	SetCapture(hWnd);
}

void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if (!sf->inputEnabled) continue;
		sf->CallOnMouseUp(buttonState, x, y);
	}

	ReleaseCapture();
}

void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if (!sf->inputEnabled) continue;
		sf->CallOnMouseMove(buttonState, x, y);
	}
	if (buttonState & 0x0001) {
		EECamera->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if (!sf->inputEnabled) continue;
		sf->CallOnMouseWheel(wheelDelta, x, y);
	}
}

void Game::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}
}
#pragma endregion