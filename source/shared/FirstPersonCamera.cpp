#include "../shared/FirstPersonCamera.h"
#include "../engine/CoreInterface.h"

// TODO: Change camera key action to a shared global namespace
#include "../editor/EditorGlobal.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
FirstPersonCamera::FirstPersonCamera()
{
	math::ViewportInfo v = g_pRenderer->GetWindowViewport();
	SetFov( 90.0f, (float)v.width / (float)v.height );
	SetSpeed( 4.0f );
	SetRotationSpeed( 240.0f );
	m_far = 1000.0f;
}
//-------------------------------------------------------------------
FirstPersonCamera::~FirstPersonCamera()
{
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void FirstPersonCamera::Update()
{
	math::Vec3 movement;
	math::Vec3 rotation;
	float boost = 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_ACCELERATE) )
		boost = 10.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_FORWARD) )
		movement.z += 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_BACKWARD) )
		movement.z -= 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_STRAFE_RIGHT) )
		movement.x += 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_STRAFE_LEFT) )
		movement.x -= 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_UP) )
		movement.y += 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_DOWN) )
		movement.y -= 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_LOOK_RIGHT) )
		rotation.y += 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_LOOK_LEFT) )
		rotation.y -= 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_LOOK_UP) )
		rotation.x -= 1.0f;

	if ( g_pKeyMap->CanPerform(editorGlobal::action::CAMERA_LOOK_DOWN) )
		rotation.x += 1.0f;

	m_pos += movement.x * GetRight() * m_speed * boost * g_pCore->GetTimeSinceLastFrame();
	m_pos += movement.y * GetUp() * m_speed * boost * g_pCore->GetTimeSinceLastFrame();
	m_pos += movement.z * GetLook() * m_speed * boost * g_pCore->GetTimeSinceLastFrame();

	m_rotationDegs.x += rotation.x * m_rotationSpeed * g_pCore->GetTimeSinceLastFrame();
	m_rotationDegs.y += rotation.y * m_rotationSpeed * g_pCore->GetTimeSinceLastFrame();

	m_rotationDegs.x = math::clamp( m_rotationDegs.x, -89.9f, 89.9f );
}
//-------------------------------------------------------------------
//	SetRotationSpeed
//-------------------------------------------------------------------
void FirstPersonCamera::SetRotationSpeed(float speed)
{
	m_rotationSpeed = speed;
}
//-------------------------------------------------------------------
//	SetSpeed
//-------------------------------------------------------------------
void FirstPersonCamera::SetSpeed(float speed)
{
	m_speed = speed;
}
//-------------------------------------------------------------------
//	SetPos
//-------------------------------------------------------------------
void FirstPersonCamera::SetPos(float x, float y, float z)
{
	m_pos = math::Vec3( x, y, z );
}
//-------------------------------------------------------------------
//	SetYawPitchRollDegs
//-------------------------------------------------------------------
void FirstPersonCamera::SetYawPitchRollDegs(float yaw, float pitch, float roll)
{
	m_rotationDegs = math::Vec3( pitch, yaw, roll );
}
//-------------------------------------------------------------------
//	SetFov
//-------------------------------------------------------------------
void FirstPersonCamera::SetFov(float fov, float aspectRatio, float _near, float _far)
{
	m_aspectRatio = aspectRatio;
	m_fovDegs = fov;
	m_near = _near;
	m_far = _far;
	
	m_proj = math::matrixPerspective( m_aspectRatio, m_fovDegs, m_near, m_far );
}
//-------------------------------------------------------------------
//	GetPos
//-------------------------------------------------------------------
math::Vec3 FirstPersonCamera::GetPos()const
{
	return m_pos;
}
//-------------------------------------------------------------------
//	GetRight
//-------------------------------------------------------------------
math::Vec3 FirstPersonCamera::GetRight()const
{
	return GetRotation().GetRow(0).Xyz();
}
//-------------------------------------------------------------------
//	GetUp
//-------------------------------------------------------------------
math::Vec3 FirstPersonCamera::GetUp()const
{
	return GetRotation().GetRow(1).Xyz();
}
//-------------------------------------------------------------------
//	GetLook
//-------------------------------------------------------------------
math::Vec3 FirstPersonCamera::GetLook()const
{
	return GetRotation().GetRow(2).Xyz();
}
//-------------------------------------------------------------------
//	GetProj
//-------------------------------------------------------------------
math::Mtx44 FirstPersonCamera::GetProj()const
{
	return m_proj;
}
//-------------------------------------------------------------------
//	GetCameraTransform
//-------------------------------------------------------------------
math::CameraTransform FirstPersonCamera::GetCameraTransform()const
{
	math::CameraTransform result;
	math::Mtx44 rotation = GetRotation();

	result.pos = m_pos;
	result.right = rotation.GetRow(0).Xyz();
	result.up = rotation.GetRow(1).Xyz();
	result.look = rotation.GetRow(2).Xyz();

	result.view = math::matrixView( result.pos, result.right, result.up, result.look );

	result.proj = m_proj;

	result.fovDegs = m_fovDegs;
	result.fNear = m_near;
	result.fFar = m_far;
	result.aspectRatio = m_aspectRatio;

	return result;
}
//-------------------------------------------------------------------
//	GetRotation
//-------------------------------------------------------------------
math::Mtx44 FirstPersonCamera::GetRotation()const
{
	return math::matrixYawPitchRollDegs( m_rotationDegs.y, m_rotationDegs.x, m_rotationDegs.z );
}