#pragma once

#include "../engine/Entity.h"
#include "../engine/Light.h"
#include "../engine/Handle.h"
#include "../engine/Light.h"

class Entity_SpotLight : public Entity
{
	CLASS_TAG(Entity_SpotLight);

	public:
		Entity_SpotLight();
		virtual ~Entity_SpotLight();

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void Update()override final;
		void OnInitialize()override final;
		void OnShutdown()override final;
			 
	private:
		Handle<SpotLight> m_hLight;
};

