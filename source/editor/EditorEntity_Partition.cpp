#include "EditorEntity_Partition.h"
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
EditorEntity_Partition::EditorEntity_Partition() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	m_debugAddEntity = false;
	m_debugCreatePartition = false;
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Partition::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE;

	math::Vec3 color;
	color.x = math::random( 0.0f, 1.0f );
	color.y = math::random( 0.0f, 1.0f );
	color.z = math::random( 0.0f, 1.0f );

	while ( (color.x + color.y + color.z) < 1.5f )
	{
		float f = math::random( 0.0f, 3.0f );
		uint index = 0;
		
		if ( f > 2.0f )
			index = 2;
		else if ( f > 1.0f )
			index = 1;
		else
			index = 0;

		color[index] += math::random(0.0f, 0.3f);
		color[index] = math::clamp( color[index], 0.0f, 1.0f );
	}

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorPartition"), false );
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->ambientIntensity = 0.0f;
	pRenderInfo->diffuseIntensity = 0.0f;
	pRenderInfo->color4 = math::saturateRgb( color.Xyzw(1.0f), 0.6f );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->cullMode = eCullMode::None;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Partition::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_PARTITION) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeQuoted(command::DEBUG_ENTITY_CONTACT, m_debugEntityContact, false) +
		command::serializeLine(command::DEBUG_ADD_ENTITY, string::boolToStr(m_debugAddEntity)) +
		command::serializeLine(command::DEBUG_CREATE_PARTITION, string::boolToStr(m_debugCreatePartition)) +
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Partition::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

	bool isCommandValid = false;
		
	isCommandValid |= pLine->HasString( command::ENTITY_PARTITION );
	isCommandValid |= ProcessCommand_GridPosScale( pLine );
	isCommandValid |= pLine->SetBool( command::DEBUG_ADD_ENTITY, m_debugAddEntity );
	isCommandValid |= pLine->SetBool( command::DEBUG_CREATE_PARTITION, m_debugCreatePartition );

	if ( !isCommandValid )
	{
		isCommandValid = true;

		String strValue;

		if ( pLine->SetQuotedString(command::DEBUG_ENTITY_CONTACT, strValue) || pLine->HasString(command::DEBUG_ENTITY_CONTACT) )
			m_debugEntityContact = strValue;
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
