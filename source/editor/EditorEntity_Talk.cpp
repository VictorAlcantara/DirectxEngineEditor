#include "EditorEntity_Talk.h"
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
EditorEntity_Talk::EditorEntity_Talk() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	m_eventFireOnce = false;
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Talk::OnShutdown_EditorEntity()
{
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Talk::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->color4 = math::normalizeRgb( 255, 70 );
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 0.0f;
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorTalk.tga"), false );
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
String EditorEntity_Talk::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_TALK) + 
		command::serializeQuoted( command::NAME, GetName() ) +
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeCommandList( command::TALK_LINE, GetTextLines() ) +
		command::serializeLine( command::EVENT_FIRE_ONCE, string::boolToStr(m_eventFireOnce) ) +
		command::serializeCommandList( command::COMMAND_LINE, GetCommandLines() ) +
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Talk::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;
	uint trackIdConnected = 0;

	isCommandValid |= pLine->HasString( command::ENTITY_TALK );
	isCommandValid |= ProcessCommand_Name(pLine);

	isCommandValid |= ProcessCommand_GridPosScale( pLine );
	isCommandValid |= pLine->SetBool( command::EVENT_FIRE_ONCE, m_eventFireOnce );

	if ( !isCommandValid )
	{
		String strData;
		isCommandValid = true;

		if ( pLine->HasString(command::TALK_LINE) )
			m_textLines.AddLast( pLine->GetQuotedString() );
		else if ( pLine->HasString(command::COMMAND_LINE) )
			m_cmdLines.AddLast( pLine->GetQuotedString() );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
//-------------------------------------------------------------------
//	AddTextLine
//-------------------------------------------------------------------
void EditorEntity_Talk::AddTextLine(const String& s)
{
	m_textLines.AddLast( s );
}
//-------------------------------------------------------------------
//	ClearTextLines
//-------------------------------------------------------------------
void EditorEntity_Talk::ClearTextLines()
{
	m_textLines.Clear();
}
//-------------------------------------------------------------------
//	GetTextLines
//-------------------------------------------------------------------
const List<String>& EditorEntity_Talk::GetTextLines()const
{
	return m_textLines;
}
//-------------------------------------------------------------------
//	AddCommandLine
//-------------------------------------------------------------------
void EditorEntity_Talk::AddCommandLine(const String& s)
{
	m_cmdLines.AddLast( s );
}
//-------------------------------------------------------------------
//	ClearCommandLines
//-------------------------------------------------------------------
void EditorEntity_Talk::ClearCommandLines()
{
	m_cmdLines.Clear();
}
//-------------------------------------------------------------------
//	GetCommandLines
//-------------------------------------------------------------------
const List<String>& EditorEntity_Talk::GetCommandLines()const
{
	return m_cmdLines;
}
