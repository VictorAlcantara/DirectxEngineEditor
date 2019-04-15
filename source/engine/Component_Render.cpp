#include "../engine/Component_Render.h"
#include "../engine/CoreInterface.h"
#include "EventEngine.h"

/****************************************************************************************
	Component_Render
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Render::Component_Render()
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Component_Render::OnShutdown()
{
	if ( m_pProcessUpdateTransform->GetState() != eProcessState::Finished )
		m_pProcessUpdateTransform->Exit();
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Component_Render::OnInitialize()
{
	m_pProcessUpdateTransform = new Process_UpdateTransform( this );
}
//-------------------------------------------------------------------
//	SetData
//-------------------------------------------------------------------
void Component_Render::SetData(SmartPtr<RenderInfo>& pData)
{
	CHECK( !m_pData );

	if ( !m_pData )
		m_pData = pData;

	UpdateTransform();
}
//-------------------------------------------------------------------
//	UpdateTransform
//-------------------------------------------------------------------
void Component_Render::UpdateTransform()
{
	auto pEventTransform = SmartPtr<Event_TransformChange>::New();
	pEventTransform->hEntity = GetOwner();
	pEventTransform->flag = eTransformation::Render;
	g_pEvent->Process( pEventTransform );
}
//-------------------------------------------------------------------
//	GetData
//-------------------------------------------------------------------
RenderInfo* Component_Render::GetData(bool updateTransform)
{
	if ( updateTransform && m_pProcessUpdateTransform->GetState() != eProcessState::Running && m_pData )
		 g_pProcess->Add( m_pProcessUpdateTransform.As<Process>() );

	return m_pData.Get();
}
//-------------------------------------------------------------------
const RenderInfo* Component_Render::GetData()const
{
	return m_pData.Get();
}
