#include "EditorEntity_Camera.h"
#include "RenderInfoEditor.h"
#include "EditorGlobal.h"
#include "../engine/Primitive.h"
#include "../engine/CoreInterface.h"
#include "../engine/Path.h"
#include "../shared/Command.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_Camera::EditorEntity_Camera() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
EditorEntity_Camera::~EditorEntity_Camera()
{
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Camera::OnInitialize_EditorEntity()
{
	m_direction = math::normalize( 0.0f, -1.0f, 1.0f );

	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE | TOOL_ALLOW_ROTATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorCamera.tga"), false );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->castShadow = false;
	pRenderInfo->renderDirection = true;
	pRenderInfo->direction = m_direction;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );


	int scale = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.12f);
	CHECK( scale > 0 );
	SetGridScale( scale, scale, scale );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Camera::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_CAMERA) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::DIRECTION, string::vec3ToStr(m_direction) ) + 
		command::serializeLine();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Camera::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_CAMERA );
	isCommandValid |= ProcessCommand_GridPosScale( pLine );
	isCommandValid |= pLine->SetFloatArray( command::DIRECTION, &m_direction[0], 3, false );

	CHECK( isCommandValid );
}
