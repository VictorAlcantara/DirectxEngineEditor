#include "Entity_CameraTrack.h"
#include "Entity_Player.h"
#include "Entity_GameCamera.h"
#include "RenderInfo_Entity.h"
#include "../engine/CoreInterface.h"
#include "../shared/Command.h"
#include "../editor/Grid.h"

uint Entity_CameraTrack::s_trackIdControl = 0;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_CameraTrack::Entity_CameraTrack() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );

	do 
	{
		s_trackIdControl++;
	} while ( g_pEntity && g_pEntity->GetEntityFinder() && g_pEntity->GetEntityFinder()->Find(Entity_CameraTrack::GetClassTagStatic().GetHash(), s_trackIdControl) );

	m_trackId = s_trackIdControl;
}
//-------------------------------------------------------------------
Entity_CameraTrack::~Entity_CameraTrack()
{
}
//-------------------------------------------------------------------
//	UpdateCamera
//-------------------------------------------------------------------
void Entity_CameraTrack::UpdateCamera(Handle<Entity> hTarget, Handle<Entity> hCamera)
{
	CHECK( hTarget && hCamera && hTarget->GetClassRelationship_Is<Entity_Player>() && hCamera->GetClassRelationship_Is<Entity_GameCamera>() );

	Entity_GameCamera* pCamera = &hCamera.GetAs<Entity_GameCamera>();
	math::Vec3 targetPos = entityGet_pos(hTarget);
	math::Vec3 cameraPos;
	float proj = 0.0f;

	if ( !math::isEqual(m_point0, m_point1) )
		proj = math::projectPointOnLine( targetPos, m_point0, m_point1 );

	proj = math::clamp( proj, 0.0f, 1.0f );
		
	cameraPos = m_point0 + (m_point1 - m_point0) * proj;

	pCamera->SetPos( cameraPos );
	pCamera->LookAt( targetPos );
}
//-------------------------------------------------------------------
//	AssignNewTrackId
//-------------------------------------------------------------------
void Entity_CameraTrack::AssignNewTrackId(uint trackId)
{
	g_pEntity->GetEntityFinder()->Remove( GetHandle() );
	m_trackId = trackId;
	g_pEntity->GetEntityFinder()->Add( GetHandle() );
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_CameraTrack::ProcessCommand(const ScriptLine* pLine)
{
	uint trackId = 0;

	bool isCommandValid = false;

	isCommandValid |= pLine->HasString( command::ENTITY_CAMERA_TRACK );
	isCommandValid |= pLine->HasString( command::GRID_POS );
	isCommandValid |= pLine->SetInt( command::TRACK_ID, (int&)trackId );

	if ( !isCommandValid )
	{
		VecGrid pointPos;

		isCommandValid = true;

		if ( pLine->SetIntArray( command::TRACK_POINT0_GRID_POS, &pointPos[0], 3 ) )
			m_point0 = Grid::ToVec3( pointPos );
		else if ( pLine->SetIntArray( command::TRACK_POINT1_GRID_POS, &pointPos[0], 3 ) )
			m_point1 = Grid::ToVec3( pointPos );
		else
			isCommandValid = false;
	}

	if ( trackId > 0 )
		AssignNewTrackId( trackId );

	CHECK( isCommandValid );
}
