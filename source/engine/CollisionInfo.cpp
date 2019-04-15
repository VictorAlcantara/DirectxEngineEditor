#include "../engine/CollisionInfo.h"
#include "../engine/CoreInterface.h"

/****************************************************************************************
	ColliderInfo
*****************************************************************************************/

ColliderInfo::ColliderInfo(eColliderInfo type) : type(type), aabbScale(1.0f), boundingSphereRadius(1.0f) 
{
	hDebugAabb = g_pScene->Debug_AddCube();
	hDebugAabb->color4 = math::normalizeRgb( 0, 255, 150 );
	hDebugAabb->visible = false;

	hDebugBoundingSphere = g_pScene->Debug_AddSphere();
	hDebugBoundingSphere->color4 = math::normalizeRgb( 0, 255, 150 );
	hDebugBoundingSphere->visible = false;

	pfncClassify = nullptr;
	pfncIgnore = nullptr;
}

ColliderInfo::~ColliderInfo()
{
	g_pScene->Debug_Remove( hDebugAabb );
	g_pScene->Debug_Remove( hDebugBoundingSphere );
}

/****************************************************************************************
	ColliderInfo_Aabb
*****************************************************************************************/

ColliderInfo_Aabb::~ColliderInfo_Aabb()
{
}

/****************************************************************************************
	ColliderInfo_Sphere
*****************************************************************************************/

ColliderInfo_Sphere::~ColliderInfo_Sphere()
{
}

/****************************************************************************************
	ColliderInfo_Ellipse
*****************************************************************************************/

ColliderInfo_Ellipse::ColliderInfo_Ellipse() : ColliderInfo(eColliderInfo::Ellipse), scale(1.0f)
{
	hDebugEllipse = g_pScene->Debug_AddSphere();
	hDebugEllipse->color4 = math::normalizeRgb( 255, 255, 0 );
	hDebugEllipse->visible = false;
}

ColliderInfo_Ellipse::~ColliderInfo_Ellipse()
{
	g_pScene->Debug_Remove( hDebugEllipse );
}
