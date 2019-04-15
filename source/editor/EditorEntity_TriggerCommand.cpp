#include "EditorEntity_TriggerCommand.h"
#include "RenderInfoEditor.h"
#include "EditorGlobal.h"
#include "../engine/Debug.h"
#include "../engine/Texture.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/ScriptReader.h"
#include "../shared/EntityUtil.h"
#include "../engine/Path.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_TriggerCommand::EditorEntity_TriggerCommand() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	m_eventFireOnce = false;
	m_active = true;
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_TriggerCommand::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->color4 = math::normalizeRgb( 255, 70 );
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 0.0f;
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorTriggerCommand.tga"), false );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_TriggerCommand::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_TRIGGER_COMMAND) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) +
		command::serializeLine(command::ACTIVE, string::boolToStr(m_active)) +
		command::serializeLine( command::EVENT_FIRE_ONCE, string::boolToStr(m_eventFireOnce) ) +
		command::serializeCommandList( command::COMMAND_LINE, m_cmds ) +
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_TriggerCommand::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;
	uint trackIdConnected = 0;

	isCommandValid |= pLine->HasString( command::ENTITY_TRIGGER_COMMAND );
	isCommandValid |= ProcessCommand_GridPosScale( pLine );
	isCommandValid |= pLine->SetBool(command::EVENT_FIRE_ONCE, m_eventFireOnce);
	isCommandValid |= pLine->SetBool( command::ACTIVE, m_active );

	if ( !isCommandValid )
	{
		isCommandValid = true;

		if ( pLine->HasString(command::COMMAND_LINE) )
			m_cmds.AddLast( pLine->GetQuotedString() );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
//-------------------------------------------------------------------
//	AddCommand
//-------------------------------------------------------------------
void EditorEntity_TriggerCommand::AddCommand(const String& s)
{
	m_cmds.AddLast( s );
}
//-------------------------------------------------------------------
//	ClearCommand
//-------------------------------------------------------------------
void EditorEntity_TriggerCommand::ClearCommands()
{
	m_cmds.Clear();
}
//-------------------------------------------------------------------
//	GetCommands
//-------------------------------------------------------------------
const List<String>& EditorEntity_TriggerCommand::GetCommands()const
{
	return m_cmds;
}
