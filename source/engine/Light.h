#pragma once

#include "../engine/Math.h"
#include "../engine/String.h"

class LightManager;

enum class eLight
{
	None = 0,
	PointLight,
	SpotLight,
};

#define LIGHT_DECL_TYPE(lightType) static eLight GetTypeStatic() { return lightType; } \
	virtual eLight GetType()const override final { return GetTypeStatic(); }

/****************************************************************************************
	Light_Base
*****************************************************************************************/

struct Light_Base
{
	friend class LightManager;

	Light_Base& operator=(const Light_Base&) = delete;
	Light_Base(const Light_Base&) = delete;

	uint GetId()const { return m_id; }

	static eLight GetTypeStatic() { return eLight::None; }
	virtual eLight GetType()const { return GetTypeStatic(); }

	bool enabled = true;

	protected:
		Light_Base();

	private:
		static uint s_idControl;
		uint m_id;
};

/****************************************************************************************
	PointLight
*****************************************************************************************/

struct PointLight : public Light_Base
{
	friend class LightManager;

	LIGHT_DECL_TYPE( eLight::PointLight );

	math::Vec3 pos;
	float range;
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float attenuation0;
	float attenuation1;
	float attenuation2;

	math::Aabb3 GetAabb()const;
	math::Aabb3 GetAabbGlobal()const;

	PointLight() : range(20.0f), diffuseColor4(math::Vec3(0.7f), 1.0f), specularColor4(math::Vec3(0.5f), 1.0f), 
		attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f) {}
};

/****************************************************************************************
	SpotLight
*****************************************************************************************/

struct SpotLight : public Light_Base
{
	friend class LightManager;

	LIGHT_DECL_TYPE( eLight::SpotLight );

	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float attenuation0;
	float attenuation1;
	float attenuation2;
	bool shadowEnabled;
	float shadowBias;
	float shadowDistanceAdjustment;

	SpotLight();

	void SetDirection(const math::Vec3& d) { SetDirection(d.x, d.y, d.z); }
	void SetDirection(float x, float y, float z) { direction.x = x; direction.y = y; direction.z = z; UpdateTransform(); }
	void SetPos(const math::Vec3& p) { SetPos(p.x, p.y, p.z); }
	void SetPos(float x, float y, float z) { pos.x = x; pos.y = y; pos.z = z; UpdateTransform(); }
	void AddPos(const math::Vec3& p) { AddPos(p.x, p.y, p.z); }
	void AddPos(float x, float y, float z) { pos.x += x; pos.y += y; pos.z += z; UpdateTransform(); } 
	void SetRange(float r) { range = r; UpdateTransform(); }
	void SetInnerCone(float degs) { innerConeDegs = degs; UpdateTransform(); }
	void SetOuterCone(float degs) { outerConeDegs = degs; UpdateTransform(); }
	void GetRotation(math::Mtx44&)const;
	void GetView(math::Mtx44&)const;
	void GetProj(math::Mtx44&)const;
	void GetViewProj(math::Mtx44& outView, math::Mtx44& outProj)const;

	const math::Aabb3& GetAabb()const;
	const math::Aabb3& GetAabbGlobal()const;

	const math::Vec3& GetPos()const { return pos; }
	const math::Vec3& GetDirection()const { return direction; }
	float GetRange()const { return range; }
	float GetInnerConeDegs()const { return innerConeDegs; }
	float GetOuterConeDegs()const { return outerConeDegs; }

	static float NormalizedToShadowBias(float value);

	private:
		math::Vec3 pos;
		math::Vec3 direction;
		float range;
		float innerConeDegs;
		float outerConeDegs;
		math::Aabb3 aabbLocal;
		math::Aabb3 aabbGlobal;
		
		void UpdateTransform();
		bool UseUpAxis()const;
};

#undef LIGHT_DECL_TYPE
