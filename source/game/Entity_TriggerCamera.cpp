#include "Entity_TriggerCamera.h"
#include "../engine/CoreInterface.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../editor/Grid.h"
#include "../shared/Command.h"
#include "Entity_CameraTrack.h"
#include "GameCollision.h"
#include "Entity_Player.h"
#include "GameEvent.h"
#include "../engine/Component_Render.h"

/****************************************************************************************
	Component_Trigger_Camera
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Trigger_Camera::Component_Trigger_Camera()
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnTrigger_Enter
//-------------------------------------------------------------------
void Component_Trigger_Camera::OnTrigger_Enter(Handle<Entity>& hOther)
{
	if ( hOther->GetClassRelationship_Is<Entity_Player>() )
	{
		auto pEvent = SmartPtr<Event_TriggerCamera>::New();
		pEvent->hTrigger = GetOwner();
		pEvent->hEntity = hOther;

		//debugToOutput( string::intToStr(GetOwner()->Convert<Entity_TriggerCamera>()->GetCameraTrackPtr()->GetTrackId()).AsChar() );

		g_pEvent->Process( pEvent );
	}
}

/****************************************************************************************
	Entity_TriggerCamera
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_TriggerCamera::Entity_TriggerCamera() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_TriggerCamera::OnInitialize()
{
	m_scale = math::Vec3(1.0f);

	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->color4 = math::normalizeRgb( 0, 150, 255, 70 );
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	pRenderInfo->hDiffuseMap = primitive::texture::createWhite();
	pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->visible = false;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hTrigger = AddComponentAs<Component_Trigger_Camera>();
	m_hTrigger->GetCollider()->aabbScale = pRenderInfo->GetTransform().GetScale();

	m_hPhysics = AddComponentAs<Component_Physics>();
	m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	m_hPhysics->EnableUpdate( false );
}
//-------------------------------------------------------------------
//	ConnectToTrack
//-------------------------------------------------------------------
void Entity_TriggerCamera::ConnectToTrack(Handle<Entity> hEntity)
{
	if ( !hEntity || hEntity->GetClassRelationship_Is<Entity_CameraTrack>() )
		m_hCameraTrack = hEntity;
	else
		ASSERT( "Invalid type" );
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_TriggerCamera::ProcessCommand(const ScriptLine* pLine)
{
	VecGrid vecGrid;
	math::Vec3 direction;
	int trackId;
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	if ( pLine->SetIntArray( command::GRID_POS, &vecGrid[0], 3 ) )
	{
		pRenderInfo->SetTranslation( Grid::ToVec3(vecGrid) );
		m_hPhysics->SetPos( pRenderInfo->GetTransform().GetPos() );
	}
	else if ( pLine->SetIntArray( command::GRID_SCALE, &vecGrid[0], 3 ) )
	{
		pRenderInfo->SetScale( Grid::ToVec3(vecGrid) );
		m_hTrigger->GetCollider()->aabbScale = pRenderInfo->GetTransform().GetScale();
	}
	else if ( pLine->SetInt( command::TRACK_ID, trackId ) )
	{
		if ( trackId > 0 )
		{
			m_pProcessTrackFind = new Process_CameraTrackFind;
			m_pProcessTrackFind->SetOwner( GetHandle() );
			m_pProcessTrackFind->SetTrackIdToFind( trackId );
			g_pProcess->Add( m_pProcessTrackFind.As<Process>() );
		}
	}
	else if ( !pLine->HasString( command::ENTITY_TRIGGER_CAMERA ) )
	{
		ASSERT( "Invalid command" );
	}
}
//-------------------------------------------------------------------
//	GetCameraTrack
//-------------------------------------------------------------------
Handle<Entity> Entity_TriggerCamera::GetCameraTrack()
{
	return m_hCameraTrack;
}
//-------------------------------------------------------------------
//	GetCameraTrackPtr
//-------------------------------------------------------------------
Entity_CameraTrack* Entity_TriggerCamera::GetCameraTrackPtr()
{
	return &m_hCameraTrack.GetAs<Entity_CameraTrack>();
}

/****************************************************************************************
	Process_CameraTrackFind
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_CameraTrackFind::Process_CameraTrackFind()
{
	m_trackId = 0;
}
//-------------------------------------------------------------------
//	OnEnter
//-------------------------------------------------------------------
void Process_CameraTrackFind::OnEnter()
{
	CHECK( m_trackId > 0 && m_hOwner && m_hOwner->GetClassRelationship_Is<Entity_TriggerCamera>() );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_CameraTrackFind::Update()
{
	Handle<Entity> hEntity = g_pEntity->GetEntityFinder()->Find( Entity_CameraTrack::GetClassTagStatic().GetHash(), m_trackId );

	if ( hEntity )
	{
		m_hOwner.GetAs<Entity_TriggerCamera>().ConnectToTrack( hEntity );
		Exit();
	}
}
//-------------------------------------------------------------------
//	SetTrackIdToFind
//-------------------------------------------------------------------
void Process_CameraTrackFind::SetTrackIdToFind(uint trackId)
{
	CHECK( GetState() != eProcessState::Running );
	m_trackId = trackId;
}
//-------------------------------------------------------------------
//	SetOwner
//-------------------------------------------------------------------
void Process_CameraTrackFind::SetOwner(Handle<Entity> hOwner)
{
	CHECK( GetState() != eProcessState::Running );
	m_hOwner = hOwner;
}
