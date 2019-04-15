#include "../engine/Entity.h"
#include "../engine/ScriptReader.h"
#include "../engine/CoreInterface.h"
#include "../engine/TableHashFunction.h"
#include "../engine/Component_Physics.h"
#include "../engine/Component_Render.h"

uint Entity::s_idControl = 0;

/****************************************************************************************
	Helper function
*****************************************************************************************/

//-------------------------------------------------------------------
//	entityGet_pos
//-------------------------------------------------------------------
math::Vec3 entityGet_pos(const Handle<Entity>& hEntity)
{
	auto pCmpPhysics = hEntity->GetComponentPtrAs<Component_Physics>();

	if ( pCmpPhysics )
		return pCmpPhysics->GetPos();
	
	auto pCmpRender = hEntity->GetComponentPtrAs<Component_Render>();

	if ( pCmpRender )
		return pCmpRender->GetData()->GetTransform().GetPos();

	ASSERT( "Cannot get position from entity" );

	return math::Vec3( 0.0f );
}
//-------------------------------------------------------------------
//	entityGet_velocity
//-------------------------------------------------------------------
math::Vec3 entityGet_velocity(const Handle<Entity>& hEntity)
{
	math::Vec3 result;
	auto pCmpPhysics = hEntity->GetComponentPtrAs<Component_Physics>();

	if ( pCmpPhysics )
		result = pCmpPhysics->GetData()->velocity;

	return result;
}

/****************************************************************************************
	Entity
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity::Entity(bool mustUpdate) : m_handleComponents(20), m_tableComponents(40, tableHashFunc_Uint), m_tableComponentUpdates(30, tableHashFunc_Uint)
{
	m_id = ++s_idControl;
	m_mustUpdate = mustUpdate;
	RegisterInheritance( GetClassTagStatic() );
	m_initialized = false;
}
//-------------------------------------------------------------------
//	SetName
//-------------------------------------------------------------------
void Entity::SetName(const HashString& name)
{
	if ( name != m_name )
	{
		g_pEntity->ChangeName( GetHandle(), name );
		m_name = name;
	}
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Entity::Shutdown()
{
	if ( m_initialized )
	{
		OnShutdown();

		for ( auto it = m_tableComponents.GetIterator(); it; it++ )
		{
			SmartPtr<Event_Component> pEvent = new Event_Component;
			pEvent->action = eComponentAction::Remove;
			pEvent->hComponent = *it;

			g_pEvent->Process( pEvent );

			(*it)->Shutdown();
		}

		m_tableComponents.Clear();
		m_tableComponentUpdates.Clear();
		m_handleComponents.Clear();
	}
	else
		ASSERT( "Not initialized" );
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Entity::Initialize()
{
	if ( !m_initialized )
	{
		OnInitialize();
		m_initialized = true;
	}
	else
		ASSERT( "Already initialized" );
}
//-------------------------------------------------------------------
//	GetHandle
//-------------------------------------------------------------------
Handle<Entity> Entity::GetHandle()
{
	return m_hSelf;
}
////-------------------------------------------------------------------
////	SetRenderInfo
////-------------------------------------------------------------------
//void Entity::SetRenderInfo(RenderInfo* pRenderInfo)
//{
//	m_pRenderInfo = pRenderInfo;
//}
////-------------------------------------------------------------------
////	GetRenderInfo
////-------------------------------------------------------------------
//RenderInfo* Entity::GetRenderInfo()
//{
//	return m_pRenderInfo.Get();
//}
////-------------------------------------------------------------------
//const RenderInfo* Entity::GetRenderInfo()const
//{
//	return m_pRenderInfo.Get();
//}
////-------------------------------------------------------------------
////	OnRender
////-------------------------------------------------------------------
//void Entity::OnRender()
//{
//	if ( m_pRenderInfo )
//		m_pRenderInfo->OnRender();
//}
//-------------------------------------------------------------------
//	GetSerialized
//-------------------------------------------------------------------
String Entity::GetSerialized()const
{
	ASSERT( "Not implemented" );
	return String();
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity::ProcessCommand(const String& cmd)
{
	ScriptReader script( cmd );

	for ( uint i = 0; i < script.GetLineCount(); i++ )
		ProcessCommand( script.GetLine(i) );
}
//-------------------------------------------------------------------
void Entity::ProcessCommand(const ScriptLine*)
{
	ASSERT( "Not implemented" );
}
// TODO: delete this
////-------------------------------------------------------------------
////	UsePhysics
////-------------------------------------------------------------------
//void Entity::UsePhysics(bool hasPhysics)
//{
//	SmartPtr<Event_EntityPhysicsInfo> pEvent = new Event_EntityPhysicsInfo;
//
//	pEvent->action = hasPhysics ? eEntityPhysicsAction::Add : eEntityPhysicsAction::Remove;
//	pEvent->hEntity = m_hSelf;
//
//	g_pEvent->Process( pEvent );
//}
//-------------------------------------------------------------------
//	OnAddComponent
//-------------------------------------------------------------------
void Entity::OnAddComponent(Handle<Component>& hComponent)
{
	CHECK( hComponent );

	m_tableComponents.Add( hComponent, hComponent->GetId() );

	hComponent->Initialize( GetHandle(), hComponent );

	SmartPtr<Event_Component> pEvent = new Event_Component;
	pEvent->action = eComponentAction::Add;
	pEvent->hComponent = hComponent;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	RemoveComponent
//-------------------------------------------------------------------
bool Entity::RemoveComponent(Handle<Component>& hComponent)
{
	if ( hComponent )
		return RemoveComponent( hComponent->GetId() );
	return false;
}
//-------------------------------------------------------------------
bool Entity::RemoveComponent(uint id)
{
	Handle<Component> hFind = m_tableComponents.Get( id );

	if ( hFind )
	{
		SmartPtr<Event_Component> pEvent = new Event_Component;
		pEvent->action = eComponentAction::Remove;
		pEvent->hComponent = hFind;

		g_pEvent->Process( pEvent );

		m_tableComponents.Remove( id );
		m_tableComponentUpdates.Remove( id );
		m_handleComponents.Remove( hFind );

		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	ComponentUpdateEveryFrame
//-------------------------------------------------------------------
void Entity::ComponentUpdateEveryFrame(bool b, uint componentId)
{
	auto hFind = m_tableComponents.Get( componentId );

	CHECK( hFind );

	if ( hFind )
	{
		if ( b )
			m_tableComponentUpdates.AddOrChange( hFind, hFind->GetId() );
		else
			m_tableComponentUpdates.Remove( hFind->GetId() );
	}
}
//-------------------------------------------------------------------
//	UpdateComponents
//-------------------------------------------------------------------
void Entity::UpdateComponents()
{
	for ( auto it = m_tableComponentUpdates.GetIterator(); it; it++ )
		(*it)->Update();
}
