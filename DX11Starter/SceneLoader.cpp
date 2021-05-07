#include "pch.h"
#include "SceneLoader.h"

SceneLoader* SceneLoader::instance = nullptr;

SceneLoader::SceneLoader()
{
	this->EEMemoryAllocator = MemoryAllocator::GetInstance();
	this->EELightHandler = LightHandler::GetInstance();
}

SceneLoader::~SceneLoader()
{
	//defaults
	for (auto texMapIter = Texture2DMap.begin(); texMapIter != Texture2DMap.end(); ++texMapIter)
	{
		texMapIter->second->Release();
		//cout << "Deleting " << texMapIter->first << endl;
	}

	for (auto texMapIter = DefaultTexturesMap.begin(); texMapIter != DefaultTexturesMap.end(); ++texMapIter)
	{
		texMapIter->second->Release();
		//cout << "Deleting " << texMapIter->first << endl;
	}

	for (auto matMapIter = DefaultMaterialsMap.begin(); matMapIter != DefaultMaterialsMap.end(); ++matMapIter)
	{
		matMapIter->second->FreeMemory();
		//cout << "Deleting " << matMapIter->first << endl;
	}

	for (auto meshMapIter = DefaultMeshesMap.begin(); meshMapIter != DefaultMeshesMap.end(); ++meshMapIter)
	{
		try
		{
			//if (meshMapIter->second->GetCenteredMesh() != nullptr) meshMapIter->second->GetCenteredMesh()->FreeMemory();
			meshMapIter->second->FreeMemory();
		}
		catch (const std::exception&)
		{
			continue;
		}
		//cout << "Deleting " << meshMapIter->first << endl;
	}

	//generated
	for (auto texMapIter = GeneratedTexturesMap.begin(); texMapIter != GeneratedTexturesMap.end(); ++texMapIter)
	{
		try
		{
			texMapIter->second->Release();
		}
		catch (const std::exception&)
		{
			continue;
		}
		//cout << "Releasing " << texMapIter->first << endl;
	}

	for (auto matMapIter = GeneratedMaterialsMap.begin(); matMapIter != GeneratedMaterialsMap.end(); ++matMapIter)
	{
		try
		{
			matMapIter->second->FreeMemory();
		}
		catch (const std::exception&)
		{
			continue;
		}
		//cout << "Deleting " << matMapIter->first << endl;
	}

	for (auto meshMapIter = GeneratedMeshesMap.begin(); meshMapIter != GeneratedMeshesMap.end(); ++meshMapIter)
	{
		//delete meshMapIter->second;
		//if (meshMapIter->second->GetCenteredMesh() != nullptr) meshMapIter->second->GetCenteredMesh()->FreeMemory();
		try
		{
			meshMapIter->second->FreeMemory();
		}
		catch (const std::exception&)
		{
			continue;
		}
		//cout << "Deleting " << meshMapIter->first << endl;
	}

	for (size_t i = 0; i < SceneEntities.size(); i++)
	{
		//delete sceneEntities[i];
		SceneEntities[i]->FreeMemory();
	}

	//delete shaders
	for (auto vertSIter = VertexShadersMap.begin(); vertSIter != VertexShadersMap.end(); ++vertSIter)
	{
		delete vertSIter->second;
	}

	for (auto pixSIter = PixelShadersMap.begin(); pixSIter != PixelShadersMap.end(); ++pixSIter)
	{
		delete pixSIter->second;
	}

	for (auto compSIter = ComputeShadersMap.begin(); compSIter != ComputeShadersMap.end(); ++compSIter)
	{
		delete compSIter->second;
	}
}

template<class SHADER>
inline SHADER* SceneLoader::LoadShader(string shaderName, string shaderFileName, map<string, SHADER*>& shaderMap, ShaderType shaderType)
{
	SHADER* shader = new SHADER();
	shaderFileName += ".cso";
	LPCWSTR wStr = Utility::StringToWideString(shaderFileName);
	shader->LoadShaderFile(wStr);
	shader->SetShaderType(shaderType);
	shaderMap.insert({ shaderName, shader });
	delete[] wStr;
	return shader;
}

Mesh* SceneLoader::LoadMesh(string meshName, string meshPath, map<string, Mesh*>& meshMap)
{
	bool success;

	Mesh* allocMesh = (Mesh*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MESH_POOL, sizeof(Mesh), success);
	Mesh mesh = Mesh(meshName, (char*)(("../../Assets/Models/" + meshPath).c_str()));
	*allocMesh = mesh;
	if (allocMesh->GetChildCount() > 0)
		allocMesh->AllocateChildren();
	meshMap.insert({ meshName, allocMesh });

	return allocMesh;
}

ID3D11ShaderResourceView* SceneLoader::LoadTexture(string texName, string texPath, map<string, ID3D11ShaderResourceView*>& texMap, bool keepTex2D)
{
	ID3D11Resource* tex2D = nullptr;
	ID3D11ShaderResourceView* tex = Utility::LoadSRV(texPath, keepTex2D ? &tex2D : nullptr);
	texMap.insert({ texName, tex });
	if (keepTex2D) {
		ID3D11Texture2D* newTex2D = reinterpret_cast<ID3D11Texture2D*>(tex2D);
		Texture2DMap.insert({ texName, newTex2D });
	}
	return tex;
}

Material* SceneLoader::CreateMaterial(string name, MaterialData matData, string vertShaderName, string pixelShaderName, map<string, Material*>& matMap)
{
	bool success;
	Material mat = Material(name, matData, VertexShadersMap[vertShaderName], PixelShadersMap[pixelShaderName], Config::Sampler);
	Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = mat;
	matMap.insert({ name, allocatedMaterial });
	return allocatedMaterial;
}

XMFLOAT3 SceneLoader::Float3FromString(string str)
{
	smatch match;
	std::sregex_iterator iter(str.begin(), str.end(), RegexObjects.transformNumIteratorRegex);
	int counter = 0;
	float parsedNumbers[3];
	for (; iter != std::sregex_iterator(); ++iter) {
		if (counter < 3) {
			match = *iter;
			try
			{
				parsedNumbers[counter] = std::stof(match.str());
			}
			catch (const std::exception&)
			{
				cout << "Invalid format: " << str << endl;
			}
		}
		counter++;
	}
	return XMFLOAT3(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);
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

void SceneLoader::GarbageCollect()
{
	size_t start = SceneEntities.size();
	for (size_t i = start; i > 0; i--)
	{
		Entity* e = SceneEntities[i - 1];
		if (e->destroyed) {
			string name = e->GetName();

			EEString<EESTRING_SIZE>* tags = e->GetTags();
			for (size_t j = 0; j < e->GetTagCount(); j++)
			{
				if (SceneEntitiesTagMap.count(tags[j].STDStr())) {
					vector<Entity*>& tagVec = SceneEntitiesTagMap[tags[j].STDStr()];
					for (int k = tagVec.size() - 1; k >= 0; k--)
					{
						if (tagVec[k] == e) tagVec.erase(tagVec.begin() + k);
					}
				}
			}

			EEString<EESTRING_SIZE>* layers = e->GetLayers();
			for (size_t j = 0; j < e->GetLayerCount(); j++)
			{
				if (SceneEntitiesLayerMap.count(layers[j].STDStr())) {
					vector<Entity*>& layerVec = SceneEntitiesLayerMap[layers[j].STDStr()];
					for (int k = layerVec.size() - 1; k >= 0; k--)
					{
						if (layerVec[k] == e) layerVec.erase(layerVec.begin() + k);
					}
				}
			}

			if(SceneEntitiesMap.count(name))
				SceneEntitiesMap.erase(name);
			SceneEntities.erase(SceneEntities.begin() + i - 1);

			garbageCollectCallback->callback(e);

			e->FreeMemory();
			EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::ENTITY_POOL, e, sizeof(Entity));
		}
	}
}

void SceneLoader::LoadAssetPreloadFile()
{
	ifstream infile("../../Assets/EEAssetPreload.txt");
	string line;
	smatch match;

	float parsedNumbers[9];
	string name;
	string type;

	while (getline(infile, line))
	{
		//cout << line << endl;
		if (line != "") {
			//if the line does not start with "//"
			if (!regex_match(line, RegexObjects.commentedLineRegex)) {

				if (regex_search(line, match, RegexObjects.typeRegex)) {
					type = match[1];

					if (!sceneLineTypes.count(type)) continue;

					int num = sceneLineTypes[type];

					if (regex_search(line, match, RegexObjects.entityNameRegex)) {
						name = match[1];
					}
					else continue;

					/*
					{ "ENTITY", 0 },
					{ "MESH", 1 },
					{ "MATERIAL", 2 },
					{ "TEXTURE", 3 },
					{ "VSHADER", 4 },
					{ "PSHADER", 5 },
					{ "CSHADER", 6 },
					{ "SKYBOX", 7 },
					{ "DIRLIGHT", 8 },
					{ "CPUPARTICLE", 9 },
					{ "GPUPARTICLE", 10 },
					*/

					switch (num)
					{
					case 4: 
					{
						//vshader
						if (regex_search(line, match, RegexObjects.shaderFileRegex)) {
							string cso = match[1];

							ShaderType shaderType = ShaderType::DEFAULT;
							if (regex_search(line, match, RegexObjects.shaderTypeRegex)) {
								if (shaderTypes.count(match[1])) {
									shaderType = shaderTypes[match[1]];
								}
							}

							LoadShader<SimpleVertexShader>(name, cso, VertexShadersMap, shaderType);
						}
						else continue;
						break;
					}
					case 5:
					{
						//pshader
						if (regex_search(line, match, RegexObjects.shaderFileRegex)) {
							string cso = match[1];

							ShaderType shaderType = ShaderType::DEFAULT;
							if (regex_search(line, match, RegexObjects.shaderTypeRegex)) {
								if (shaderTypes.count(match[1])) {
									shaderType = shaderTypes[match[1]];
								}
							}

							LoadShader<SimplePixelShader>(name, cso, PixelShadersMap, shaderType);
						}
						else continue;
						break;
					}
					case 6:
					{
						//cshader
						if (regex_search(line, match, RegexObjects.shaderFileRegex)) {
							string cso = match[1];

							ShaderType shaderType = ShaderType::DEFAULT;
							if (regex_search(line, match, RegexObjects.shaderTypeRegex)) {
								if (shaderTypes.count(match[1])) {
									shaderType = shaderTypes[match[1]];
								}
							}

							LoadShader<SimpleComputeShader>(name, cso, ComputeShadersMap, shaderType);
						}
						else continue;
						break;
					}
					case 1:
					{
						//mesh
						if (regex_search(line, match, RegexObjects.pathRegex)) {
							string path = match[1];

							LoadMesh(name, path, DefaultMeshesMap);
						}
						else continue;
						break;
					}
					case 3:
					{
						//texture
						if (regex_search(line, match, RegexObjects.pathRegex)) {
							string path = match[1];

							bool saveTex = false;
							if (regex_search(line, match, RegexObjects.texArrayRegex)) {
								saveTex = (match[1] == "true" || match[1] == "TRUE");
							}

							LoadTexture(name, path, DefaultTexturesMap, saveTex);
						}
						else continue;
						break;
					}
					case 2:
					{
						//material

						MaterialData matData;

						string vShader;
						string pShader;
						
						if (regex_search(line, match, RegexObjects.vShaderRegex)) {
							if (VertexShadersMap.count(match[1])) vShader = match[1];
							else vShader = "DEFAULT";
						}

						if (regex_search(line, match, RegexObjects.pShaderRegex)) {
							if (PixelShadersMap.count(match[1])) pShader = match[1];
							else pShader = "DEFAULT";
						}

						if (regex_search(line, match, RegexObjects.ambientTexRegex)) {
							if (DefaultTexturesMap.count(match[1])) matData.AmbientTextureMapSRV = DefaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, RegexObjects.diffuseTexRegex)) {
							if (DefaultTexturesMap.count(match[1])) matData.DiffuseTextureMapSRV = DefaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, RegexObjects.specColorTexRegex)) {
							if (DefaultTexturesMap.count(match[1])) matData.SpecularColorTextureMapSRV = DefaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, RegexObjects.specHighlightTexRegex)) {
							if (DefaultTexturesMap.count(match[1])) matData.SpecularHighlightTextureMapSRV = DefaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, RegexObjects.alphaTexRegex)) {
							if (DefaultTexturesMap.count(match[1])) matData.AlphaTextureMapSRV = DefaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, RegexObjects.normalTexRegex)) {
							if (DefaultTexturesMap.count(match[1])) matData.NormalTextureMapSRV = DefaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, RegexObjects.ambientColorRegex)) {
							matData.AmbientColor = Float3FromString(match[1]);
						}

						if (regex_search(line, match, RegexObjects.diffuseColorRegex)) {
							matData.DiffuseColor = Float3FromString(match[1]);
						}

						if (regex_search(line, match, RegexObjects.specularColorRegex)) {
							matData.SpecularColor = Float3FromString(match[1]);
						}

						if (regex_search(line, match, RegexObjects.specularExponentRegex)) {
							matData.SpecularExponent = std::stof(match[1].str());
						}

						if (regex_search(line, match, RegexObjects.transparencyRegex)) {
							matData.Transparency = std::stof(match[1].str());
						}

						if (regex_search(line, match, RegexObjects.illuminationRegex)) {
							matData.Illumination = std::stoi(match[1].str());
						}

						if (regex_search(line, match, RegexObjects.ssaoRegex)) {
							if (match[1] == "true" || match[1] == "TRUE") matData.SSAO = true;
							if (match[1] == "false" || match[1] == "FALSE") matData.SSAO = false;
						}

						if (regex_search(line, match, RegexObjects.hbaoPlusRegex)) {
							if (match[1] == "true" || match[1] == "TRUE") matData.hbaoPlusEnabled = true;
							if (match[1] == "false" || match[1] == "FALSE") matData.hbaoPlusEnabled = false;
						}

						CreateMaterial(name, matData, vShader, pShader, DefaultMaterialsMap);

						break;
					}
					case 11:
					{
						//layermask
						if (regex_search(line, match, RegexObjects.layerOffsetRegex)) {
							unsigned int offset = stoi(match[1]);
							if (regex_search(line, match, RegexObjects.layerValueRegex)) {
								unsigned int value = stoi(match[1]);
								Config::EntityLayers.insert({ name, (value << (offset * 4)) });
							}
						}
						else continue;
						break;
					}
					default:
						break;
					}
				}
			}
		}
	}
}

MESH_TYPE SceneLoader::AutoLoadOBJMTL(string name)
{
	//mesh exists in default meshes map, which will remain untouched during program execution
	if (DefaultMeshesMap.count(name))
		return MESH_TYPE::DEFAULT_MESH;

	//if mesh is already loaded
	if (GeneratedMeshesMap.count(name)) {
		//if the mesh is not already recorded as utilized, utilize it
		if (!utilizedMeshesMap.count(name)) {
			utilizedMeshesMap.insert({ name, true });
			//get the material names utilized under the mesh
			vector<string> utilizedMaterials = GeneratedMeshesMap[name]->GetMaterialNameList();
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
		return MESH_TYPE::GENERATED_MESH;
	}

	string objPath = modelPath + name + ".obj";

	//Mesh will change bool ref to false if OBJ file does not exist, otherwise it will generate it and add to map
	bool success;
	Mesh* newMesh = (Mesh*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MESH_POOL, sizeof(Mesh), success);
	if (!success) {
		cout << "Cannot load Object (OBJ) file: " + string(objPath) << endl;
		return MESH_TYPE::LOAD_FAILURE;
	}

	Mesh someMesh = Mesh(name, (char*)objPath.c_str(), &success);
	*newMesh = someMesh;
	if(newMesh->GetChildCount() > 0)
		newMesh->AllocateChildren();
	if (!success) {
		cout << "Cannot load Object (OBJ) file: " + string(objPath) << endl;
		return MESH_TYPE::LOAD_FAILURE;
	}

	GeneratedMeshesMap.insert({ name, newMesh });

	//record mesh as utilized
	utilizedMeshesMap.insert({ name,true });

	string mtlPath = GeneratedMeshesMap[name]->GetMTLPath();

	if (mtlPath == "") {
		cout << "Material Template Library (MTL) link not found inside \"" + name + ".obj\". If this is unintentional, link MTL file inside OBJ file." << endl;
		return MESH_TYPE::GENERATED_MESH;
	}

	ifstream infile(modelPath + mtlPath);

	if (!infile.is_open()) {
		cout << "Material Template Library (MTL) file not found. Please include MTL file in same directory as OBJ file or remove the internal OBJ link to " + mtlPath + "." << endl;
		return MESH_TYPE::GENERATED_MESH;
	}

	using namespace Utility;

	bool ongoingMat = false;
	string ongoingMatName = "";
	MaterialData matData;
	bool clampSampler = false;

	string line;
	smatch match;

	while (getline(infile, line)) {
		if (line != "" && !regex_search(line, match, regex("^#"))) {
			//search for new material line
			if (regex_search(line, match, RegexObjects.newMtlRgx)) {
				line = regex_replace(line, RegexObjects.newMtlRgx, "");
				//new material line was found but a material was in progress, complete this material before continuing
				if (ongoingMat && !GeneratedMaterialsMap.count(ongoingMatName)) {
					Material someMaterial;

					//Different shaders based on matData values
					if (matData.NormalTextureMapSRV) {
						someMaterial = Material(ongoingMatName, matData, VertexShadersMap["Normal"], PixelShadersMap["Normal"], (clampSampler) ? Config::ClampSampler : Config::Sampler);
					}
					else {
						someMaterial = Material(ongoingMatName, matData, VertexShadersMap["DEFAULT"], PixelShadersMap["DEFAULT"], (clampSampler) ? Config::ClampSampler : Config::Sampler);
					}

					matData = {};

					//alloc the material to memallocator
					bool success = false;
					Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, sizeof(Material), success);
					if (success) {
						*allocatedMaterial = someMaterial;
						GeneratedMaterialsMap.insert({ ongoingMatName, allocatedMaterial });

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
			else if (regex_search(line, match, RegexObjects.ambientColorRgx)) {
				line = regex_replace(line, RegexObjects.ambientColorRgx, "");
				ParseFloat3FromString(line, matData.AmbientColor);
			}
			//diffuse color
			else if (regex_search(line, match, RegexObjects.diffuseColorRgx)) {
				line = regex_replace(line, RegexObjects.diffuseColorRgx, "");
				ParseFloat3FromString(line, matData.DiffuseColor);
			}
			//specular color
			else if (regex_search(line, match, RegexObjects.specularColorRgx)) {
				line = regex_replace(line, RegexObjects.specularColorRgx, "");
				ParseFloat3FromString(line, matData.SpecularColor);
			}
			//specular value
			else if (regex_search(line, match, RegexObjects.specularExpRgx)) {
				line = regex_replace(line, RegexObjects.specularExpRgx, "");
				ParseFloatFromString(line, matData.SpecularExponent);
			}
			//transparency value
			else if (regex_search(line, match, RegexObjects.dTransparencyRgx)) {
				line = regex_replace(line, RegexObjects.dTransparencyRgx, "");
				ParseFloatFromString(line, matData.Transparency);
			}
			//transparency value
			else if (regex_search(line, match, RegexObjects.trTransparencyRgx)) {
				line = regex_replace(line, RegexObjects.trTransparencyRgx, "");
				ParseFloatFromString(line, matData.Transparency);
				matData.Transparency = 1.0f - matData.Transparency;
			}
			//illumination value
			else if (regex_search(line, match, RegexObjects.illuminationRgx)) {
				line = regex_replace(line, RegexObjects.illuminationRgx, "");
				ParseIntFromString(line, matData.Illumination);
			}
			//check for texture repeat
			else if (regex_search(line, match, RegexObjects.repeatTextureRegex))
				matData.repeatTexture = XMFLOAT2(std::stof(match[1].str()), std::stof(match[2].str()));
			//check for texture clamp
			else if (regex_search(line, match, RegexObjects.clampTexRegex)) {
				if(match[1] == "true" || match[1] == "TRUE")
					clampSampler = true;
			}
			else if (regex_search(line, match, RegexObjects.uvOffsetRegex)) {
				matData.uvOffset = XMFLOAT2(std::stof(match[1].str()), std::stof(match[2].str()));
			}
			//ambient occlusion map
			else if (regex_search(line, match, RegexObjects.ambientTextureRgx)) {
				line = regex_replace(line, RegexObjects.ambientTextureRgx, "");
				if (!GeneratedTexturesMap.count(line)) {
					GeneratedTexturesMap.insert({ line, Utility::LoadSRV(line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.AmbientTextureMapSRV = GeneratedTexturesMap[line];
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
			//diffuse map
			else if (regex_search(line, match, RegexObjects.diffuseTextureRgx)) {
				line = regex_replace(line, RegexObjects.diffuseTextureRgx, "");
				if (!GeneratedTexturesMap.count(line)) {
					GeneratedTexturesMap.insert({ line, Utility::LoadSRV(line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.DiffuseTextureMapSRV = GeneratedTexturesMap[line];
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
			//specular color map
			else if (regex_search(line, match, RegexObjects.specularColorTextureRgx)) {
				line = regex_replace(line, RegexObjects.specularColorTextureRgx, "");
				if (!GeneratedTexturesMap.count(line)) {
					GeneratedTexturesMap.insert({ line, Utility::LoadSRV(line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.SpecularColorTextureMapSRV = GeneratedTexturesMap[line];
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
			//specular highlight map
			else if (regex_search(line, match, RegexObjects.specularHighlightTextureRgx)) {
				line = regex_replace(line, RegexObjects.specularHighlightTextureRgx, "");
				if (!GeneratedTexturesMap.count(line)) {
					GeneratedTexturesMap.insert({ line, Utility::LoadSRV(line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.SpecularHighlightTextureMapSRV = GeneratedTexturesMap[line];
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
			//alpha map
			else if (regex_search(line, match, RegexObjects.alphaTextureRgx)) {
				line = regex_replace(line, RegexObjects.alphaTextureRgx, "");
				if (!GeneratedTexturesMap.count(line)) {
					GeneratedTexturesMap.insert({ line, Utility::LoadSRV(line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.AlphaTextureMapSRV = GeneratedTexturesMap[line];
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
			//bump map
			else if (regex_search(line, match, RegexObjects.normalTextureRgx)) {
				line = regex_replace(line, RegexObjects.normalTextureRgx, "");
				if (!GeneratedTexturesMap.count(line)) {
					GeneratedTexturesMap.insert({ line, Utility::LoadSRV(line) });

					//record texture as utilized
					utilizedTexturesMap.insert({ line,true });
				}
				matData.NormalTextureMapSRV = GeneratedTexturesMap[line];
				materialTextureAssociationMap[ongoingMatName].push_back(line);
			}
		}
	}
	//basically only executes if the end of the file is reached and there was an ongoing material being created
	if (ongoingMat && !GeneratedMaterialsMap.count(ongoingMatName)) {
		Material someMaterial;

		//Different shaders based on matData values
		if (matData.NormalTextureMapSRV) {
			someMaterial = Material(ongoingMatName, matData, VertexShadersMap["Normal"], PixelShadersMap["Normal"], (clampSampler) ? Config::ClampSampler : Config::Sampler);
		}
		else {
			someMaterial = Material(ongoingMatName, matData, VertexShadersMap["DEFAULT"], PixelShadersMap["DEFAULT"], (clampSampler) ? Config::ClampSampler : Config::Sampler);
		}

		matData = {};
		ongoingMat = false;

		//alloc the material to memallocator
		bool success = false;
		Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, sizeof(Material), success);
		if (success) {
			*allocatedMaterial = someMaterial;
			GeneratedMaterialsMap.insert({ ongoingMatName, allocatedMaterial });

			//record material as utilized
			utilizedMaterialsMap.insert({ ongoingMatName,true });
		}
	}
	infile.close();
	return MESH_TYPE::GENERATED_MESH;
}

void SceneLoader::LoadScene(string sceneName)
{
	//remove all current entities loaded
	for (size_t i = 0; i < SceneEntities.size(); i++)
	{
		SceneEntities[i]->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::ENTITY_POOL, SceneEntities[i], sizeof(Entity));
	}
	SceneEntities.clear();
	SceneEntitiesMap.clear();
	SceneEntitiesTagMap.clear();
	utilizedMeshesMap.clear();
	utilizedMaterialsMap.clear();
	utilizedTexturesMap.clear();

	ifstream infile("../../Assets/Scenes/" + sceneName + ".txt");
	string line;
	smatch match;
	float parsedNumbers[9];
	string objName;
	string type;
	while (getline(infile, line))
	{
		//cout << line << endl;
		if (line != "") {
			//if the line does not start with "//"
			if (!regex_match(line, RegexObjects.commentedLineRegex)) {

				if (regex_search(line, match, RegexObjects.typeRegex)) {
					type = match[1];

					if (!sceneLineTypes.count(type)) continue;

					int num = sceneLineTypes[type];

					/*
					{ "ENTITY", 0 },
					{ "MESH", 1 },
					{ "MATERIAL", 2 },
					{ "TEXTURE", 3 },
					{ "VSHADER", 4 },
					{ "PSHADER", 5 },
					{ "CSHADER", 6 },
					{ "SKYBOX", 7 }, 
					{ "DIRLIGHT", 8 },
					{ "CPUPARTICLE", 9 },
					{ "GPUPARTICLE", 10 },
					*/

					switch (num)
					{
					case 0: break; //entity

					case 7:
						{
							//skybox
							if (regex_search(line, match, RegexObjects.skyboxRegex)) {
								string name = match[1];
								if (!GeneratedTexturesMap.count(name)) {
									GeneratedTexturesMap.insert({ "DDS_" + name, Utility::LoadDDSSRV(name + ".dds") });
									//record texture as utilized
									utilizedTexturesMap.insert({ "DDS_" + name,true });
								}
								EERenderer->SetSkybox(GeneratedTexturesMap["DDS_" + name]);
							}
							continue;
						}
					case 8:
						{
							//directional light
							string lightName;
							Light dLight;
							dLight.Type = LIGHT_TYPE_DIR;
							if (regex_search(line, match, RegexObjects.entityNameRegex)) {
								lightName = match[1];
							}
							if (regex_search(line, match, RegexObjects.lightPosRegex)) {
								string transformData = match[0];
								dLight.Position = Float3FromString(transformData);
							}
							if (regex_search(line, match, RegexObjects.lightDirRegex)) {
								string transformData = match[0];
								dLight.Direction = Float3FromString(transformData);
							}
							if (regex_search(line, match, RegexObjects.lightColorRegex)) {
								string transformData = match[0];
								dLight.Color = Float3FromString(transformData);
							}
							if (regex_search(line, match, RegexObjects.lightIntensityRegex)) {
								string transformData = match[1];
								dLight.Intensity = std::stof(transformData);
							}
							LightContainer newLight = { dLight, lightName };
							EELightHandler->AddLight(newLight);
							continue;
						}

					case 12:
					{
						//ambientocclusion
						if (regex_search(line, match, RegexObjects.enabledRegex)) {
							if (match[1] == "true" || match[1] == "TRUE") Config::HBAOPlusEnabled = true;
							if (match[1] == "false" || match[1] == "FALSE") Config::HBAOPlusEnabled = false;
						}
						if (regex_search(line, match, RegexObjects.blurEnabledRegex)) {
							if (match[1] == "true" || match[1] == "TRUE") Config::HBAOBlurEnabled = true;
							if (match[1] == "false" || match[1] == "FALSE") Config::HBAOBlurEnabled = false;
						}
						if (regex_search(line, match, RegexObjects.radiusRegex)) {
							Config::HBAORadius = std::stof(match[1].str());
						}
						if (regex_search(line, match, RegexObjects.powerRegex)) {
							Config::HBAOPowerExponent = std::stof(match[1].str());
						}
						if (regex_search(line, match, RegexObjects.blurSharpnessRegex)) {
							Config::HBAOBlurSharpness = std::stof(match[1].str());
						}
						if (regex_search(line, match, RegexObjects.blurRadiusRegex)) {
							Config::HBAOBlurRadius = (GFSDK_SSAO_BlurRadius)std::stoi(match[1].str());
						}
						EERenderer->InitHBAOPlus();
						continue;
					}
					case 13:
					{
						//ambient light
						if (regex_search(line, match, RegexObjects.lightIntensityRegex)) {
							Config::SceneBrightness = std::stof(match[1].str());
						}
						continue;
					}
					case 14:
					{
						//point light
						string lightName;
						Light dLight;
						dLight.Type = LIGHT_TYPE_POINT;
						if (regex_search(line, match, RegexObjects.entityNameRegex)) {
							lightName = match[1];
						}
						if (regex_search(line, match, RegexObjects.lightPosRegex)) {
							string transformData = match[0];
							dLight.Position = Float3FromString(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightColorRegex)) {
							string transformData = match[0];
							dLight.Color = Float3FromString(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightIntensityRegex)) {
							string transformData = match[1];
							dLight.Intensity = std::stof(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightRangeRegex)) {
							string transformData = match[1];
							dLight.Range = std::stof(transformData);
						}
						LightContainer newLight = { dLight, lightName };
						EELightHandler->AddLight(newLight);
						continue;
					}
					case 15:
					{
						//spot light
						string lightName;
						Light dLight;
						dLight.Type = LIGHT_TYPE_SPOT;
						if (regex_search(line, match, RegexObjects.entityNameRegex)) {
							lightName = match[1];
						}
						if (regex_search(line, match, RegexObjects.lightPosRegex)) {
							string transformData = match[0];
							dLight.Position = Float3FromString(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightDirRegex)) {
							string transformData = match[0];
							dLight.Direction = Float3FromString(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightColorRegex)) {
							string transformData = match[0];
							dLight.Color = Float3FromString(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightIntensityRegex)) {
							string transformData = match[1];
							dLight.Intensity = std::stof(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightRangeRegex)) {
							string transformData = match[1];
							dLight.Range = std::stof(transformData);
						}
						if (regex_search(line, match, RegexObjects.lightSpotFalloff)) {
							string transformData = match[1];
							dLight.SpotFalloff = std::stof(transformData);
						}
						LightContainer newLight = { dLight, lightName };
						EELightHandler->AddLight(newLight);
						continue;
					}
					default:
						break;
					}
				}

				MESH_TYPE meshType;

				//search for OBJ name at start of line
				if (regex_search(line, match, RegexObjects.objNameRegex)) {
					objName = match[1];
					//load mesh, material, and textures, and if they already exist then mark them as utilized
					meshType = AutoLoadOBJMTL(objName);
				}
				else
					meshType = MESH_TYPE::EMPTY_OBJECT;

				Entity someEntity;

				//search for entity name in scene file
				string originalEntityName = regex_search(line, match, RegexObjects.entityNameRegex) ? match[1] : objName;
				string entityName = originalEntityName;

				//check if entity name already exists, if it does then add (1), (2), etc
				int sameNameEntityCnt = 1;
				while (SceneEntitiesMap.count(entityName)) {
					entityName = originalEntityName + " (" + to_string(sameNameEntityCnt) + ")";
					sameNameEntityCnt++;
				}

				//figure out what map to pull from
				switch (meshType) {
				case MESH_TYPE::LOAD_FAILURE:
					continue;
				case MESH_TYPE::EMPTY_OBJECT:
					if (entityName == "") continue;
					someEntity = Entity(entityName);
					someEntity.isEmptyObj = true;
					break;
				case MESH_TYPE::DEFAULT_MESH:
					someEntity = Entity(entityName, DefaultMeshesMap[objName]);
					break;
				case MESH_TYPE::GENERATED_MESH: {
					someEntity = Entity(entityName, GeneratedMeshesMap[objName]);
					break;
				}
				default:
					break;
				}

				//finally add the entity to the appropriate lists
				bool success = false;
				Entity* allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
				if (success) {
					//allocatedEntity->CopyCollections(&someEntity);
					//allocatedEntity->SetupCollections();
					*allocatedEntity = someEntity;
					SceneEntitiesMap.insert({ entityName, allocatedEntity });
					SceneEntities.push_back(allocatedEntity);
				}

				if (meshType == MESH_TYPE::GENERATED_MESH) {
					//generated meshes should have a list of required materials,
					//add them if they do or add the default (just black) if they dont
					vector<string> requiredMaterials = allocatedEntity->GetMaterialNameList();
					for (int i = 0; i < requiredMaterials.size(); i++)
					{
						string requiredMat = requiredMaterials[i];
						if (GeneratedMaterialsMap.count(requiredMat))
							allocatedEntity->AddMaterial(GeneratedMaterialsMap[requiredMat]);
					}
					if (requiredMaterials.size() == 0) {
						allocatedEntity->AddMaterial(DefaultMaterialsMap["DEFAULT"]);
					}
				}

				//check for manual material
				if (meshType == MESH_TYPE::DEFAULT_MESH) {
					if (regex_search(line, match, RegexObjects.materialNameRegex)) {
						if (DefaultMaterialsMap.count(match[1]))
							allocatedEntity->AddMaterial(DefaultMaterialsMap[match[1]], true);
						else if(GeneratedMaterialsMap.count(match[1]))
							allocatedEntity->AddMaterial(GeneratedMaterialsMap[match[1]], true);
					}
					else {
						allocatedEntity->AddMaterial(DefaultMaterialsMap["DEFAULT"], true);
					}
				}

				//uv offset
				if (regex_search(line, match, RegexObjects.uvOffsetRegex)) {
					allocatedEntity->SetUVOffset(std::stof(match[1].str()), std::stof(match[2].str()));
				}

				//hbaoplus enabled
				if (regex_search(line, match, RegexObjects.hbaoPlusRegex)) {
					if (match[1] == "true" || match[1] == "TRUE") allocatedEntity->ToggleHBAOPlus(true);
					if (match[1] == "false" || match[1] == "FALSE") allocatedEntity->ToggleHBAOPlus(false);
				}

				//check for texture repeat
				if (regex_search(line, match, RegexObjects.repeatTextureRegex))
					allocatedEntity->SetRepeatTexture(std::stof(match[1].str()), std::stof(match[2].str()));

				//check for entity tag
				if (regex_search(line, match, RegexObjects.tagNameRegex)) {
					string tags = match[1].str();
					std::sregex_iterator iter(tags.begin(), tags.end(), RegexObjects.scriptNamesIteratorRegex);
					for (; iter != std::sregex_iterator(); ++iter) {
						match = *iter;
						string tag = match.str();
						if (!SceneEntitiesTagMap.count(tag)) SceneEntitiesTagMap.insert({ tag, vector<Entity*>() });
						SceneEntitiesTagMap[tag].push_back(allocatedEntity);
						allocatedEntity->AddTag(tag);
					}
				}

				//check for entity layer
				if (regex_search(line, match, RegexObjects.layerNameRegex)) {
					string layers = match[1].str();
					std::sregex_iterator iter(layers.begin(), layers.end(), RegexObjects.scriptNamesIteratorRegex);
					for (; iter != std::sregex_iterator(); ++iter) {
						match = *iter;
						string layer = match.str();
						if (!SceneEntitiesLayerMap.count(layer)) SceneEntitiesLayerMap.insert({ layer, vector<Entity*>() });
						SceneEntitiesLayerMap[layer].push_back(allocatedEntity);
						allocatedEntity->AddLayer(layer);
					}
				}

				//check for transformation data associated with this entity
				if (regex_search(line, match, RegexObjects.transformationDataRegex)) {
					string transformData = match[0];
					std::sregex_iterator iter(transformData.begin(), transformData.end(), RegexObjects.transformNumIteratorRegex);
					int counter = 0;
					for (; iter != std::sregex_iterator(); ++iter) {
						if (counter < 9) {
							match = *iter;
							parsedNumbers[counter] = std::stof(match.str());
						}
						counter++;
					}
					allocatedEntity->SetPosition(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);

					if (regex_search(line, match, RegexObjects.raaRegex)) {
						if (match[1] == "true" || match[1] == "TRUE") {
							allocatedEntity->RotateAroundAxis(Z_AXIS, DirectX::XMConvertToRadians(parsedNumbers[5]));
							allocatedEntity->RotateAroundAxis(Y_AXIS, DirectX::XMConvertToRadians(parsedNumbers[4]));
							allocatedEntity->RotateAroundAxis(X_AXIS, DirectX::XMConvertToRadians(parsedNumbers[3]));
						}
						else {
							allocatedEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
						}
					}
					else {
						allocatedEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
					}
					allocatedEntity->SetScale(parsedNumbers[6], parsedNumbers[7], parsedNumbers[8]);
					allocatedEntity->CalcWorldMatrix();
				}

				if (regex_search(line, match, RegexObjects.quaternionRegex)) {
					string transformData = match[0];
					std::sregex_iterator iter(transformData.begin(), transformData.end(), RegexObjects.transformNumIteratorRegex);
					int counter = 0;
					for (; iter != std::sregex_iterator(); ++iter) {
						if (counter < 4) {
							match = *iter;
							parsedNumbers[counter] = std::stof(match.str());
						}
						counter++;
					}
					allocatedEntity->SetRotation(XMFLOAT4(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2], parsedNumbers[3]));
					allocatedEntity->CalcWorldMatrix();
				}

				//check if object is collision enabled
				if (regex_search(line, match, RegexObjects.collidersEnabledRegex)) {
					string collidersEnabled = match[1];
					if (collidersEnabled == "true" || collidersEnabled == "TRUE") {
						allocatedEntity->collisionsEnabled = true;
						allocatedEntity->AddAutoBoxCollider();
						float mass = 0.0f;
						BulletColliderShape collShape = BulletColliderShape::BOX;

						//check if there is a mass
						if (regex_search(line, match, RegexObjects.massRegex)) {
							mass = std::stof(match[1].str());
						}
						
						//check if there is a collider type
						if (regex_search(line, match, RegexObjects.colliderTypeRegex)) {
							string collType = match[1];
							if (bulletColliders.count(collType)) {
								collShape = bulletColliders[collType];
							}
						}
						allocatedEntity->isCollisionStatic = (mass == 0.0f);
						allocatedEntity->InitRigidBody(collShape, mass);

						vector<Collider*> colls = allocatedEntity->GetColliders();
						Mesh* mesh = allocatedEntity->GetMesh();
						if (colls.size() > 0 && mesh != nullptr) {
							UINT childCnt = mesh->GetChildCount();
							XMFLOAT3 offset;
							if (childCnt == 0) {
								if (mesh->GetCenteredMesh() == nullptr) {
									offset = colls[0]->GetCenterLocal();
									XMStoreFloat3(&offset, XMVectorScale(XMLoadFloat3(&offset), -1.0f));
									mesh->GenerateCenteredMesh(offset);
								}
							}
							else {
								Mesh** children = mesh->GetChildren();
								for (size_t i = 0; i < colls.size(); i++)
								{
									if (children[i]->GetCenteredMesh() == nullptr) {
										offset = colls[i]->GetCenterLocal();
										XMStoreFloat3(&offset, XMVectorScale(XMLoadFloat3(&offset), -1.0f));
										children[i]->GenerateCenteredMesh(offset);
									}
								}
							}
						}
					}
				}
				
				//check for debug lines
				if (regex_search(line, match, RegexObjects.debugRegex)) {
					string debugEnabled = match[1];
					if (debugEnabled == "true" || debugEnabled == "TRUE") {
						allocatedEntity->colliderDebugLinesEnabled = true;
						if (Config::EtherealDebugLinesEnabled && allocatedEntity->collisionsEnabled) {
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
					}
				}

				//check for scripts
				if (regex_search(line, match, RegexObjects.scriptNamesRegex)) {
					string scripts = match[1].str();
					std::sregex_iterator iter(scripts.begin(), scripts.end(), RegexObjects.scriptNamesIteratorRegex);
					for (; iter != std::sregex_iterator(); ++iter) {
						match = *iter;
						string script = match.str();
						//scriptCallback(allocatedEntity, script);
						scriptPairs.push_back({ allocatedEntity, script });
					}
				}
			}
		}
	}

	infile.close();

	//clean up memory from prior scene, wont reload any resources that already exist and that
	//are needed, but will remove unused resources in the current scene
	vector<string> meshesToDelete;
	for (auto meshMapIter = GeneratedMeshesMap.begin(); meshMapIter != GeneratedMeshesMap.end(); ++meshMapIter)
	{
		if (!utilizedMeshesMap.count(meshMapIter->first)) {
			meshesToDelete.push_back(meshMapIter->first);
		}
	}
	for (size_t i = 0; i < meshesToDelete.size(); i++)
	{
		Mesh* toDelete = GeneratedMeshesMap[meshesToDelete[i]];
		toDelete->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::MESH_POOL, toDelete, sizeof(Mesh));
		GeneratedMeshesMap.erase(meshesToDelete[i]);
	}

	vector<string> texturesToDelete;
	for (auto texMapIter = GeneratedTexturesMap.begin(); texMapIter != GeneratedTexturesMap.end(); ++texMapIter)
	{
		if (!utilizedTexturesMap.count(texMapIter->first)) {
			texturesToDelete.push_back(texMapIter->first);
		}
	}
	for (size_t i = 0; i < texturesToDelete.size(); i++)
	{
		GeneratedTexturesMap[texturesToDelete[i]]->Release();
		GeneratedTexturesMap.erase(texturesToDelete[i]);
	}

	vector<string> materialsToDelete;
	for (auto matMapIter = GeneratedMaterialsMap.begin(); matMapIter != GeneratedMaterialsMap.end(); ++matMapIter)
	{
		if (!utilizedMaterialsMap.count(matMapIter->first)) {
			materialsToDelete.push_back(matMapIter->first);
		}
	}
	for (size_t i = 0; i < materialsToDelete.size(); i++)
	{
		Material* toDelete = GeneratedMaterialsMap[materialsToDelete[i]];
		toDelete->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, toDelete, sizeof(Material));
		GeneratedMaterialsMap.erase(materialsToDelete[i]);
	}
}

void SceneLoader::SetModelPath(string path)
{
	modelPath = path;
}

void SceneLoader::SetScriptLoader(void(*callback)(Entity* e, string script))
{
	scriptCallback = callback;
}

Entity* SceneLoader::CreateEntity(EntityCreationParameters& para)
{
	if (para.entityName == "") {
		return nullptr;
	}

	//naming of entity internally
	string entityName = para.entityName; //temporary, should have entity name in scene file
	int sameNameEntityCnt = 1;
	while (SceneEntitiesMap.count(entityName)) {
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
		if (GeneratedMeshesMap.count(para.meshName)) {
			mesh = GeneratedMeshesMap[para.meshName];
			e = Entity(para.entityName, mesh);
		}
		else if (DefaultMeshesMap.count(para.meshName)) {
			mesh = DefaultMeshesMap[para.meshName];
			e = Entity(para.entityName, mesh);
		}
		else {
			return nullptr;
		}

		allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
		if (success) {
			*allocatedEntity = e;
			SceneEntitiesMap.insert({ para.entityName, allocatedEntity });
			SceneEntities.push_back(allocatedEntity);
		}

		if (para.materialName != "") {
			if (GeneratedMaterialsMap.count(para.materialName)) {
				mat = GeneratedMaterialsMap[para.materialName];
				allocatedEntity->AddMaterial(mat, true);
			}
			else if (DefaultMaterialsMap.count(para.materialName)) {
				mat = DefaultMaterialsMap[para.materialName];
				allocatedEntity->AddMaterial(mat, true);
			}
			else {
				para.materialName = "DEFAULT";
				mat = DefaultMaterialsMap["DEFAULT"];
				allocatedEntity->AddMaterial(mat, true);
			}
		}
	}
	else {
		e = Entity(para.entityName);
		para.drawEntity = false;

		allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
		if (success) {
			*allocatedEntity = e;
			SceneEntitiesMap.insert({ para.entityName, allocatedEntity });
			SceneEntities.push_back(allocatedEntity);
		}
	}

	allocatedEntity->SetPosition(para.position);
	allocatedEntity->SetRotation(para.rotationRadians);
	allocatedEntity->SetScale(para.scale);

	allocatedEntity->CalcWorldMatrix();

	if (para.collisionsEnabled) {
		allocatedEntity->collisionsEnabled = true;
		allocatedEntity->AddAutoBoxCollider();
	}

	if (para.drawShadow)
		allocatedEntity->ToggleShadows(true);

	if (EERenderer != nullptr && para.drawEntity)
		EERenderer->AddRenderObject(allocatedEntity, mesh, mat);

	if (para.initRigidBody) {
		float mass = para.entityMass;
		allocatedEntity->isCollisionStatic = (mass == 0.0f);
		allocatedEntity->InitRigidBody(para.bulletColliderShape, para.entityMass);
	}

	vector<Collider*> colls = allocatedEntity->GetColliders();
	if (colls.size() > 0 && mesh != nullptr) {
		UINT childCnt = mesh->GetChildCount();
		XMFLOAT3 offset;
		if (childCnt == 0) {
			if (mesh->GetCenteredMesh() == nullptr) {
				offset = colls[0]->GetCenterLocal();
				XMStoreFloat3(&offset, XMVectorScale(XMLoadFloat3(&offset), -1.0f));
				mesh->GenerateCenteredMesh(offset);
			}
		}
		else {
			Mesh** children = mesh->GetChildren();
			for (size_t i = 0; i < colls.size(); i++)
			{
				if (children[i]->GetCenteredMesh() == nullptr) {
					offset = colls[i]->GetCenterLocal();
					XMStoreFloat3(&offset, XMVectorScale(XMLoadFloat3(&offset), -1.0f));
					children[i]->GenerateCenteredMesh(offset);
				}
			}
		}
	}

	if (Config::EtherealDebugLinesEnabled && allocatedEntity->colliderDebugLinesEnabled) {
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

	if (para.tagName != "") {
		allocatedEntity->AddTag(para.tagName);
		if (!SceneEntitiesTagMap.count(para.tagName)) SceneEntitiesTagMap.insert({ para.tagName, vector<Entity*>() });
		SceneEntitiesTagMap[para.tagName].push_back(allocatedEntity);
	}

	if (para.layerName != "") {
		allocatedEntity->AddLayer(para.layerName);
		if (!SceneEntitiesLayerMap.count(para.layerName)) SceneEntitiesLayerMap.insert({ para.layerName, vector<Entity*>() });
		SceneEntitiesLayerMap[para.layerName].push_back(allocatedEntity);
	}

	for (size_t i = 0; i < para.scriptCount; i++)
	{
		scriptCallback(allocatedEntity, para.scriptNames[i]);
	}

	return allocatedEntity;
}

std::vector<Entity*> SceneLoader::SplitMeshIntoChildEntities(Entity* e, float componentMass)
{
	std::vector<Entity*> childEntities;

	int meshChildCnt = e->GetMeshChildCount();
	if (meshChildCnt == 0) return childEntities;
	bool success;
	Mesh** children = e->GetMesh()->GetChildren();
	vector<Collider*> colls = e->GetColliders();
	
	for (size_t i = 0; i < meshChildCnt; i++)
	{
		Mesh* newCenteredMesh = children[i]->GetCenteredMesh();
		Material* mat = e->GetMaterial(e->GetMeshMaterialName(i));
		Entity newE(children[i]->GetName(), newCenteredMesh, mat);
		Entity* allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
		*allocatedEntity = newE;
		//e->AddChildEntity(allocatedEntity);
		//e->CalcWorldMatrix();
		XMFLOAT3 newPos = e->GetPosition();
		XMFLOAT3 collCenter = colls[i]->GetCenterGlobal();
		XMStoreFloat3(&newPos, XMVectorAdd(XMLoadFloat3(&newPos), XMVectorScale(XMLoadFloat3(&collCenter), 1.0f)));
		allocatedEntity->SetPosition(newPos); // TODO: this informs the rigidbody creation, so is the cause of not having centered rotations? Could create offset in the Mesh class by creating a new mesh but this effects everything. 
		XMFLOAT4 r = e->GetRotationQuaternion();
		allocatedEntity->SetRotation(r);
		//allocatedEntity->SetRotation(e->GetRotationQuaternion());
		allocatedEntity->SetScale(e->GetScale());
		allocatedEntity->CalcWorldMatrix();
		allocatedEntity->AddAutoBoxCollider();
		allocatedEntity->InitRigidBody(BulletColliderShape::BOX, componentMass, true);
		// allocatedEntity->GetRBody()->getWorldTransform().setOrigin(allocatedEntity->GetRBody()->getCenterOfMassPosition());
		SceneEntitiesMap.insert({ children[i]->GetName(), allocatedEntity });
		SceneEntities.push_back(allocatedEntity);
		EERenderer->AddRenderObject(allocatedEntity, newCenteredMesh, mat);

		childEntities.push_back(allocatedEntity);

		allocatedEntity->GetRBody()->applyCentralImpulse(Utility::Float3ToBulletVector(collCenter) * 15.0f);
	}
	//e->EmptyEntity();
	e->Destroy();

	return childEntities;
}

void SceneLoader::DestroyEntity(string entityName)
{
	if (SceneEntitiesMap.count(entityName)) {
		SceneEntitiesMap[entityName]->Destroy();
		SceneEntitiesMap.erase(entityName);
	}
}

void SceneLoader::DestroyEntitiesByTag(string tag)
{
	if (SceneEntitiesTagMap.count(tag)) {
		for (size_t i = 0; i < SceneEntitiesTagMap[tag].size(); i++)
		{
			Entity* e = SceneEntitiesTagMap[tag][i];
			e->Destroy();
			SceneEntitiesMap.erase(e->GetName());
		}
		SceneEntitiesTagMap.erase(tag);
	}
}
