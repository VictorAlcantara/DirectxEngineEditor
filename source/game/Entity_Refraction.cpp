#include "Entity_Refraction.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/CoreInterface.h"
#include "../editor/Grid.h"
#include "../shared/EntityUtil.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_Refraction::Entity_Refraction() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
Entity_Refraction::~Entity_Refraction()
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_Refraction::OnShutdown()
{
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Refraction::OnInitialize()
{
	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hNormalMap = primitive::texture::createWhite();
	pRenderInfo->color4 = math::Vec4(1.0f);
	pRenderInfo->refractionIntensity = 0.1f;
	pRenderInfo->material = eMaterial::Refraction;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetCollider( eColliderInfo::Aabb );
	
	UpdateCollisionInfo();
}
//-------------------------------------------------------------------
//	UpdateCollisionInfo
//-------------------------------------------------------------------
void Entity_Refraction::UpdateCollisionInfo()
{
	m_hCollision->GetCollider(true)->aabbScale = GetComponentPtrAs<Component_Render>()->GetData()->GetTransform().GetScale();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Refraction::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_REFRACTION );

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::REFRACTION_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::COLOR_SATURATION, pRenderInfo->colorSaturation );
	isCommandValid |= pLine->SetFloat( command::COLOR_SEPARATION, pRenderInfo->colorSeparation );

	if ( !isCommandValid )
	{
		float floatData;
		VecGrid gridData;
		math::Vec2 vec2Data;
		math::Vec4 vec4Data;

		isCommandValid = true;

		if ( pLine->SetFloat( command::TEXTURE_POS_U, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexTranslate();
			texData.x = floatData;
			pRenderInfo->SetTexTranslate( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_POS_V, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexTranslate();
			texData.y = floatData;
			pRenderInfo->SetTexTranslate( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_SCALE_U, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexScale();
			texData.x = floatData;
			pRenderInfo->SetTexScale( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_SCALE_V, floatData ) )
		{
			math::Vec2 texData = pRenderInfo->GetTexScale();
			texData.y = floatData;
			pRenderInfo->SetTexScale( texData );
		}
		else if ( pLine->SetFloat( command::TEXTURE_ROTATION, floatData ) )
			pRenderInfo->SetTexRotationDegs( floatData );
		else if ( pLine->SetFloatArray(command::TEXTURE_POS, &vec4Data[0], 2, false) )
			pRenderInfo->SetTexTranslate( vec4Data.Xy() );
		else if ( pLine->SetFloatArray(command::TEXTURE_SCALE, &vec4Data[0], 2, false) )
			pRenderInfo->SetTexScale ( vec4Data.Xy() );
		else if ( pLine->SetIntArray(command::GRID_POS, &gridData[0], 3) )
			pRenderInfo->SetTranslation(Grid::ToVec3(gridData));
		else if ( pLine->SetIntArray(command::GRID_SCALE, &gridData[0], 3) )
			pRenderInfo->SetScale( Grid::ToVec3(gridData) );
		else if ( pLine->HasString(command::TEXTURE_NORMAL_MAP) )
			pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );

	UpdateCollisionInfo();
}
