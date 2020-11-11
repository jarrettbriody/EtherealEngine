#include "SceneLoader.h"

SceneLoader::SceneLoader(btDiscreteDynamicsWorld * dw)
{
  this->dynamicsWorld = dw;
}

SceneLoader::~SceneLoader()
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
}

void SceneLoader::LoadShaders()
{
	//vertex shaders
	SimpleVertexShader* defaultVS = new SimpleVertexShader(Config::Device, Config::Context);
	defaultVS->LoadShaderFile(L"DefaultVS.cso");
	vertexShadersMap.insert({ "DEFAULT", defaultVS });

	SimpleVertexShader* shadowVS = new SimpleVertexShader(Config::Device, Config::Context);
	shadowVS->LoadShaderFile(L"ShadowVS.cso");
	vertexShadersMap.insert({ "Shadow", shadowVS });

	SimpleVertexShader* skyVS = new SimpleVertexShader(Config::Device, Config::Context);
	skyVS->LoadShaderFile(L"SkyVS.cso");
	vertexShadersMap.insert({ "Sky", skyVS });

	SimpleVertexShader* normalVS = new SimpleVertexShader(Config::Device, Config::Context);
	normalVS->LoadShaderFile(L"NormalVS.cso");
	vertexShadersMap.insert({ "Normal", normalVS });

	SimpleVertexShader* debugLineVS = new SimpleVertexShader(Config::Device, Config::Context);
	debugLineVS->LoadShaderFile(L"DebugLineVS.cso");
	vertexShadersMap.insert({ "DebugLine", debugLineVS });

	//pixel shaders
	SimplePixelShader* defaultPS = new SimplePixelShader(Config::Device, Config::Context);
	defaultPS->LoadShaderFile(L"DefaultPS.cso");
	pixelShadersMap.insert({ "DEFAULT", defaultPS });

	SimplePixelShader* skyPS = new SimplePixelShader(Config::Device, Config::Context);
	skyPS->LoadShaderFile(L"SkyPS.cso");
	pixelShadersMap.insert({ "Sky", skyPS });

	SimplePixelShader* normalPS = new SimplePixelShader(Config::Device, Config::Context);
	normalPS->LoadShaderFile(L"NormalPS.cso");
	pixelShadersMap.insert({ "Normal", normalPS });

	SimplePixelShader* debugLinePS = new SimplePixelShader(Config::Device, Config::Context);
	debugLinePS->LoadShaderFile(L"DebugLinePS.cso");
	pixelShadersMap.insert({ "DebugLine", debugLinePS });

	SimplePixelShader* terrainPS = new SimplePixelShader(Config::Device, Config::Context);
	terrainPS->LoadShaderFile(L"TerrainPS.cso");
	pixelShadersMap.insert({ "Terrain", terrainPS });

	SimplePixelShader* waterPS = new SimplePixelShader(Config::Device, Config::Context);
	waterPS->LoadShaderFile(L"WaterPS.cso");
	pixelShadersMap.insert({ "Water", waterPS });

}

void SceneLoader::LoadDefaultMeshes()
{
	defaultMeshesMap.insert({ "Cube", new Mesh("Cube", "../../Assets/Models/Default/cube.obj", Config::Device) });
	defaultMeshesMap.insert({ "Cylinder", new Mesh("Cylinder", "../../Assets/Models/Default/cylinder.obj", Config::Device) });
	defaultMeshesMap.insert({ "Cone", new Mesh("Cone", "../../Assets/Models/Default/cone.obj", Config::Device) });
	defaultMeshesMap.insert({ "Sphere", new Mesh("Sphere", "../../Assets/Models/Default/sphere.obj", Config::Device) });
	defaultMeshesMap.insert({ "Helix", new Mesh("Helix", "../../Assets/Models/Default/helix.obj", Config::Device) });
	defaultMeshesMap.insert({ "Torus", new Mesh("Torus", "../../Assets/Models/Default/torus.obj", Config::Device) });
	defaultMeshesMap.insert({ "Ground", defaultMeshesMap["Cube"] });
}

void SceneLoader::LoadDefaultTextures()
{
	defaultTexturesMap.insert({ "GrassDiffuse", Utility::LoadSRV("Default/Grass/DefaultGrassDiffuse.jpg") });
	defaultTexturesMap.insert({ "GrassNormal", Utility::LoadSRV("Default/Grass/DefaultGrassNormal.jpg") });
	defaultTexturesMap.insert({ "Red", Utility::LoadSRV("Default/red.png") });
	defaultTexturesMap.insert({ "Marble", Utility::LoadSRV("Default/marble.png") });
	defaultTexturesMap.insert({ "Hedge", Utility::LoadSRV("Default/hedge.jpg") });
	defaultTexturesMap.insert({ "terrain2", Utility::LoadSRV("grass.png") });
	defaultTexturesMap.insert({ "terrain3", Utility::LoadSRV("rocky.png") });
	defaultTexturesMap.insert({ "terrain1", Utility::LoadSRV("snow.jpg") });
	defaultTexturesMap.insert({ "terrainNormal2", Utility::LoadSRV("grass_normal.png") });
	defaultTexturesMap.insert({ "terrainNormal3", Utility::LoadSRV("rocky_normal.png") });
	defaultTexturesMap.insert({ "terrainNormal1", Utility::LoadSRV("snow_normal.jpg") });
	defaultTexturesMap.insert({ "terrainBlendMap", Utility::LoadSRV("blendMap.png") });
	defaultTexturesMap.insert({ "waterBase", Utility::LoadSRV("water_base.png") });
	defaultTexturesMap.insert({ "waterFoam", Utility::LoadSRV("water_foam.jpg") });
	defaultTexturesMap.insert({ "waterNormal1", Utility::LoadSRV("water_normal1.jpeg") });
	defaultTexturesMap.insert({ "waterNormal2", Utility::LoadSRV("water_normal2.png") });
}

void SceneLoader::LoadDefaultMaterials()
{
	MaterialData materialData = {};
	defaultMaterialsMap.insert({ "DEFAULT", new Material("DEFAULT", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["GrassDiffuse"];
	materialData.NormalTextureMapSRV = defaultTexturesMap["GrassNormal"];
	defaultMaterialsMap.insert({ "Grass", new Material("Grass", materialData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Red"];
	defaultMaterialsMap.insert({ "Red", new Material("Red", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Marble"];
	defaultMaterialsMap.insert({ "Marble", new Material("Marble", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler) });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Hedge"];
	defaultMaterialsMap.insert({ "Hedge", new Material("Hedge", materialData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler) });

	/*
	TerrainMaterialData terrainMaterialData = {};
	terrainMaterialData.SurfaceTexture1 = defaultTexturesMap["terrain1"];
	terrainMaterialData.SurfaceTexture2 = defaultTexturesMap["terrain2"];
	terrainMaterialData.SurfaceTexture3 = defaultTexturesMap["terrain3"];
	terrainMaterialData.SurfaceNormal1 = defaultTexturesMap["terrainNormal1"];
	terrainMaterialData.SurfaceNormal2 = defaultTexturesMap["terrainNormal2"];
	terrainMaterialData.SurfaceNormal3 = defaultTexturesMap["terrainNormal3"];
	terrainMaterialData.uvScale = 50.0f;
	terrainMaterialData.BlendMap = defaultTexturesMap["terrainBlendMap"];
	defaultMaterialsMap.insert({ "Terrain", new TerrainMaterial("Terrain", terrainMaterialData, vertexShadersMap["DEFAULT"], pixelShadersMap["Terrain"], Config::Sampler) });

	WaterMaterialData waterMaterialData = {};
	waterMaterialData.SurfaceTexture1 = defaultTexturesMap["waterBase"];
	waterMaterialData.SurfaceTexture2 = defaultTexturesMap["waterFoam"];
	waterMaterialData.SurfaceNormal1 = defaultTexturesMap["waterNormal1"];
	waterMaterialData.SurfaceNormal2 = defaultTexturesMap["waterNormal2"];
	waterMaterialData.uvScale = 20.0f;
	defaultMaterialsMap.insert({ "Water", new WaterMaterial("Water", waterMaterialData, vertexShadersMap["DEFAULT"], pixelShadersMap["Water"], Config::Sampler) });
	*/

}

void SceneLoader::BuildDefaultEntity(string entityName, string objName, Entity* e)
{
	if (objName == "Ground") {
		e->AddMaterial(defaultMaterialsMap["Grass"]);
		e->AddMaterialNameToMesh("Grass");
		XMFLOAT3 s = e->GetScale();
		e->SetRepeatTexture(s.x / 2.0f, s.z / 2.0f);
	}
}

Utility::MESH_TYPE SceneLoader::AutoLoadOBJMTL(string name)
{
	//mesh exists in default meshes map, which will remain untouched during program execution
	if (defaultMeshesMap.count(name))
		return Utility::MESH_TYPE::DEFAULT_MESH;

	//if mesh is already loaded
	if (generatedMeshesMap.count(name)) {
		//if the mesh is not already recorded as utilized, utilize it
		if (!utilizedMeshesMap.count(name)) {
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
	generatedMeshesMap.insert({ name, new Mesh(name, (char*)objPath.c_str(), Config::Device, &success) });
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
						generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler) });
					}
					else {
						generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler) });
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
					generatedTexturesMap.insert({ line, Utility::LoadSRV(line) });

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
					generatedTexturesMap.insert({ line, Utility::LoadSRV(line) });

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
					generatedTexturesMap.insert({ line, Utility::LoadSRV(line) });

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
					generatedTexturesMap.insert({ line, Utility::LoadSRV(line) });

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
					generatedTexturesMap.insert({ line, Utility::LoadSRV(line) });

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
					generatedTexturesMap.insert({ line, Utility::LoadSRV(line) });

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
			generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler) });
		}
		else {
			generatedMaterialsMap.insert({ ongoingMatName, new Material(ongoingMatName, matData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler) });
		}
		matData = {};
		ongoingMat = false;
	}
	infile.close();
	return Utility::MESH_TYPE::GENERATED_MESH;
}

void SceneLoader::LoadScene(string sceneName)
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
					someEntity = new Entity(entityName, dynamicsWorld, defaultMeshesMap[objName]);
					break;
				case Utility::GENERATED_MESH: {
					someEntity = new Entity(entityName, dynamicsWorld, generatedMeshesMap[objName]);

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
				if(someEntity->collisionsEnabled)
					someEntity->AddAutoBoxCollider();
				someEntity->SetPosition(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);
				someEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
				someEntity->SetScale(parsedNumbers[6], parsedNumbers[7], parsedNumbers[8]);
				someEntity->CalcWorldMatrix();
				if (someEntity->collisionsEnabled && someEntity->colliderDebugLinesEnabled) {
					vector<Collider*> colliders = someEntity->GetColliders();
					for (size_t d = 0; d < colliders.size(); d++)
					{
						DebugLines* dl = new DebugLines(entityName, d);
						XMFLOAT3 c = XMFLOAT3(1.0f, 0.0f, 0.0f);
						dl->color = c;
						dl->worldMatrix = colliders[d]->GetWorldMatrix();
						XMFLOAT3* colliderCorners = colliders[d]->GetUntransformedColliderCorners();
						dl->GenerateCuboidVertexBuffer(colliderCorners, 8);
					}
				}

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