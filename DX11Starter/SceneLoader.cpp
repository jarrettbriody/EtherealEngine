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

	for (auto compSIter = computeShadersMap.begin(); compSIter != computeShadersMap.end(); ++compSIter)
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
	meshMap.insert({ meshName, allocMesh });

	return allocMesh;
}

ID3D11ShaderResourceView* SceneLoader::LoadTexture(string texName, string texPath, map<string, ID3D11ShaderResourceView*>& texMap)
{
	ID3D11ShaderResourceView* tex = Utility::LoadSRV(texPath);
	texMap.insert({ texName, tex });
	return tex;
}

Material* SceneLoader::CreateMaterial(string name, MaterialData matData, string vertShaderName, string pixelShaderName, map<string, Material*>& matMap)
{
	bool success;
	Material mat = Material(name, matData, vertexShadersMap[vertShaderName], pixelShadersMap[pixelShaderName], Config::Sampler);
	Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, sizeof(Material), success);
	*allocatedMaterial = mat;
	matMap.insert({ name, allocatedMaterial });
	return allocatedMaterial;
}

XMFLOAT3 SceneLoader::Float3FromString(string str)
{
	smatch match;
	std::sregex_iterator iter(str.begin(), str.end(), transformNumIteratorRegex);
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
			if (!regex_match(line, commentedLineRegex)) {

				if (regex_search(line, match, typeRegex)) {
					type = match[1];

					if (!sceneLineTypes.count(type)) continue;

					int num = sceneLineTypes[type];

					if (regex_search(line, match, entityNameRegex)) {
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
						if (regex_search(line, match, shaderFileRegex)) {
							string cso = match[1];

							ShaderType shaderType = ShaderType::DEFAULT;
							if (regex_search(line, match, shaderTypeRegex)) {
								if (shaderTypes.count(match[1])) {
									shaderType = shaderTypes[match[1]];
								}
							}

							LoadShader<SimpleVertexShader>(name, cso, vertexShadersMap, shaderType);
						}
						else continue;
						break;
					}
					case 5:
					{
						//pshader
						if (regex_search(line, match, shaderFileRegex)) {
							string cso = match[1];

							ShaderType shaderType = ShaderType::DEFAULT;
							if (regex_search(line, match, shaderTypeRegex)) {
								if (shaderTypes.count(match[1])) {
									shaderType = shaderTypes[match[1]];
								}
							}

							LoadShader<SimplePixelShader>(name, cso, pixelShadersMap, shaderType);
						}
						else continue;
						break;
					}
					case 6:
					{
						//cshader
						if (regex_search(line, match, shaderFileRegex)) {
							string cso = match[1];

							ShaderType shaderType = ShaderType::DEFAULT;
							if (regex_search(line, match, shaderTypeRegex)) {
								if (shaderTypes.count(match[1])) {
									shaderType = shaderTypes[match[1]];
								}
							}

							LoadShader<SimpleComputeShader>(name, cso, computeShadersMap, shaderType);
						}
						else continue;
						break;
					}
					case 1:
					{
						//mesh
						if (regex_search(line, match, pathRegex)) {
							string path = match[1];

							LoadMesh(name, path, defaultMeshesMap);
						}
						else continue;
						break;
					}
					case 3:
					{
						//texture
						if (regex_search(line, match, pathRegex)) {
							string path = match[1];

							LoadTexture(name, path, defaultTexturesMap);
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
						
						if (regex_search(line, match, vShaderRegex)) {
							if (vertexShadersMap.count(match[1])) vShader = match[1];
							else vShader = "DEFAULT";
						}

						if (regex_search(line, match, pShaderRegex)) {
							if (pixelShadersMap.count(match[1])) pShader = match[1];
							else pShader = "DEFAULT";
						}

						if (regex_search(line, match, ambientTexRegex)) {
							if (defaultTexturesMap.count(match[1])) matData.AmbientTextureMapSRV = defaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, diffuseTexRegex)) {
							if (defaultTexturesMap.count(match[1])) matData.DiffuseTextureMapSRV = defaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, specColorTexRegex)) {
							if (defaultTexturesMap.count(match[1])) matData.SpecularColorTextureMapSRV = defaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, specHighlightTexRegex)) {
							if (defaultTexturesMap.count(match[1])) matData.SpecularHighlightTextureMapSRV = defaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, alphaTexRegex)) {
							if (defaultTexturesMap.count(match[1])) matData.AlphaTextureMapSRV = defaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, normalTexRegex)) {
							if (defaultTexturesMap.count(match[1])) matData.NormalTextureMapSRV = defaultTexturesMap[match[1]];
						}

						if (regex_search(line, match, ambientColorRegex)) {
							matData.AmbientColor = Float3FromString(match[1]);
						}

						if (regex_search(line, match, diffuseColorRegex)) {
							matData.DiffuseColor = Float3FromString(match[1]);
						}

						if (regex_search(line, match, specularColorRegex)) {
							matData.SpecularColor = Float3FromString(match[1]);
						}

						if (regex_search(line, match, specularExponentRegex)) {
							matData.SpecularExponent = std::stof(match[1].str());
						}

						if (regex_search(line, match, transparencyRegex)) {
							matData.Transparency = std::stof(match[1].str());
						}

						if (regex_search(line, match, illuminationRegex)) {
							matData.Illumination = std::stoi(match[1].str());
						}

						if (regex_search(line, match, ssaoRegex)) {
							if (match[1] == "true" || match[1] == "TRUE") matData.SSAO = true;
							if (match[1] == "false" || match[1] == "FALSE") matData.SSAO = false;
						}

						if (regex_search(line, match, hbaoPlusRegex)) {
							if (match[1] == "true" || match[1] == "TRUE") matData.hbaoPlusEnabled = true;
							if (match[1] == "false" || match[1] == "FALSE") matData.hbaoPlusEnabled = false;
						}

						CreateMaterial(name, matData, vShader, pShader, defaultMaterialsMap);

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
	if (defaultMeshesMap.count(name))
		return MESH_TYPE::DEFAULT_MESH;

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

	generatedMeshesMap.insert({ name, newMesh });

	//record mesh as utilized
	utilizedMeshesMap.insert({ name,true });

	string mtlPath = generatedMeshesMap[name]->GetMTLPath();

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
						someMaterial = Material(ongoingMatName, matData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler);
					}
					else {
						someMaterial = Material(ongoingMatName, matData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
					}

					matData = {};

					//alloc the material to memallocator
					bool success = false;
					Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, sizeof(Material), success);
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
			someMaterial = Material(ongoingMatName, matData, vertexShadersMap["Normal"], pixelShadersMap["Normal"], Config::Sampler);
		}
		else {
			someMaterial = Material(ongoingMatName, matData, vertexShadersMap["DEFAULT"], pixelShadersMap["DEFAULT"], Config::Sampler);
		}

		matData = {};
		ongoingMat = false;

		//alloc the material to memallocator
		bool success = false;
		Material* allocatedMaterial = (Material*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, sizeof(Material), success);
		if (success) {
			*allocatedMaterial = someMaterial;
			generatedMaterialsMap.insert({ ongoingMatName, allocatedMaterial });

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
	for (size_t i = 0; i < sceneEntities.size(); i++)
	{
		sceneEntities[i]->FreeMemory();
		EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sceneEntities[i], sizeof(Entity));
	}
	sceneEntities.clear();
	sceneEntitiesMap.clear();
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
			if (!regex_match(line, commentedLineRegex)) {

				if (regex_search(line, match, typeRegex)) {
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
							if (regex_search(line, match, skyboxRegex)) {
								string name = match[1];
								if (!generatedTexturesMap.count(name)) {
									generatedTexturesMap.insert({ "DDS_" + name, Utility::LoadDDSSRV(name + ".dds") });
									//record texture as utilized
									utilizedTexturesMap.insert({ "DDS_" + name,true });
								}
								EERenderer->SetSkybox(generatedTexturesMap["DDS_" + name]);
							}
							continue;
						}
					case 8:
						{
							//directional light
							string lightName;
							Light * dLight = new Light;
							dLight->Type = LIGHT_TYPE_DIR;
							if (regex_search(line, match, entityNameRegex)) {
								lightName = match[1];
							}
							if (regex_search(line, match, lightPosRegex)) {
								string transformData = match[0];
								dLight->Position = Float3FromString(transformData);
							}
							if (regex_search(line, match, lightDirRegex)) {
								string transformData = match[0];
								dLight->Direction = Float3FromString(transformData);
							}
							if (regex_search(line, match, lightColorRegex)) {
								string transformData = match[0];
								dLight->Color = Float3FromString(transformData);
							}
							if (regex_search(line, match, lightIntensityRegex)) {
								string transformData = match[1];
								dLight->Intensity = std::stof(transformData);
							}
							EERenderer->AddLight(lightName, dLight);
							continue;
						}
					default:
						break;
					}
				}

				MESH_TYPE meshType;

				//search for OBJ name at start of line
				if (regex_search(line, match, objNameRegex)) {
					objName = match[1];
					//load mesh, material, and textures, and if they already exist then mark them as utilized
					meshType = AutoLoadOBJMTL(objName);
				}
				else
					meshType = MESH_TYPE::EMPTY_OBJECT;

				Entity someEntity;

				//search for entity name in scene file
				string originalEntityName = regex_search(line, match, entityNameRegex) ? match[1] : objName;
				string entityName = originalEntityName;

				//check if entity name already exists, if it does then add (1), (2), etc
				int sameNameEntityCnt = 1;
				while (sceneEntitiesMap.count(entityName)) {
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
					someEntity = Entity(entityName, defaultMeshesMap[objName]);
					break;
				case MESH_TYPE::GENERATED_MESH: {
					someEntity = Entity(entityName, generatedMeshesMap[objName]);
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
					sceneEntitiesMap.insert({ entityName, allocatedEntity });
					sceneEntities.push_back(allocatedEntity);
				}

				if (meshType == MESH_TYPE::GENERATED_MESH) {
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

				//check for manual material
				if (meshType == MESH_TYPE::DEFAULT_MESH) {
					if (regex_search(line, match, materialNameRegex)) {
						if (defaultMaterialsMap.count(match[1]))
							allocatedEntity->AddMaterial(defaultMaterialsMap[match[1]], true);
					}
					else {
						allocatedEntity->AddMaterial(defaultMaterialsMap["DEFAULT"], true);
					}
				}

				//check for texture repeat
				if (regex_search(line, match, repeatTextureRegex))
					allocatedEntity->SetRepeatTexture(std::stof(match[1].str()), std::stof(match[2].str()));

				//check for entity tag
				if (regex_search(line, match, tagNameRegex))
					allocatedEntity->tag = match[1];

				//check for entity layer
				if (regex_search(line, match, layerNameRegex))
					allocatedEntity->layer = match[1];

				//check for scripts
				if (regex_search(line, match, scriptNamesRegex)) {
					string scripts = match[1].str();
					std::sregex_iterator iter(scripts.begin(), scripts.end(), scriptNamesIteratorRegex);
					for (; iter != std::sregex_iterator(); ++iter) {
						match = *iter;
						string script = match.str();
						scriptCallback(allocatedEntity, script);
					}
				}

				//check for transformation data associated with this entity
				if (regex_search(line, match, transformationDataRegex)) {
					string transformData = match[0];
					std::sregex_iterator iter(transformData.begin(), transformData.end(), transformNumIteratorRegex);
					int counter = 0;
					for (; iter != std::sregex_iterator(); ++iter) {
						if (counter < 9) {
							match = *iter;
							parsedNumbers[counter] = std::stof(match.str());
						}
						counter++;
					}
					allocatedEntity->SetPosition(parsedNumbers[0], parsedNumbers[1], parsedNumbers[2]);
					allocatedEntity->SetRotation(DirectX::XMConvertToRadians(parsedNumbers[3]), DirectX::XMConvertToRadians(parsedNumbers[4]), DirectX::XMConvertToRadians(parsedNumbers[5]));
					allocatedEntity->SetScale(parsedNumbers[6], parsedNumbers[7], parsedNumbers[8]);
					allocatedEntity->CalcWorldMatrix();
				}

				//check if object is collision enabled
				if (regex_search(line, match, collidersEnabledRegex)) {
					string collidersEnabled = match[1];
					if (collidersEnabled == "true" || collidersEnabled == "TRUE") {
						allocatedEntity->collisionsEnabled = true;
						allocatedEntity->AddAutoBoxCollider();
						float mass = 0.0f;
						BulletColliderShape collShape = BulletColliderShape::BOX;

						//check if there is a mass
						if (regex_search(line, match, massRegex)) {
							mass = std::stof(match[1].str());
						}
						
						//check if there is a collider type
						if (regex_search(line, match, colliderTypeRegex)) {
							string collType = match[1];
							if (bulletColliders.count(collType)) {
								collShape = bulletColliders[collType];
							}
						}
						allocatedEntity->isCollisionStatic = (mass == 0.0f);
						allocatedEntity->InitRigidBody(collShape, mass);
					}
				}
				
				//check for debug lines
				if (regex_search(line, match, debugRegex)) {
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
		EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::MESH_POOL, toDelete, sizeof(Mesh));
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
		EEMemoryAllocator->DeallocateFromPool((unsigned int)MEMORY_POOL::MATERIAL_POOL, toDelete, sizeof(Material));
		generatedMaterialsMap.erase(materialsToDelete[i]);
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

		allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
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

		allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
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
		allocatedEntity->InitRigidBody(para.bulletColliderShape, para.entityMass);

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

	for (size_t i = 0; i < para.scriptCount; i++)
	{
		scriptCallback(allocatedEntity, para.scriptNames[i]);
	}

	allocatedEntity->tag = para.tagName;
	allocatedEntity->layer = para.layerName;

	return allocatedEntity;
}

void SceneLoader::SplitMeshIntoChildEntities(Entity* e, float componentMass)
{
	int meshChildCnt = e->GetMeshChildCount();
	if (meshChildCnt == 0) return;
	bool success;
	Mesh** children = e->GetMesh()->GetChildren();
	for (size_t i = 0; i < meshChildCnt; i++)
	{
		Entity newE(children[i]->GetName(), children[i], e->GetMaterial(e->GetMeshMaterialName(i)));
		Entity* allocatedEntity = (Entity*)EEMemoryAllocator->AllocateToPool((unsigned int)MEMORY_POOL::ENTITY_POOL, sizeof(Entity), success);
		*allocatedEntity = newE;
		//e->AddChildEntity(allocatedEntity);
		//e->CalcWorldMatrix();
		allocatedEntity->SetPosition(e->GetPosition());
		XMFLOAT3 r = e->GetEulerAngles();
		r.y += DirectX::XM_PI;
		allocatedEntity->SetRotation(r);
		//allocatedEntity->SetRotation(e->GetRotationQuaternion());
		allocatedEntity->SetScale(e->GetScale());
		allocatedEntity->CalcWorldMatrix();
		allocatedEntity->AddAutoBoxCollider();
		allocatedEntity->InitRigidBody(BulletColliderShape::BOX, componentMass, true);
		sceneEntitiesMap.insert({ children[i]->GetName(), allocatedEntity });
		sceneEntities.push_back(allocatedEntity);
		EERenderer->AddRenderObject(allocatedEntity, children[i], e->GetMaterial(e->GetMeshMaterialName(i)));
	}
	//e->EmptyEntity();
	e->Destroy();
}
