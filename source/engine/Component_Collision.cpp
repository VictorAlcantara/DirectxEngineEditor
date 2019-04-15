#include "../engine/Component_Collision.h"
#include "../engine/Component_Physics.h"
#include "../engine/Entity.h"
#include "EventEngine.h"
#include "../engine/CoreInterface.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Collision::Component_Collision()
{
	RegisterInheritance( GetClassTagStatic() );
	m_noclip = false;
	m_canActivateTrigger = false;
}
//-------------------------------------------------------------------
void Component_Collision::SetActivateTrigger(bool canActivateTrigger)
{
	m_canActivateTrigger = canActivateTrigger;

	auto pEvent = SmartPtr<Event_ComponentPropertyChange>::New();
	pEvent->hComponent = GetHandle();
	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Component_Collision::OnShutdown()
{
	if ( m_pProcessUpdateTransform->GetState() != eProcessState::Finished )
		m_pProcessUpdateTransform->Exit();
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Component_Collision::OnInitialize()
{
	m_pProcessUpdateTransform = new Process_UpdateTransform( this );
}
//-------------------------------------------------------------------
//	UpdateTransform
//-------------------------------------------------------------------
void Component_Collision::UpdateTransform()
{
	auto pEventTransform = SmartPtr<Event_TransformChange>::New();
	pEventTransform->hEntity = GetOwner();
	pEventTransform->flag = eTransformation::Collision;
	g_pEvent->Process( pEventTransform );
}
//-------------------------------------------------------------------
//	GetAabbGlobal
//-------------------------------------------------------------------
math::Aabb3 Component_Collision::GetAabbGlobal()const
{
	math::Aabb3 result( GetCollider()->aabbScale );
	math::Vec3 pos = entityGet_pos( GetOwner() );
	
	return result + pos;
}
//-------------------------------------------------------------------
//	SetCollider
//-------------------------------------------------------------------
void Component_Collision::SetCollider(eColliderInfo colliderType)
{
	CHECK( !m_pCollider );

	switch ( colliderType )
	{
		case eColliderInfo::Aabb:
			m_pCollider = new ColliderInfo_Aabb;
			break;
		case eColliderInfo::Ellipse:
			m_pCollider = new ColliderInfo_Ellipse;
			break;
		case eColliderInfo::Sphere:
			m_pCollider = new ColliderInfo_Sphere;
			break;
		default:
			ASSERT( "Unrecognized collider" );
	};

	UpdateTransform();
}
//-------------------------------------------------------------------
//	GetCollider
//-------------------------------------------------------------------
ColliderInfo* Component_Collision::GetCollider(bool updateTransform)
{
	CHECK( m_pCollider );

	if ( updateTransform && m_pProcessUpdateTransform->GetState() != eProcessState::Running && m_pCollider )
		g_pProcess->Add( m_pProcessUpdateTransform.As<Process>() );

	return m_pCollider.Get();
}
//-------------------------------------------------------------------
const ColliderInfo* Component_Collision::GetCollider()const
{
	CHECK( m_pCollider );
	return m_pCollider.Get();
}
//-------------------------------------------------------------------
//	HasCollider
//-------------------------------------------------------------------
bool Component_Collision::HasCollider()const
{
	return m_pCollider;
}
//-------------------------------------------------------------------
//	SetNoclip
//-------------------------------------------------------------------
void Component_Collision::SetNoclip(bool b)
{
	m_noclip = b;
}
//-------------------------------------------------------------------
//	Ignore
//-------------------------------------------------------------------
bool Component_Collision::Ignore(const Handle<Entity>&)const
{
	return false;
}
//-------------------------------------------------------------------
//	ClassifyCollider
//-------------------------------------------------------------------
void Component_Collision::ClassifyCollider(const Handle<Entity>& hOther, eColliderInfo& outColliderTypeThis, eColliderInfo& inOutColliderTypeOther)const
{
	outColliderTypeThis = GetCollider()->GetType();
}
