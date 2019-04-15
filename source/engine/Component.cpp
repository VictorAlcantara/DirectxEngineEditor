#include "../engine/Component.h"
#include "../engine/Entity.h"

uint Component::s_idControl = 1;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component::Component()
{
	RegisterInheritance( GetClassTagStatic() );
	m_initialized = false;
	m_id = s_idControl++;
}
//-------------------------------------------------------------------
Component::~Component()
{
	if ( m_initialized )
	{
		ASSERT( "Initialized component being deleted without Shutdown()" );
	}
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Component::Shutdown()
{
	if ( m_initialized )
	{
		OnShutdown();
		m_initialized = false;
	}
	else
	{
		ASSERT( "Cannot shutdown uninitialized component" );
	}
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Component::Initialize(Handle<Entity>& hOwner, Handle<Component>& hSelf)
{
	if ( !m_initialized )
	{
		CHECK( hOwner );
		CHECK( GetId() == hSelf->GetId() );
		m_hSelf = hSelf;
		m_hOwner = hOwner;
		OnInitialize();
		m_initialized = true;
	}
	else
	{
		ASSERT( "Cannot initialize already initialized component" );
	}
}
//-------------------------------------------------------------------
//	UpdateEveryFrame
//-------------------------------------------------------------------
void Component::UpdateEveryFrame(bool b)
{
	GetOwner()->ComponentUpdateEveryFrame( b, GetId() );
}
//-------------------------------------------------------------------
//	GetHandle
//-------------------------------------------------------------------
Handle<Component> Component::GetHandle()
{
	return m_hSelf;
}
//-------------------------------------------------------------------
//	GetOwner
//-------------------------------------------------------------------
Handle<Entity> Component::GetOwner()
{
	return m_hOwner;
}
//-------------------------------------------------------------------
const Handle<Entity> Component::GetOwner()const
{
	return m_hOwner;
}
