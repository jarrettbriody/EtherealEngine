#include "pch.h"
#include "Game.h"
#include "Vertex.h"

using namespace DirectX;

void SceneChangeCallback::Call() {
	cancelDraw = true;

	for (size_t i = 0; i < DecalHandler::GetInstance()->decalsVec.size(); i++)
	{
		DecalHandler::GetInstance()->decalsVec[i]->alive = false;
	}
	DecalHandler::GetInstance()->decalsMap.clear();

	for (size_t i = 0; i < ParticleEmitter::EmitterVector.size(); i++)
	{
		ParticleEmitter::EmitterVector[i]->Destroy();
	}
	ParticleEmitter::EntityEmitterMap.clear();

	for (size_t i = 0; i < LightHandler::GetInstance()->lightsVec.size(); i++)
	{
		LightHandler::GetInstance()->lightsVec[i]->alive = false;
	}
	LightHandler::GetInstance()->lightsMap.clear();
	LightHandler::GetInstance()->entityLightMap.clear();
	LightHandler::GetInstance()->dirLight = nullptr;

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager::scriptFunctions[i]->destroyed = true;
	}
	ScriptManager::scriptFunctionsMap.clear();
	ScriptManager::scriptFunctionsMapVector.clear();

	Keyboard::GetInstance()->PurgeBuffers();
	Mouse::GetInstance()->PurgeBuffers();

	//((Game*)game)->GarbageCollect();
}

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
	for (size_t i = 0; i < ParticleEmitter::EmitterVector.size(); i++)
	{
		delete ParticleEmitter::EmitterVector[i];
	}

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		delete ScriptManager::scriptFunctions[i];
	}

	SceneLoader::DestroyInstance();

	Config::Sampler->Release();
	Config::ClampSampler->Release();

	//delete terrain;
	//delete water;

	// delete physicsDraw;

	//delete EECamera;

	DecalHandler::DestroyInstance();

	LightHandler::DestroyInstance();

	NavmeshHandler::DestroyInstance();

	Renderer::DestroyInstance();

	MemoryAllocator::DestroyInstance();

	Keyboard::DestroyInstance();

	Mouse::DestroyInstance();

	// FMOD
	unsigned int count;
	for (count = 0; count < 4; count++)
		Config::MainTheme[count]->release();
	for (count = 0; count < 3; count++)
		Config::CombatTheme[count]->release();
	Config::MasterGroup->release();
	Config::MusicGroup->release();
	Config::FMODSystem->close();
	Config::FMODSystem->release();

	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
	delete solver;
	delete Config::DynamicsWorld;
}

void Game::Init()
{
	//dont delete this, its for finding mem leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(527625);
	//_CrtSetBreakAlloc(49892);

	//gContactBreakingThreshold = 0.1f;

	Config::hWnd = hWnd;
	sceneChangeCallback.game = (void*)this;

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
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	broadphase = new btDbvtBroadphase();
	solver = new  btSequentialImpulseConstraintSolver;
	Config::DynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	Config::DynamicsWorld->setGravity(btVector3(0, -20.0f, 0));

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Config::Device->CreateSamplerState(&samplerDesc, &Config::Sampler);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	Config::Device->CreateSamplerState(&samplerDesc, &Config::ClampSampler);

	MemoryAllocator::SetupInstance(Config::MemoryAllocatorSize, Config::MemoryAllocatorAlignment);
	EEMemoryAllocator = MemoryAllocator::GetInstance();
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::ENTITY_POOL, Config::MemoryAllocatorEntityPoolSize, sizeof(Entity));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::MESH_POOL, Config::MemoryAllocatorMeshPoolSize, sizeof(Mesh));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::MATERIAL_POOL, Config::MemoryAllocatorMaterialPoolSize, sizeof(Material));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::DECAL_POOL, Config::MemoryAllocatorDecalPoolSize, sizeof(DecalBucket));
	EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::LIGHT_POOL, Config::MemoryAllocatorLightPoolSize, sizeof(LightContainer));
	//EEMemoryAllocator->CreatePool((unsigned int)MEMORY_POOL::PARTICLE_POOL, Config::MemoryAllocatorParticlePoolSize, sizeof(Particle));

	LightHandler::SetupInstance();
	EELightHandler = LightHandler::GetInstance();

	EECamera = new Camera();
	EECamera->CalcProjMatrix();

	SceneLoader::SetupInstance();
	EESceneLoader = SceneLoader::GetInstance();

	EESceneLoader->LoadAssetPreloadFile();

	EESceneLoader->SetScriptLoader([](Entity* e, string script) {Scripts::CreateScript(e, script); });

	DecalHandler::SetupInstance();
	EEDecalHandler = DecalHandler::GetInstance();

	sceneLoaderGarbageCallback.EEDecalHandler = EEDecalHandler;
	EESceneLoader->garbageCollectCallback = (SceneLoaderCallback*)(&sceneLoaderGarbageCallback);
	EESceneLoader->sceneChangeCallback = (Utility::Callback*)(&sceneChangeCallback);

	Renderer::SetupInstance();
	EERenderer = Renderer::GetInstance();
	EERenderer->AddCamera("main", EECamera);
	EERenderer->EnableCamera("main");

	RendererShaders rShaders;
	rShaders.depthStencilVS = EESceneLoader->VertexShadersMap["DepthStencil"];
	rShaders.depthStencilPS = EESceneLoader->PixelShadersMap["DepthStencil"];
	rShaders.debugLineVS = EESceneLoader->VertexShadersMap["DebugLine"];
	rShaders.debugLinePS = EESceneLoader->PixelShadersMap["DebugLine"];
	rShaders.decalVS = EESceneLoader->VertexShadersMap["Decal"];
	rShaders.decalPS = EESceneLoader->PixelShadersMap["Decal"];
	rShaders.skyVS = EESceneLoader->VertexShadersMap["Sky"];
	rShaders.skyPS = EESceneLoader->PixelShadersMap["Sky"];

	EERenderer->SetRendererShaders(rShaders);
	EERenderer->InitDepthStencil();
	EERenderer->InitHBAOPlus();
	EERenderer->InitBlendState();
	EERenderer->InitPostProcessRTV();
	EESceneLoader->EERenderer = EERenderer;

	EESceneLoader->SetModelPath("../../Assets/Models/MainMenu/");
	EESceneLoader->LoadScene("MainMenu");

	//EESceneLoader->SetModelPath("../../Assets/Models/Kamchatka/");
	//EESceneLoader->LoadScene("Kamchatka");

	EERenderer->SetShadowCascadeInfo(0, 4096, 0.1f, 2000.0f, 100.0f, 100.0f);
	EERenderer->SetShadowCascadeInfo(1, 4096, 0.1f, 2000.0f, 250.0f, 250.0f);
	EERenderer->SetShadowCascadeInfo(2, 4096, 0.1f, 2000.0f, 750.0f, 750.0f);
	EERenderer->SetShadowCascadeInfo(3, 2048, 0.1f, 2000.0f, 3000.0f, 3000.0f);
	EERenderer->InitShadows(4);
	EERenderer->InitSkybox();

	EERenderer->SetEntities(&(EESceneLoader->SceneEntities));

	ID3D11ShaderResourceView* decals[8];
	decals[0] = EESceneLoader->DefaultTexturesMap["BLOOD1"];
	decals[1] = EESceneLoader->DefaultTexturesMap["BLOOD2"];
	decals[2] = EESceneLoader->DefaultTexturesMap["BLOOD3"];
	decals[3] = EESceneLoader->DefaultTexturesMap["BLOOD4"];
	decals[4] = EESceneLoader->DefaultTexturesMap["BLOOD5"];
	decals[5] = EESceneLoader->DefaultTexturesMap["BLOOD6"];
	decals[6] = EESceneLoader->DefaultTexturesMap["BLOOD7"];
	decals[7] = EESceneLoader->DefaultTexturesMap["BLOOD8"];
	EERenderer->SetDecals(decals);

	EERenderer->SetMeshes(
		EESceneLoader->DefaultMeshesMap["Cube"], 
		EESceneLoader->DefaultMeshesMap["InverseCube"],
		EESceneLoader->DefaultMeshesMap["Sphere"], 
		EESceneLoader->DefaultMeshesMap["Cone"]);

	DefaultGPUParticleShaders gpuParticleShaders;
	gpuParticleShaders.copyDrawCountCS = EESceneLoader->ComputeShadersMap["ParticleDrawArgs"];
	gpuParticleShaders.initDeadListCS = EESceneLoader->ComputeShadersMap["InitDeadList"];
	gpuParticleShaders.particleEmissionCS = EESceneLoader->ComputeShadersMap["EmitParticle"];
	gpuParticleShaders.particleUpdateCS = EESceneLoader->ComputeShadersMap["UpdateParticle"];
	gpuParticleShaders.randomNumsCS = EESceneLoader->ComputeShadersMap["RandomNums"];
	gpuParticleShaders.particleVS = EESceneLoader->VertexShadersMap["Particle"];
	gpuParticleShaders.particlePS = EESceneLoader->PixelShadersMap["Particle"];
	GPUParticleEmitter::SetDefaultShaders(gpuParticleShaders);

	DefaultCPUParticleShaders cpuParticleShaders;
	cpuParticleShaders.particleVS = EESceneLoader->VertexShadersMap["CPUParticle"];
	cpuParticleShaders.particlePS = EESceneLoader->PixelShadersMap["CPUParticle"];
	CPUParticleEmitter::SetDefaultShaders(cpuParticleShaders);

	/*
	ParticleEmitterDescription emitDesc;
	//emitDesc.parentName = "FPSController";
	//emitDesc.parentWorld = EESceneLoader->sceneEntitiesMap["FPSController"]->GetWorldMatrixPtr();
	emitDesc.emitterPosition = XMFLOAT3(-30, 0.1f, 0);
	
	ParticleColor partColors[3] = {
		{XMFLOAT4(1.0f,0,0,0.5f),5.0f},
		{XMFLOAT4(1.0f,0,0,0.5f),5.0f},
		{XMFLOAT4(1.0f,0,0,0.2f),5.0f},
	};
	emitDesc.colors = partColors;
	emitDesc.colorCount = 3;
	emitDesc.maxParticles = 100;
	emitDesc.emissionRate = 0.5f;
	//emitDesc.emissionRotation = XMFLOAT3(-XM_PIDIV2,0.0f,0.0f);
	emitDesc.emitterDirection = Y_AXIS;
	emitDesc.particleInitMinSpeed = 10.0f;
	emitDesc.particleInitMaxSpeed = 15.0f;
	emitDesc.particleMinLifetime = 10.0f;
	emitDesc.particleMaxLifetime = 15.0f;
	//emitDesc.particleInitMinScale = 0.1f;
	//emitDesc.particleInitMaxScale = 0.2f;
	emitDesc.particleInitMinScale = 0.1f;
	emitDesc.particleInitMaxScale = 0.11f;
	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, -20.0f);

	CPUParticleEmitter* cpuEmitter = new CPUParticleEmitter(emitDesc);
	//cpuEmitter->SetBlendingEnabled(true);

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
	
			if (entity->HasLayer("decal")) {
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

	//emitDesc.maxParticles = 30000;
	//emitDesc.emissionRate = 3000.0f;
	//emitDesc.particleInitMinSpeed = 10.0f;
	//emitDesc.particleInitMaxSpeed = 15.0f;
	//emitDesc.particleMinLifetime = 2.0f;
	//emitDesc.particleMaxLifetime = 3.0f;
	//new GPUParticleEmitter(emitDesc);
	*/

	/*
	Entity* e;
	for (size_t i = 0; i < EESceneLoader->SceneEntities.size(); i++)
	{
		e = EESceneLoader->SceneEntities[i];
		if(!e->isEmptyObj)
			EERenderer->AddRenderObject(e, e->GetMesh(), e->GetMaterial(e->GetMeshMaterialName()));
	}
	*/

	ScriptManager::EERenderer = EERenderer;

	NavmeshHandler::SetupInstance();

	Config::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Audio -----------------
	Config::FMODResult = FMOD::System_Create(&Config::FMODSystem); // Create the Studio System object
	if (Config::FMODResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", Config::FMODResult, FMOD_ErrorString(Config::FMODResult));
		exit(-1);
	}

	Config::FMODResult = Config::FMODSystem->init(32, FMOD_INIT_NORMAL, 0); // Initialize FMOD with 32 max channels
	if (Config::FMODResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", Config::FMODResult, FMOD_ErrorString(Config::FMODResult));
		exit(-1);
	}

	// Test to see if 3D/2D audio works - EXAMPLE CODE

	//fmodResult = Config::FMODSystem->createSound("../../Assets/Audio/CityofDawn.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF | FMOD_LOOP_NORMAL, 0, &Config::BackgroundMusic); // Create a 3D/Looping sound with linear roll off
	//FmodErrorCheck(fmodResult);

	//fmodResult = Config::FMODSystem->createSound("../../Assets/Audio/wow.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound[0]); // Create a non-looping 2D sound in the first slot
	//FmodErrorCheck(fmodResult);

	//fmodResult = Config::FMODSystem->createChannelGroup("SFX Group", &sfxGroup); // Create a channel group for sound effects
	//FmodErrorCheck(fmodResult);

	// Add the SFX group as a child of the master group as an example. Technically doesn't need to be done because the master group already controls everything
	//fmodResult = masterGroup->addGroup(sfxGroup);
	//FmodErrorCheck(fmodResult);

	//fmodResult = Config::FMODSystem->playSound(backgroundMusic, 0, false, &musicChannel); // Start playing the 3D sound
	//FmodErrorCheck(fmodResult);

	//FMOD_VECTOR pos = { 1.0f, 50.0f, 1.0f };
	//FMOD_VECTOR vel = { 0, 0, 0 };

	// Set the 3D values for the channel
	//Config::MusicChannel->set3DAttributes(&pos, &vel);
	//Config::MusicChannel->set3DMinMaxDistance(0, 15.0f);

	// Load in our music here for now. We can do some regex stuff later
	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 1/Section 1 (Loop).wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Config::MainTheme[0]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 1/Section 2 (Build Up (Play Once)).wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Config::MainTheme[1]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 1/Section 3 (Action Loop).wav", FMOD_2D | FMOD_LOOP_NORMAL, 0, &Config::MainTheme[2]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 1/Section 4 (Outro).wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Config::MainTheme[3]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 2/Section 1.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Config::CombatTheme[0]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 2/Section 2 (Loop).wav", FMOD_2D | FMOD_LOOP_NORMAL, 0, &Config::CombatTheme[1]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createSound("../../Assets/Audio/Music/Track 2/Section 3 (Outro).wav", FMOD_2D | FMOD_LOOP_OFF, 0, &Config::CombatTheme[2]);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->createChannelGroup("Music Group", &Config::MusicGroup); // Create a channel group for playing music
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->getMasterChannelGroup(&Config::MasterGroup); // Assign MasterGroup as the master channel
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::MasterGroup->addGroup(Config::MusicGroup);
	AudioManager::FMODErrorCheck(Config::FMODResult);


	if (Config::Fullscreen) {
		RECT desktopRect;
		GetClientRect(GetDesktopWindow(), &desktopRect);
		Config::ViewPortWidth = desktopRect.right;
		Config::ViewPortHeight = desktopRect.bottom;
		Config::SwapChain->SetFullscreenState(true, NULL);
	}

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


	for (size_t i = 0; i < EESceneLoader->scriptPairs.size(); i++)
	{
		Scripts::CreateScript(EESceneLoader->scriptPairs[i].e, EESceneLoader->scriptPairs[i].script);
	}
}

void Game::OnResize()
{
	DXCore::OnResize();
	EECamera->CalcProjMatrix();
	EERenderer->InitDepthStencil();
	EERenderer->InitHBAOPlus();
	EERenderer->InitPostProcessRTV();
}

void Game::Update(double deltaTime, double totalTime)
{
	if (sceneChangeCallback.cancelDraw) {
		EERenderer->InitDepthStencil();
		EERenderer->InitHBAOPlus();
		EERenderer->InitPostProcessRTV();
		EERenderer->InitShadows(4);
		sceneChangeCallback.cancelDraw = false;
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		Config::SwapChain->SetFullscreenState(false, NULL);
		Quit();
	}

	EECamera->Update(deltaTime);

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if(!sf->GetIsInitialized())
			sf->CallInit();
	}

	PhysicsStep(deltaTime);

	// Play the 2D sound only if the channel group is not playing something
	/*sfxGroup->isPlaying(&isPlaying);
	if (GetAsyncKeyState('P') & 0x8000 && !isPlaying) {
		//fmodResult = Config::FMODSystem->playSound(sound[0], sfxGroup, false, 0); // Play the sound using any channel in the sfx group (free channels are used first)
		fmodResult = Config::FMODSystem->playSound(Config::BackgroundMusic, sfxGroup, false, &Config::MusicChannel);
		FmodErrorCheck(fmodResult);
	}*/

	// Mute/unmute the master group
	if (GetAsyncKeyState('M') & 0x8000)
	{
		bool mute = true;
		Config::MasterGroup->getMute(&mute);
		Config::MasterGroup->setMute(!mute);
	}

	int numManifolds = Config::DynamicsWorld->getDispatcher()->getNumManifolds();
	//btPersistentManifold** manifolds = Config::DynamicsWorld->getDispatcher()->getInternalManifoldPointer();
	for (int i = numManifolds - 1; i >= 0 ; i--)
	{
		btPersistentManifold* contactManifold = Config::DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);//manifolds[i];//
		//if (contactManifold->getNumContacts() == 0) continue;
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
					scripts[j]->CallOnCollision(obA);
				}
			}
		}

		if (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperB->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
			wrapperA->callback(contactManifold);
		}

		if (wrapperB->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
			wrapperB->callback(contactManifold);
		}

		//Config::DynamicsWorld->getDispatcher()->releaseManifold(contactManifold);
	}

	for (size_t i = 0; i < ScriptManager::scriptFunctions.size(); i++)
	{
		ScriptManager* sf = ScriptManager::scriptFunctions[i];
		if (!sf->destroyed) sf->CallUpdate();
	}

	XMFLOAT4X4 view = EERenderer->GetCamera("main")->GetViewMatrix();
	for (size_t i = 0; i < ParticleEmitter::EmitterVector.size(); i++)
	{
		if (ParticleEmitter::EmitterVector[i]->GetIsAlive()) ParticleEmitter::EmitterVector[i]->Update(deltaTime, totalTime, view);
	}

	ScriptManager::deltaTime = deltaTime;

	EnforcePhysics();

	for (size_t i = 0; i < DebugLines::debugLines.size(); i++)
	{
		DebugLines* dbl = DebugLines::debugLines[i];
		if (dbl->willUpdate) {
			dbl->worldMatrix = EESceneLoader->SceneEntitiesMap[dbl->entityName]->GetCollider(dbl->colliderID)->GetTransform().GetWorldMatrix();
		}
	}

	EECamera->Update(deltaTime);

	//EEDecalHandler->UpdateDecals();

	EELightHandler->Update(EECamera->GetViewMatrix());

	AudioStep();

	/*if (!GetAsyncKeyState(VK_CONTROL))
	{
		testLight->Position = camera->position;
		testLight->Direction = camera->direction;
	}*/

	GarbageCollect();
}

void Game::PhysicsStep(double deltaTime)
{
	btCollisionObject* obj = nullptr;
	btRigidBody* body = nullptr;
	btTransform transform;
	Entity* entity = nullptr;

	Config::DynamicsWorld->applyGravity();
	Config::DynamicsWorld->stepSimulation((deltaTime * deltaTimeScalar), 10, 1.f / 60.f); // Config::DynamicsWorld->stepSimulation(deltaTime, 1, btScalar(1.0) / btScalar(60.0)); --> don't believe this framerate independent, needed to add max steps variable

	int numCollisionObjects = Config::DynamicsWorld->getNumCollisionObjects();
	for (int i = numCollisionObjects - 1; i >= 0; i--)
	{
		obj = Config::DynamicsWorld->getCollisionObjectArray()[i];

		//if (!obj->isActive()) return;

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
			entity->GetTransform().SetPosition(pos);
			entity->GetTransform().SetRotationQuaternion(XMFLOAT4(q.getX(), q.getY(), q.getZ(), q.getW()));
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

	int numCollisionObjects = Config::DynamicsWorld->getNumCollisionObjects();
	for (int i = numCollisionObjects - 1; i >= 0; i--)
	{
		obj = Config::DynamicsWorld->getCollisionObjectArray()[i];

		//if (!obj->isActive()) return;

		if (obj->getInternalType() == btCollisionObject::CO_RIGID_BODY) {
			body = btRigidBody::upcast(obj);

			transform = body->getCenterOfMassTransform();

			PhysicsWrapper* wrapper = (PhysicsWrapper*)body->getUserPointer();

			if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
				entity = (Entity*)wrapper->objectPointer;

				//if (entity->HasTag("Body Part")) {
				//	cout << "here" << endl;
				//}

				XMFLOAT3 pos = entity->GetTransform().GetPosition();
				//XMFLOAT3 centerLocal = entity->GetCollider()->GetCenterLocal();
				//XMFLOAT3 scale = entity->GetScale();
				//centerLocal = XMFLOAT3(centerLocal.x * scale.x, centerLocal.y * scale.y, centerLocal.z * scale.z);
				//pos = XMFLOAT3(pos.x + centerLocal.x, pos.y + centerLocal.y, pos.z + centerLocal.z);
				pos = XMFLOAT3(pos.x, pos.y, pos.z);

				XMFLOAT4 rot = entity->GetTransform().GetRotationQuaternion();

				btVector3 transformPos = btVector3(pos.x, pos.y, pos.z);
				transform.setOrigin(transformPos);

				btQuaternion res = btQuaternion(rot.x, rot.y, rot.z, rot.w);
				transform.setRotation(res);

				XMFLOAT3 scale = entity->GetTransform().GetScale();
				btVector3 scl = Utility::Float3ToBulletVector(scale);
				body->getCollisionShape()->setLocalScaling(scl);

				body->setCenterOfMassTransform(transform);

				//body->setWorldTransform(transform);

				// body->getMotionState()->getWorldTransform(transform);
			}
		}

		else if (obj->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {

		}
	}
}

void Game::AudioStep()
{
	// Set our listener position as the camera's position for now
	listener_pos.x = EECamera->GetTransform().GetPosition().x;
	listener_pos.y = EECamera->GetTransform().GetPosition().y;
	listener_pos.z = EECamera->GetTransform().GetPosition().z;

	// Set the listener forward to the camera's forward
	listener_forward.x = EECamera->GetTransform().GetDirectionVector().x;
	listener_forward.y = EECamera->GetTransform().GetDirectionVector().y;
	listener_forward.z = EECamera->GetTransform().GetDirectionVector().z;

	// Set the listener up to the camera's up
	XMFLOAT3 yAxis = Y_AXIS;
	listener_up.x = yAxis.x;
	listener_up.y = yAxis.y;
	listener_up.z = yAxis.z;

	//printf("Listener forward = x: %f y: %f z: %f \n", listener_forward.x, listener_forward.y, listener_forward.z);

	Config::FMODSystem->set3DListenerAttributes(0, &listener_pos, 0, &listener_forward, &listener_up); // Update 'ears'
	Config::FMODSystem->update();
}

void Game::Draw(double deltaTime, double totalTime)
{
	if (sceneChangeCallback.cancelDraw) return;

	EERenderer->ClearFrame();

	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["DEFAULT"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Normal"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Decal"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Fluid"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Water"]);
	//EERenderer->SendAllLightsToShader(EESceneLoader->pixelShadersMap["Terrain"]);

	//EERenderer->SendSSAOKernelToShader(EESceneLoader->pixelShadersMap["DEFAULT_SSAO"]);

	EERenderer->RenderShadowMap();
	EERenderer->RenderDepthStencil();
	EERenderer->RenderFrame();
	EERenderer->RenderDecals();
	EERenderer->RenderHBAOPlus();
	EERenderer->RenderDebugLines();
	EERenderer->RenderSkybox();

	XMFLOAT4X4 view = EERenderer->GetCamera("main")->GetViewMatrix();
	XMFLOAT4X4 proj = EERenderer->GetCamera("main")->GetProjMatrix();
	for (size_t i = 0; i < ParticleEmitter::EmitterVector.size(); i++)
	{
		ParticleEmitter::EmitterVector[i]->Draw(view, proj);
	}

	EERenderer->RenderTransparents();

	EERenderer->RenderPostProcess();

	EERenderer->RenderUI();

	EERenderer->PresentFrame();
}

void Game::GarbageCollect()
{
	int start = ParticleEmitter::EmitterVector.size();
	for (int i = start - 1; i >= 0; i--)
	{
		ParticleEmitter* e = ParticleEmitter::EmitterVector[i];
		if (!e->GetIsAlive()) {
			string parentName = e->GetParentName();
			string name = e->GetName();
			if (parentName != "") {
				if (ParticleEmitter::EntityEmitterMap.count(parentName)) {
					if (ParticleEmitter::EntityEmitterMap[parentName].count(name)) {
						ParticleEmitter::EntityEmitterMap[parentName].erase(name);
					}
				}
			}
			ParticleEmitter::EmitterVector.erase(ParticleEmitter::EmitterVector.begin() + i);
			ParticleEmitter::EmitterMap.erase(e->GetName());
			delete e;
		}
	}

	EELightHandler->GarbageCollect();
	EEDecalHandler->GarbageCollect();

	start = DebugLines::debugLines.size();
	for (size_t i = start; i > 0; i--)
	{
		DebugLines* d = DebugLines::debugLines[i - 1];
		if (d->destroyed) {
			DebugLines::debugLines.erase(DebugLines::debugLines.begin() + i - 1);
			delete d;
		}
	}

	EESceneLoader->GarbageCollect();

	start = ScriptManager::scriptFunctions.size();
	for (int i = start - 1; i >= 0; i--)
	{
		ScriptManager* s = ScriptManager::scriptFunctions[i];
		if (s->destroyed) {
			ScriptManager::scriptFunctions.erase(ScriptManager::scriptFunctions.begin() + i);
			delete s;
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