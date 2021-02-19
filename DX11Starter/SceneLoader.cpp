#include "pch.h"
#include "SceneLoader.h"

SceneLoader* SceneLoader::instance = nullptr;

SceneLoader::SceneLoader()
{
	this->EEMemoryAllocator = MemoryAllocator::GetInstance();
}

SceneLoader::~SceneLoader()
{
	//defaults
	for (auto texMapIter = defaultTexturesMap.begin(); texMapIter != defaultTexturesMap.end(); ++texMapIter)
	{
		texMapIter->second->Release();
		//cout << "Deleting " << texMapIter->first << endl;
	}

	for (auto matMapIter = defaultMaterialsMap.begin(); matMapIter != defaultMaterialsMap.end(); ++matMapIter)
	{
		matMapIter->second->FreeMemory();
		//cout << "Deleting " << matMapIter->first << endl;
	}

	for (auto meshMapIter = defaultMeshesMap.begin(); meshMapIter != defaultMeshesMap.end(); ++meshMapIter)
	{
		if (meshMapIter->first != "Ground" &&
			meshMapIter->first != "Blood_Button" &&
			meshMapIter->first != "Graybox" &&
			meshMapIter->first != "Wall" &&
			meshMapIter->first != "Manhole" &&
			meshMapIter->first != "Floor") {
			//delete meshMapIter->second;
			meshMapIter->second->FreeMemory();
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
		matMapIter->second->FreeMemory();
		//cout << "Deleting " << matMapIter->first << endl;
	}

	for (auto meshMapIter = generatedMeshesMap.begin(); meshMapIter != generatedMeshesMap.end(); ++meshMapIter)
	{
		//delete meshMapIter->second;
		meshMapIter->second->FreeMemory();
		//cout << "Deleting " << meshMapIter->first << endl;
	}

	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		//delete sceneEntities[i];
		sceneEntities[i]->FreeMemory();
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

bool SceneLoader::SetupInstance()
{
	if (instance == nullptr) {
		instance = new SceneLoader();
		return true;
	}
	return false;
}

SceneLoader* SceneLoader::GetInstance()
{
	return instance;
}

bool SceneLoader::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
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
	bool success;

	Mesh* cubeMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	Mesh cube = Mesh("Cube", "../../Assets/Models/Default/cube.obj", Config::Device);
	*cubeMesh = cube;
	defaultMeshesMap.insert({ "Cube", cubeMesh });

	Mesh* cylinderMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	Mesh cylinder = Mesh("Cylinder", "../../Assets/Models/Default/cylinder.obj", Config::Device);
	*cylinderMesh = cylinder;
	defaultMeshesMap.insert({ "Cylinder", cylinderMesh });

	Mesh* coneMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	Mesh cone = Mesh("Cone", "../../Assets/Models/Default/cone.obj", Config::Device);
	*coneMesh = cone;
	defaultMeshesMap.insert({ "Cone", coneMesh });

	Mesh* sphereMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	Mesh sphere = Mesh("Sphere", "../../Assets/Models/Default/sphere.obj", Config::Device);
	*sphereMesh = sphere;
	defaultMeshesMap.insert({ "Sphere", sphereMesh });

	Mesh* helixMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	Mesh helix = Mesh("Helix", "../../Assets/Models/Default/helix.obj", Config::Device);
	*helixMesh = helix;
	defaultMeshesMap.insert({ "Helix", helixMesh });

	Mesh* torusMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	Mesh torus = Mesh("Torus", "../../Assets/Models/Default/torus.obj", Config::Device);
	*torusMesh = torus;
	defaultMeshesMap.insert({ "Torus", torusMesh });


	defaultMeshesMap.insert({ "Ground", defaultMeshesMap["Cube"] });
	defaultMeshesMap.insert({ "Blood_Button", defaultMeshesMap["Cube"] });
	defaultMeshesMap.insert({ "Wall", defaultMeshesMap["Cube"] });
	defaultMeshesMap.insert({ "Floor", defaultMeshesMap["Cube"] });
	defaultMeshesMap.insert({ "Manhole", defaultMeshesMap["Cylinder"] });
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
	defaultTexturesMap.insert({ "Grey", Utility::LoadSRV("Default/grey.png") });
	defaultTexturesMap.insert({ "Grey4", Utility::LoadSRV("Default/grey4.png") });
	defaultTexturesMap.insert({ "White", Utility::LoadSRV("Default/white.png") });
}

void SceneLoader::LoadDefaultMaterials()
{
	MaterialData materialData = {};
	Material* allocatedMaterial;
	bool success;

	Material defaultMaterial = Material("DEFAULT", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = defaultMaterial;
	defaultMaterialsMap.insert({ "DEFAULT", allocatedMaterial});

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["GrassDiffuse"];
	materialData.NormalTextureMapSRV = defaultTexturesMap["GrassNormal"];
	Material grassMaterial = Material("Grass", materialData, ShaderType::NORMAL, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = grassMaterial;
	defaultMaterialsMap.insert({ "Grass", allocatedMaterial });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Red"];
	Material redMaterial = Material("Red", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = redMaterial;
	defaultMaterialsMap.insert({ "Red", allocatedMaterial });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Marble"];
	Material marbleMaterial = Material("Marble", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = marbleMaterial;
	defaultMaterialsMap.insert({ "Marble", allocatedMaterial });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Hedge"];
	Material hedgeMaterial = Material("Hedge", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = hedgeMaterial;
	defaultMaterialsMap.insert({ "Hedge", allocatedMaterial });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Grey"];
	materialData.SpecularExponent = 500;
	Material greyMaterial = Material("Grey", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = greyMaterial;
	defaultMaterialsMap.insert({ "Grey", allocatedMaterial });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["Grey4"];
	materialData.SpecularExponent = 900;
	Material grey4Material = Material("Grey4", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = grey4Material;
	defaultMaterialsMap.insert({ "Grey4", allocatedMaterial });

	materialData = {};
	materialData.DiffuseTextureMapSRV = defaultTexturesMap["White"];
	materialData.SpecularExponent = 100;
	Material whiteMaterial = Material("White", materialData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
	allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = whiteMaterial;
	defaultMaterialsMap.insert({ "White", allocatedMaterial });
}

void SceneLoader::BuildDefaultEntity(string entityName, string objName, Entity* e)
{
	if (objName == "Ground") {
		e->AddMaterial(defaultMaterialsMap["Grass"], true);
		XMFLOAT3 s = e->GetScale();
		e->SetRepeatTexture(s.x / 2.0f, s.z / 2.0f);
	}
	if (objName == "Blood_Button") {
		e->AddMaterial(defaultMaterialsMap["Grey"], true);
		XMFLOAT3 s = e->GetScale();
		e->SetRepeatTexture(s.x / 2.0f, s.y / 2.0f);
	}
	if (objName == "Wall") {
		e->AddMaterial(defaultMaterialsMap["White"], true);
		XMFLOAT3 s = e->GetScale();
		e->SetRepeatTexture(s.x / 2.0f, s.z / 2.0f);
	}
	if (objName == "Floor") {
		e->AddMaterial(defaultMaterialsMap["Grey4"], true);
		XMFLOAT3 s = e->GetScale();
		e->SetRepeatTexture(s.x / 2.0f, s.z / 2.0f);
	}
	if (objName == "Manhole") {
		e->AddMaterial(defaultMaterialsMap["White"], true);
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
					vector<string> utilizedTextures = materialTextureAssociationMap[utilizedMaterials[i]];
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

	string objPath = modelPath + name + ".obj";

	//Mesh will change bool ref to false if OBJ file does not exist, otherwise it will generate it and add to map
	bool success;
	Mesh* newMesh = (Mesh*)EEMemoryAllocator->AllocateToPool(Utility::MESH_POOL, sizeof(Mesh), success);
	if (!success) {
		cout << "Cannot load Object (OBJ) file: " + string(objPath) << endl;
		return Utility::MESH_TYPE::LOAD_FAILURE;
	}

	Mesh someMesh = Mesh(name, (char*)objPath.c_str(), Config::Device, &success);
	*newMesh = someMesh;
	if(newMesh->GetChildCount() > 0)
		newMesh->AllocateChildren();
	if (!success) {
		cout << "Cannot load Object (OBJ) file: " + string(objPath) << endl;
		return Utility::MESH_TYPE::LOAD_FAILURE;
	}

	generatedMeshesMap.insert({ name, newMesh });

	//record mesh as utilized
	utilizedMeshesMap.insert({ name,true });

	string mtlPath = generatedMeshesMap[name]->GetMTLPath();

	if (mtlPath == "") {
		cout << "Material Template Library (MTL) link not found inside \"" + name + ".obj\". If this is unintentional, link MTL file inside OBJ file." << endl;
		return Utility::MESH_TYPE::GENERATED_MESH;
	}

	ifstream infile(modelPath + mtlPath);

	if (!infile.is_open()) {
		cout << "Material Template Library (MTL) file not found. Please include MTL file in same directory as OBJ file or remove the internal OBJ link to " + mtlPath + "." << endl;
		return Utility::MESH_TYPE::GENERATED_MESH;
	}

	using namespace Utility;

	regex newMtlRgx("^(newmtl\\s+)");
	regex ambientColorRgx("^(Ka\\s+)");
	regex diffuseColorRgx("^(Kd\\s+)");
	regex specularColorRgx("^(Ks\\s+)");
	regex specularExpRgx("^(Ns\\s+)");
	regex dTransparencyRgx("^(d\\s+)");
	regex trTransparencyRgx("^(Tr\\s+)");
	regex illuminationRgx("^(illum\\s+)");
	regex ambientTextureRgx("^(map_Ka\\s+)");
	regex diffuseTextureRgx("^(map_Kd\\s+)");
	regex specularColorTextureRgx("^(map_Ks\\s+)");
	regex specularHighlightTextureRgx("^(map_Ns\\s+)");
	regex alphaTextureRgx("^(map_d\\s+)");
	regex normalTextureRgx("^(map_Bump\\s+)");

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
				if (ongoingMat && !generatedMaterialsMap.count(ongoingMatName)) {
					Material someMaterial;

					//Different shaders based on matData values
					if (matData.NormalTextureMapSRV) {
						someMaterial = Material(ongoingMatName, matData, ShaderType::NORMAL, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler);
					}
					else {
						someMaterial = Material(ongoingMatName, matData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
					}

					matData = {};

					//alloc the material to memallocator
					bool success = false;
					Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
					if (success) {
						*allocatedMaterial = someMaterial;
						generatedMaterialsMap.insert({ ongoingMatName, allocatedMaterial });

						//record material as utilized
						utilizedMaterialsMap.insert({ ongoingMatName,true });
					}
				}
				ongoingMat = true;
				ongoingMatName = line;
				if (!materialTextureAssociationMap.count(ongoingMatName)) {
					vector<string> v;
					materialTextureAssociationMap.insert({ ongoingMatName, v });
				}
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
				materialTextureAssociationMap[ongoingMatName].push_back(line);
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
				materialTextureAssociationMap[ongoingMatName].push_back(line);
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
				materialTextureAssociationMap[ongoingMatName].push_back(line);
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
				materialTextureAssociationMap[ongoingMatName].push_back(line);
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
				materialTextureAssociationMap[ongoingMatName].push_back(line);
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
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
		}
	}
	//basically only executes if the end of the file is reached and there was an ongoing material being created
	if (ongoingMat && !generatedMaterialsMap.count(ongoingMatName)) {
		Material someMaterial;

		//Different shaders based on matData values
		if (matData.NormalTextureMapSRV) {
			someMaterial = Material(ongoingMatName, matData, ShaderType::NORMAL, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler);
		}
		else {
			someMaterial = Material(ongoingMatName, matData, ShaderType::DEFAULT, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
		}

		matData = {};
		ongoingMat = false;

		//alloc the material to memallocator
		bool success = false;
		Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool(Utility::MATERIAL_POOL, sizeof(Material), success);
		if (success) {
			*allocatedMaterial = someMaterial;
			generatedMaterialsMap.insert({ ongoingMatName, allocatedMaterial });

			//record material as utilized
			utilizedMaterialsMap.insert({ ongoingMatName,true });
		}
	}
	infile.close();
	return Utility::MESH_TYPE::GENERATED_MESH;
}

void SceneLoader::LoadScene(string sceneName)
{
	//remove all current entities loaded
	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		sceneEntities[i]->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool(Utility::ENTITY_POOL, sceneEntities[i], sizeof(Entity));
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

				Entity someEntity;

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
					someEntity = Entity(entityName, defaultMeshesMap[objName]);
					break;
				case Utility::GENERATED_MESH: {
					someEntity = Entity(entityName, generatedMeshesMap[objName]);
					break;
				}
				default:
					break;
				}

				//finally add the entity to the appropriate lists
				bool success = false;
				Entity* allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool(Utility::ENTITY_POOL, sizeof(Entity), success);
				if (success) {
					//allocatedEntity->CopyCollections(&someEntity);
					//allocatedEntity->SetupCollections();
					*allocatedEntity = someEntity;
					sceneEntitiesMap.insert({ entityName, allocatedEntity });
					sceneEntities.push_back(allocatedEntity);
				}

				if (meshType == Utility::GENERATED_MESH) {
					//generated meshes should have a list of required materials,
					//add them if they do or add the default (just black) if they dont
					vector<string> requiredMaterials = allocatedEntity->GetMaterialNameList();
					for (int i = 0; i < requiredMaterials.size(); i++)
					{
						string requiredMat = requiredMaterials[i];
						if (generatedMaterialsMap.count(requiredMat))
							allocatedEntity->AddMaterial(generatedMaterialsMap[requiredMat]);
					}
					if (requiredMaterials.size() == 0) {
						allocatedEntity->AddMaterial(defaultMaterialsMap["DEFAULT"]);
					}
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
				if(allocatedEntity->collisionsEnabled)
					allocatedEntity->AddAutoBoxCollider();
				allocatedEntity->SetPosition(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);
				allocatedEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
				allocatedEntity->SetScale(parsedNumbers[6], parsedNumbers[7], parsedNumbers[8]);
				allocatedEntity->CalcWorldMatrix();
				allocatedEntity->InitRigidBody(Config::DynamicsWorld, 0.0f);
				if (Config::DebugLinesEnabled && allocatedEntity->colliderDebugLinesEnabled) {
					vector<Collider*> colliders = allocatedEntity->GetColliders();
					for (size_t d = 0; d < colliders.size(); d++)
					{
						DebugLines* dl = new DebugLines(entityName, d);
						XMFLOAT3 c = XMFLOAT3(1.0f, 0.0f, 0.0f);
						dl->color = c;
						dl->worldMatrix = colliders[d]->GetWorldMatrix();
						XMFLOAT3* colliderCorners = colliders[d]->GetPivotShiftedColliderCorners();
						dl->GenerateCuboidVertexBuffer(colliderCorners, 8);
					}
				}

				if (meshType == Utility::DEFAULT_MESH)
					BuildDefaultEntity(entityName, objName, allocatedEntity);
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
		Mesh* toDelete = generatedMeshesMap[meshesToDelete[i]];
		toDelete->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool(MESH_POOL, toDelete, sizeof(Mesh));
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
		generatedTexturesMap[texturesToDelete[i]]->Release();
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
		Material* toDelete = generatedMaterialsMap[materialsToDelete[i]];
		toDelete->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool(MATERIAL_POOL, toDelete, sizeof(Material));
		generatedMaterialsMap.erase(materialsToDelete[i]);
	}
}

void SceneLoader::SetModelPath(string path)
{
	modelPath = path;
}

Entity* SceneLoader::CreateEntity(EntityCreationParameters& para)
{
	if (para.entityName == "") {
		return nullptr;
	}

	//naming of entity internally
	string entityName = para.entityName; //temporary, should have entity name in scene file
	int sameNameEntityCnt = 1;
	while (sceneEntitiesMap.count(entityName)) {
		entityName = para.entityName + " (" + to_string(sameNameEntityCnt) + ")";
		sameNameEntityCnt++;
	}
	para.entityName = entityName;

	Entity e;
	Entity* allocatedEntity = nullptr;
	Mesh* mesh = nullptr;
	Material* mat = nullptr;

	bool success = false;

	if (para.meshName != "") {
		if (generatedMeshesMap.count(para.meshName)) {
			mesh = generatedMeshesMap[para.meshName];
			e = Entity(para.entityName, mesh);
		}
		else if (defaultMeshesMap.count(para.meshName)) {
			mesh = defaultMeshesMap[para.meshName];
			e = Entity(para.entityName, mesh);
		}
		else {
			return nullptr;
		}

		allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool(Utility::ENTITY_POOL, sizeof(Entity), success);
		if (success) {
			*allocatedEntity = e;
			sceneEntitiesMap.insert({ para.entityName, allocatedEntity });
			sceneEntities.push_back(allocatedEntity);
		}

		if (para.materialName != "") {
			if (generatedMaterialsMap.count(para.materialName)) {
				mat = generatedMaterialsMap[para.materialName];
				allocatedEntity->AddMaterial(mat, true);
			}
			else if (defaultMaterialsMap.count(para.materialName)) {
				mat = defaultMaterialsMap[para.materialName];
				allocatedEntity->AddMaterial(mat, true);
			}
			else {
				para.materialName = "DEFAULT";
				mat = defaultMaterialsMap["DEFAULT"];
				allocatedEntity->AddMaterial(mat, true);
			}
		}
	}
	else {
		e = Entity(para.entityName);
		para.drawEntity = false;

		allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool(Utility::ENTITY_POOL, sizeof(Entity), success);
		if (success) {
			*allocatedEntity = e;
			sceneEntitiesMap.insert({ para.entityName, allocatedEntity });
			sceneEntities.push_back(allocatedEntity);
		}
	}

	if (para.collisionsEnabled)
		allocatedEntity->AddAutoBoxCollider();

	allocatedEntity->SetPosition(para.position);
	allocatedEntity->SetRotation(para.rotationRadians);
	allocatedEntity->SetScale(para.scale);
	
	allocatedEntity->CalcWorldMatrix();

	if (para.drawShadow)
		allocatedEntity->ToggleShadows(true);
	
	if (EERenderer != nullptr && para.drawEntity)
		EERenderer->AddRenderObject(allocatedEntity, mesh, mat);
	if (para.initRigidBody)
		allocatedEntity->InitRigidBody(Config::DynamicsWorld, para.entityMass);

	if (Config::DebugLinesEnabled && allocatedEntity->colliderDebugLinesEnabled) {
		vector<Collider*> colliders = allocatedEntity->GetColliders();
		for (size_t d = 0; d < colliders.size(); d++)
		{
			DebugLines* dl = new DebugLines(entityName, d);
			XMFLOAT3 c = XMFLOAT3(1.0f, 0.0f, 0.0f);
			dl->color = c;
			dl->worldMatrix = colliders[d]->GetWorldMatrix();
			XMFLOAT3* colliderCorners = colliders[d]->GetPivotShiftedColliderCorners();
			dl->GenerateCuboidVertexBuffer(colliderCorners, 8);
		}
	}

	return allocatedEntity;
}
