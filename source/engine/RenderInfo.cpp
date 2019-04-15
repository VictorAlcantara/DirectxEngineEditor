#include "../engine/RenderInfo.h"

// Something tells me this is really wrong... =/
#define MACRO_RENDER_INFO_DEFINITION_USE_ADD(methodTypeName, type, groupVariable) \
	void RenderInfo::Use##methodTypeName(uint count) \
	{ \
		groupVariable.Create( count ); \
	} \
	void RenderInfo::Add##methodTypeName(type* pointer, renderFlag flag) \
	{ \
		groupVariable.Add( pointer, flag ); \
	}

MACRO_RENDER_INFO_DEFINITION_USE_ADD( Model, Handle<Model>, modelGroup );
MACRO_RENDER_INFO_DEFINITION_USE_ADD( Float, float, floatGroup );
MACRO_RENDER_INFO_DEFINITION_USE_ADD( Vec2, math::Vec2, vec2Group );
MACRO_RENDER_INFO_DEFINITION_USE_ADD( Vec3, math::Vec3, vec3Group );
MACRO_RENDER_INFO_DEFINITION_USE_ADD( Vec4, math::Vec4, vec4Group );
MACRO_RENDER_INFO_DEFINITION_USE_ADD( Matrix3, math::Mtx33, matrix3Group );
MACRO_RENDER_INFO_DEFINITION_USE_ADD( Matrix4, math::Mtx44, matrix4Group );

//-------------------------------------------------------------------
//	UseTexture
//-------------------------------------------------------------------
void RenderInfo::UseTexture(uint count)
{
	textureGroup.Create( count );
	textureSamplerGroup.Create( count );
}
//-------------------------------------------------------------------
//	AddTexture
//-------------------------------------------------------------------
void RenderInfo::AddTexture(Handle<ITexture>* phTexture, TextureSampler* pSampler, renderFlag flag)
{
	textureGroup.Add( phTexture, flag );
	textureSamplerGroup.Add( pSampler, flag );
}
//-------------------------------------------------------------------
//	SetTransform
//-------------------------------------------------------------------
void RenderInfo::SetTransform(const math::Transform& t)
{
	transform = t;
	OnUpdateTransform();
}
//-------------------------------------------------------------------
//	SetTranslation
//-------------------------------------------------------------------
void RenderInfo::SetTranslation(const math::Vec3& pos)
{
	transform.translate = math::matrixTranslate( pos );
	OnUpdateTransform();
}
//-------------------------------------------------------------------
//	SetScale
//-------------------------------------------------------------------
void RenderInfo::SetScale(const math::Vec3& scale)
{
	transform.scale = math::matrixScale( scale );
	OnUpdateTransform();
}
//-------------------------------------------------------------------
//	SetRotation
//-------------------------------------------------------------------
void RenderInfo::SetRotation(const math::Euler& angle)
{
	transform.rotation = math::matrixYawPitchRoll( angle );
	OnUpdateTransform();
}
//-------------------------------------------------------------------
void RenderInfo::SetRotation(const math::Mtx44& m)
{
	transform.rotation = m;
	OnUpdateTransform();
}

#undef RENDER_INFO_DEFINITION

//=============================================================================
//	RenderInfo_EntitySceneBase
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
RenderInfo_EntitySceneBase::RenderInfo_EntitySceneBase(uint floatGroupSize, uint vec4GroupSize, uint modelGroupSize, uint matrix3GroupSize)
{
	texScale = math::Vec2(1.0f);
	texRotateDegs = 0.0f;
	color4 = math::Vec4(1.0f);
	emissiveIntensity = 0.0f;
	ambientIntensity = 1.0f;
	diffuseIntensity = 1.0f;
	specularIntensity = 0.0f;
	specularPower = 1.0f;
	refractionIntensity = 0.5f;
	colorSaturation = 0.0f;
	colorSeparation = 0.0f;

	cullMode = eCullMode::Back;

	UseFloat( floatGroupSize + 8 );
	AddFloat( &emissiveIntensity, (renderFlag)eRenderFlagFloat::EmissiveIntensity );
	AddFloat( &ambientIntensity, (renderFlag)eRenderFlagFloat::AmbientIntensity );
	AddFloat( &diffuseIntensity, (renderFlag)eRenderFlagFloat::DiffuseIntensity );
	AddFloat( &specularIntensity, (renderFlag)eRenderFlagFloat::SpecularIntensity );
	AddFloat( &specularPower, (renderFlag)eRenderFlagFloat::SpecularPower );
	AddFloat( &refractionIntensity, (renderFlag)eRenderFlagFloat::RefractionIntensity );
	AddFloat( &colorSaturation, (renderFlag)eRenderFlagFloat::ColorSaturation );
	AddFloat( &colorSeparation, (renderFlag)eRenderFlagFloat::ColorSeparation );

	UseVec4( vec4GroupSize + 1 );
	AddVec4( &color4, (renderFlag)eRenderFlagVec4::Color );

	UseModel( modelGroupSize + 1 );
	AddModel( &hModel, (renderFlag)eRenderFlagModel::Primary );

	UseMatrix3( matrix3GroupSize + 1 );
	AddMatrix3( &texTransform, (renderFlag)eRenderFlagMatrix3::TextureTransform );
}
//-------------------------------------------------------------------
//	SetModel
//-------------------------------------------------------------------
void RenderInfo_EntitySceneBase::SetModel(Handle<Model>& _hModel)
{
	hModel = _hModel;
	UpdateAabb();
}
//-------------------------------------------------------------------
//	SetTexTranslate
//-------------------------------------------------------------------
void RenderInfo_EntitySceneBase::SetTexTranslate(const math::Vec2& pos)
{
	texTranslate = pos;
	texTransform = math::matrixTexTransformDegs( texTranslate, texScale, texRotateDegs );
}
//-------------------------------------------------------------------
//	SetTexScale
//-------------------------------------------------------------------
void RenderInfo_EntitySceneBase::SetTexScale(const math::Vec2& scale)
{
	texScale = scale;
	texTransform = math::matrixTexTransformDegs( texTranslate, texScale, texRotateDegs );
}
//-------------------------------------------------------------------
//	SetTexRotationDegs
//-------------------------------------------------------------------
void RenderInfo_EntitySceneBase::SetTexRotationDegs(float degs)
{
	texRotateDegs = degs;
	texTransform = math::matrixTexTransformDegs( texTranslate, texScale, texRotateDegs );
}
//-------------------------------------------------------------------
//	UpdateAabb
//-------------------------------------------------------------------
void RenderInfo_EntitySceneBase::UpdateAabb()
{
	math::Vec3 aabbScale = hModel->GetAabb().GetScale();

	math::Vec3 aabbCube[8];

	getAabb3Points( aabbScale, aabbCube, ARRAY_COUNT(aabbCube) );

	math::Mtx44 aabbTransform = transform.scale * transform.rotation;
	aabbLocal.min = aabbCube[0] * aabbTransform;
	aabbLocal.max = aabbCube[0] * aabbTransform;

	for ( uint i = 1; i < ARRAY_COUNT(aabbCube); i++ )
	{
		math::Vec3 aabbPoint = aabbCube[i] * aabbTransform;
		math::setMinMax( aabbPoint.x, aabbLocal.min.x, aabbLocal.max.x );
		math::setMinMax( aabbPoint.y, aabbLocal.min.y, aabbLocal.max.y );
		math::setMinMax( aabbPoint.z, aabbLocal.min.z, aabbLocal.max.z );
	}

	aabbGlobal = aabbLocal + transform.GetPos();
}
//-------------------------------------------------------------------
//	OnUpdateTransform
//-------------------------------------------------------------------
void RenderInfo_EntitySceneBase::OnUpdateTransform()
{
	UpdateAabb();
}
