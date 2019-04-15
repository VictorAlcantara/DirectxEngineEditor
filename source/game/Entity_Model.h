#pragma once

#include "../engine/Entity.h"
#include "../engine/Component_Collision.h"

class Entity_Model : public Entity
{
	CLASS_TAG( Entity_Model );

	public:
		Entity_Model();
		virtual ~Entity_Model();

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize()override final;

	private:
		void UpdateCollisionInfo();

		HandleAs<Component, Component_Collision> m_hCollision;
};


