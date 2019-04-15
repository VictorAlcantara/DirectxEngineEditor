#include "Entity_PointLight.h"
#include "../engine/CoreInterface.h"
#include "../shared/Command.h"
#include "../editor/Grid.h"
#include "../shared/EntityUtil.h"
#include "../shared/Component_LightFlicker.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_PointLight::Entity_PointLight() : Entity(false)
{
	RegisterInheritance( "Entity_PointLight" );
}
//-------------------------------------------------------------------
Entity_PointLight::~Entity_PointLight()
{
	
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_PointLight::OnShutdown()
{
	g_pScene->GetLightMngr()->RemovePointLight( m_hLight->GetId() );
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_PointLight::OnInitialize()
{
	m_hLight = g_pScene->GetLightMngr()->AddPointLight();

	auto pLightFlicker = AddComponentAs<Component_LightFlicker>();
	pLightFlicker->SetLight( m_hLight->GetId() );
	pLightFlicker->Enable( false );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Entity_PointLight::Update()
{
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_PointLight::ProcessCommand(const ScriptLine* pLine)
{
	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_POINT_LIGHT );
	isCommandValid |= pLine->SetFloat( command::LIGHT_RANGE, m_hLight->range );

	isCommandValid |= processCommand_MaterialColor( command::DIFFUSE_COLOR, &m_hLight->diffuseColor4[0], m_hLight->diffuseColor4.GetComponentCount(), pLine );
	isCommandValid |= processCommand_MaterialColor( command::SPECULAR_COLOR, &m_hLight->specularColor4[0], m_hLight->specularColor4.GetComponentCount(), pLine );

	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_0, m_hLight->attenuation0 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_1, m_hLight->attenuation1 );
	isCommandValid |= pLine->SetFloat( command::LIGHT_ATTENUATION_2, m_hLight->attenuation2 );

	if ( !isCommandValid )
	{
		VecGrid gridData;
		math::Vec4 vec4Data;
		bool bData;
		isCommandValid = true;

		if ( pLine->SetIntArray( command::GRID_POS, &gridData[0], 3 ) )
		{
			m_hLight->pos = Grid::ToVec3( gridData );
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
}
