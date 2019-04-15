#include "EditorEntityFinder.h"
#include "../engine/TableHashFunction.h"
#include "../engine/CoreInterface.h"
#include "EditorEntity.h"
#include "EditorEntity_CameraTrack.h"
#include "EditorEntity_TriggerCamera.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorEntityFinder::EditorEntityFinder() : m_tableCameraTrack(200, tableHashFunc_Uint)
{
	m_initialized = false;

	for ( uint i = 0; i < ARRAY_COUNT(m_tables); i++ )
		m_tables[i] = nullptr;

	m_tables[(uint)eEditorEntityType::CameraTrack] = &m_tableCameraTrack;
}
//-------------------------------------------------------------------
EditorEntityFinder::~EditorEntityFinder()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void EditorEntityFinder::Clear()
{
	for ( uint i = 0; i < ARRAY_COUNT(m_tables); i++ )
	{
		if ( m_tables[i] )
		m_tables[i]->Clear();
	}
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EditorEntityFinder::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
	}
	else
	{
		ShutdownListener();
		m_initialized = false;
	}
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EditorEntityFinder::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
	}
	else
	{
		InitializeListener();
		m_initialized = true;
	}
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorEntityFinder::Update()
{

}
//-------------------------------------------------------------------
//	Find
//-------------------------------------------------------------------
Handle<Entity> EditorEntityFinder::Find(uint flag, uint key)
{
	Table<Handle<Entity>, uint>* pTable = GetTable(flag);
	CHECK( pTable );

	return pTable->Get(key);
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
void EditorEntityFinder::Add(Handle<Entity> hEntity)
{
	uint flag = (uint)hEntity.GetAs<EditorEntity>().GetType();
	Table<Handle<Entity>, uint>* pTable = GetTable( flag );

	if ( hEntity->GetClassRelationship_Is<EditorEntity_CameraTrack>() )
	{
		uint trackId = hEntity.GetAs<EditorEntity_CameraTrack>().GetTrackId();

		#ifdef DEBUG_MODE
			Handle<Entity> hFind = pTable->Get( trackId );
			CHECK( !hFind );
		#endif

		pTable->Add( hEntity, trackId );
	}
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void EditorEntityFinder::Remove(Handle<Entity> hEntity)
{
	uint flag = (uint)hEntity.GetAs<EditorEntity>().GetType();
	Table<Handle<Entity>, uint>* pTable = GetTable( flag );

	if ( hEntity->GetClassRelationship_Is<EditorEntity_CameraTrack>() )
	{
		uint trackId = hEntity.GetAs<EditorEntity_CameraTrack>().GetTrackId();
		m_tableCameraTrack.Remove( trackId );
	}
}
//-------------------------------------------------------------------
//	GetTable
//-------------------------------------------------------------------
Table<Handle<Entity>, uint>* EditorEntityFinder::GetTable(uint flag)
{
	CHECK( flag < (uint)eEditorEntityType::Count );
	Table<Handle<Entity>, uint>* pResult = m_tables[flag];
	return pResult;
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void EditorEntityFinder::HandleEvent(Event* pEvent)
{
	Event_EntityManager* pEventEntity = nullptr;

	if ( isEvent<Event_EntityManager>(pEvent, &pEventEntity) )
	{
		if ( pEventEntity->action == eEventEntity::Create )
		{
			Add( pEventEntity->hEntity );
		}
		else if ( pEventEntity->action == eEventEntity::Remove )
		{
			Remove( pEventEntity->hEntity );
		}
	}
}
