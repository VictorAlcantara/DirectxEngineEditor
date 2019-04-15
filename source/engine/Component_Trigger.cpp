#include "../engine/Component_Trigger.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Trigger::Component_Trigger()
{
	RegisterInheritance( GetClassTagStatic() );
	m_noclip = false;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Component_Trigger::OnInitialize()
{
	m_pCollider = new ColliderInfo_Aabb;
}
//-------------------------------------------------------------------
//	GetCollider
//-------------------------------------------------------------------
ColliderInfo_Aabb* Component_Trigger::GetCollider()
{
	CHECK( m_pCollider );
	return m_pCollider.Get();
}
//-------------------------------------------------------------------
const ColliderInfo_Aabb* Component_Trigger::GetCollider()const
{
	CHECK( m_pCollider );
	return m_pCollider.Get();
}
//-------------------------------------------------------------------
//	SetNoclip
//-------------------------------------------------------------------
void Component_Trigger::SetNoclip(bool b)
{
	m_noclip = b;
}
//-------------------------------------------------------------------
//	Ignore
//-------------------------------------------------------------------
bool Component_Trigger::Ignore(const Handle<Entity>&)const
{
	return false;
}
