#include "EditorEntity_Refraction.h"
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
EditorEntity_Refraction::EditorEntity_Refraction() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Refraction::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hNormalMap = primitive::texture::createWhite();
	pRenderInfo->color4 = math::normalizeRgb(255, 255, 255);
	pRenderInfo->refractionIntensity = 0.05f;
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Refraction;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Refraction::GetSerialized()const
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

	return command::serializeLine(command::ENTITY_REFRACTION) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeLine(command::COLOR, string::vec4ToStr(pRenderInfo->color4) ) + 
		command::serializeLine(command::REFRACTION_INTENSITY, string::floatToStr(pRenderInfo->refractionIntensity) ) + 
		command::serializeLine(command::COLOR_SATURATION, string::floatToStr(pRenderInfo->colorSaturation) ) + 
		command::serializeLine(command::COLOR_SEPARATION, string::floatToStr(pRenderInfo->colorSeparation) ) + 
		command::serializeLine(command::TEXTURE_POS, string::vec2ToStr(pRenderInfo->GetTexTranslate()) ) + 
		command::serializeLine(command::TEXTURE_SCALE, string::vec2ToStr(pRenderInfo->GetTexScale()) ) + 
		command::serializeLine(command::TEXTURE_ROTATION, string::floatToStr(pRenderInfo->GetTexRotationDegs()) ) + 
		command::serializeLine(command::TEXTURE_NORMAL_MAP, pRenderInfo->hNormalMap ? pRenderInfo->hNormalMap->GetName().GetString() : "" ) + 
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Refraction::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(true);

	bool isCommandValid = false;
		
	isCommandValid |= pLine->HasString( command::ENTITY_REFRACTION );

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::REFRACTION_INTENSITY, pRenderInfo->refractionIntensity );
	isCommandValid |= pLine->SetFloat( command::COLOR_SATURATION, pRenderInfo->colorSaturation );
	isCommandValid |= pLine->SetFloat( command::COLOR_SEPARATION, pRenderInfo->colorSeparation );
		
	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	if ( !isCommandValid )
	{
		float floatData;
		VecGrid gridData;
		math::Vec2 vec2Data;
		math::Vec4 vec4Data;

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
		else if ( pLine->HasString(command::TEXTURE_NORMAL_MAP) )
			pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
