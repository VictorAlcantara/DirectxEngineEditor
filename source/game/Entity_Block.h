#pragma once

#include "../engine/Entity.h"
#include "../engine/Component_Collision.h"

class Entity_Block : public Entity
{
	CLASS_TAG( Entity_Block );

	public:
		Entity_Block();
		virtual ~Entity_Block();

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void UpdateCollisionInfo();

		HandleAs<Component, Component_Collision> m_hCollision;
};
