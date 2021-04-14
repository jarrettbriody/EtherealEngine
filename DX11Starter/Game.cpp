#include "pch.h"
#include "Game.h"
#include "Vertex.h"

using namespace DirectX;

Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,					// The application's handle
		"Ethereal Engine",		 	// Text for the window's title bar
		true)						// Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	CreateConsoleWindow(500, 120, 32, 120);
	if (!Config::ShowCursor) ShowCursor(false);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

Game::~Game()
{
	SceneLoader::DestroyInstance();

	delete cpuEmitter;
	delete gpuEmitter;

	Config::Sampler->Release();

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
	delete Config::DynamicsWorld;
	// delete physicsDraw;

	Keyboard::DestroyInstance();
	Mouse::DestroyInstance();

	//delete EECamera;
	Renderer::DestroyInstance();

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		delete ScriptManager::scriptFunctions[i];
	}

	MemoryAllocator::DestroyInstance();

	DecalHandler::DestroyInstance();
}

void Game::Init()
{
	//dont delete this, its for finding mem leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(193404);
	//_CrtSetBreakAlloc(49892);

	srand(static_cast <unsigned> (time(0)));
	// Input 
	if (Keyboard::SetupInstance())
	{
		keyboard = Keyboard::GetInstance();
	}
	
	if (Mouse::SetupInstance())
	{
		mouse = Mouse::GetInstance();
	}

	// Physics -----------------
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new  btCollisionDispatcher(collisionConfiguration);
	broadphase = new  btDbvtBroadphase();
	solver = new  btSequentialImpulseConstraintSolver;
	Config::DynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	Config::DynamicsWorld->setGravity(btVector3(0, -10.0f, 0));

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Config::Device->CreateSamplerState(&samplerDesc, &Config::Sampler);

	MemoryAllocator::SetupInstance(Config::MemoryAllocatorSize, Config::MemoryAllocatorAlignment);
	EEMemoryAllocator = MemoryAllocator::GetInstance();
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::ENTITY_POOL, Config::MemoryAllocatorEntityPoolSize, sizeof(Entity));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::MESH_POOL, Config::MemoryAllocatorMeshPoolSize, sizeof(Mesh));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::MATERIAL_POOL, Config::MemoryAllocatorMaterialPoolSize, sizeof(Material));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::DECAL_POOL, Config::MemoryAllocatorDecalPoolSize, sizeof(DecalBucket));
	//EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::PARTICLE_POOL, Config::MemoryAllocatorParticlePoolSize, sizeof(Particle));

	EECamera = new Camera();
	EECamera->UpdateProjectionMatrix();

	SceneLoader::SetupInstance();
	EESceneLoader = SceneLoader::GetInstance();

	EESceneLoader->LoadAssetPreloadFile();

	//EESceneLoader->LoadShaders();

	//EESceneLoader->LoadDefaultMeshes();
	//EESceneLoader->LoadDefaultTextures();
	//EESceneLoader->LoadDefaultMaterials();

	EESceneLoader->SetScriptLoader([](Entity* e, string script) {Scripts::CreateScript(e, script); });

	/*
	Light* dLight = new Light;
	dLight->Type = LIGHT_TYPE_DIR;
	XMFLOAT3 c = XMFLOAT3(1.0f, 252.0f / 255.0f, 222.0f / 255.0f);
	dLight->Color = c;
	XMFLOAT3 d = XMFLOAT3(-0.265943f, -0.92075f, 0.28547f);
	dLight->Direction = d;
	dLight->Intensity = 0.25f;
	dLight->Position = XMFLOAT3(-334.0f, 179.5f, -175.9f);
	*/

	/*testLight = new Light;
	testLight->Type = LIGHT_TYPE_SPOT;
	testLight->Direction = camera->direction;
	testLight->Intensity = 5.f;
	testLight->Position = XMFLOAT3(-3.2f, 2.f, -5.f);
	testLight->Color = XMFLOAT3(1.f, 1.f, 1.f);
	testLight->Range = 10.f;
	testLight->SpotFalloff = 20.f;*/

	DecalHandler::SetupInstance();
	EEDecalHandler = DecalHandler::GetInstance();

	Renderer::SetupInstance();
	EERenderer = Renderer::GetInstance();
	EERenderer->AddCamera("main", EECamera);
	EERenderer->EnableCamera("main");

	RendererShaders rShaders;
	rShaders.depthStencilVS = EESceneLoader->vertexShadersMap["DepthStencil"];
	rShaders.depthStencilPS = EESceneLoader->pixelShadersMap["DepthStencil"];
	rShaders.debugLineVS = EESceneLoader->vertexShadersMap["DebugLine"];
	rShaders.debugLinePS = EESceneLoader->pixelShadersMap["DebugLine"];
	rShaders.decalVS = EESceneLoader->vertexShadersMap["Decal"];
	rShaders.decalPS = EESceneLoader->pixelShadersMap["Decal"];
	rShaders.skyVS = EESceneLoader->vertexShadersMap["Sky"];
	rShaders.skyPS = EESceneLoader->pixelShadersMap["Sky"];

	EERenderer->SetRendererShaders(rShaders);
	EERenderer->InitDepthStencil();
	EERenderer->InitHBAOPlus();
	EERenderer->InitBlendState();
	EESceneLoader->EERenderer = EERenderer;

	//EESceneLoader->LoadScene("ArenaV2");

	EESceneLoader->SetModelPath("../../Assets/Models/City/");
	EESceneLoader->LoadScene("City");

	EERenderer->SetShadowMapResolution(4096);
	EERenderer->InitShadows();
	EERenderer->InitSkybox();

	EERenderer->SetEntities(&(EESceneLoader->sceneEntities));
	
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["DEFAULT"]);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Normal"]);
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Decal"]);

	ID3D11ShaderResourceView* decals[8];
	decals[0] = EESceneLoader->defaultTexturesMap["BLOOD1"];
	decals[1] = EESceneLoader->defaultTexturesMap["BLOOD2"];
	decals[2] = EESceneLoader->defaultTexturesMap["BLOOD3"];
	decals[3] = EESceneLoader->defaultTexturesMap["BLOOD4"];
	decals[4] = EESceneLoader->defaultTexturesMap["BLOOD5"];
	decals[5] = EESceneLoader->defaultTexturesMap["BLOOD6"];
	decals[6] = EESceneLoader->defaultTexturesMap["BLOOD7"];
	decals[7] = EESceneLoader->defaultTexturesMap["BLOOD8"];
	EERenderer->SetDecals(decals);

	EERenderer->SetMeshes(EESceneLoader->defaultMeshesMap["Cube"], EESceneLoader->defaultMeshesMap["InverseCube"]);

	DefaultGPUParticleShaders gpuParticleShaders;
	gpuParticleShaders.copyDrawCountCS = EESceneLoader->computeShadersMap["ParticleDrawArgs"];
	gpuParticleShaders.initDeadListCS = EESceneLoader->computeShadersMap["InitDeadList"];
	gpuParticleShaders.particleEmissionCS = EESceneLoader->computeShadersMap["EmitParticle"];
	gpuParticleShaders.particleUpdateCS = EESceneLoader->computeShadersMap["UpdateParticle"];
	gpuParticleShaders.particleVS = EESceneLoader->vertexShadersMap["Particle"];
	gpuParticleShaders.particlePS = EESceneLoader->pixelShadersMap["Particle"];
	GPUParticleEmitter::SetDefaultShaders(gpuParticleShaders);

	DefaultCPUParticleShaders cpuParticleShaders;
	cpuParticleShaders.particleVS = EESceneLoader->vertexShadersMap["CPUParticle"];
	cpuParticleShaders.particlePS = EESceneLoader->pixelShadersMap["CPUParticle"];
	CPUParticleEmitter::SetDefaultShaders(cpuParticleShaders);

	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = XMFLOAT3(-410.543f, 12.0f, -90.21f);
	emitDesc.colorCount = 1;
	ParticleColor partColors[1] = {
		{XMFLOAT4(0.5f,0,0,1.0f),10},
		//{XMFLOAT4(0,1,0,1),10},
		//{XMFLOAT4(0,0,1,1),10},
		//{XMFLOAT4(1,1,0,1),10},
		//{XMFLOAT4(1,1,1,1),10},
		//{XMFLOAT4(0,1,1,1),10},
		//{XMFLOAT4(1,0,1,1),10},
		//{XMFLOAT4(1,0,0,1),10},
	};
	emitDesc.colors = partColors;
	emitDesc.maxParticles = 100;
	emitDesc.emissionRate = 3.0f;
	//emitDesc.emissionRotation = XMFLOAT3(-XM_PIDIV2,0.0f,0.0f);
	emitDesc.emitterDirection = Y_AXIS;
	emitDesc.particleInitMinSpeed = 10.0f;
	emitDesc.particleInitMaxSpeed = 15.0f;
	emitDesc.particleMinLifetime = 10.0f;
	emitDesc.particleMaxLifetime = 15.0f;
	emitDesc.particleInitMinScale = 0.1f;
	emitDesc.particleInitMaxScale = 0.2f;
	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, -20.0f);

	cpuEmitter = new CPUParticleEmitter(emitDesc);

	cpuEmitter->SetCollisionsEnabled([](void* collision) {
		btPersistentManifold* manifold = (btPersistentManifold*)collision;
		btCollisionObject* obA = (btCollisionObject*)(manifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(manifold->getBody1());
	
		PhysicsWrapper* wrapperA = (PhysicsWrapper*)obA->getUserPointer();
		PhysicsWrapper* wrapperB = (PhysicsWrapper*)obB->getUserPointer();
	
		if (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperB->type == PHYSICS_WRAPPER_TYPE::ENTITY || 
			wrapperB->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) {

			ParticlePhysicsWrapper* particleWrapper = (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE) ? (ParticlePhysicsWrapper*)wrapperA->objectPointer : (ParticlePhysicsWrapper*)wrapperB->objectPointer;
			Entity* entity = (wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) ? (Entity*)wrapperA->objectPointer : (Entity*)wrapperB->objectPointer;
	
			if (entity->layer == "decal") {
				Particle* particle = (Particle*)particleWrapper->particle;
				CPUParticleEmitter* emitter = (CPUParticleEmitter*)particleWrapper->emitter;
				ParticleEmitter* emitterBase = (ParticleEmitter*)emitter;

				//if (manifold->getNumContacts() <= 0) return;

				//btVector3 point = manifold->getContactPoint(0).getPositionWorldOnA();
				//XMFLOAT3 hitLocation = XMFLOAT3(point.getX(), point.getY(), point.getZ());

				XMFLOAT3 particleWorld;
				XMVECTOR particlePos = XMLoadFloat3(&particle->position);
				XMMATRIX emitterWorld = XMMatrixTranspose(XMLoadFloat4x4(&emitterBase->GetWorldMatrix()));
				particlePos.m128_f32[3] = 1.0f;
				XMStoreFloat3(&particleWorld, XMVector4Transform(particlePos, emitterWorld));

				XMFLOAT3 normalizedVel;
				XMVECTOR particleVel = XMLoadFloat3(&particle->velocity);
				particleVel.m128_f32[3] = 0.0f;
				XMStoreFloat3(&normalizedVel, XMVector3Normalize(XMVector4Transform(particleVel, emitterWorld)));

				DecalHandler::GetInstance()->GenerateDecal(entity, normalizedVel, particleWorld, XMFLOAT3(10.0f, 10.0f, 15.0f), DecalType(rand() % 8));

				//cout << particleWrapper->particleIndex << endl;

				emitter->KillParticle(particleWrapper->particleIndex);
			}
		}
	});

	emitDesc.maxParticles = 2000;
	emitDesc.particleInitMinScale = 0.25f;
	emitDesc.particleInitMaxScale = 0.3f;
	emitDesc.emissionRate = 100.0f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);

	Entity* e;
	for (size_t i = 0; i < EESceneLoader->sceneEntities.size(); i++)
	{
		e = EESceneLoader->sceneEntities[i];
		if(!e->isEmptyObj)
			EERenderer->AddRenderObject(e, e->GetMesh(), e->GetMaterial(e->GetMeshMaterialName()));
	}

	ScriptManager::EERenderer = EERenderer;

	Config::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Audio -----------------
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

	FMOD_VECTOR pos = { 1.0f, 50.0f, 1.0f };
	FMOD_VECTOR vel = { 0, 0, 0 };

	// Set the 3D values for the channel
	musicChannel->set3DAttributes(&pos, &vel);
	musicChannel->set3DMinMaxDistance(0, 15.0f);


	if(Config::Fullscreen)
		Config::SwapChain->SetFullscreenState(true, NULL);

	//cout << sizeof(Entity);

	// Physics debug lines initialization once all physical bodies are set-up
	// https://pybullet.org/Bullet/BulletFull/classbtIDebugDraw.html\

	if (Config::BulletDebugLinesEnabled)
	{
		DebugLines* physicsDraw = new DebugLines("PhysicsDebugCore", 0, false);
		physicsDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		Config::DynamicsWorld->setDebugDrawer(physicsDraw);
		Config::DynamicsWorld->debugDrawWorld(); // Use this to draw physics world once on start 
	}

	//-------------------------------------------------------
	// Behavior tree tests
	//-------------------------------------------------------

	TestBehavior tb;
	if (tb.initializeCalled == 0)
		cout << "TEST 1 - SUCCESS" << endl;
	else
		cout << "TEST 1 - FAILURE" << endl;
	tb.Tick();
	if (tb.initializeCalled == 1)
		cout << "TEST 2 - SUCCESS" << endl;
	else
		cout << "TEST 2 - FAILURE" << endl;
}

void Game::OnResize()
{
	DXCore::OnResize();
	EECamera->UpdateProjectionMatrix();
	EERenderer->InitDepthStencil();
	EERenderer->InitHBAOPlus();
}

void Game::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_ESCAPE)) {
		Config::SwapChain->SetFullscreenState(false, NULL);
		Quit();
	}

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if(!sf->GetIsInitialized())
			sf->CallInit();
	}

	GarbageCollect();

	PhysicsStep(deltaTime);

	// Play the 2D sound only if the channel group is not playing something
	sfxGroup->isPlaying(&isPlaying);
	//if (GetAsyncKeyState('P') & 0x8000 && !isPlaying) {
		//fmodResult = fmodSystem->playSound(sound[0], sfxGroup, false, 0); // Play the sound using any channel in the sfx group (free channels are used first)
		//FmodErrorCheck(fmodResult);
	//}

	// Mute/unmute the master group
	if (GetAsyncKeyState('M') & 0x8000)
	{
		bool mute = true;
		masterGroup->getMute(&mute);
		masterGroup->setMute(!mute);
	}
	
	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		sf->CallUpdate(deltaTime);
	}

	cpuEmitter->Update(deltaTime, totalTime, EERenderer->GetCamera("main")->GetViewMatrix());
	gpuEmitter->Update(deltaTime, totalTime);

	int numManifolds = Config::DynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = Config::DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		PhysicsWrapper* wrapperA = (PhysicsWrapper*)obA->getUserPointer();
		PhysicsWrapper* wrapperB = (PhysicsWrapper*)obB->getUserPointer();

		if(wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY && wrapperB->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
			Entity* a = (Entity*)wrapperA->objectPointer;
			Entity* b = (Entity*)wrapperB->objectPointer;

			if (ScriptManager::scriptFunctionsMap.count(a->GetName())) {
				vector<ScriptManager*> scripts = ScriptManager::scriptFunctionsMapVector[a->GetName()];
				for (size_t j = 0; j < scripts.size(); j++)
				{
					scripts[j]->CallOnCollision(obB);
				}
			}

			if (ScriptManager::scriptFunctionsMap.count(b->GetName())) {
				vector<ScriptManager*> scripts = ScriptManager::scriptFunctionsMapVector[b->GetName()];
				for (size_t j = 0; j < scripts.size(); j++)
				{
					scripts[j]->CallOnCollision(obB);
				}
			}
		}

		if (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperB->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
			wrapperA->callback(contactManifold);
		}

		if (wrapperB->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
			wrapperB->callback(contactManifold);
		}
	}

	EnforcePhysics();

	for (size_t i = 0; i < DebugLines::debugLines.size(); i++)
	{
		DebugLines* dbl = DebugLines::debugLines[i];
		if (dbl->willUpdate) {
			dbl->worldMatrix = EESceneLoader->sceneEntitiesMap[dbl->entityName]->GetCollider(dbl->colliderID)->GetWorldMatrix();
		}
	}

	EECamera->Update();

	//EEDecalHandler->UpdateDecals();

	AudioStep();

	/*if (!GetAsyncKeyState(VK_CONTROL))
	{
		testLight->Position = camera->position;
		testLight->Direction = camera->direction;
	}*/

}

void Game::PhysicsStep(float deltaTime)
{
	btCollisionObject* obj = nullptr;
	btRigidBody* body = nullptr;
	btTransform transform;
	Entity* entity = nullptr;

	Config::DynamicsWorld->applyGravity();
	Config::DynamicsWorld->stepSimulation(deltaTime * deltaTimeScalar, 10, 1.f / 60.f); // Config::DynamicsWorld->stepSimulation(deltaTime, 1, btScalar(1.0) / btScalar(60.0)); --> don't believe this framerate independent, needed to add max steps variable

	for (int i = 0; i < Config::DynamicsWorld->getNumCollisionObjects(); i++)
	{
		obj = Config::DynamicsWorld->getCollisionObjectArray()[i];

		if (obj->getInternalType() == btCollisionObject::CO_RIGID_BODY) {
			body = btRigidBody::upcast(obj);

			//transform = body->getWorldTransform();
			PhysicsWrapper* wrapper = (PhysicsWrapper*)body->getUserPointer();
			if (wrapper->type != PHYSICS_WRAPPER_TYPE::ENTITY) continue;

			entity = (Entity*)wrapper->objectPointer;

			transform = body->getCenterOfMassTransform();
			btVector3 p = transform.getOrigin();
			//XMFLOAT3 centerLocal = entity->GetCollider()->GetCenterLocal();
			//XMFLOAT3 scale = entity->GetScale();
			//centerLocal = XMFLOAT3(centerLocal.x * scale.x, centerLocal.y * scale.y, centerLocal.z * scale.z);
			//XMFLOAT3 pos = XMFLOAT3(p.getX() - centerLocal.x, p.getY() - centerLocal.y, p.getZ() - centerLocal.z);
			XMFLOAT3 pos = XMFLOAT3(p.getX(), p.getY(), p.getZ());

			btQuaternion q = transform.getRotation();
			entity->SetPosition(pos);
			entity->SetRotation(XMFLOAT4(q.getX(), q.getY(), q.getZ(), q.getW()));
			entity->CalcWorldMatrix();
		}
	}

	//EESceneLoader->sceneEntities[0]->GetRBody()->setLinearVelocity(btVector3(0.0f, EESceneLoader->sceneEntities[0]->GetRBody()->getLinearVelocity().getY(), 0.0f));
}

void Game::EnforcePhysics()
{
	btCollisionObject* obj = nullptr;
	btRigidBody* body = nullptr;
	btGhostObject* ghost = nullptr;
	btTransform transform;
	Entity* entity = nullptr;

	for (int i = 0; i < Config::DynamicsWorld->getNumCollisionObjects(); i++)
	{
		obj = Config::DynamicsWorld->getCollisionObjectArray()[i];

		if (obj->getInternalType() == btCollisionObject::CO_RIGID_BODY) {
			body = btRigidBody::upcast(obj);

			transform = body->getCenterOfMassTransform();

			PhysicsWrapper* wrapper = (PhysicsWrapper*)body->getUserPointer();

			if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
				entity = (Entity*)wrapper->objectPointer;
				entity->CalcWorldMatrix();

				XMFLOAT3 pos = entity->GetPosition();
				//XMFLOAT3 centerLocal = entity->GetCollider()->GetCenterLocal();
				//XMFLOAT3 scale = entity->GetScale();
				//centerLocal = XMFLOAT3(centerLocal.x * scale.x, centerLocal.y * scale.y, centerLocal.z * scale.z);
				//pos = XMFLOAT3(pos.x + centerLocal.x, pos.y + centerLocal.y, pos.z + centerLocal.z);
				pos = XMFLOAT3(pos.x, pos.y, pos.z);

				XMFLOAT4 rot = entity->GetRotationQuaternion();

				btVector3 transformPos = btVector3(pos.x, pos.y, pos.z);
				transform.setOrigin(transformPos);

				btQuaternion res = btQuaternion(rot.x, rot.y, rot.z, rot.w);
				transform.setRotation(res);

				//TODO: ENFORCE LOCAL SCALING OF COLLIDER

				body->setCenterOfMassTransform(transform);

				// body->getMotionState()->setWorldTransform(transform);

				// body->getMotionState()->getWorldTransform(transform);
			}
		}

		else if (obj->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
			ghost = btGhostObject::upcast(obj);

			transform = ghost->getWorldTransform();

			PhysicsWrapper* wrapper = (PhysicsWrapper*)ghost->getUserPointer();

			//if (wrapper->type == PHYSICS_WRAPPER_TYPE::PARTICLE) {
			//	ParticlePhysicsWrapper* particleWrap = (ParticlePhysicsWrapper*)wrapper->objectPointer;
			//	if (particleWrap->particleIndex == 4) {
			//		btVector3 p = transform.getOrigin();
			//		std::cout << p.getX() << " | " << p.getY() << " | " << p.getZ() << std::endl;
			//	}
			//}
		}
	}
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
	EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Decal"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Water"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Terrain"]);

	//EERenderer->SendSSAOKernelToShader(EESceneLoader->pixelShadersMap["DEFAULT_SSAO"]);

	EERenderer->RenderShadowMap();
	EERenderer->RenderDepthStencil();
	EERenderer->RenderFrame();

	EERenderer->RenderSkybox();

	cpuEmitter->Draw(EERenderer->GetCamera("main")->GetViewMatrix(), EERenderer->GetCamera("main")->GetProjMatrix());
	gpuEmitter->Draw(EERenderer->GetCamera("main")->GetViewMatrix(), EERenderer->GetCamera("main")->GetProjMatrix());

	EERenderer->PresentFrame();
}

void Game::GarbageCollect()
{
	size_t start = EESceneLoader->sceneEntities.size();
	for (size_t i = start; i > 0; i--)
	{
		Entity* e = EESceneLoader->sceneEntities[i - 1];
		if (e->destroyed) {
			string name = e->GetName();
			EESceneLoader->sceneEntitiesMap.erase(name);
			EESceneLoader->sceneEntities.erase(EESceneLoader->sceneEntities.begin() + i - 1);

			EEDecalHandler->DestroyDecals(name);

			if (Config::EtherealDebugLinesEnabled) {
				DebugLines::debugLinesMap[name]->destroyed = true;
				DebugLines::debugLinesMap.erase(name);
			}

			e->FreeMemory();
			EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::ENTITY_POOL, e, sizeof(Entity));

			vector<ScriptManager*> scriptFuncs = ScriptManager::scriptFunctionsMapVector[name];
			size_t cnt = scriptFuncs.size();
			for (size_t j = cnt; j > 0; j--)
			{
				scriptFuncs[j - 1]->destroyed = true;
			}
			ScriptManager::scriptFunctionsMap.erase(name);
			ScriptManager::scriptFunctionsMapVector.erase(name);
		}
	}

	start = ScriptManager::scriptFunctions.size();
	for (size_t i = start; i > 0; i--)
	{
		ScriptManager* s = ScriptManager::scriptFunctions[i - 1];
		if (s->destroyed) {
			ScriptManager::scriptFunctions.erase(ScriptManager::scriptFunctions.begin() + i - 1);
			delete s;
		}
	}

	start = DebugLines::debugLines.size();
	for (size_t i = start; i > 0; i--)
	{
		DebugLines* d = DebugLines::debugLines[i - 1];
		if (d->destroyed) {
			DebugLines::debugLines.erase(DebugLines::debugLines.begin() + i - 1);
			delete d;
		}
	}
}

/*
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

	// Create the world matrix for the debug line
	XMFLOAT4X4 wm;
	XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));
	
	// Create the transformation matrices for our raycast
	XMMATRIX proj = XMMatrixTranspose(XMLoadFloat4x4(&(EECamera->GetProjMatrix())));
	XMMATRIX view = XMMatrixTranspose(XMLoadFloat4x4(&(EECamera->GetViewMatrix())));
	XMMATRIX world = XMMatrixTranspose(XMLoadFloat4x4(&wm));

	// Get the unprojected vector of the mouse click position in world space
	XMVECTOR unprojVec = XMVector3Unproject(XMVectorSet(x, y, 1.0f, 1.0f), 0, 0, Config::ViewPortWidth, Config::ViewPortHeight, 0.0f, 1.0f, proj, view, world);
	XMFLOAT3 start = EECamera->position;
	XMFLOAT3 end = XMFLOAT3(XMVectorGetX(unprojVec), XMVectorGetY(unprojVec), XMVectorGetZ(unprojVec));
	//printf("Projected values|- X: %f, Y: %f, Z: %f\n", end.x, end.y, end.z);

	/*
	// Create debug line
	DebugLines* dl = new DebugLines("TestRay", 0, false);
	XMFLOAT3 c = XMFLOAT3(0.0f, 1.0f, 0.0f);
	dl->color = c;
	dl->worldMatrix = wm;

	// Draw the debug line to show the raycast
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
	delete[] rayPoints;
	

	if (Config::DynamicsWorld)
	{
		// Update physics
		Config::DynamicsWorld->updateAabbs();
		Config::DynamicsWorld->computeOverlappingPairs();

		// Redefine our vectors using bullet's silly types
		btVector3 from(start.x, start.y, start.z);
		btVector3 to(end.x, end.y, end.z);

		// Create variable to store the ray hit and set flags
		btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
		closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

		if (closestResult.hasHit())
		{
			// Get the entity associated with the rigid body we hit
			PhysicsWrapper* wrapper = (PhysicsWrapper*)closestResult.m_collisionObject->getUserPointer();
			if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
				Entity* hit = (Entity*)(wrapper->objectPointer);
				printf("Hit: %s\n", hit->GetName().c_str());
				btRigidBody* rigidBody = hit->GetRBody();

				/*
				// In order to update the values associated with the rigid body we need to remove it from the dynamics world first
				Config::DynamicsWorld->removeRigidBody(rigidBody);
				btVector3 inertia(0, 0, 0);
				float mass = 1.0f;
				rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
				rigidBody->setActivationState(DISABLE_DEACTIVATION);
				rigidBody->setMassProps(mass, inertia);
				*/

				// Useful functions for updating an object in motion, but not really needed here
				/*
				rigidBody->setLinearFactor(btVector3(1, 1, 1));
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
				rigidBody->setWorldTransform(transform); 
				
				btVector3 h = closestResult.m_hitPointWorld;
				XMFLOAT3 hitLocation(h.getX(), h.getY(), h.getZ());
				EEDecalHandler->GenerateDecal(hit, XMFLOAT3(hitLocation.x - start.x, hitLocation.y - start.y, hitLocation.z - start.z), hitLocation, XMFLOAT3(10.0f, 10.0f, 15.0f), DecalType(rand() % 8));

				//Config::DynamicsWorld->addRigidBody(rigidBody); // Add the rigid body back into bullet

				if (hit->MeshHasChildren()) {
					EESceneLoader->SplitMeshIntoChildEntities(hit, 0.5f);
				}
			}
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
	if (buttonState & 0x0001 || !Config::ShowCursor) {
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

#pragma endregion
*/

void Game::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}
}