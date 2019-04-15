#include "EditorEntity_Monster.h"
#include "EditorGlobal.h"
#include "RenderInfoEditor.h"
#include "../engine/Texture.h"
#include "../engine/TgaLoader.h"
#include "../engine/Path.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_Monster::EditorEntity_Monster() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
	m_spawnOnCreate = true;
	m_active = true;
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Monster::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE | TOOL_ALLOW_ROTATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->emissiveIntensity = 0.5f;
	pRenderInfo->diffuseIntensity = 0.5f;
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorMonster.tga"), false );
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->castShadow = true;
	pRenderInfo->direction = math::Vec3( 0.0f, 0.0f, 1.0f );

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int xz = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.65f);
	int y = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 1.35f);
	SetGridScale( xz, y, xz );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Monster::GetSerialized()const
{
	return command::serializeLine(command::ENTITY_MONSTER) + 
		command::serializeQuoted(command::NAME, GetName()) +
		command::serializeLine(command::GRID_POS, GetGridPos().AsString()) +
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString()) +
		command::serializeLine(command::SPAWN_ON_CREATE, string::boolToStr(m_spawnOnCreate)) +
		command::serializeLine(command::ACTIVE, string::boolToStr(m_active)) +
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Monster::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_MONSTER );
	isCommandValid |= ProcessCommand_Name(pLine);
	isCommandValid |= pLine->SetBool( command::SPAWN_ON_CREATE, m_spawnOnCreate );
	isCommandValid |= pLine->SetBool( command::ACTIVE, m_active );

	if ( !isCommandValid )
	{
		VecGrid gridData;

		if ( pLine->SetIntArray(command::GRID_POS, &gridData[0], 3) )
		{
			SetGridPos( gridData, false );
			isCommandValid = true;
		}
		else if ( pLine->SetIntArray(command::GRID_SCALE, &gridData[0], 3) )
		{
			SetGridScale( gridData );
			isCommandValid = true;
		}
	}

	CHECK( isCommandValid );
}
