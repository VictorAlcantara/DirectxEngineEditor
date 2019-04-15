#include "EditorEntity_PlayerSpawn.h"
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
EditorEntity_PlayerSpawn::EditorEntity_PlayerSpawn() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_PlayerSpawn::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE | TOOL_ALLOW_ROTATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->emissiveIntensity = 0.5f;
	pRenderInfo->diffuseIntensity = 0.5f;
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->CreateFromFile( path::texture("editorPlayerSpawn.tga"), false );
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->castShadow = false;
	pRenderInfo->renderDirection = true;
	pRenderInfo->direction = math::Vec3( 0.0f, 0.0f, 1.0f );

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int xz = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.45f);
	int y = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 1.2f);
	SetGridScale( xz, y, xz );
}
//-------------------------------------------------------------------
//	RotateYawPitchRoll
//-------------------------------------------------------------------
void EditorEntity_PlayerSpawn::RotateYawPitchRoll(float yaw, float pitch, float roll)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);
	pRenderInfo->direction = (pRenderInfo->direction.Xyzw(0.0f) * math::matrixYaw( yaw )).Xyz();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_PlayerSpawn::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_PLAYER_SPAWN );
	isCommandValid |= pLine->SetFloatArray( command::DIRECTION, &pRenderInfo->direction[0], 3, false);

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
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_PlayerSpawn::GetSerialized()const
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

	return command::serializeLine(command::ENTITY_PLAYER_SPAWN) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString()) +
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString()) +
		command::serializeLine(command::DIRECTION, string::vec3ToStr(pRenderInfo->direction) ) + 
		command::serializeLine();
};
