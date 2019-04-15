#pragma once

#include "CoreModule.h"
#include "../engine/Table.h"
#include "../engine/Handle.h"
#include "../engine/Math.h"
#include "../engine/EventListener.h"

class Entity;
class Component;
class Component_Physics;

class Physics : public EventListener
{
	CORE_MODULE;

	public:
		~Physics();

		void Update();

		void SetGravity(const math::Vec3& gravityAccel) { m_gravity = gravityAccel; }

	private:
		Physics();

		void Initialize();
		void Shutdown();

		void HandleEvent(Event*)override final;

		bool m_initialized;
		math::Vec3 m_gravity;
		Table<HandleAs<Component, Component_Physics>, uint> m_components;
};
