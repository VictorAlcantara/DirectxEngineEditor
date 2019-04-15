#include "EditorEntity_AmbientLight.h"
#include "../engine/CoreInterface.h"
#include "RenderInfoEditor.h"
#include "../engine/Primitive.h"
#include "EditorGlobal.h"
#include "../shared/Command.h"
#include "../engine/Path.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_AmbientLight::EditorEntity_AmbientLight() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_AmbientLight::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE;
	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorAmbientLight.tga"), false );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int scale = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.1f);
	CHECK( scale > 0 );
	SetGridScale( scale, scale, scale );

	m_ambientColor4 = math::Vec3(0.1f).Xyzw(1.0f);
	g_pScene->GetLightMngr()->SetAmbientLight( m_ambientColor4 );
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_AmbientLight::OnShutdown_EditorEntity()
{
	g_pScene->GetLightMngr()->SetAmbientLight( math::Vec3(0.0f).Xyzw(1.0f) );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_AmbientLight::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_AMBIENT_LIGHT) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::AMBIENT_COLOR, string::vec4ToStr(m_ambientColor4) ) + 
		command::serializeLine();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_AmbientLight::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_AMBIENT_LIGHT );
	isCommandValid |= pLine->SetFloatArray( command::AMBIENT_COLOR, &m_ambientColor4[0], 4, false );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_COLOR_RED, m_ambientColor4.x );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_COLOR_GREEN, m_ambientColor4.y );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_COLOR_BLUE, m_ambientColor4.z );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_COLOR_ALPHA, m_ambientColor4.w );

	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	CHECK( isCommandValid );
	
	g_pScene->GetLightMngr()->SetAmbientLight( m_ambientColor4 );
}
