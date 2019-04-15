#include "EditorEntity_SpotLight.h"
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
EditorEntity_SpotLight::EditorEntity_SpotLight() : EditorEntity(true)
{
	RegisterInheritance( GetClassTagStatic() );
	m_editorShadowBias = 0.0;
}
//-------------------------------------------------------------------
//	OnShutdown_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_SpotLight::OnShutdown_EditorEntity()
{
	g_pScene->GetLightMngr()->RemoveSpotLight( m_hLight->GetId() );
}
//-------------------------------------------------------------------
//	OnInitialize_EditorEntity
//-------------------------------------------------------------------
void EditorEntity_SpotLight::OnInitialize_EditorEntity()
{
	m_adjustPosToPair = false;
	m_toolActionFlag = TOOL_ALLOW_TRANSLATE | TOOL_ALLOW_ROTATE;

	auto pRenderInfo = SmartPtr<RenderInfo_EditorEntity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->SetModel( GetModel() );
	pRenderInfo->hDiffuseTexture = primitive::texture::createWhite();
	pRenderInfo->highlightColor4 = editorGlobal::entity::HIGHLIGHT_COLOR;
	pRenderInfo->material = eMaterial::Opaque;
	pRenderInfo->castShadow = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	int scale = (int)((float)editorGlobal::grid::UNIT_SIZE_ONE * 0.1f);
	CHECK( scale > 0 );
	SetGridScale( scale, scale, scale );

	m_hLight = g_pScene->GetLightMngr()->AddSpotLight();
	m_hLight->specularColor4 = math::normalizeRgb(255, 255, 255);
	m_hLight->attenuation0 = 0.7f;
	m_hLight->attenuation1 = 0.6f;
	m_hLight->attenuation2 = 1.0f;
	m_hLight->shadowEnabled = false;

	pRenderInfo->renderAabb = true;
	pRenderInfo->renderDirection = true;
	pRenderInfo->direction = m_hLight->GetDirection();

	auto pLightFlicker = AddComponentAs<Component_LightFlicker>();
	pLightFlicker->SetLight( m_hLight->GetId() );
	pLightFlicker->Enable( false );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntity_SpotLight::Update()
{
	if ( !math::isEqual(m_hLight->GetPos(), GetPos())  )
		m_hLight->SetPos( GetPos() );
}
//-------------------------------------------------------------------
//	RotateYawPitchRoll
//-------------------------------------------------------------------
void EditorEntity_SpotLight::RotateYawPitchRoll(float yaw, float pitch, float roll)
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();
	math::Mtx44 rotate = math::matrixYawPitchRoll( yaw, pitch, roll );

	m_hLight->SetDirection(m_hLight->GetDirection() * rotate);
	pRenderInfo->direction = m_hLight->GetDirection();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void EditorEntity_SpotLight::ProcessCommand(const ScriptLine* pLine)
{
	auto hLight = GetLight();

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_SPOT_LIGHT );

	isCommandValid |= processCommand_MaterialColor( command::DIFFUSE_COLOR, &hLight->diffuseColor4[0], hLight->diffuseColor4.GetComponentCount(), pLine );
	isCommandValid |= processCommand_MaterialColor( command::SPECULAR_COLOR, &hLight->specularColor4[0], hLight->specularColor4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_0, hLight->attenuation0 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_1, hLight->attenuation1 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_2, hLight->attenuation2 );
	isCommandValid |= pLine->SetFloat( command::SHADOW_DISTANCE_ADJUSTMENT, hLight->shadowDistanceAdjustment );
	isCommandValid |= pLine->SetBool( command::SHADOW_ENABLED, hLight->shadowEnabled );

	isCommandValid |= ProcessCommand_GridPosScale( pLine );

	if ( !isCommandValid )
	{
		math::Vec3 vec3Data;
		float floatData;
		bool bData;

		isCommandValid = true;

		if ( pLine->SetFloatArray(command::DIRECTION, &vec3Data[0], 3, false) )
			hLight->SetDirection( vec3Data );
		else if ( pLine->SetFloat(command::LIGHT_RANGE, floatData) )
			hLight->SetRange( floatData );
		else if ( pLine->SetFloat(command::LIGHT_INNER_CONE_DEGS, floatData) )
			hLight->SetInnerCone( floatData );
		else if ( pLine->SetFloat(command::LIGHT_OUTER_CONE_DEGS, floatData) )
			hLight->SetOuterCone( floatData );
		else if ( pLine->SetFloat(command::SHADOW_BIAS, m_editorShadowBias) )
		{
			hLight->shadowBias = SpotLight::NormalizedToShadowBias( m_editorShadowBias );
		}
		else if ( pLine->SetBool(command::LIGHT_FLICKER, bData) )
		{
			auto pLightFlicker = GetComponentPtrAs<Component_LightFlicker>();
			pLightFlicker->Enable( bData );
		}
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );

	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();
	pRenderInfo->direction = m_hLight->GetDirection();
}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String EditorEntity_SpotLight::GetSerialized()const
{
	auto pLightFlicker = GetComponentPtrAs<Component_LightFlicker>();

	return command::serializeLine(command::ENTITY_SPOT_LIGHT) + 
		command::serializeLine(command::GRID_POS, GetGridPos().AsString() ) + 
		command::serializeLine(command::DIRECTION, string::vec3ToStr(m_hLight->GetDirection()) ) + 
		command::serializeLine(command::LIGHT_RANGE, string::floatToStr(m_hLight->GetRange()) ) + 
		command::serializeLine(command::LIGHT_INNER_CONE_DEGS, string::floatToStr(m_hLight->GetInnerConeDegs()) ) + 
		command::serializeLine(command::LIGHT_OUTER_CONE_DEGS, string::floatToStr(m_hLight->GetOuterConeDegs()) ) + 
		command::serializeLine(command::DIFFUSE_COLOR, string::vec4ToStr(m_hLight->diffuseColor4) ) + 
		command::serializeLine(command::SPECULAR_COLOR, string::vec4ToStr(m_hLight->specularColor4) ) + 
		command::serializeLine(command::LIGHT_ATTENUATION_0, string::floatToStr(m_hLight->attenuation0) ) + 
		command::serializeLine(command::LIGHT_ATTENUATION_1, string::floatToStr(m_hLight->attenuation1) ) + 
		command::serializeLine(command::LIGHT_ATTENUATION_2, string::floatToStr(m_hLight->attenuation2) ) + 
		command::serializeLine(command::SHADOW_ENABLED, string::boolToStr(m_hLight->shadowEnabled)) +
		command::serializeLine(command::SHADOW_BIAS, string::floatToStr(m_editorShadowBias)) +
		command::serializeLine(command::SHADOW_DISTANCE_ADJUSTMENT, string::floatToStr(m_hLight->shadowDistanceAdjustment)) +
		command::serializeLine(command::LIGHT_FLICKER, string::boolToStr(pLightFlicker->IsEnabled()) )+ 
		command::serializeLine();
}
