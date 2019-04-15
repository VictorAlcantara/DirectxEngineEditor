#include "CameraManager.h"
#include "../engine/CoreInterface.h"
#include "Entity_GameCamera.h"
#include "GameEvent.h"
#include "Entity_CameraTrack.h"
#include "Entity_TriggerCamera.h"
#include "Entity_Player.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
CameraManager::CameraManager()
{
	m_mode = eCameraMode::Normal;
}
//-------------------------------------------------------------------
CameraManager::~CameraManager()
{
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void CameraManager::Shutdown()
{
	ShutdownListener();
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void CameraManager::Initialize()
{
	InitializeListener();

	m_hCamera = g_pEntity->Create<Entity_GameCamera>();
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void CameraManager::Update()
{
	if ( m_mode == eCameraMode::Normal )
		UpdateMode_Normal();
	else
		UpdateMode_Back();
}
//-------------------------------------------------------------------
//	UpdateMode_Normal
//-------------------------------------------------------------------
void CameraManager::UpdateMode_Normal()
{
	if ( m_hCamera && m_hTarget )
	{
		if ( m_hTrigger )
		{
			Entity_CameraTrack* pTrack = m_hTrigger->Convert<Entity_TriggerCamera>()->GetCameraTrackPtr();
			pTrack->UpdateCamera( m_hTarget, m_hCamera );
		}
		else
			m_hCamera->Convert<Entity_GameCamera>()->LookAt( entityGet_pos(m_hTarget) );
	}
}
//-------------------------------------------------------------------
//	UpdateMode_Back
//-------------------------------------------------------------------
void CameraManager::UpdateMode_Back()
{
	if ( m_hCamera )
	{
		math::Vec3 offset( 0.0f, 1.0f, -4.0f );
		math::Vec3 targetLook = math::AXIS_Z;
		math::Vec3 targetPos;

		if ( m_hTarget )
		{
			if ( m_hTarget->GetClassRelationship_Is<Entity_Player>() )
			{
				Entity_Player* pPlayer = m_hTarget->Convert<Entity_Player>();
				targetLook = pPlayer->GetLook();
			}

			if ( m_hTarget->GetClassRelationship<Entity>() == eClassRelationship::Inherits )
			{
				targetPos = entityGet_pos(m_hTarget);
			}
		}

		math::Vec3 cameraPos;
		cameraPos = targetPos + targetLook * offset.z;
		cameraPos.y += offset.y;
		m_hCamera->Convert<Entity_GameCamera>()->SetPos( cameraPos );
		m_hCamera->Convert<Entity_GameCamera>()->LookAt( targetPos );
	}
}
//-------------------------------------------------------------------
//	SetTarget
//-------------------------------------------------------------------
void CameraManager::SetTarget(Handle<Entity> hTarget)
{
	m_hTarget = hTarget;
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void CameraManager::HandleEvent(Event* pEvent)
{
	Event_TriggerCamera* pEventTriggerCamera = nullptr;
	Event_ChangeCameraMode* pEventChangeCameraMode = nullptr;

	if ( isEvent<Event_TriggerCamera>(pEvent, &pEventTriggerCamera) )
	{
		if ( pEventTriggerCamera->hTrigger->GetClassRelationship_Is<Entity_TriggerCamera>() && pEventTriggerCamera->hEntity->GetId() == m_hTarget->GetId() )
		{
			m_hTrigger = pEventTriggerCamera->hTrigger;
		}
	}
	else if ( isEvent<Event_ChangeCameraMode>(pEvent, &pEventChangeCameraMode) )
	{
		SetMode( pEventChangeCameraMode->mode );
	}
}
//-------------------------------------------------------------------
//	GetActiveCamera
//-------------------------------------------------------------------
const Entity_GameCamera* CameraManager::GetActiveCamera()const
{
	if ( m_hCamera )
		return m_hCamera->Convert<Entity_GameCamera>();
	return nullptr;
}
