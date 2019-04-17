#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"


using namespace DirectX;

Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,					// The application's handle
		"Small Shooting Arena",	   	// Text for the window's title bar
		1600,						// Width of the window's client area
		900,						// Height of the window's client area
		true)						// Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

#if defined(DEBUG) || defined(_DEBUG)
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

Game::~Game()
{
	marbleSRV->Release();
	hedgeSRV->Release();
	redSRV->Release();
	sampler->Release();

	skySRV->Release();
	skyDepthState->Release();
	skyRasterState->Release();
	delete skyVS;
	delete skyPS;

	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		delete sceneEntities[i];
	}

	delete meshMap["Cube"];
	delete meshMap["Cylinder"];
	delete meshMap["Cone"];
	delete meshMap["Sphere"];
	delete meshMap["Helix"];
	delete meshMap["Torus"];
	delete meshMap["Wall"];
	delete meshMap["Barrier1"];
	delete meshMap["Barrier2"];
	delete meshMap["Ruin"];

	for (auto matMapIter = materialMap.begin(); matMapIter != materialMap.end(); matMapIter++)
	{
		delete matMapIter->second;
	}

	for (auto texMapIter = textureMap.begin(); texMapIter != textureMap.end(); texMapIter++)
	{
		texMapIter->second->Release();
	}

	delete material;
	delete material2;

	delete camera;
	delete renderer;

	delete vertexShader;
	delete pixelShader;
}

void Game::Init()
{
	LoadShaders();

	camera = new Camera();
	camera->UpdateProjectionMatrix(width, height);

	renderer = new Renderer();

	DirectX::CreateWICTextureFromFile(device, context, L"../../Assets/Textures/marble.png", 0, &marbleSRV);

	DirectX::CreateWICTextureFromFile(device, context, L"../../Assets/Textures/hedge.jpg", 0, &hedgeSRV);

	DirectX::CreateWICTextureFromFile(device, context, L"../../Assets/Textures/red.png", 0, &redSRV);

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

	MaterialData md;
	md.DiffuseTextureMapSRV = redSRV;

	material = new Material("Red", md,vertexShader, pixelShader, sampler);

	md.DiffuseTextureMapSRV = hedgeSRV;

	material2 = new Material("Hedges", md, vertexShader, pixelShader, sampler);

	LoadModels();
	LoadMaterials();
	LoadScene();

	prevMousePos.x = 0;
	prevMousePos.y = 0;

	dLight.Type = LIGHT_TYPE_DIR;
	dLight.Color = XMFLOAT3(1.0f, 244.0f / 255.0f, 214.0f / 255.0f);
	dLight.Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);

	renderer->AddLight("directional", dLight);

	renderer->SendAllLightsToShader(pixelShader);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"SkyPS.cso");
}

void Game::LoadModels()
{
	meshMap.insert({ "Cube", new Mesh("Cube", "../../Assets/Models/cube.obj", device) });
	meshMap.insert({ "Cylinder", new Mesh("Cylinder", "../../Assets/Models/cylinder.obj", device) });
	meshMap.insert({ "Cone", new Mesh("Cone", "../../Assets/Models/cone.obj", device) });
	meshMap.insert({ "Sphere", new Mesh("Sphere", "../../Assets/Models/sphere.obj", device) });
	meshMap.insert({ "Helix", new Mesh("Helix", "../../Assets/Models/helix.obj", device) });
	meshMap.insert({ "Torus", new Mesh("Torus", "../../Assets/Models/torus.obj", device) });
	meshMap.insert({ "Ground", meshMap["Cube"] });
	meshMap.insert({ "Wall", new Mesh("Wall", "../../Assets/Models/Wall.obj", device) });
	meshMap.insert({ "Barrier1", new Mesh("Barrier1", "../../Assets/Models/Barrier1.obj", device) });
	meshMap.insert({ "Barrier2", new Mesh("Barrier2", "../../Assets/Models/Barrier2.obj", device) });
	meshMap.insert({ "Ruin", new Mesh("Ruin", "../../Assets/Models/Ruin.obj", device) });
}

void Game::LoadMaterials()
{
	regex newMtlRgx("^(newmtl )");
	regex ambientColorRgx("^(Ka )");
	regex diffuseColorRgx("^(Kd )");
	regex specularColorRgx("^(Ks )");
	regex specularExpRgx("^(Ns )");
	regex dTransparencyRgx("^(d )");
	regex trTransparencyRgx("^(Tr )");
	regex illuminationRgx("^(illum )");
	regex ambientTextureRgx("^(map_Ka )");
	regex diffuseTextureRgx("^(map_Kd )");
	regex specularColorTextureRgx("^(map_Ks )");
	regex specularHighlightTextureRgx("^(map_Ns )");
	regex alphaTextureRgx("^(map_d )");
	bool ongoingMat = false;
	string ongoingMatName = "";
	MaterialData matData;
	vector<string> mtlPaths = Mesh::GetMtlPaths();
	for (size_t i = 0; i < mtlPaths.size(); i++)
	{
		ongoingMat = false;
		string filepath("../../Assets/Models/");
		string file(mtlPaths[i]);
		filepath += file;
		ifstream infile(filepath);
		string line;
		smatch match;
		while (getline(infile, line)) {
			if (line != "" && !regex_search(line, match, regex("^#"))) {
				if (regex_search(line, match, newMtlRgx)) {
					line = regex_replace(line, newMtlRgx, "");
					if (ongoingMat) {
						materialMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShader, pixelShader, sampler) });
						matData = {};
					}
					ongoingMat = true;
					ongoingMatName = line;
				}
				else if (regex_search(line, match, ambientColorRgx)) {
					line = regex_replace(line, ambientColorRgx, "");
					Utility::ParseFloat3FromString(line, matData.AmbientColor);
				}
				else if (regex_search(line, match, diffuseColorRgx)) {
					line = regex_replace(line, diffuseColorRgx, "");
					Utility::ParseFloat3FromString(line, matData.DiffuseColor);
				}
				else if (regex_search(line, match, specularColorRgx)) {
					line = regex_replace(line, specularColorRgx, "");
					Utility::ParseFloat3FromString(line, matData.SpecularColor);
				}
				else if (regex_search(line, match, specularExpRgx)) {
					line = regex_replace(line, specularExpRgx, "");
					Utility::ParseFloatFromString(line, matData.SpecularExponent);
				}
				else if (regex_search(line, match, dTransparencyRgx)) {
					line = regex_replace(line, dTransparencyRgx, "");
					Utility::ParseFloatFromString(line, matData.Transparency);
				}
				else if (regex_search(line, match, trTransparencyRgx)) {
					line = regex_replace(line, trTransparencyRgx, "");
					Utility::ParseFloatFromString(line, matData.Transparency);
					matData.Transparency = 1.0f - matData.Transparency;
				}
				else if (regex_search(line, match, illuminationRgx)) {
					line = regex_replace(line, illuminationRgx, "");
					Utility::ParseIntFromString(line, matData.Illumination);
				}
				else if (regex_search(line, match, ambientTextureRgx)) {
					line = regex_replace(line, ambientTextureRgx, "");
					ID3D11ShaderResourceView* srv;
					wchar_t path[100] = L"../../Assets/Textures/";
					wchar_t file[50];
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &line.c_str()[0], -1, &file[0], 50);
					DirectX::CreateWICTextureFromFile(device, context, wcsncat(path, file, 100), 0, &srv);
					textureMap.insert({ line, srv });
					matData.AmbientTextureMapSRV = textureMap[line];
				}
				else if (regex_search(line, match, diffuseTextureRgx)) {
					line = regex_replace(line, diffuseTextureRgx, "");
					ID3D11ShaderResourceView* srv;
					wchar_t path[100] = L"../../Assets/Textures/";
					wchar_t file[50];
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &line.c_str()[0], -1, &file[0], 50);
					DirectX::CreateWICTextureFromFile(device, context, wcsncat(path, file, 100), 0, &srv);
					textureMap.insert({ line, srv });
					matData.DiffuseTextureMapSRV = textureMap[line];
				}
				else if (regex_search(line, match, specularColorTextureRgx)) {
					line = regex_replace(line, specularColorTextureRgx, "");
					ID3D11ShaderResourceView* srv;
					wchar_t path[100] = L"../../Assets/Textures/";
					wchar_t file[50];
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &line.c_str()[0], -1, &file[0], 50);
					DirectX::CreateWICTextureFromFile(device, context, wcsncat(path, file, 100), 0, &srv);
					textureMap.insert({ line, srv });
					matData.SpecularColorTextureMapSRV = textureMap[line];
				}
				else if (regex_search(line, match, specularHighlightTextureRgx)) {
					line = regex_replace(line, specularHighlightTextureRgx, "");
					ID3D11ShaderResourceView* srv;
					wchar_t path[100] = L"../../Assets/Textures/";
					wchar_t file[50];
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &line.c_str()[0], -1, &file[0], 50);
					DirectX::CreateWICTextureFromFile(device, context, wcsncat(path, file, 100), 0, &srv);
					textureMap.insert({ line, srv });
					matData.SpecularHighlightTextureMapSRV = textureMap[line];
				}
				else if (regex_search(line, match, alphaTextureRgx)) {
					line = regex_replace(line, alphaTextureRgx, "");
					ID3D11ShaderResourceView* srv;
					wchar_t path[100] = L"../../Assets/Textures/";
					wchar_t file[50];
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &line.c_str()[0], -1, &file[0], 50);
					DirectX::CreateWICTextureFromFile(device, context, wcsncat(path, file, 100), 0, &srv);
					textureMap.insert({ line, srv });
					matData.AlphaTextureMapSRV = textureMap[line];
				}
			}
		}
		if (ongoingMat) {
			materialMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShader, pixelShader, sampler) });
			matData = {};
			ongoingMat = false;
		}
	}
}

void Game::LoadScene()
{
	regex transformationRegex[3] = { regex("P\\(.*?\\)"),regex("R\\(.*?\\)"), regex("S\\(.*?\\)") };
	regex iteratorRegex = regex("-\\d*\\.\\d*|\\d*\\.\\d*|-\\d+|\\d+");

	ifstream infile("../../Assets/Scenes/scene.txt");
	string line;
	smatch match;
	float parsedNumbers[9];
	string objName;
	while (getline(infile, line))
	{
		if (line != "") {
			regex_search(line, match, regex("^(\\S+)"));
			objName = match[0];
			line = regex_replace(line, regex("^(\\S+ )"), "");
			std::sregex_iterator iter(line.begin(), line.end(), iteratorRegex);
			int counter = 0;
			for (; iter != std::sregex_iterator(); ++iter) {
				if (counter < 9) {
					match = *iter;
					parsedNumbers[counter] = std::stof(match.str());
				}
				counter++;
			}
			Entity* someEntity = new Entity(meshMap[objName]);
			vector<string> requiredMaterials = someEntity->GetMaterialNameList();
			for (int i = 0; i < requiredMaterials.size(); i++)
			{
				string requiredMat = requiredMaterials[i];
				if (materialMap.count(requiredMat))
					someEntity->AddMaterial(materialMap[requiredMat]);
			}
			if (requiredMaterials.size() == 0) {
				someEntity->AddMaterial(material);
				someEntity->AddMaterialNameToMesh(material->GetName());
			}
			someEntity->SetPosition(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);
			someEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
			someEntity->SetScale(parsedNumbers[6], parsedNumbers[7], parsedNumbers[8]);
			someEntity->CalcWorldMatrix();
			sceneEntities.push_back(someEntity);
		}
	}
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

	camera->Update();
}

void Game::Draw(float deltaTime, float totalTime)
{
	// Background color
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		for (int j = -1; j < sceneEntities[i]->GetMeshChildCount(); j++)
		{
			if (sceneEntities[i]->MeshHasChildren() && j == -1)
				j++;

			ID3D11Buffer* vbo = sceneEntities[i]->GetMeshVertexBuffer(j);
			context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
			context->IASetIndexBuffer(sceneEntities[i]->GetMeshIndexBuffer(j), DXGI_FORMAT_R32_UINT, 0);

			//if(sceneEntities[i]->GetMeshMaterialName(j))
			sceneEntities[i]->PrepareMaterial(sceneEntities[i]->GetMeshMaterialName(j), camera->GetViewMatrix(), camera->GetProjMatrix());

			context->DrawIndexed(
				sceneEntities[i]->GetMeshIndexCount(j),		// The number of indices to use (we could draw a subset if we wanted)
				0,											// Offset to the first index we want to use
				0);											// Offset to add to each index when looking up vertices
		}
	}

	DrawSky();

	swapChain->Present(0, 0);
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}


void Game::DrawSky() {
	ID3D11Buffer* vb = meshMap["Cube"]->GetVertexBuffer();
	ID3D11Buffer* ib = meshMap["Cube"]->GetIndexBuffer();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	skyVS->SetMatrix4x4("view", camera->GetViewMatrix());
	skyVS->SetMatrix4x4("projection", camera->GetProjMatrix());
	skyVS->CopyAllBufferData();
	skyVS->SetShader();

	skyPS->SetShaderResourceView("Sky", skySRV);
	skyPS->SetSamplerState("BasicSampler", sampler);
	skyPS->SetShader();

	context->RSSetState(skyRasterState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	context->DrawIndexed(meshMap["Cube"]->GetIndexCount(), 0, 0);

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
	camera->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

	prevMousePos.x = x;
	prevMousePos.y = y;
}

void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	
}
#pragma endregion