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
	delete sceneLoader;

	sampler->Release();

	skySRV->Release();
	skyDepthState->Release();
	skyRasterState->Release();

	sound3D->release();
	sound2D->release();
	fmodSystem->close();
	fmodSystem->release();

	delete terrain;
	delete water;

	delete camera;
	delete renderer;
}

void Game::Init()
{
	//EtherealEngine::GetInstance()->SetDevice(device);
	//EtherealEngine::GetInstance()->SetContext(context);
	DebugLines::device = device;

	camera = new Camera();
	camera->UpdateProjectionMatrix(width, height);
	//EtherealEngine::GetInstance()->SetCamera(camera);

	DirectX::CreateDDSTextureFromFile(device, L"../../Assets/Textures/SunnyCubeMap.dds", 0, &skySRV);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);

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

	sceneLoader = new SceneLoader(device, context, sampler);

	sceneLoader->LoadShaders();

	sceneLoader->LoadDefaultMeshes();
	sceneLoader->LoadDefaultTextures();
	sceneLoader->LoadDefaultMaterials();

	sceneLoader->LoadScene("ArenaV2");

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
	sphere1 = new Entity("sphere1", sceneLoader->defaultMeshesMap["Sphere"]);
	sphere1->AddMaterial(sceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere1->AddMaterialNameToMesh("DEFAULT");
	sphere1->SetPosition(8.0f, 8.0f, 8.0f);
	sphere1->SetRotation(DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(30));
	sphere1->SetScale(1.0f, 2.0f, 1.0f);
	sceneLoader->sceneEntitiesMap.insert({ "sphere1", sphere1 });
	sceneLoader->sceneEntities.push_back(sphere1);

	Entity* sphere2;
	sphere2 = new Entity("sphere2", sceneLoader->defaultMeshesMap["Sphere"]);
	sphere2->AddMaterial(sceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere2->AddMaterialNameToMesh("DEFAULT");
	sphere2->SetPosition(2.0f, 2.0f, 2.0f);
	sphere2->SetRotation(0.0f, 0.0f, 0.0f);
	sphere2->SetScale(1.0f, 1.0f, 2.0f);
	sceneLoader->sceneEntitiesMap.insert({ "sphere2", sphere2 });
	sceneLoader->sceneEntities.push_back(sphere2);

	sphere1->AddChildEntity(sphere2);

	Entity* sphere3;
	sphere3 = new Entity("sphere3", sceneLoader->defaultMeshesMap["Sphere"]);
	sphere3->AddMaterial(sceneLoader->defaultMaterialsMap["DEFAULT"]);
	sphere3->AddMaterialNameToMesh("DEFAULT");
	sphere3->SetPosition(2.0f, 2.0f, 0.0f);
	sphere3->SetRotation(0.0f, 0.0f, 90.0f);
	sphere3->SetScale(1.0f, 1.0f, 1.0f);
	sceneLoader->sceneEntitiesMap.insert({ "sphere3", sphere3 });
	sceneLoader->sceneEntities.push_back(sphere3);

	sphere2->AddChildEntity(sphere3);

	sphere1->CalcWorldMatrix();

	prevMousePos.x = 0;
	prevMousePos.y = 0;

	Light* dLight = new Light;
	dLight->Type = LIGHT_TYPE_DIR;
	dLight->Color = XMFLOAT3(1.0f, 244.0f / 255.0f, 214.0f / 255.0f);
	dLight->Direction = XMFLOAT3(0.5f, -1.0f, 1.0f);
	dLight->Intensity = 1.f;

	/*testLight = new Light;
	testLight->Type = LIGHT_TYPE_SPOT;
	testLight->Direction = camera->direction;
	testLight->Intensity = 5.f;
	testLight->Position = XMFLOAT3(-3.2f, 2.f, -5.f);
	testLight->Color = XMFLOAT3(1.f, 1.f, 1.f);
	testLight->Range = 10.f;
	testLight->SpotFalloff = 20.f;*/

	renderer = new Renderer(device, context, swapChain, backBufferRTV, depthStencilView, width, height);
	renderer->SetCamera(camera);
	renderer->SetShadowVertexShader(sceneLoader->vertexShadersMap["Shadow"]);
	renderer->SetDebugLineVertexShader(sceneLoader->vertexShadersMap["DebugLine"]);
	renderer->SetDebugLinePixelShader(sceneLoader->pixelShadersMap["DebugLine"]);
	renderer->SetEntities(&(sceneLoader->sceneEntities));
	renderer->AddLight("Sun", dLight);
	//renderer->AddLight("testLight", testLight);
	renderer->SendAllLightsToShader(sceneLoader->pixelShadersMap["DEFAULT"]);
	renderer->SendAllLightsToShader(sceneLoader->pixelShadersMap["Normal"]);
	renderer->SetShadowMapResolution(4096);
	renderer->InitShadows();
	//EtherealEngine::GetInstance()->SetRenderer(renderer);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//terrain -----------------

	terrain = new Terrain(device, "../../Assets/valley.raw16", 513, 513, 1.0f, 50.0f, 1.0f);
	Entity* terrainEntity = new Entity("Terrain", terrain);
	terrainEntity->AddMaterial(sceneLoader->defaultMaterialsMap["Terrain"]);
	terrainEntity->AddMaterialNameToMesh("Terrain");
	terrainEntity->SetPosition(0.f, -10.f, 0.f);
	terrainEntity->SetRotation(0.f, 0.f, 0.f);
	terrainEntity->SetScale(1.0f, 1.0f, 1.0f);
	sceneLoader->sceneEntitiesMap.insert({ "Terrain", terrainEntity });
	sceneLoader->sceneEntities.push_back(terrainEntity);
	terrainEntity->CalcWorldMatrix();

	water = new Water(0.0002f, device, 513, 513, 1.f, 1.f, 1.f, sceneLoader->pixelShadersMap["Water"]);
	water->SetOffsets(0.2f, 0.1f, 0.1f, 0.2f);
	Entity* waterEntity = new Entity("Water", water->terrain);
	waterEntity->AddMaterial(sceneLoader->defaultMaterialsMap["Water"]);
	waterEntity->AddMaterialNameToMesh("Water");
	waterEntity->SetPosition(0.f, -3.f, 0.f);
	waterEntity->SetRotation(0.f, 0.f, 0.f);
	waterEntity->SetScale(1.f, 1.f, 1.f);
	sceneLoader->sceneEntitiesMap.insert({ "Water", waterEntity });
	sceneLoader->sceneEntities.push_back(waterEntity);
	waterEntity->CalcWorldMatrix();

	// Audio -----------------

	// Basic set-up for sound. Audio and error functionality will probably be moved to their own class, but it is kept inline until then

	fmodResult = FMOD::System_Create(&fmodSystem); // Create the Studio System object
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
		exit(-1);
	}

	fmodResult = fmodSystem->init(512, FMOD_INIT_NORMAL, 0); // Initialize FMOD
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
		exit(-1);
	}

	// Test to see if 3D/2D audio works

	fmodResult = fmodSystem->createSound("../../Assets/Audio/CityofDawn.wav", FMOD_3D | FMOD_LOOP_NORMAL, 0, &sound3D); // Create a 3D/Looping sound
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
	}

	fmodResult = fmodSystem->createSound("../../Assets/Audio/wow.wav", FMOD_2D | FMOD_LOOP_OFF, 0, &sound2D); // Create a non-looping 2D sound
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
	}

	fmodResult = fmodSystem->playSound(sound3D, 0, false, &channel1); // Start playing the 3D sound
	if (fmodResult != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
	}

	//channel->set3DAttributes(0, 0); // If you want to change the position/velocity
	//channel->setPaused(false);
	//FMOD_DEFAULT | FMOD_LOOP_OFF
}

void Game::OnResize()
{
	DXCore::OnResize();
	camera->UpdateProjectionMatrix(width, height);
}

void Game::Update(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	if (GetAsyncKeyState('F') & 0x8000) {
		DirectX::XMFLOAT3 rot = sceneLoader->sceneEntitiesMap["sphere1"]->GetRotation();
		rot.y += DirectX::XMConvertToRadians(2.0f);
		sceneLoader->sceneEntitiesMap["sphere1"]->SetRotation(rot.x,rot.y,rot.z);
		sceneLoader->sceneEntitiesMap["sphere1"]->CalcWorldMatrix();
	}

	if (GetAsyncKeyState('G') & 0x8000) {
		DirectX::XMFLOAT3 rot = sceneLoader->sceneEntitiesMap["sphere1"]->GetRotation();
		rot.y -= DirectX::XMConvertToRadians(2.0f);
		sceneLoader->sceneEntitiesMap["sphere1"]->SetRotation(rot.x, rot.y, rot.z);
		sceneLoader->sceneEntitiesMap["sphere1"]->CalcWorldMatrix();
	}

	// Play the 2D sound only if the channel is not playing something
	if (GetAsyncKeyState('M') & 0x8000 && channel2->isPlaying(&isPlaying) == FMOD_ERR_INVALID_HANDLE) {
		fmodResult = fmodSystem->playSound(sound2D, 0, false, &channel2);
		if (fmodResult != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", fmodResult, FMOD_ErrorString(fmodResult));
		}
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		DirectX::XMFLOAT3 trans = sceneLoader->sceneEntitiesMap["barrel_1"]->GetPosition();
		trans.x -= 0.1f;
		sceneLoader->sceneEntitiesMap["barrel_1"]->SetPosition(trans.x, trans.y, trans.z);
		sceneLoader->sceneEntitiesMap["barrel_1"]->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		DirectX::XMFLOAT3 trans = sceneLoader->sceneEntitiesMap["barrel_1"]->GetPosition();
		trans.x += 0.1f;
		sceneLoader->sceneEntitiesMap["barrel_1"]->SetPosition(trans.x, trans.y, trans.z);
		sceneLoader->sceneEntitiesMap["barrel_1"]->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_UP))
	{
		DirectX::XMFLOAT3 trans = sceneLoader->sceneEntitiesMap["barrel_1"]->GetPosition();
		trans.z += 0.1f;
		sceneLoader->sceneEntitiesMap["barrel_1"]->SetPosition(trans.x, trans.y, trans.z);
		sceneLoader->sceneEntitiesMap["barrel_1"]->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		DirectX::XMFLOAT3 trans = sceneLoader->sceneEntitiesMap["barrel_1"]->GetPosition();
		trans.z -= 0.1f;
		sceneLoader->sceneEntitiesMap["barrel_1"]->SetPosition(trans.x, trans.y, trans.z);
		sceneLoader->sceneEntitiesMap["barrel_1"]->CalcWorldMatrix();
	}

	if (sceneLoader->sceneEntitiesMap["barrel_1"]->CheckSATCollision(sceneLoader->sceneEntitiesMap["barrel_1 (1)"]))
	{
		cout << "colliding" << endl;
	}

	camera->Update();
	water->Update();

	// Set our listener position as the camera's position for now
	listener_pos.x = camera->position.x;
	listener_pos.y = camera->position.y;
	listener_pos.z = camera->position.z;

	// Set the listener forward to the camera's forward
	listener_forward.x = camera->GetViewMatrix()._13;
	listener_forward.y = camera->GetViewMatrix()._23;
	listener_forward.z = camera->GetViewMatrix()._33;

	// Set the listener up to the camera's up
	listener_up.x = camera->GetViewMatrix()._12;
	listener_up.y = camera->GetViewMatrix()._22;
	listener_up.z = camera->GetViewMatrix()._32;

	fmodSystem->set3DListenerAttributes(0, &listener_pos, 0, &listener_forward, &listener_up); // Update 'ears'
	fmodSystem->update();

	/*if (!GetAsyncKeyState(VK_CONTROL))
	{
		testLight->Position = camera->position;
		testLight->Direction = camera->direction;
	}*/
}

void Game::Draw(float deltaTime, float totalTime)
{
	renderer->ClearFrame();

	renderer->SendAllLightsToShader(sceneLoader->pixelShadersMap["DEFAULT"]);
	renderer->SendAllLightsToShader(sceneLoader->pixelShadersMap["Normal"]);
	renderer->SendAllLightsToShader(sceneLoader->pixelShadersMap["Water"]);
	renderer->SendAllLightsToShader(sceneLoader->pixelShadersMap["Terrain"]);

	renderer->RenderShadowMap();

	renderer->RenderFrame();

	DrawSky();

	renderer->PresentFrame();
}


void Game::DrawSky() {
	ID3D11Buffer* vb = sceneLoader->defaultMeshesMap["Cube"]->GetVertexBuffer();
	ID3D11Buffer* ib = sceneLoader->defaultMeshesMap["Cube"]->GetIndexBuffer();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	sceneLoader->vertexShadersMap["Sky"]->SetMatrix4x4("view", camera->GetViewMatrix());
	sceneLoader->vertexShadersMap["Sky"]->SetMatrix4x4("projection", camera->GetProjMatrix());
	sceneLoader->vertexShadersMap["Sky"]->CopyAllBufferData();
	sceneLoader->vertexShadersMap["Sky"]->SetShader();

	sceneLoader->pixelShadersMap["Sky"]->SetShaderResourceView("Sky", skySRV);
	sceneLoader->pixelShadersMap["Sky"]->SetSamplerState("BasicSampler", sampler);
	sceneLoader->pixelShadersMap["Sky"]->SetShader();

	context->RSSetState(skyRasterState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	context->DrawIndexed(sceneLoader->defaultMeshesMap["Cube"]->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

#pragma region Mouse Input
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hWnd);
}

void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{

	ReleaseCapture();
}

void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001) {
		camera->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	
}
#pragma endregion