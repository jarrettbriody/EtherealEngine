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
	//defaults
	for (auto texMapIter = defaultTexturesMap.begin(); texMapIter != defaultTexturesMap.end(); ++texMapIter)
	{
		texMapIter->second->Release();
		//cout << "Releasing " << texMapIter->first << endl;
	}

	for (auto matMapIter = defaultMaterialsMap.begin(); matMapIter != defaultMaterialsMap.end(); ++matMapIter)
	{
		delete matMapIter->second;
		//cout << "Deleting " << matMapIter->first << endl;
	}

	for (auto meshMapIter = defaultMeshesMap.begin(); meshMapIter != defaultMeshesMap.end(); ++meshMapIter)
	{
		if (meshMapIter->first != "Ground") {
			delete meshMapIter->second;
			//cout << "Deleting " << meshMapIter->first << endl;
		}
	}

	//generated
	for (auto texMapIter = generatedTexturesMap.begin(); texMapIter != generatedTexturesMap.end(); ++texMapIter)
	{
		texMapIter->second->Release();
		//cout << "Releasing " << texMapIter->first << endl;
	}

	for (auto matMapIter = generatedMaterialsMap.begin(); matMapIter != generatedMaterialsMap.end(); ++matMapIter)
	{
		delete matMapIter->second;
		//cout << "Deleting " << matMapIter->first << endl;
	}

	for (auto meshMapIter = generatedMeshesMap.begin(); meshMapIter != generatedMeshesMap.end(); ++meshMapIter)
	{
		delete meshMapIter->second;
		//cout << "Deleting " << meshMapIter->first << endl;
	}

	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		delete sceneEntities[i];
	}

	//delete shaders
	for (auto vertSIter = vertexShadersMap.begin(); vertSIter != vertexShadersMap.end(); ++vertSIter)
	{
		delete vertSIter->second;
	}

	for (auto pixSIter = pixelShadersMap.begin(); pixSIter != pixelShadersMap.end(); ++pixSIter)
	{
		delete pixSIter->second;
	}

	sampler->Release();

	skySRV->Release();
	skyDepthState->Release();
	skyRasterState->Release();

	delete camera;
	delete renderer;
}

void Game::Init()
{
	LoadShaders();

	camera = new Camera();
	camera->UpdateProjectionMatrix(width, height);

	renderer = new Renderer(device, context, swapChain, backBufferRTV, depthStencilView, width, height);
	renderer->SetCamera(camera);
	renderer->SetShadowVertexShader(vertexShadersMap["Shadow"]);
	renderer->SetEntities(&sceneEntities);

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

	LoadDefaultMeshes();
	LoadDefaultTextures();
	LoadDefaultMaterials();

	LoadScene("ArenaV2");

	prevMousePos.x = 0;
	prevMousePos.y = 0;

	Light dLight;
	dLight.Type = LIGHT_TYPE_DIR;
	dLight.Color = XMFLOAT3(1.0f, 244.0f / 255.0f, 214.0f / 255.0f);
	dLight.Direction = XMFLOAT3(0.5f, -1.0f, 1.0f);

	renderer->AddLight("Sun", dLight);

	renderer->SendAllLightsToShader(pixelShadersMap["DEFAULT"]);

	renderer->SendAllLightsToShader(pixelShadersMap["Normal"]);

	renderer->SetShadowMapResolution(4096);

	renderer->InitShadows();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Game::LoadShaders()
{
	//vertex shaders
	SimpleVertexShader* defaultVS = new SimpleVertexShader(device, context);
	defaultVS->LoadShaderFile(L"DefaultVS.cso");
	vertexShadersMap.insert({ "DEFAULT", defaultVS });

	SimpleVertexShader* shadowVS = new SimpleVertexShader(device, context);
	shadowVS->LoadShaderFile(L"ShadowVS.cso");
	vertexShadersMap.insert({ "Shadow", shadowVS });

	SimpleVertexShader* skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"SkyVS.cso");
	vertexShadersMap.insert({ "Sky", skyVS });

	SimpleVertexShader* normalVS = new SimpleVertexShader(device, context);
	normalVS->LoadShaderFile(L"NormalVS.cso");
	vertexShadersMap.insert({ "Normal", normalVS });

	//pixel shaders
	SimplePixelShader* defaultPS = new SimplePixelShader(device, context);
	defaultPS->LoadShaderFile(L"DefaultPS.cso");
	pixelShadersMap.insert({ "DEFAULT", defaultPS });

	SimplePixelShader* skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"SkyPS.cso");
	pixelShadersMap.insert({ "Sky", skyPS });

	SimplePixelShader* normalPS = new SimplePixelShader(device, context);
	normalPS->LoadShaderFile(L"NormalPS.cso");
	pixelShadersMap.insert({ "Normal", normalPS });
}

void Game::LoadDefaultMeshes()
{
	defaultMeshesMap.insert({ "Cube", new Mesh("Cube", "../../Assets/Models/Default/cube.obj", device) });
	defaultMeshesMap.insert({ "Cylinder", new Mesh("Cylinder", "../../Assets/Models/Default/cylinder.obj", device) });
	defaultMeshesMap.insert({ "Cone", new Mesh("Cone", "../../Assets/Models/Default/cone.obj", device) });
	defaultMeshesMap.insert({ "Sphere", new Mesh("Sphere", "../../Assets/Models/Default/sphere.obj", device) });
	defaultMeshesMap.insert({ "Helix", new Mesh("Helix", "../../Assets/Models/Default/helix.obj", device) });
	defaultMeshesMap.insert({ "Torus", new Mesh("Torus", "../../Assets/Models/Default/torus.obj", device) });
	defaultMeshesMap.insert({ "Ground", defaultMeshesMap["Cube"] });
}

void Game::LoadDefaultTextures()
{
	defaultTexturesMap.insert({ "GrassDiffuse", Utility::LoadSRV(device,context,"Default/Grass/DefaultGrassDiffuse.jpg") });
	defaultTexturesMap.insert({ "GrassNormal", Utility::LoadSRV(device,context,"Default/Grass/DefaultGrassNormal.jpg") });
	defaultTexturesMap.insert({ "Red", Utility::LoadSRV(device,context,"Default/red.png") });
	defaultTexturesMap.insert({ "Marble", Utility::LoadSRV(device,context,"Default/marble.png") });
	defaultTexturesMap.insert({ "Hedge", Utility::LoadSRV(device,context,"Default/hedge.jpg") });
}

void Game::LoadDefaultMaterials()
{
	MaterialData materialData = {};
	defaultMaterialsMap.insert({"DEFAULT", new Material("DEFAULT", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], sampler)});

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["GrassDiffuse"];
	materialData.NormalTextureMapSRV = defaultTexturesMap["GrassNormal"];
	defaultMaterialsMap.insert({ "Grass", new Material("Grass", materialData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Red"];
	defaultMaterialsMap.insert({ "Red", new Material("Red", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Marble"];
	defaultMaterialsMap.insert({ "Marble", new Material("Marble", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Hedge"];
	defaultMaterialsMap.insert({ "Hedge", new Material("Hedge", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], sampler) });
}

void Game::BuildDefaultEntity(string entityName, string objName, Entity* e)
{
	if (objName == "Ground") {
		e->AddMaterial(defaultMaterialsMap["Grass"]);
		e->AddMaterialNameToMesh("Grass");
		XMFLOAT3 s = e->GetScale();
		e->SetRepeatTexture(s.x / 2.0f, s.z / 2.0f);
	}
}

Utility::MESH_TYPE Game::AutoLoadOBJMTL(string name)
{
	//mesh exists in default meshes map, which will remain untouched during program execution
	if (defaultMeshesMap.count(name))
		return Utility::MESH_TYPE::DEFAULT_MESH;

	//if mesh is already loaded
	if (generatedMeshesMap.count(name)) {
		//if the mesh is not already recorded as utilized, utilize it
		if (!utilizedMeshesMap.count(name)){
			utilizedMeshesMap.insert({ name, true });
			//get the material names utilized under the mesh
			vector<string> utilizedMaterials = generatedMeshesMap[name]->GetMaterialNameList();
			for (int i = 0; i < utilizedMaterials.size(); i++)
			{
				//if the material is not already utilized, utilize it
				if (!utilizedMaterialsMap.count(utilizedMaterials[i])) {
					utilizedMaterialsMap.insert({ utilizedMaterials[i], true });
					//get the texture names utilized under the material
					vector<string> utilizedTextures = generatedMaterialsMap[utilizedMaterials[i]]->GetMaterialData().SRVNames;
					for (int j = 0; j < utilizedTextures.size(); j++)
					{
						//if the texture is not already utilized, utilize it
						if (!utilizedTexturesMap.count(utilizedTextures[i]))
							utilizedTexturesMap.insert({ utilizedTextures[i], true });
					}
				}
			}
		}
		//mesh is already loaded and mesh, materials, and textures are now marked as utilized, exit func
		return Utility::MESH_TYPE::GENERATED_MESH;
	}

	string objPath = "../../Assets/Models/" + name + ".obj";

	//Mesh will change bool ref to false if OBJ file does not exist, otherwise it will generate it and add to map
	bool success;
	generatedMeshesMap.insert({ name, new Mesh(name, (char*)objPath.c_str(), device, &success) });
	if (!success) {
		cout << "Cannot load Object (OBJ) file: " + string(objPath) << endl;
		return Utility::MESH_TYPE::LOAD_FAILURE;
	}

	//record mesh as utilized
	utilizedMeshesMap.insert({ name,true });

	string mtlPath = generatedMeshesMap[name]->GetMTLPath();

	if (mtlPath == "") {
		cout << "Material Template Library (MTL) link not found inside \"" + name + ".obj\". If this is unintentional, link MTL file inside OBJ file." << endl;
		return Utility::MESH_TYPE::GENERATED_MESH;
	}

	ifstream infile("../../Assets/Models/" + mtlPath);

	if (!infile.is_open()) {
		cout << "Material Template Library (MTL) file not found. Please include MTL file in same directory as OBJ file or remove the internal OBJ link to " + mtlPath + "." << endl;
		return Utility::MESH_TYPE::GENERATED_MESH;
	}

	using namespace Utility;

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
	regex normalTextureRgx("^(map_Bump )");

	bool ongoingMat = false;
	string ongoingMatName = "";
	MaterialData matData;

	string line;
	smatch match;

	while (getline(infile, line)) {
		if (line != "" && !regex_search(line, match, regex("^#"))) {
			//search for new material line
			if (regex_search(line, match, newMtlRgx)) {
				line = regex_replace(line, newMtlRgx, "");
				//new material line was found but a material was in progress, complete this material before continuing
				if (ongoingMat) {
					//Different shaders based on matData values
					if (matData.NormalTextureMapSRV) {
						generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], sampler) });
					}
					else {
						generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], sampler) });
					}
					matData = {};
				}
				ongoingMat = true;
				ongoingMatName = line;

				//record material as utilized
				utilizedMaterialsMap.insert({ ongoingMatName,true });
			}
			//ambient color
			else if (regex_search(line, match, ambientColorRgx)) {
				line = regex_replace(line, ambientColorRgx, "");
				ParseFloat3FromString(line, matData.AmbientColor);
			}
			//diffuse color
			else if (regex_search(line, match, diffuseColorRgx)) {
				line = regex_replace(line, diffuseColorRgx, "");
				ParseFloat3FromString(line, matData.DiffuseColor);
			}
			//specular color
			else if (regex_search(line, match, specularColorRgx)) {
				line = regex_replace(line, specularColorRgx, "");
				ParseFloat3FromString(line, matData.SpecularColor);
			}
			//specular value
			else if (regex_search(line, match, specularExpRgx)) {
				line = regex_replace(line, specularExpRgx, "");
				ParseFloatFromString(line, matData.SpecularExponent);
			}
			//transparency value
			else if (regex_search(line, match, dTransparencyRgx)) {
				line = regex_replace(line, dTransparencyRgx, "");
				ParseFloatFromString(line, matData.Transparency);
			}
			//transparency value
			else if (regex_search(line, match, trTransparencyRgx)) {
				line = regex_replace(line, trTransparencyRgx, "");
				ParseFloatFromString(line, matData.Transparency);
				matData.Transparency = 1.0f - matData.Transparency;
			}
			//illumination value
			else if (regex_search(line, match, illuminationRgx)) {
				line = regex_replace(line, illuminationRgx, "");
				ParseIntFromString(line, matData.Illumination);
			}
			//ambient occlusion map
			else if (regex_search(line, match, ambientTextureRgx)) {
				line = regex_replace(line, ambientTextureRgx, "");
				if (!generatedTexturesMap.count(line)) {
					generatedTexturesMap.insert({ line, LoadSRV(device,context,line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.AmbientTextureMapSRV = generatedTexturesMap[line];
				matData.SRVNames.push_back(line);
			}
			//diffuse map
			else if (regex_search(line, match, diffuseTextureRgx)) {
				line = regex_replace(line, diffuseTextureRgx, "");
				if (!generatedTexturesMap.count(line)) {
					generatedTexturesMap.insert({ line, LoadSRV(device,context,line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.DiffuseTextureMapSRV = generatedTexturesMap[line];
				matData.SRVNames.push_back(line);
			}
			//specular color map
			else if (regex_search(line, match, specularColorTextureRgx)) {
				line = regex_replace(line, specularColorTextureRgx, "");
				if (!generatedTexturesMap.count(line)) {
					generatedTexturesMap.insert({ line, LoadSRV(device,context,line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.SpecularColorTextureMapSRV = generatedTexturesMap[line];
				matData.SRVNames.push_back(line);
			}
			//specular highlight map
			else if (regex_search(line, match, specularHighlightTextureRgx)) {
				line = regex_replace(line, specularHighlightTextureRgx, "");
				if (!generatedTexturesMap.count(line)) {
					generatedTexturesMap.insert({ line, LoadSRV(device,context,line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.SpecularHighlightTextureMapSRV = generatedTexturesMap[line];
				matData.SRVNames.push_back(line);
			}
			//alpha map
			else if (regex_search(line, match, alphaTextureRgx)) {
				line = regex_replace(line, alphaTextureRgx, "");
				if (!generatedTexturesMap.count(line)) {
					generatedTexturesMap.insert({ line, LoadSRV(device,context,line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.AlphaTextureMapSRV = generatedTexturesMap[line];
				matData.SRVNames.push_back(line);
			}
			//bump map
			else if (regex_search(line, match, normalTextureRgx)) {
				line = regex_replace(line, normalTextureRgx, "");
				if (!generatedTexturesMap.count(line)) {
					generatedTexturesMap.insert({ line, LoadSRV(device,context,line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.NormalTextureMapSRV = generatedTexturesMap[line];
				matData.SRVNames.push_back(line);
			}
		}
	}
	//basically only executes if the end of the file is reached and there was an ongoing material being created
	if (ongoingMat) {
		if (matData.NormalTextureMapSRV) {
			generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], sampler) });
		}
		else {
			generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], sampler) });
		}
		matData = {};
		ongoingMat = false;
	}
	infile.close();
	return Utility::MESH_TYPE::GENERATED_MESH;
}

void Game::LoadScene(string sceneName)
{
	//remove all current entities loaded
	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		delete sceneEntities[i];
	}
	sceneEntities.clear();
	sceneEntitiesMap.clear();
	utilizedMeshesMap.clear();
	utilizedMaterialsMap.clear();
	utilizedTexturesMap.clear();

	//for iterating over each line to get the float values for transformations
	regex iteratorRegex = regex("-\\d*\\.\\d*|\\d*\\.\\d*|-\\d+|\\d+");

	ifstream infile("../../Assets/Scenes/" + sceneName + ".txt");
	string line;
	smatch match;
	float parsedNumbers[9];
	string objName;
	while (getline(infile, line))
	{
		//cout << line << endl;
		if (line != "") {
			//if the line does not start with "//"
			if (!regex_match(line, regex("//.*"))) {
				//search for OBJ name at start of line
				regex_search(line, match, regex("^(\\S+)"));
				objName = match[0];

				//load mesh, material, and textures, and if they already exist then mark them as utilized
				Utility::MESH_TYPE meshType = AutoLoadOBJMTL(objName);

				Entity* someEntity;

				//naming of entity internally
				string entityName = objName; //temporary, should have entity name in scene file
				int sameNameEntityCnt = 1;
				while (sceneEntitiesMap.count(entityName)) {
					entityName = objName + " (" + to_string(sameNameEntityCnt) + ")";
					sameNameEntityCnt++;
				}

				//figure out what map to pull from
				switch (meshType) {
				case Utility::LOAD_FAILURE:
					continue;
				case Utility::DEFAULT_MESH:
					someEntity = new Entity(entityName, defaultMeshesMap[objName]);
					break;
				case Utility::GENERATED_MESH: {
					someEntity = new Entity(entityName, generatedMeshesMap[objName]);

					//generated meshes should have a list of required materials,
					//add them if they do or add the default (just black) if they dont
					vector<string> requiredMaterials = someEntity->GetMaterialNameList();
					for (int i = 0; i < requiredMaterials.size(); i++)
					{
						string requiredMat = requiredMaterials[i];
						if (generatedMaterialsMap.count(requiredMat))
							someEntity->AddMaterial(generatedMaterialsMap[requiredMat]);
					}
					if (requiredMaterials.size() == 0) {
						someEntity->AddMaterial(defaultMaterialsMap["DEFAULT"]);
						someEntity->AddMaterialNameToMesh("DEFAULT");
					}
					break;
				}
				default:
					break;
				}
				
				//get the transformation data associated with this entity
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
				someEntity->SetPosition(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);
				someEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
				someEntity->SetScale(parsedNumbers[6], parsedNumbers[7], parsedNumbers[8]);
				someEntity->CalcWorldMatrix();

				if (meshType == Utility::DEFAULT_MESH)
					BuildDefaultEntity(entityName, objName, someEntity);

				//finally add the entity to the appropriate lists
				sceneEntitiesMap.insert({ entityName,someEntity });
				sceneEntities.push_back(someEntity);
			}
		}
	}

	infile.close();

	//clean up memory from prior scene, wont reload any resources that already exist and that
	//are needed, but will remove unused resources in the current scene
	vector<string> meshesToDelete;
	for (auto meshMapIter = generatedMeshesMap.begin(); meshMapIter != generatedMeshesMap.end(); ++meshMapIter)
	{
		if (!utilizedMeshesMap.count(meshMapIter->first)) {
			meshesToDelete.push_back(meshMapIter->first);
		}
	}
	for (size_t i = 0; i < meshesToDelete.size(); i++)
	{
		delete generatedMeshesMap[meshesToDelete[i]];
		generatedMeshesMap.erase(meshesToDelete[i]);
	}

	vector<string> texturesToDelete;
	for (auto texMapIter = generatedTexturesMap.begin(); texMapIter != generatedTexturesMap.end(); ++texMapIter)
	{
		if (!utilizedTexturesMap.count(texMapIter->first)) {
			texturesToDelete.push_back(texMapIter->first);
		}
	}
	for (size_t i = 0; i < texturesToDelete.size(); i++)
	{
		delete generatedTexturesMap[texturesToDelete[i]];
		generatedTexturesMap.erase(texturesToDelete[i]);
	}

	vector<string> materialsToDelete;
	for (auto matMapIter = generatedMaterialsMap.begin(); matMapIter != generatedMaterialsMap.end(); ++matMapIter)
	{
		if (!utilizedMaterialsMap.count(matMapIter->first)) {
			materialsToDelete.push_back(matMapIter->first);
		}
	}
	for (size_t i = 0; i < materialsToDelete.size(); i++)
	{
		delete generatedMaterialsMap[materialsToDelete[i]];
		generatedMaterialsMap.erase(materialsToDelete[i]);
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
	renderer->ClearFrame();

	renderer->RenderShadowMap();

	renderer->RenderFrame();

	DrawSky();

	renderer->PresentFrame();
}


void Game::DrawSky() {
	ID3D11Buffer* vb = defaultMeshesMap["Cube"]->GetVertexBuffer();
	ID3D11Buffer* ib = defaultMeshesMap["Cube"]->GetIndexBuffer();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	vertexShadersMap["Sky"]->SetMatrix4x4("view", camera->GetViewMatrix());
	vertexShadersMap["Sky"]->SetMatrix4x4("projection", camera->GetProjMatrix());
	vertexShadersMap["Sky"]->CopyAllBufferData();
	vertexShadersMap["Sky"]->SetShader();

	pixelShadersMap["Sky"]->SetShaderResourceView("Sky", skySRV);
	pixelShadersMap["Sky"]->SetSamplerState("BasicSampler", sampler);
	pixelShadersMap["Sky"]->SetShader();

	context->RSSetState(skyRasterState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	context->DrawIndexed(defaultMeshesMap["Cube"]->GetIndexCount(), 0, 0);

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