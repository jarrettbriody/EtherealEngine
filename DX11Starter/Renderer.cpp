#include "Renderer.h"



Renderer::Renderer()
{
	
}


Renderer::~Renderer()
{
}

bool Renderer::AddLight(std::string name, Light newLight)
{
	if (lightMap.count(name) || lightCount >= MAX_LIGHTS) {
		return false;
	}
	lights[lightCount] = newLight;
	lightMap.insert({ name, lightCount });
	lightCount++;
	return true;
}

bool Renderer::RemoveLight(std::string name)
{
	if (!lightMap.count(name)) {
		return false;
	}

}
