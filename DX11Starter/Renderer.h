#pragma once
#include "Lights.h"
#include <map>
#include <string>

#define MAX_LIGHTS 128

class Renderer
{
private:
	Light lights[MAX_LIGHTS];
	std::map<std::string, int> lightMap;
	int lightCount = 0;
public:
	Renderer();
	~Renderer();
	bool AddLight(std::string name, Light newLight);
	bool RemoveLight(std::string name);
};

