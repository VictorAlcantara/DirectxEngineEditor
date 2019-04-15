#include "Entity_SpotLight.h"
#include "../engine/CoreInterface.h"
#include "../shared/Command.h"
#include "../editor/Grid.h"
#include "../shared/EntityUtil.h"
#include "../shared/Component_LightFlicker.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_SpotLight::Entity_SpotLight() : Entity(false)
{
	RegisterInheritance( "Entity_SpotLight" );
}
//-------------------------------------------------------------------
Entity_SpotLight::~Entity_SpotLight()
{

}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_SpotLight::OnShutdown()
{
	g_pScene->GetLightMngr()->RemovePointLight( m_hLight->GetId() );
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_SpotLight::OnInitialize()
{
	m_hLight = g_pScene->GetLightMngr()->AddSpotLight();

	auto pLightFlicker = AddComponentAs<Component_LightFlicker>();
	pLightFlicker->SetLight( m_hLight->GetId() );
	pLightFlicker->Enable( false );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Entity_SpotLight::Update()
{
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_SpotLight::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_SPOT_LIGHT );

	isCommandValid |= processCommand_MaterialColor( command::DIFFUSE_COLOR, &m_hLight->diffuseColor4[0], m_hLight->diffuseColor4.GetComponentCount(), pLine );
	isCommandValid |= processCommand_MaterialColor( command::SPECULAR_COLOR, &m_hLight->specularColor4[0], m_hLight->specularColor4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_0, m_hLight->attenuation0 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_1, m_hLight->attenuation1 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_2, m_hLight->attenuation2 );
	isCommandValid |= pLine->SetFloat( command::SHADOW_DISTANCE_ADJUSTMENT, m_hLight->shadowDistanceAdjustment );

	isCommandValid |= pLine->SetBool( command::SHADOW_ENABLED, m_hLight->shadowEnabled );

	if ( !isCommandValid )
	{
		VecGrid gridData;
		math::Vec3 vec3Data;
		float floatData;
		bool bData;

		isCommandValid = true;

		if ( pLine->SetIntArray( command::GRID_POS, &gridData[0], 3 ) )
		{
			m_hLight->SetPos( Grid::ToVec3(gridData) );
		}
		else if ( pLine->SetFloatArray(command::DIRECTION, &vec3Data[0], 3, false) )
			m_hLight->SetDirection( vec3Data );
		else if ( pLine->SetFloat(command::LIGHT_RANGE, floatData) )
			m_hLight->SetRange( floatData );
		else if ( pLine->SetFloat(command::LIGHT_INNER_CONE_DEGS, floatData) )
			m_hLight->SetInnerCone( floatData );
		else if ( pLine->SetFloat(command::LIGHT_OUTER_CONE_DEGS, floatData) )
			m_hLight->SetOuterCone( floatData );
		else if ( pLine->SetFloat(command::SHADOW_BIAS, floatData) )
			m_hLight->shadowBias = SpotLight::NormalizedToShadowBias( floatData );
		else if ( pLine->SetBool(command::LIGHT_FLICKER, bData) )
		{
			auto pLightFlicker = GetComponentPtrAs<Component_LightFlicker>();
			pLightFlicker->Enable( bData );
		}
		else
			isCommandValid = false;
	}

	CHECK( isCommandValid );
}
