#pragma once

#include "../engine/Component.h"
#include "../engine/Handle.h"

struct Light_Base;

class Component_LightFlicker : public Component
{
	CLASS_TAG( Component_LightFlicker );

	public:
		Component_LightFlicker();
		virtual ~Component_LightFlicker() = default;

		void SetLight(uint lightId);
		void Enable(bool);
		bool IsEnabled()const { return m_enabled; }

	protected:
		void OnInitialize()override final;
		void Update()override final;

	private:
		uint m_lightId;
		float m_timer;
		bool m_enabled;

		void ResetTimer();
};
