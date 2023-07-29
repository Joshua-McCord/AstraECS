#pragma once
#include <string>

struct TransformComponent
{
	TransformComponent(float x = 0.0f, float y = 0.0f, std::string name = "T") : x(x), y(y), name(name) {}
	std::string name = "TransformComponent";
	float x = 0;
	float y = 1;
};