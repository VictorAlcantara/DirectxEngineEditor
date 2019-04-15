#include "../engine/Component_Physics.h"
#include "../engine/CoreInterface.h"
#include "EventEngine.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Physics::Component_Physics()
{
	RegisterInheritance( GetClassTagStatic() );
	m_updateEnabled = true;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Component_Physics::OnShutdown()
{
	if ( m_pProcessUpdateTransform && m_pProcessUpdateTransform->GetState() != eProcessState::Finished )
		m_pProcessUpdateTransform->Exit();
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Component_Physics::OnInitialize()
{
	//m_pProcessUpdateTransform = new Process_UpdateTransform( this );
}
//-------------------------------------------------------------------
//	EnableUpdate
//-------------------------------------------------------------------
void Component_Physics::EnableUpdate(bool b)
{
	m_updateEnabled = b;

	auto pEvent = SmartPtr<Event_ComponentPropertyChange>::New();
	pEvent->hComponent = GetHandle();
	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	UpdateTransform
//-------------------------------------------------------------------
void Component_Physics::UpdateTransform()
{
	auto pEventTransform = SmartPtr<Event_TransformChange>::New();
	pEventTransform->hEntity = GetOwner();
	pEventTransform->flag = eTransformation::Physics;
	g_pEvent->Process( pEventTransform );
}
//-------------------------------------------------------------------
//	SetPos
//-------------------------------------------------------------------
void Component_Physics::SetPos(const math::Vec3& pos)
{
	m_pos = pos;
	UpdateTransform();
}
//-------------------------------------------------------------------
//	AddPos
//-------------------------------------------------------------------
void Component_Physics::AddPos(const math::Vec3& pos)
{
	m_pos += pos;
	UpdateTransform();
}
//-------------------------------------------------------------------
//	AddForce
//-------------------------------------------------------------------
void Component_Physics::AddForce(const math::Vec3& force)
{
	m_force += force;
}
//-------------------------------------------------------------------
//	ClearForce
//-------------------------------------------------------------------
void Component_Physics::ClearForce()
{
	m_force = math::Vec3( 0.0f );
}
