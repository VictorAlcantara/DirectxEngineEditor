#include "Entity_Talk.h"
#include "../engine/CoreInterface.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../editor/Grid.h"
#include "../shared/Command.h"
#include "GameCollision.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_Talk::Entity_Talk() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	m_eventFireOnce = false;
	m_eventFireCount = 0;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Talk::OnInitialize()
{
	m_scale = math::Vec3(1.0f);

	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->color4 = math::normalizeRgb( 50, 88, 40, 70 );
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = primitive::texture::createWhite();
	pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->visible = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetCollider( eColliderInfo::Aabb );
	m_hCollision->GetCollider(true)->aabbScale = pRenderInfo->GetTransform().GetScale();
	m_hCollision->SetNoclip( true );

	m_hPhysics = AddComponentAs<Component_Physics>();
	m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	m_hPhysics->EnableUpdate( false );
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Talk::ProcessCommand(const ScriptLine* pLine)
{
	String strData;
	VecGrid vecGrid;
	bool bData;
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	if ( pLine->SetIntArray( command::GRID_POS, &vecGrid[0], 3 ) )
	{
		pRenderInfo->SetTranslation( Grid::ToVec3(vecGrid) );
		m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	}
	else if ( pLine->SetIntArray( command::GRID_SCALE, &vecGrid[0], 3 ) )
	{
		pRenderInfo->SetScale( Grid::ToVec3(vecGrid) );
		m_hCollision->GetCollider(true)->aabbScale = pRenderInfo->GetTransform().GetScale();
	}
	else if ( pLine->SetQuotedString(command::NAME, strData) )
	{
		SetName( strData );
	}
	else if ( pLine->HasString(command::TALK_LINE) )
	{
		m_talkLines.AddLast( pLine->GetQuotedString() );
	}
	else if ( pLine->HasString(command::TALK_LINE_CLEAR) )
	{
		m_talkLines.Clear();
	}
	else if ( pLine->HasString(command::COMMAND_LINE) )
	{
		m_cmdLines.AddLast( pLine->GetQuotedString() );
	}
	else if ( pLine->HasString(command::COMMAND_LINE_CLEAR) )
	{
		m_cmdLines.Clear();
	}
	else if ( pLine->SetBool(command::EVENT_FIRE_ONCE, bData) )
	{
		m_eventFireOnce = bData;
	}
	else if ( !pLine->HasString(command::ENTITY_TALK) )
	{
		ASSERT( "Invalid command" );
	}
}
//-------------------------------------------------------------------
//	GetTalkLines
//-------------------------------------------------------------------
const List<String>& Entity_Talk::GetTalkLines()const
{
	return m_talkLines;
}
//-------------------------------------------------------------------
//	GetCommandLines
//-------------------------------------------------------------------
const List<String>& Entity_Talk::GetCommandLines(bool increaseEventCounter)const
{
	if ( m_cmdLines.Count() > 0 && increaseEventCounter )
		m_eventFireCount++;
	return m_cmdLines;
}
