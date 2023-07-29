#pragma once
#include <string>

struct PhysicsComponent
{
	PhysicsComponent(float vel = 0.0f, float acc = 0.0f, std::string name = "P") : vel(vel), acc(acc), name(name) {}
	std::string name = "PhysicsComponent";
	float vel = 1.0f;
	float acc = 1.0f;
};