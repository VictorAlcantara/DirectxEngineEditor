#pragma once

#include "../engine/Math.h"

struct PhysicsData
{
	float mass;
	math::Vec3 acceleration;
	float maxVelocity;
	math::Vec3 velocity;
	float friction;

	PhysicsData() : mass(1.0f), maxVelocity(10.0f), friction(0.1f) {}
};
