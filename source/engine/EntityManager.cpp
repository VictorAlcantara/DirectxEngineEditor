#include "EntityManager.h"
#include "../engine/TableHashFunction.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EntityManager::EntityManager() : 
	m_handles(1000), 
	m_tableEntity(3000, tableHashFunc_Uint), 
	m_tableEntityName(3000, tableHashFunc_HashString), 
	m_tableEntityClassTag(3000, tableHashFunc_HashString), 
	m_tableUpdate(500, tableHashFunc_Uint)
{
	m_initialized = false;
}
//-------------------------------------------------------------------
EntityManager::~EntityManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EntityManager::Shutdown()
{
	SmartPtr<Event_EntityManager> pEvent = new Event_EntityManager;
	pEvent->action = eEventEntity::RemoveAll;

	SendEvent( pEvent );

	ClearEntityFinder();

	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_tableUpdate.Clear();

	for ( auto it = m_tableEntity.GetIterator(); it; it++ )
	{
		(*it)->Shutdown();
	}

	m_tableEntityName.Clear();
	m_tableEntityClassTag.Clear();
	m_tableEntity.Clear();
	m_handles.Clear();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EntityManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EntityManager::Update()
{
	//g_pCore->ProfileStart( engineDefault::profile::ENTITY );

	for ( auto it = m_scheduleRemove.GetIterator(); it; it++ )
		Remove( (*it) );
	m_scheduleRemove.Clear();

	for ( auto it = m_tableUpdate.GetIterator(); it; it++ )
	{
		(*it)->Update();
		(*it)->UpdateComponents();
	}

	//g_pCore->ProfileEnd( engineDefault::profile::ENTITY );
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void EntityManager::Remove(uint id)
{
	Handle<Entity> hEntity = m_tableEntity.Get( id );

	if ( hEntity.IsValid() )
	{
		SmartPtr<Event_EntityManager> pEvent = new Event_EntityManager;
		pEvent->action = eEventEntity::Remove;
		pEvent->hEntity = hEntity;

		SendEvent( pEvent );

		if ( m_pFinder )
			m_pFinder->Remove( hEntity );

		ChangeName( hEntity, "" );
		RemoveFromClassTagTable( hEntity );

		hEntity->Shutdown();

		//HashString classTag = hEntity->GetClassTag();
		//static HashString classTagPlayer = "Entity_Player";
		//CHECK( classTag != classTagPlayer );

		m_tableUpdate.Remove( id );
		m_tableEntity.Remove( id );
		m_handles.Remove( hEntity );
	}
}
//-------------------------------------------------------------------
void EntityManager::Remove(Handle<Entity> hEntity)
{
	if ( hEntity.IsValid() )
		Remove( hEntity->GetId() );
}
//-------------------------------------------------------------------
//	RemoveNextFrame
//-------------------------------------------------------------------
void EntityManager::RemoveNextFrame(uint id)
{
	m_scheduleRemove.AddLast( id );
}
//-------------------------------------------------------------------
void EntityManager::RemoveNextFrame(Handle<Entity> entity)
{
	RemoveNextFrame( entity->GetId() );
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void EntityManager::Clear()
{
	ClearEntityFinder();

	List<uint> ids;

	for ( auto it = m_tableEntity.GetIterator(); it; it++ )
		ids.AddLast( (*it)->GetId() );

	for ( auto it = ids.GetIterator(); it; it++ )
		Remove( (*it) );
}
//-------------------------------------------------------------------
//	GetIterator
//-------------------------------------------------------------------
TableIterator<Handle<Entity>> EntityManager::GetIterator()
{
	return m_tableEntity.GetIterator();
}
//-------------------------------------------------------------------
TableIteratorConst<Handle<Entity>> EntityManager::GetIterator()const
{
	return m_tableEntity.GetIterator();
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
Handle<Entity> EntityManager::GetById(uint id)
{
	return m_tableEntity.Get( id );
}
//-------------------------------------------------------------------
TableIterator<Handle<Entity>> EntityManager::GetByName(const HashString& name)
{
	TableIterator<Handle<Entity>> result;
	auto pTable = m_tableEntityName.Get( name );
	
	if ( pTable )
		result = pTable->GetIterator();
		
	return result;
}
//-------------------------------------------------------------------
TableIterator<Handle<Entity>> EntityManager::GetByClassTag(const HashString& classTag)
{
	TableIterator<Handle<Entity>> result;
	auto pTable = m_tableEntityClassTag.Get( classTag );

	if ( pTable )
		result = pTable->GetIterator();

	return result;
}
//-------------------------------------------------------------------
//	CountName
//-------------------------------------------------------------------
uint EntityManager::CountName(const HashString& name)const
{
	uint result = 0;

	auto pTable = m_tableEntityName.Get( name );

	if ( pTable )
		result = pTable->Count();

	return result;
}
//-------------------------------------------------------------------
//	CountClassTag
//-------------------------------------------------------------------
uint EntityManager::CountClassTag(const HashString& name)const
{
	uint result = 0;

	auto pTable = m_tableEntityClassTag.Get( name );

	if ( pTable )
		result = pTable->Count();

	return result;
}
//-------------------------------------------------------------------
//	SendEvent
//-------------------------------------------------------------------
void EntityManager::SendEvent(SmartPtr<Event_EntityManager> pEvent)
{
	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	OnCreate
//-------------------------------------------------------------------
void EntityManager::OnCreate(Handle<Entity> hEntity)
{
	hEntity->m_hSelf = hEntity;
	hEntity->Initialize();

	if ( hEntity->MustUpdate() )
		m_tableUpdate.Add( hEntity, hEntity->GetId() );

	SmartPtr<Event_EntityManager> pEvent = new Event_EntityManager;
	pEvent->action = eEventEntity::Create;
	pEvent->hEntity = hEntity;

	SendEvent( pEvent );

	if ( m_pFinder )
		m_pFinder->Add( hEntity );

	ChangeName( hEntity, hEntity->GetName() );
	AddToClassTagTable( hEntity );
}
//-------------------------------------------------------------------
//	ChangeName
//-------------------------------------------------------------------
void EntityManager::ChangeName(Handle<Entity> hEntity, const HashString& newName)
{
	HashString oldName = hEntity->GetName();

	if ( oldName.IsValid() )
	{
		auto pTableOld = m_tableEntityName.Get( oldName );
		CHECK( pTableOld );
		pTableOld->Remove( hEntity->GetId() );
	}


	if ( newName.IsValid() )
	{
		auto pTableNew = m_tableEntityName.Get( newName );

		if ( !pTableNew )
		{
			pTableNew = new Table<Handle<Entity>, uint>( 50, tableHashFunc_Uint );
			m_tableEntityName.Add( pTableNew, newName );
		}

		if ( newName.IsValid() )
			pTableNew->Add( hEntity, hEntity->GetId() );
	}
}
//-------------------------------------------------------------------
//	AddToClassTagTable
//-------------------------------------------------------------------
void EntityManager::AddToClassTagTable(Handle<Entity> hEntity)
{
	const HashString& classTag = hEntity->GetClassTag();
	auto pFind = m_tableEntityClassTag.Get( classTag );

	if ( !pFind )
	{
		pFind = new Table<Handle<Entity>, uint>( 1000, tableHashFunc_Uint );
		m_tableEntityClassTag.Add( pFind, classTag );
	}

	pFind->Add( hEntity, hEntity->GetId() );
}
//-------------------------------------------------------------------
//	RemoveFromClassTagTable
//-------------------------------------------------------------------
void EntityManager::RemoveFromClassTagTable(Handle<Entity> hEntity)
{
	const HashString& classTag = hEntity->GetClassTag();
	auto pFind = m_tableEntityClassTag.Get( classTag );

	CHECK( pFind && pFind->Get(hEntity->GetId()) );

	if ( pFind )
		pFind->Remove( hEntity->GetId() );
}
//-------------------------------------------------------------------
//	SetEntityFinder
//-------------------------------------------------------------------
void EntityManager::SetEntityFinder(EntityFinder* pFinder)
{
	ClearEntityFinder();
	m_pFinder = pFinder;
	if ( !m_pFinder->IsInitialized() )
		m_pFinder->Initialize();
}
//-------------------------------------------------------------------
//	GetEntityFinder
//-------------------------------------------------------------------
EntityFinder* EntityManager::GetEntityFinder()
{
	CHECK( m_pFinder );
	return m_pFinder.Get();
}
//-------------------------------------------------------------------
//	ClearEntityFinder
//-------------------------------------------------------------------
void EntityManager::ClearEntityFinder()
{
	if ( m_pFinder )
		m_pFinder->Shutdown();
	m_pFinder = nullptr;
}
