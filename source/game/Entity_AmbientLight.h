#pragma once

#include "../engine/Entity.h"
#include "../engine/Handle.h"

class Entity_AmbientLight : public Entity
{
	CLASS_TAG( Entity_AmbientLight );

	public:
		Entity_AmbientLight();
		virtual ~Entity_AmbientLight() = default;

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnShutdown()override final;
};
