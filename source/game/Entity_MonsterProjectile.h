#pragma once

#include "../engine/Entity.h"
#include "../engine/Component_Collision.h"
#include "../engine/Component_Physics.h"

/****************************************************************************************
	Component_Collision_MonsterProjectile
*****************************************************************************************/

class Component_Collision_MonsterProjectile : public Component_Collision
{
	CLASS_TAG( Component_Collision_MonsterProjectile );

	public:
		Component_Collision_MonsterProjectile();
		virtual ~Component_Collision_MonsterProjectile() = default;

		virtual bool Ignore(const Handle<Entity>&)const override final;
};

/****************************************************************************************
	Component_Physics_MonsterProjectile
*****************************************************************************************/

class Component_Physics_MonsterProjectile : public Component_Physics
{
	CLASS_TAG( Component_Physics_MonsterProjectile );

	public:
		Component_Physics_MonsterProjectile();
		virtual ~Component_Physics_MonsterProjectile() = default;

		void OnPhysicsUpdate(Contact*, uint contactCount)override final;
};

/****************************************************************************************
	Entity_MonsterProjectile
*****************************************************************************************/

class Entity_MonsterProjectile : public Entity
{
	friend class Component_Physics_MonsterProjectile;
	CLASS_TAG( Entity_MonsterProjectile );

	public:
		Entity_MonsterProjectile();
		virtual ~Entity_MonsterProjectile() = default;

		void SetIsMini(bool);

	protected:
		void Update()override final;
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void UpdateScale();

		bool m_isMini;
		bool m_isShrinking;
		float m_scaleBase;
		float m_scaleFactor;
		float m_shrinkTime;
		float m_shrinkCurrentTime;
		float m_entityCurrentTime;

		HandleAs<Component, Component_Collision_MonsterProjectile> m_hCollision;
		HandleAs<Component, Component_Physics_MonsterProjectile> m_hPhysics;
};

