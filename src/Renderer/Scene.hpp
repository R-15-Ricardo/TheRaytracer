#pragma once

#include "../Entity/Camera.hpp"
#include "../Entity/LightSource.hpp"
#include "../Entity/Object.hpp"

#include <string>

struct Scene {
	Camera camera;

	std::vector<LightSource*> lightSources;
	Texture skybox;
	double ambientLight;

	std::vector<Object*> objects;
};

Scene loadFromJSON(const std::string path);