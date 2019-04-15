#include "../engine/Light.h"
#include "../engine/CoreInterface.h"
#include "EventEngine.h"

/****************************************************************************************
	Light_Base
*****************************************************************************************/

uint Light_Base::s_idControl = 1;

Light_Base::Light_Base()
{
	m_id = s_idControl++;
}

/****************************************************************************************
	PointLight
*****************************************************************************************/

//-------------------------------------------------------------------
//	GetAabb3
//-------------------------------------------------------------------
math::Aabb3 PointLight::GetAabb()const
{
	math::Vec3 scale( range );
	return math::Aabb3( -scale, scale );
}
//-------------------------------------------------------------------
math::Aabb3 PointLight::GetAabbGlobal()const
{
	return GetAabb() + pos;
}

/****************************************************************************************
	SpotLight
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
SpotLight::SpotLight(): direction( math::normalize(0.0f, 0.0f, 1.0f) ), range(20.0f),
	diffuseColor4(math::Vec3(0.7f), 1.0f), specularColor4(math::Vec3(0.5f), 1.0f),
	innerConeDegs(60.0f), outerConeDegs(20.0f), attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f),
	shadowEnabled(true), shadowBias(0.00001f), shadowDistanceAdjustment(0.0f)
{
	UpdateTransform();
}
//-------------------------------------------------------------------
//	GetAabb3
//-------------------------------------------------------------------
const math::Aabb3& SpotLight::GetAabb()const
{
	return aabbLocal;
}
//-------------------------------------------------------------------
const math::Aabb3& SpotLight::GetAabbGlobal()const
{
	return aabbGlobal;
}
//-------------------------------------------------------------------
//	UpdateTransform
//-------------------------------------------------------------------
void SpotLight::UpdateTransform()
{
	// In relation to the origin, cube "looking" at +z
	math::Vec3 scale;
	scale.z = range;
	scale.x = scale.z * math::tanDegs( (innerConeDegs + outerConeDegs) * 0.5f );
	scale.y = scale.x;

	math::Mtx44 mScale = math::matrixScale( scale );
	math::Mtx44 mRotation;
	GetRotation( mRotation );
	math::Mtx44 mWorld = mScale * mRotation;

	math::Vec3 pyramidTransformed[5] = {
		// Front
		math::Vec3( 0.0f,   0.0f,  0.0f),

		// Back
		math::Vec3(-1.0f,  0.0f,  1.0f) * mWorld,
		math::Vec3( 0.0f,  1.0f,  1.0f) * mWorld,
		math::Vec3( 1.0f,  0.0f,  1.0f) * mWorld,
		math::Vec3( 0.0f, -1.0f,  1.0f) * mWorld,
	};

	aabbLocal = math::calculateAabb3( pyramidTransformed, ARRAY_COUNT(pyramidTransformed) );
	aabbGlobal = aabbLocal + pos;

	auto pEvent = SmartPtr<Event_LightTransform>::New();
	pEvent->lightId = GetId();
	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	UseUpAxis
//-------------------------------------------------------------------
bool SpotLight::UseUpAxis()const
{
	return math::isEqual(math::abs(direction), math::AXIS_Y, 0.001f);
}
//-------------------------------------------------------------------
//	GetRotationViewProj
//-------------------------------------------------------------------
void SpotLight::GetRotation(math::Mtx44& outRotation)const
{
	outRotation = UseUpAxis() ? math::matrixOrthogonalLookUp(direction, math::AXIS_Y) : math::matrixOrthogonalLook(direction);
}
//-------------------------------------------------------------------
void SpotLight::GetView(math::Mtx44& outView)const
{
	outView = UseUpAxis() ? math::matrixViewLookUp(pos, direction, math::AXIS_Y) : math::matrixViewLook(pos, direction);
}
//-------------------------------------------------------------------
void SpotLight::GetProj(math::Mtx44& outProj)const
{
	outProj = math::matrixPerspective( 1.0f, outerConeDegs + innerConeDegs, 0.001f, range );
}
//-------------------------------------------------------------------
void SpotLight::GetViewProj(math::Mtx44& outView, math::Mtx44& outProj)const
{
	GetView( outView );
	GetProj( outProj );
}
//-------------------------------------------------------------------
//	NormalizedToShadowBias
//-------------------------------------------------------------------
float SpotLight::NormalizedToShadowBias(float value)
{
	return math::smoothCube( 0.0000001f, 0.000003f, value );
}
