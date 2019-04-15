#include "Entity_Block.h"
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
Entity_Block::Entity_Block() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	//m_pCollision = nullptr;
}
//-------------------------------------------------------------------
Entity_Block::~Entity_Block()
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_Block::OnShutdown()
{
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Block::OnInitialize()
{
	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = primitive::texture::createWhite();
	pRenderInfo->material = eMaterial::Opaque;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetCollider( eColliderInfo::Aabb );
	
	UpdateCollisionInfo();
}
//-------------------------------------------------------------------
//	UpdateCollisionInfo
//-------------------------------------------------------------------
void Entity_Block::UpdateCollisionInfo()
{
	m_hCollision->GetCollider(true)->aabbScale = GetComponentPtrAs<Component_Render>()->GetData()->GetTransform().GetScale();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Block::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_BLOCK );

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::EMISSIVE_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_INTENSITY, pRenderInfo->ambientIntensity );
	isCommandValid |= pLine->SetFloat( command::DIFFUSE_INTENSITY, pRenderInfo->diffuseIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_INTENSITY, pRenderInfo->specularIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_POWER, pRenderInfo->specularPower );

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
		else if ( pLine->HasString(command::TEXTURE_DIFFUSE) )
			pRenderInfo->hDiffuseMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->HasString(command::TEXTURE_SPECULAR) )
			pRenderInfo->hSpecularMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->HasString(command::TEXTURE_NORMAL_MAP) )
			pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );

	UpdateCollisionInfo();
}
