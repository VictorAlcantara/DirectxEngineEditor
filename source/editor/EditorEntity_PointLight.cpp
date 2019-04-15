#include "EditorEntity_PointLight.h"
#include "EditorGlobal.h"
#include "RenderInfoEditor.h"
#include "../engine/CoreInterface.h"
#include "../engine/Scene.h"
#include "../engine/Primitive.h"
#include "../engine/ScriptReader.h"
#include "../shared/Command.h"
#include "../shared/EntityUtil.h"
#include "../engine/Component_Render.h"
#include "../shared/Component_LightFlicker.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntity_PointLight::EditorEntity_PointLight() : EditorEntity(true)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_PointLight::OnShutdown_EditorEntity()
{
	g_pScene->GetLightMngr()->RemovePointLight( m_hLight->GetId() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_PointLight::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->renderBoundingSphere = true;
	pRenderInfo->renderAabb = true;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int scale = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.1f);
	CHECK( scale > 0 );
	SetGridScale( scale, scale, scale );

	m_hLight = g_pScene->GetLightMngr()->AddPointLight();
	m_hLight->specularColor4 = math::normalizeRgb(255, 255, 255);
	m_hLight->attenuation0 = 0.8f;
	m_hLight->attenuation1 = 0.5f;
	m_hLight->attenuation2 = 1.5f;

	auto pLightFlicker = AddComponentAs<Component_LightFlicker>();
	pLightFlicker->SetLight( m_hLight->GetId() );
	pLightFlicker->Enable( false );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntity_PointLight::Update()
{
	m_hLight->pos = GetPos();
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_PointLight::GetSerialized()const
{
	auto pLightFlicker = GetComponentPtrAs<Component_LightFlicker>();

	return command::serializeLine(command::ENTITY_POINT_LIGHT) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::DIFFUSE_COLOR, string::vec4ToStr(m_hLight->diffuseColor4) ) + 
		command::serializeLine(command::SPECULAR_COLOR, string::vec4ToStr(m_hLight->specularColor4) ) + 
		command::serializeLine(command::LIGHT_RANGE, string::floatToStr(m_hLight->range) ) + 
		command::serializeLine(command::LIGHT_ATTENUATION_0, string::floatToStr(m_hLight->attenuation0) ) + 
		command::serializeLine(command::LIGHT_ATTENUATION_1, string::floatToStr(m_hLight->attenuation1) ) + 
		command::serializeLine(command::LIGHT_ATTENUATION_2, string::floatToStr(m_hLight->attenuation2) ) + 
		command::serializeLine(command::LIGHT_FLICKER, string::boolToStr(pLightFlicker->IsEnabled()) )+ 
		command::serializeLine();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_PointLight::ProcessCommand(const ScriptLine* pLine)
{
	auto hLight = GetLight();

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_POINT_LIGHT );
	isCommandValid |= pLine->SetFloat( command::LIGHT_RANGE, hLight->range );

	isCommandValid |= processCommand_MaterialColor( command::DIFFUSE_COLOR, &hLight->diffuseColor4[0], hLight->diffuseColor4.GetComponentCount(), pLine );
	isCommandValid |= processCommand_MaterialColor( command::SPECULAR_COLOR, &hLight->specularColor4[0], hLight->specularColor4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_0, hLight->attenuation0 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_1, hLight->attenuation1 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_2, hLight->attenuation2 );

	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	if ( !isCommandValid )
	{
		bool bData;
		if ( pLine->SetBool(command::LIGHT_FLICKER, bData) )
		{
			auto pLightFlicker = GetComponentPtrAs<Component_LightFlicker>();
			pLightFlicker->Enable( bData );

			isCommandValid = true;
		}
	}

	CHECK( isCommandValid );

	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();
	pRenderInfo->boundingSphereScale = math::Vec3( hLight->range );
}
