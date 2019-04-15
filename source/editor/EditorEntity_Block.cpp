#include "EditorEntity_Block.h"
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
EditorEntity_Block::EditorEntity_Block() : EditorEntity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_Block::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = true;
	m_toolActionFlag = TOOL_ALLOW_RESIZE | TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->ambientIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 1.0f;
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->specularPower = 10.0f;
	pRenderInfo->specularIntensity = 1.0f;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	SetGridScale( VecGrid(editorGlobal::grid::UNIT_SIZE_ONE) );
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_Block::GetSerialized()const
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

	return command::serializeLine(command::ENTITY_BLOCK) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::GRID_SCALE, GetGridScale().AsString() ) + 
		command::serializeLine(command::COLOR, string::vec4ToStr(pRenderInfo->color4) ) + 
		command::serializeLine(command::EMISSIVE_INTENSITY, string::floatToStr(pRenderInfo->emissiveIntensity) ) + 
		command::serializeLine(command::AMBIENT_INTENSITY, string::floatToStr(pRenderInfo->ambientIntensity) ) + 
		command::serializeLine(command::DIFFUSE_INTENSITY, string::floatToStr(pRenderInfo->diffuseIntensity) ) + 
		command::serializeLine(command::SPECULAR_INTENSITY, string::floatToStr(pRenderInfo->specularIntensity) ) + 
		command::serializeLine(command::SPECULAR_POWER, string::floatToStr(pRenderInfo->specularPower) ) + 
		command::serializeLine(command::TEXTURE_POS, string::vec2ToStr(pRenderInfo->GetTexTranslate()) ) + 
		command::serializeLine(command::TEXTURE_SCALE, string::vec2ToStr(pRenderInfo->GetTexScale()) ) + 
		command::serializeLine(command::TEXTURE_ROTATION, string::floatToStr(pRenderInfo->GetTexRotationDegs()) ) + 
		command::serializeLine(command::TEXTURE_DIFFUSE, pRenderInfo->hDiffuseTexture ? pRenderInfo->hDiffuseTexture->GetName().GetString() : "" ) + 
		command::serializeLine(command::TEXTURE_SPECULAR, pRenderInfo->hSpecularTexture ? pRenderInfo->hSpecularTexture->GetName().GetString() : "" ) + 
		command::serializeLine(command::TEXTURE_NORMAL_MAP, pRenderInfo->hNormalMap ? pRenderInfo->hNormalMap->GetName().GetString() : "" ) + 
		command::serializeLine();
};
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_Block::ProcessCommand(const ScriptLine* pLine)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>(false);

	bool isCommandValid = false;
		
	isCommandValid |= pLine->HasString( command::ENTITY_BLOCK );

	isCommandValid |= processCommand_MaterialColor( command::COLOR, &pRenderInfo->color4[0], pRenderInfo->color4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::EMISSIVE_INTENSITY, pRenderInfo->emissiveIntensity );
	isCommandValid |= pLine->SetFloat( command::AMBIENT_INTENSITY, pRenderInfo->ambientIntensity );
	isCommandValid |= pLine->SetFloat( command::DIFFUSE_INTENSITY, pRenderInfo->diffuseIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_INTENSITY, pRenderInfo->specularIntensity );
	isCommandValid |= pLine->SetFloat( command::SPECULAR_POWER, pRenderInfo->specularPower );
		
	isCommandValid |= ProcessCommand_GridPosScale(pLine);

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
		else if ( pLine->HasString(command::TEXTURE_DIFFUSE) )
		{
			String oldTexture = pRenderInfo->hDiffuseTexture ? pRenderInfo->hDiffuseTexture->GetName().GetString() : "";
			String newTexture = pLine->GetByType(eScriptType::String, 1);

			pRenderInfo->hDiffuseTexture = g_pResource->GetTextureMngr()->Get( newTexture );

			if ( !g_pEditor->IsLoadingMap() && newTexture.Length() > 0 )
			{
				String oldSpecular = pRenderInfo->hSpecularTexture ? pRenderInfo->hSpecularTexture->GetName().GetString() : "";
				String oldNormal = pRenderInfo->hNormalMap ? pRenderInfo->hNormalMap->GetName().GetString() : "";
				String newSpecular = newTexture + "_spec";
				String newNormal = newTexture + "_nrml";

				if ( string::contains(oldSpecular, oldTexture) || oldSpecular.Length() == 0 )
					pRenderInfo->hSpecularTexture = g_pResource->GetTextureMngr()->Get( newSpecular );
				if ( string::contains(oldNormal, oldTexture) || oldNormal.Length() == 0 )
					pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( newNormal );
			}
		}
		else if ( pLine->HasString(command::TEXTURE_SPECULAR) )
			pRenderInfo->hSpecularTexture = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else if ( pLine->HasString(command::TEXTURE_NORMAL_MAP) )
			pRenderInfo->hNormalMap = g_pResource->GetTextureMngr()->Get( HashString(pLine->GetByType(eScriptType::String, 1)) );
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
