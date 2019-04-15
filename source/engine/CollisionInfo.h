#pragma once

#include "../engine/Math.h"
#include "../engine/Event.h"
#include "../engine/Handle.h"

class Entity;
struct Debug_Cube;
struct Debug_Sphere;
struct Debug_Cylinder;

/****************************************************************************************
	Enum
*****************************************************************************************/

enum class eEntityCollisionAction
{
	None,
	Add,
	Remove,
};

enum class eColliderInfo
{
	None,
	Aabb,
	Sphere,
	Ellipse,
	Trigger,
};

enum class eCollision
{
	Static = 0,
	Dynamic = 1,
	None,
};

/****************************************************************************************
	Typedefs
*****************************************************************************************/

typedef bool (*funcCollisionIgnore)(Handle<Entity> entityMain, Handle<Entity> entityOther);
typedef void (*funcCollisionClassify)(Handle<Entity> entityMain, Handle<Entity> entityOther, eColliderInfo& outEntity1, eColliderInfo& outEntity2);
typedef void (*funcCollisionTriggerEnter)(Handle<Entity> trigger, Handle<Entity> entity);
typedef void (*funcCollisionTriggerUpdate)(Handle<Entity> trigger, Handle<Entity> entity);
typedef void (*funcCollisionTriggerExit)(Handle<Entity> trigger);

/****************************************************************************************
	Struct
*****************************************************************************************/

struct Contact
{
	eCollision collisionType;
	float distSq; // distance between contact point and position of entity (NOT entity contact)
	math::Vec3 oldPos;
	math::Vec3 newPos;
	float collisionAmount;
	math::Vec3 surfaceNormal;
	math::Vec3 surfacePoint;
	Handle<Entity> hEntityContact;

	Contact() : collisionType(eCollision::None), collisionAmount(0.0f), distSq(0.0f) {}
};

struct RaycastInfo
{
	Handle<Entity> hEntity;
	math::IntersectRayAabbInfo intersect;
};

struct ColliderInfo
{
	ColliderInfo() = delete;
	virtual ~ColliderInfo();

	eColliderInfo GetType()const { return type; }

	math::Vec3 aabbScale;
	float boundingSphereRadius;

	Handle<Debug_Sphere> hDebugBoundingSphere;
	Handle<Debug_Cube> hDebugAabb;

	funcCollisionIgnore pfncIgnore;
	funcCollisionClassify pfncClassify;

	protected:
		ColliderInfo(eColliderInfo type);

		eColliderInfo type;
};

struct ColliderInfo_Aabb : public ColliderInfo
{
	ColliderInfo_Aabb() : ColliderInfo(eColliderInfo::Aabb) {}
	virtual ~ColliderInfo_Aabb();
};

struct ColliderInfo_Sphere : public ColliderInfo
{
	ColliderInfo_Sphere() : ColliderInfo(eColliderInfo::Sphere) {}
	virtual ~ColliderInfo_Sphere();
};

struct ColliderInfo_Ellipse : public ColliderInfo
{
	math::Vec3 scale;
	Handle<Debug_Sphere> hDebugEllipse;

	ColliderInfo_Ellipse();
	virtual ~ColliderInfo_Ellipse();
};
