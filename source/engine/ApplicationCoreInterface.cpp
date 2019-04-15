#include "ApplicationCoreInterface.h"
#include "../engine/Exception.h"
#include "Application.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ApplicationCoreInterface::ApplicationCoreInterface(Application* pApplication)
{
	m_pApplication = pApplication;

	if ( !m_pApplication )
	{
		THROW( "Application is null" );
	}
}
//-------------------------------------------------------------------
//	ShowCursor
//-------------------------------------------------------------------
void ApplicationCoreInterface::ShowCursor(bool show)
{
	m_pApplication->ShowCursor( show );
}
//-------------------------------------------------------------------
//	SetMousePos
//-------------------------------------------------------------------
void ApplicationCoreInterface::SetMousePos(const math::Vec2& pos)
{
	m_pApplication->SetCursorPos( (uint)pos.x, (uint)pos.y );
}
//-------------------------------------------------------------------
//	IsCursorVisible
//-------------------------------------------------------------------
bool ApplicationCoreInterface::IsCursorVisible()
{
	return m_pApplication->IsCursorVisible();
}
