#include "EditorEntity_Door.h"
#include "RenderInfoEditor.h"
#include "EditorGlobal.h"
#include "../engine/Debug.h"
#include "../engine/Texture.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../shared/Command.h"
#include "../engine/ScriptReader.h"
#include "../shared/EntityUtil.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_Door::EditorEntity_Door() : EditorEntity(true)
{
	RegisterInheritance( GetClassTagStatic() );
	m_locked = false;
	m_openDirection = math::AXIS_X;
	m_openAmount = 0.9f;
	m_duration = 0.2f;
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Door::OnShutdown_EditorEntity()
{
	g_pScene->Debug_Remove( m_hLine );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Door::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE | TOOL_ALLOW_PICK_AXIS;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->ambientIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 1.0f;
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->specularPower = 1.0f;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );

	m_hLine = g_pScene->Debug_AddLine();
	m_hLine->color4 = math::normalizeRgb( 255, 200, 0 );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntity_Door::Update()
{
	float scale = math::max( math::max( GetScale() ), 2.0f );

	m_hLine->point0 = GetPos();
	m_hLine->point1 = m_hLine->point0 + m_openDirection * scale * 2.5f;
}
//-------------------------------------------------------------------
//	SetOpenDirection
//-------------------------------------------------------------------
void EditorEntity_Door::SetOpenDirection(const math::Vec3& dir)
{
	CHECK( !math::isZero(dir) );
	m_openDirection = math::normalize( dir );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Door::GetSerialized()const
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

	return command::serializeLine(command::ENTITY_DOOR) + 
		command::serializeQuoted( command::NAME, GetName() ) + 
		command::serializeLine( command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine( command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeLine( command::COLOR, string::vec4ToStr(pRenderInfo->color4) ) + 
		command::serializeLine( command::EMISSIVE_INTENSITY, string::floatToStr(pRenderInfo->emissiveIntensity) ) + 
		command::serializeLine( command::AMBIENT_INTENSITY, string::floatToStr(pRenderInfo->ambientIntensity) ) + 
		command::serializeLine( command::DIFFUSE_INTENSITY, string::floatToStr(pRenderInfo->diffuseIntensity) ) + 
		command::serializeLine( command::SPECULAR_INTENSITY, string::floatToStr(pRenderInfo->specularIntensity) ) + 
		command::serializeLine( command::SPECULAR_POWER, string::floatToStr(pRenderInfo->specularPower) ) + 
		command::serializeLine( command::TEXTURE_POS, string::vec2ToStr(pRenderInfo->GetTexTranslate()) ) + 
		command::serializeLine( command::TEXTURE_SCALE, string::vec2ToStr(pRenderInfo->GetTexScale()) ) + 
		command::serializeLine( command::TEXTURE_ROTATION, string::floatToStr(pRenderInfo->GetTexRotationDegs()) ) + 
		command::serializeLine( command::TEXTURE_DIFFUSE, pRenderInfo->hDiffuseTexture ? pRenderInfo->hDiffuseTexture->GetName().GetString() : "" ) + 
		command::serializeLine( command::TEXTURE_NORMAL_MAP, pRenderInfo->hNormalMap ? pRenderInfo->hNormalMap->GetName().GetString() : "" ) + 
		command::serializeLine( command::LOCKED, string::boolToStr(m_locked) ) + 
		command::serializeLine( command::OPEN_DIRECTION, string::vec3ToStr(m_openDirection) ) + 
		command::serializeLine( command::OPEN_AMOUNT, string::floatToStr(m_openAmount) ) + 
		command::serializeLine( command::DOOR_DURATION, string::floatToStr(m_duration) ) + 
		command::serializeCommandList( command::LOCK_TEXT, GetLockTextLines() ) +
		command::serializeLine();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Door::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_DOOR );
	isCommandValid |= ProcessCommand_Name(pLine);

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::EMISSIVE_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_INTENSITY, pRenderInfo->ambientIntensity );
	isCommandValid |= pLine->SetFloat( command::DIFFUSE_INTENSITY, pRenderInfo->diffuseIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_INTENSITY, pRenderInfo->specularIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_POWER, pRenderInfo->specularPower );

	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	isCommandValid |= pLine->SetBool( command::LOCKED, m_locked );
	isCommandValid |= pLine->SetFloatArray( command::OPEN_DIRECTION, &m_openDirection[0], 3, false );
	isCommandValid |= pLine->SetFloat( command::OPEN_AMOUNT, m_openAmount );
	isCommandValid |= pLine->SetFloat( command::DOOR_DURATION, m_duration );

	if ( !isCommandValid )
	{
		float floatData;
		VecGrid gridData;
		math::Vec2 vec2Data;
		math::Vec4 vec4Data;
		String strData;

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
		else if ( pLine->HasString(command::TEXTURE_DIFFUSE) )
			pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->HasString(command::TEXTURE_NORMAL_MAP) )
			pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->SetQuotedString( command::NAME, strData ) )
			SetName( strData );
		else if ( pLine->HasString(command::LOCK_TEXT_CLEAR) )
			ClearLockTextLines();
		else if ( pLine->SetQuotedString( command::LOCK_TEXT, strData ) )
			AddLockTextLine( strData );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
