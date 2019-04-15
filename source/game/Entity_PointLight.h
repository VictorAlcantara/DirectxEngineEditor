#pragma once

#include "../engine/Entity.h"
#include "../engine/Handle.h"
#include "../engine/Light.h"

class Entity_PointLight : public Entity
{
	CLASS_TAG( Entity_PointLight );

	public:
		Entity_PointLight();
		virtual ~Entity_PointLight();

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void Update()override final;
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		Handle<PointLight> m_hLight;
};

