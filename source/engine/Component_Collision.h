#pragma once

#include "../engine/Component.h"
#include "../engine/CollisionInfo.h"
#include "ProcessEngine.h"
#include "EventEngine.h"

class Component_Physics;

class Component_Collision : public Component
{
	CLASS_TAG( Component_Collision );

	public:
		Component_Collision();
		virtual ~Component_Collision() = default;
		
		void SetCollider(eColliderInfo);

		math::Aabb3 GetAabbGlobal()const;

		ColliderInfo* GetCollider(bool updateTransform = false);
		const ColliderInfo* GetCollider()const;

		void SetNoclip(bool);
		bool Noclip()const { return m_noclip; }

		virtual bool Ignore(const Handle<Entity>&)const;
		virtual void ClassifyCollider(const Handle<Entity>& hOther, eColliderInfo& outColliderThis, eColliderInfo& inOutColliderOther)const;

		void UpdateTransform();

		bool HasCollider()const;

		void SetActivateTrigger(bool canActivateTrigger);
		bool CanActivateTrigger()const { return m_canActivateTrigger; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SmartPtr<ColliderInfo> m_pCollider;
		bool m_noclip;
		SmartPtr<Process_UpdateTransform> m_pProcessUpdateTransform;
		bool m_canActivateTrigger;
};
