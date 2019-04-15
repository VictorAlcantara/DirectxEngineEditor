#include "../shared/Component_LightFlicker.h"
#include "../engine/Light.h"
#include "../engine/CoreInterface.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_LightFlicker::Component_LightFlicker()
{
	RegisterInheritance( GetClassTagStatic() );
	m_timer = -1.0f;
	m_enabled = false;
	m_lightId = 0;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Component_LightFlicker::OnInitialize()
{
	UpdateEveryFrame( true );
}
//-------------------------------------------------------------------
//	SetLight
//-------------------------------------------------------------------
void Component_LightFlicker::SetLight(uint lightId)
{
	m_lightId = lightId;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Component_LightFlicker::Update()
{
	if ( !m_enabled )
		return;

	if ( m_timer < 0.0f && m_lightId > 0 )
	{
		auto pLight = g_pScene->GetLightMngr()->GetLightPtr(m_lightId);
		if ( pLight )
		{
			bool lightState = math::random() < 0.5f ? true : false;
			pLight->enabled = lightState;
		}

		ResetTimer();
	}
	else
		m_timer -= g_pCore->GetTimeSinceLastFrame();
}
//-------------------------------------------------------------------
//	ResetTimer
//-------------------------------------------------------------------
void Component_LightFlicker::ResetTimer()
{
	m_timer = math::random( 0.1f, 0.8f );
}
//-------------------------------------------------------------------
//	Enable
//-------------------------------------------------------------------
void Component_LightFlicker::Enable(bool b)
{
	m_enabled = b;

	if ( !m_enabled )
	{
		auto pLight = g_pScene->GetLightMngr()->GetLightPtr(m_lightId);
		if ( pLight )
			pLight->enabled = true;
	}
}
