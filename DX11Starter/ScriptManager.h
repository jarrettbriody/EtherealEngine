#pragma once
#include "Renderer.h"
#include "SceneLoader.h"

struct EntityCreationParameters {
	string entityName = "";
	string meshName = "";
	string materialName = "";
	XMFLOAT3 position = ZERO_VECTOR3;
	XMFLOAT3 rotationRadians = ZERO_VECTOR3;
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	bool initRigidBody = true;
	bool drawEntity = true;
	bool drawShadow = true;
};

class ScriptManager
{
public:
	static vector<ScriptManager*> scriptFunctions;
	static map<string, vector<ScriptManager*>> scriptFunctionsMap;
	static map<string, Entity*>* sceneEntitiesMap;
	static vector<Entity*>* sceneEntities;
	static Renderer* renderer;
	static SceneLoader* sceneLoader;

	bool destroyed = false;

	bool inputEnabled = false;

	void CallInit();
	void CallUpdate();
	void CallOnMouseDown(WPARAM buttonState, int x, int y);
	void CallOnMouseUp(WPARAM buttonState, int x, int y);
	void CallOnMouseMove(WPARAM buttonState, int x, int y);
	void CallOnMouseWheel(float wheelDelta, int x, int y);

	void Setup(string name, Entity* e);

	virtual ~ScriptManager();
protected:
	bool initialized = false;
	string name;
	Entity* entity;
	ScriptManager();
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void OnMouseDown(WPARAM buttonState, int x, int y) {};
	virtual void OnMouseUp(WPARAM buttonState, int x, int y) {};
	virtual void OnMouseMove(WPARAM buttonState, int x, int y) {};
	virtual void OnMouseWheel(float wheelDelta, int x, int y) {};
	void CreateEntity(EntityCreationParameters& para);
};

