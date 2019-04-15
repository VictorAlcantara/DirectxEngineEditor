#pragma once

#include "../engine/Component.h"
#include "../engine/CollisionInfo.h"

class Component_Trigger : public Component
{
	CLASS_TAG( Component_Trigger );

	public:
		Component_Trigger();
		virtual ~Component_Trigger() = default;

		ColliderInfo_Aabb* GetCollider();
		const ColliderInfo_Aabb* GetCollider()const;

		void SetNoclip(bool);
		bool Noclip()const { return m_noclip; }

		virtual bool Ignore(const Handle<Entity>&)const;

		virtual void OnTrigger_Enter(Handle<Entity>&) {}
		virtual void OnTrigger_Update(Handle<Entity>&) {}
		virtual void OnTrigger_Exit(Handle<Entity>&) {}

	protected:
		void OnInitialize()override final;

	private:
		bool m_noclip;
		SmartPtr<ColliderInfo_Aabb> m_pCollider;
};
