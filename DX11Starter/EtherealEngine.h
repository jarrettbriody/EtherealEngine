#pragma once
/*
#include <iostream>
#include "Renderer.h"
#include "Camera.h"

using namespace std;

class EtherealEngine
{
private:
	static EtherealEngine* instance;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	Camera* camera;
	Renderer* renderer;

	EtherealEngine();
	
public:
	static EtherealEngine* GetInstance() {
		if (instance == nullptr)
			instance = new EtherealEngine();
		return instance;
	}

	ID3D11Device* GetDevice();
	void SetDevice(ID3D11Device* d);

	ID3D11DeviceContext* GetContext();
	void SetContext(ID3D11DeviceContext* c);

	Camera* GetCamera();
	void SetCamera(Camera* cam);

	Renderer* GetRenderer();
	void SetRenderer(Renderer* rend);
};
*/