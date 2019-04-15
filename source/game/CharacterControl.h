#pragma once

#include "../engine/Math.h"
#include "../engine/PhysicsInfo.h"

enum eCharacterControlCommand
{
	CHARACTER_CONTROL_NONE = 1 << 0,
	CHARACTER_CONTROL_FORWARD = 1 << 1,
	CHARACTER_CONTROL_BACKWARD = 1 << 2,
	CHARACTER_CONTROL_RIGHT = 1 << 3,
	CHARACTER_CONTROL_LEFT = 1 << 4,
	CHARACTER_CONTROL_TURN_RIGHT = 1 << 5,
	CHARACTER_CONTROL_TURN_LEFT = 1 << 6,
};

struct CharacterControlConfig
{
	float walkForce;

	CharacterControlConfig() : walkForce(10.0f) {}
};

class CharacterControl
{
	public:
		CharacterControl();
		~CharacterControl();

		void Update(uint characterControlCommandFlag, bool grounded, const math::Mtx44& orientation);
		math::Vec3 GetForce()const;

		CharacterControlConfig& GetConfig() { return m_config; }
		const CharacterControlConfig& GetConfig()const { return m_config; }

	private:
		math::Vec3 m_force;
		CharacterControlConfig m_config;
};

