#pragma once

#include "../engine/Entity.h"
#include "../engine/Component_Collision.h"

class Entity_Refraction : public Entity
{
	CLASS_TAG( Entity_Refraction );

	public:
		Entity_Refraction();
		virtual ~Entity_Refraction();

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void UpdateCollisionInfo();

		HandleAs<Component, Component_Collision> m_hCollision;
};
