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

	EESceneLoader = new SceneLoader(dynamicsWorld);

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
	sphere1->InitRigidBody(dynamicsWorld, 0.0f);
	EESceneLoader->sceneEntitiesMap.insert({ "sphere1", sphere1 });
	EESceneLoader->sceneEntities.push_back(sphere1);

	Entity* sphere2;
	sphere2 = new Entity("sphere2", EESceneLoader->defaultMeshesMap["Sphere"]);
	sphere2->AddMaterial(EESceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere2->AddMaterialNameToMesh("DEFAULT");
	sphere2->SetPosition(2.0f, 2.0f, 2.0f);
	sphere2->SetRotation(0.0f, 0.0f, 0.0f);
	sphere2->SetScale(1.0f, 1.0f, 2.0f);
	sphere2->InitRigidBody(dynamicsWorld, 0.0f);
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
	sphere3->InitRigidBody(dynamicsWorld, 0.0f);
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

	// FPS CONTROLLER
	Entity* fpsController = new Entity("FPSController");
	fpsController->SetPosition(XMFLOAT3(0, 5, 5));
	fpsController->SetScale(5.0f, 10.0f, 5.0f);
	fpsController->InitRigidBody(dynamicsWorld, 1.0f);
	// EESceneLoader->AddEntity(fpsController);
	EESceneLoader->sceneEntitiesMap.insert({ "FPSController", fpsController });
	EESceneLoader->sceneEntities.push_back(fpsController);

	/*EESceneLoader->sceneEntitiesMap["FPSController"]->collisionsEnabled = true;
	EESceneLoader->sceneEntitiesMap["FPSController"]->colliderDebugLinesEnabled = true;
	EESceneLoader->sceneEntitiesMap["FPSController"]->CheckSATCollisionAndCorrect();
	EESceneLoader->sceneEntitiesMap["FPSController"]->isCollisionStatic = false;*/

	//Entity* camera = new Entity("Camera");
	//camera->SetPosition(XMFLOAT3(0, 7, 5));
	//camera->InitRigidBody(dynamicsWorld, 0.0f);
	//// EESceneLoader->AddEntity(camera);
	//EESceneLoader->sceneEntitiesMap.insert({ "Camera", camera });
	//EESceneLoader->sceneEntities.push_back(camera);

	//fpsController->AddChildEntity(camera);

	playerScript = new FPSController();
	playerScript->Setup("FPSController", EESceneLoader->sceneEntitiesMap["FPSController"]);

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		sf->CallInit();
	}
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

	PhysicsStep(deltaTime);
	
	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		sf->CallUpdate();
	}

	AudioStep();

	/*if (!GetAsyncKeyState(VK_CONTROL))
	{
		testLight->Position = camera->position;
		testLight->Direction = camera->direction;
	}*/
}

void Game::PhysicsStep(float deltaTime)
{
	// btTransform transform;
	for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);

		btTransform transform = body->getWorldTransform();
		Entity* entity = (Entity*)body->getUserPointer();
		XMFLOAT3 pos = entity->GetPosition();
		XMFLOAT4 rot = entity->GetRotationQuaternion();
		
		btVector3 transformPos = btVector3(pos.x, pos.y, pos.z);
		transform.setOrigin(transformPos);

		btQuaternion res = btQuaternion(rot.x, rot.y, rot.z, rot.w);
		transform.setRotation(res);
		
		body->setCenterOfMassTransform(transform);

		// body->getMotionState()->setWorldTransform(transform);

		dynamicsWorld->applyGravity();
		dynamicsWorld->stepSimulation(deltaTime);

		// body->getMotionState()->getWorldTransform(transform);

		transform = body->getCenterOfMassTransform();

		btQuaternion q = transform.getRotation();
		entity->SetPosition(body->getCenterOfMassPosition().getX(), body->getCenterOfMassPosition().getY(), body->getCenterOfMassPosition().getZ());
		entity->SetRotation(XMFLOAT4(q.getX(), q.getY(), q.getZ(), q.getW()));
		entity->CalcWorldMatrix();
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

	

	// printf("Mouse Pos: %d, %d\n", x, y);

	// Create debug line
	DebugLines* dl = new DebugLines("TestRay", 0, false);
	XMFLOAT3 c = XMFLOAT3(0.0f, 1.0f, 0.0f);
	dl->color = c;

	// Create the world matrix for the debug line
	XMFLOAT4X4 wm;
	XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));
	dl->worldMatrix = wm;

	// Create the transformation matrices for our raycast
	XMMATRIX proj = XMMatrixTranspose(XMLoadFloat4x4(&(EECamera->GetProjMatrix())));
	XMMATRIX view = XMMatrixTranspose(XMLoadFloat4x4(&(EECamera->GetViewMatrix())));
	XMMATRIX world = XMMatrixTranspose(XMLoadFloat4x4(&wm));

	// Get the unprojected vector of the mouse click position in world space
	XMVECTOR unprojVec = XMVector3Unproject(XMVectorSet(x, y, 1.0f, 1.0f), 0, 0, 1600, 900, 0.0f, 1.0f, proj, view, world);
	XMFLOAT3 end = XMFLOAT3(XMVectorGetX(unprojVec), XMVectorGetY(unprojVec), XMVectorGetZ(unprojVec));
	//printf("Projected values|- X: %f, Y: %f, Z: %f\n", end.x, end.y, end.z);

	// Draw the debug line to show the raycast
	XMFLOAT3 start = EECamera->position;
	XMFLOAT3* rayPoints = new XMFLOAT3[8];
	rayPoints[0] = start;
	rayPoints[1] = start;
	rayPoints[2] = start;
	rayPoints[3] = start;
	rayPoints[4] = end;
	rayPoints[5] = end;
	rayPoints[6] = end;
	rayPoints[7] = end;
	dl->GenerateCuboidVertexBuffer(rayPoints, 8);

	if (dynamicsWorld)
	{
		// Update physics
		dynamicsWorld->updateAabbs();
		dynamicsWorld->computeOverlappingPairs();

		// Redefine our vectors using bullet's silly types
		btVector3 from(start.x, start.y, start.z);
		btVector3 to(end.x, end.y, end.z);

		// Create variable to store the ray hit and set flags
		btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
		closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		dynamicsWorld->rayTest(from, to, closestResult); // Raycast

		if (closestResult.hasHit())
		{
			// Get the entity associated with the rigid body we hit
			Entity* hit = (Entity*)(closestResult.m_collisionObject->getUserPointer());
			//printf("Hit: %s\n", hit->GetName().c_str());
			btRigidBody* rigidBody = hit->GetRBody();

			// In order to update the values associated with the rigid body we need to remove it from the dynamics world first
			dynamicsWorld->removeRigidBody(rigidBody);
			btVector3 inertia(0, 0, 0);
			float mass = 1.0f;
			rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);
			rigidBody->setMassProps(mass, inertia);

			// Useful functions for updating an object in motion, but not really needed here
			/*rigidBody->setLinearFactor(btVector3(1, 1, 1));
			rigidBody->setAngularFactor(btVector3(1, 1, 1));
			rigidBody->updateInertiaTensor();
			rigidBody->clearForces();
			btTransform transform;
			transform.setIdentity();
			rigidBody->getMotionState()->getWorldTransform(transform);
			float x = transform.getOrigin().getX();
			float y = transform.getOrigin().getY();
			float z = transform.getOrigin().getZ();
			transform.setOrigin(btVector3(x, y, z));
			rigidBody->getCollisionShape()->setLocalScaling(btVector3(1, 1, 1));
			rigidBody->setWorldTransform(transform);*/

			dynamicsWorld->addRigidBody(rigidBody); // Add the rigid body back into bullet		
		}
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