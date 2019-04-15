#include "Entity_TriggerCommand.h"
#include "../engine/CoreInterface.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../editor/Grid.h"
#include "../shared/Command.h"
#include "GameCollision.h"
#include "GameGlobal.h"
#include "Entity_Player.h"
#include "../engine/Component_Render.h"

/****************************************************************************************
	Component_Trigger_Command
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Trigger_Command::Component_Trigger_Command()
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	Ignore
//-------------------------------------------------------------------
bool Component_Trigger_Command::Ignore(const Handle<Entity>& hOther)const
{
	return hOther->GetClassRelationship<Entity_Player>() == eClassRelationship::None;
}
//-------------------------------------------------------------------
//	OnTrigger_Enter
//-------------------------------------------------------------------
void Component_Trigger_Command::OnTrigger_Update(Handle<Entity>& hOther)
{
	CHECK( GetOwner()->GetClassRelationship_Is<Entity_TriggerCommand>() );

	for ( auto it = GetOwner().GetAs<Entity_TriggerCommand>().GetCommands().GetIterator(); it; it++ )
	{
		g_pCommand->Execute( *it );
		if (GetOwner().GetAs<Entity_TriggerCommand>().IsFireOnce() )
		{
			SetNoclip( true );
		}
	}
}

/****************************************************************************************
	Entity_TriggerCommand
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_TriggerCommand::Entity_TriggerCommand() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	m_fireOnce = false;
	m_active = true;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_TriggerCommand::OnInitialize()
{
	m_scale = math::Vec3(1.0f);

	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->color4 = math::normalizeRgb( 180, 0, 255, 70 );
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = primitive::texture::createWhite();
	pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->visible = false;
	
	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Trigger_Command>();
	m_hCollision->GetCollider()->aabbScale = pRenderInfo->GetTransform().GetScale();

	m_hPhysics = AddComponentAs<Component_Physics>();
	m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	m_hPhysics->EnableUpdate( false );
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_TriggerCommand::ProcessCommand(const ScriptLine* pLine)
{
	VecGrid vecGrid;
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	if ( pLine->SetIntArray( command::GRID_POS, &vecGrid[0], 3 ) )
	{
		pRenderInfo->SetTranslation( Grid::ToVec3(vecGrid) );
		m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	}
	else if (pLine->SetBool(command::ACTIVE, m_active))
	{
		m_hCollision->SetNoclip( !m_active );
	}
	else if ( pLine->SetBool(command::EVENT_FIRE_ONCE, m_fireOnce) )
	{
	}
	else if ( pLine->SetIntArray( command::GRID_SCALE, &vecGrid[0], 3 ) )
	{
		pRenderInfo->SetScale( Grid::ToVec3(vecGrid) );
		m_hCollision->GetCollider()->aabbScale = pRenderInfo->GetTransform().GetScale();
	}
	else if ( pLine->HasString(command::COMMAND_LINE) )
	{
		m_cmds.AddLast( pLine->GetQuotedString() );
	}
	else if ( !pLine->HasString(command::ENTITY_TRIGGER_COMMAND) )
	{
		ASSERT( "Invalid command" );
	}
}
//-------------------------------------------------------------------
//	GetCommands
//-------------------------------------------------------------------
const List<String>& Entity_TriggerCommand::GetCommands()const
{
	return m_cmds;
}
