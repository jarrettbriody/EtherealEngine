#pragma once
#include <DirectXMath.h>
#include "Lights.h"
#include "SimpleShader.h"
#include <map>
#include <string>

using namespace std;

#define MAX_LIGHTS 32

class Renderer
{
private:
	map<std::string, Light> lights;
	int lightCount = 0;
public:
	Renderer();
	~Renderer();
	bool AddLight(std::string name, Light &newLight);
	bool RemoveLight(std::string name);
	void SendAllLightsToShader(SimplePixelShader* pixelShader);
};

