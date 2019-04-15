#include "GameEntityFinder.h"
#include "../engine/Entity.h"
#include "Entity_CameraTrack.h"
#include "../engine/TableHashFunction.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GameEntityFinder::GameEntityFinder() : m_tableCameraTrack(50, tableHashFunc_Uint)
{
}
//-------------------------------------------------------------------
GameEntityFinder::~GameEntityFinder()
{
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
void GameEntityFinder::Add(Handle<Entity> hEntity)
{
	if ( hEntity->GetClassRelationship_Is<Entity_CameraTrack>() )
	{
		uint trackId = hEntity.GetAs<Entity_CameraTrack>().GetTrackId();
		Handle<Entity> hFind = m_tableCameraTrack.Get( trackId );

		CHECK( !hFind || hFind->GetId() == hEntity->GetId() );

		if ( !hFind )
			m_tableCameraTrack.Add( hEntity, trackId );
	}
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void GameEntityFinder::Remove(Handle<Entity> hEntity)
{
	if ( hEntity->GetClassRelationship_Is<Entity_CameraTrack>() )
	{
		uint trackId = hEntity.GetAs<Entity_CameraTrack>().GetTrackId();
		
		#ifdef DEBUG_MODE
			Handle<Entity> hFind = m_tableCameraTrack.Get( trackId );
			CHECK( !hFind || hFind->GetId() == hEntity->GetId() );
		#endif

		m_tableCameraTrack.Remove( trackId );
	}
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void GameEntityFinder::Clear()
{
	m_tableCameraTrack.Clear();
}
//-------------------------------------------------------------------
//	Find
//-------------------------------------------------------------------
Handle<Entity> GameEntityFinder::Find(uint flag, uint key)
{
	if ( flag == Entity_CameraTrack::GetClassTagStatic().GetHash() )
		return m_tableCameraTrack.Get( key );
	else
		ASSERT( "Invalid flag" );

	return Handle<Entity>();
}
