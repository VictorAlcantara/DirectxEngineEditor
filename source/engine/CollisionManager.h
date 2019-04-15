#pragma once

#include "CoreModule.h"
#include "../engine/EventListener.h"
#include "../engine/Handle.h"
#include "../engine/Table.h"
#include "../engine/CollisionInfo.h"

class Entity;
struct Debug_Line;
class Component;
class Component_Trigger;
class Component_Collision;
class PartitionCollision;

class CollisionManager : public EventListener
{
	CORE_MODULE;

	public:
		~CollisionManager();	

		void QueryAndSortContacts(Handle<Entity>, const math::Vec3& entityVelocity, float deltaTime, uint maxContacts, List<Contact>&);
		uint CalculateContacts(Handle<Entity> hEntity, math::Vec3& inOutvelocity, float deltaTime, Contact*, uint maxContacts);
		Contact CalculateContact(Handle<Entity> hEntityCurrent, const math::Vec3& entityPos, Handle<Entity> hEntityTestingAgainst, const math::Vec3& velocity, float deltaTime);

		uint Raycast(const math::Vec3& rayPos, const math::Vec3& rayDir, RaycastInfo* contacts, uint maxRaycasts, float rayMaxLength = 0.0f);

		void Update();

		void CreatePartition(const math::Vec3& pos, const math::Vec3& scale, bool debugAddEntity, const String& debugEntityContact);

	private:
		CollisionManager();

		void Initialize();
		void Shutdown();

		Contact CalculateContact_Aabb_Aabb(Handle<Entity> hEntityAabb1, const math::Vec3& entity1Pos, Handle<Entity> hEntityAabb2, const math::Vec3& velocity, float deltaTime);
		Contact CalculateContact_Sphere_Aabb(Handle<Entity> hEntitySphere, const math::Vec3& entity1Pos, Handle<Entity> hEntityAabb, const math::Vec3& velocity, float deltaTime);
		Contact CalculateContact_Aabb_Sphere(Handle<Entity> hEntityAabb, const math::Vec3& entity1Pos, Handle<Entity> hEntitySphere, const math::Vec3& velocity, float deltaTime);
		Contact CalculateContact_Ellipse_Aabb(Handle<Entity> hEntityEllipse, const math::Vec3& entity1Pos, Handle<Entity> hEntityAabb, const math::Vec3& velocity, float deltaTime);

		void HideNormals();
		void ShowNormal(const math::Vec3& point0, const math::Vec3& point1);

		void HandleEvent(Event*)override final;

		bool m_initialized;
		SimplePtr<PartitionCollision> m_pPartition;
		Table<HandleAs<Component, Component_Collision>, uint> m_collisions;
		Table<HandleAs<Component, Component_Collision>, uint> m_triggerCollisionCandidate;
		Table<HandleAs<Component, Component_Trigger>, uint> m_triggers;
		Table<uint, uint> m_triggerIdContacts;

		Handle<Debug_Line> m_normals[50];
		uint m_currentNormal;
		bool m_isQuerying;
};
