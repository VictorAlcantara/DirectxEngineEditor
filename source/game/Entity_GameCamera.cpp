#include "Entity_GameCamera.h"
#include "../engine/CoreInterface.h"
#include "../shared/Command.h"
#include "../shared/VecGrid.h"
#include "../editor/Grid.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_GameCamera::Entity_GameCamera() : Entity(false)
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
Entity_GameCamera::~Entity_GameCamera()
{
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_GameCamera::OnShutdown()
{
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_GameCamera::OnInitialize()
{
	m_transform.aspectRatio = g_pRenderer->GetWindowViewport().GetAspectRatio();
	m_transform.fNear = 0.001f;
	m_transform.fFar = 35.0f;
	m_transform.fovDegs = 60.0f;
	m_transform.right = math::AXIS_X;
	m_transform.up = math::AXIS_Y;
	m_transform.look = math::AXIS_Z;

	m_transform.proj = math::matrixPerspective( m_transform.aspectRatio, m_transform.fovDegs, m_transform.fNear, m_transform.fFar );
}
//-------------------------------------------------------------------
//	SetPos
//-------------------------------------------------------------------
void Entity_GameCamera::SetPos(const math::Vec3& cameraPos)
{
	m_transform.pos = cameraPos;
	m_transform.view = math::matrixViewLookUp( m_transform.pos, m_transform.look, m_transform.up );
	m_transform.right = math::cross( m_transform.up, m_transform.look );
}
//-------------------------------------------------------------------
//	TrackTarget
//-------------------------------------------------------------------
void Entity_GameCamera::LookAt(const math::Vec3& targetPos)
{
	m_transform.look = math::normalize(targetPos - m_transform.pos);
	m_transform.view = math::matrixViewLookUp( m_transform.pos, m_transform.look, m_transform.up );
	m_transform.right = math::cross( m_transform.up, m_transform.look );
}
//-------------------------------------------------------------------
//	GetCameraTransform
//-------------------------------------------------------------------
math::CameraTransform Entity_GameCamera::GetCameraTransform()const
{
	return m_transform;
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_GameCamera::ProcessCommand(const ScriptLine* pLine)
{
	VecGrid vecGrid;
	math::Vec3 direction;

	if ( pLine->SetIntArray( command::GRID_POS, &vecGrid[0], 3 ) )
	{
		m_transform.pos = Grid::ToVec3( vecGrid );
	}
	else if ( pLine->SetFloatArray( command::DIRECTION, &direction[0], 3, false ) )
	{
		m_transform.look = math::normalize(direction);
	}
	else if ( !pLine->HasString( command::ENTITY_CAMERA ) )
	{
		ASSERT( "Invalid command" );
	}
}
