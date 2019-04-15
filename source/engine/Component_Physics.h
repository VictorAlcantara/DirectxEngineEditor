#pragma once

#include "../engine/Component.h"
#include "../engine/PhysicsInfo.h"
#include "../engine/CollisionInfo.h"
#include "ProcessEngine.h"

class Component_Physics : public Component
{
	CLASS_TAG( Component_Physics );

	public:
		Component_Physics();
		virtual ~Component_Physics() = default;

		void AddForce(const math::Vec3&);
		void ClearForce();
		const math::Vec3& GetForce()const { return m_force; }

		virtual void OnPhysicsUpdate(Contact*, uint contactCount) {}

		PhysicsData* GetData() { return &m_data; }
		const PhysicsData* GetData()const { return &m_data; }

		void SetPos(const math::Vec3&);
		void AddPos(const math::Vec3&);

		const math::Vec3& GetPos()const { return m_pos; }

		void EnableUpdate(bool);
		bool IsUpdateEnabled()const { return m_updateEnabled; }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void UpdateTransform();

		math::Vec3 m_force;
		math::Vec3 m_pos;
		PhysicsData m_data;

		SmartPtr<Process_UpdateTransform> m_pProcessUpdateTransform;

		bool m_updateEnabled;
};

