#include "Entity_Player.h"
#include "../engine/CoreInterface.h"
#include "../shared/Command.h"
#include "Entity_Test.h"
#include "../editor/Grid.h"
#include "Entity_Rocket.h"
#include "GameCollision.h"
#include "GameEvent.h"
#include "RenderInfo_Entity.h"
#include "../engine/Path.h"

Handle<Debug_Sphere> hDebugSphere;

/****************************************************************************************
	Entity_Player
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Entity_Player::Entity_Player() : Entity(true)
{
	RegisterInheritance( GetClassTagStatic() );
	m_damageTimeEnd = -1.0f;
	m_movementForce = 10.0f;
	m_rotationSpeed = 300.0f;
	m_active = true;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Entity_Player::OnShutdown()
{
	g_pScene->Debug_Remove( hDebugSphere );
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Entity_Player::OnInitialize()
{
	//m_hTest = g_pEntity->Create<Entity_Test>();

	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( g_pResource->GetModelMngr()->LoadObj<Vertex_P3N3Tg3Bn3T2>(path::model("player")) );
	pRenderInfo->hDiffuseMap = g_pResource->GetTextureMngr()->CreateFromFile(path::texture("player"), false);
	pRenderInfo->emissiveIntensity = 0.1f;
	pRenderInfo->diffuseIntensity = 1.0f;
	pRenderInfo->castShadow = true;
	pRenderInfo->specularIntensity = 0.0f;
	pRenderInfo->material = eMaterial::Opaque;

	m_hRender = AddComponentAs<Component_Render>();
	m_hRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision>();
	m_hCollision->SetActivateTrigger( true );
	m_hCollision->SetCollider( eColliderInfo::Aabb );
	//m_hCollision->SetNoclip( true );
	SetScale( math::Vec3( 0.5f, 1.4f, 0.5f ) );

	m_rotationSpeed = 170.0f;
	m_movementForce = 140.0f;
	m_hPhysics = AddComponentAs<Component_Physics>();
	m_hPhysics->GetData()->friction = 0.3f;
	m_hPhysics->GetData()->mass = 1.0f;
	m_hPhysics->GetData()->acceleration = math::Vec3(0.0f, -980.0f * (m_hCollision->Noclip() ? 0.0f : 0.1f), 0.0f);
	m_hPhysics->GetData()->maxVelocity = 5.0f;

	hDebugSphere = g_pScene->Debug_AddSphere();

	m_hDebugDirection = g_pScene->Debug_AddLine();
	m_hDebugDirection->color4 = math::normalizeRgb( 0, 100, 255 );

	hDebugSphere->visible = false;
	m_hDebugDirection->visible = false;

	UpdateRenderTransformInfo();
	UpdateRenderRotationInfo();
}
//-------------------------------------------------------------------
//	Active
//-------------------------------------------------------------------
void Entity_Player::Active(bool b)
{
	m_active = b;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Entity_Player::Update()
{
	if ( !m_active )
		return;

	//UpdateViewRotation();

	UpdateCharacterControl();

	math::Vec3 pos = m_hPhysics->GetPos();
	math::Vec3 scale = m_hCollision->GetCollider()->aabbScale;
	hDebugSphere->pos = pos;
	hDebugSphere->scale = scale;

	math::Mtx44 mtxScale = math::matrixScale( scale );
	math::Vec3 dir = GetLook();
	m_hDebugDirection->point0 = pos + (dir * mtxScale);
	m_hDebugDirection->point1 = m_hDebugDirection->point0 + (dir * 1.5f * mtxScale);

	if ( g_pInput->IsKeyPress(KEY_E) )
	{
		SmartPtr<Event_PlayerUse> pEvent = new Event_PlayerUse;
		pEvent->hEntity = GetHandle();
		pEvent->pos = pos;
		pEvent->aabbScale = m_hCollision->GetCollider()->aabbScale;
		pEvent->dir = dir;

		g_pEvent->Process( pEvent );
	}

	if ( m_damageTimeEnd > g_pCore->GetTime() )
		hDebugSphere->color4 = math::normalizeRgb( 255, 60, 150 );
	else
		hDebugSphere->color4 = math::normalizeRgb( 255 );

	UpdateRenderTransformInfo();
}
//-------------------------------------------------------------------
//	UpdateRenderTransformInfo
//-------------------------------------------------------------------
void Entity_Player::UpdateRenderTransformInfo()
{
	auto pRenderInfo = m_hRender->GetData(false);

	if ( !math::isEqual(pRenderInfo->GetTransform().GetPos(), m_hPhysics->GetPos()) )
	{
		pRenderInfo->SetTranslation( m_hPhysics->GetPos() );
		m_hRender->UpdateTransform();
	}
}
//-------------------------------------------------------------------
//	UpdateRenderRotationInfo
//-------------------------------------------------------------------
void Entity_Player::UpdateRenderRotationInfo()
{
	auto pRenderInfo = m_hRender->GetData(true);
	pRenderInfo->SetRotation( m_orientation );
}
//-------------------------------------------------------------------
//	UpdateCharacterControl
//-------------------------------------------------------------------
void Entity_Player::UpdateCharacterControl()
{
	math::Vec3 forceDir;
	float turn = 0.0f;

	if ( g_pKeyMap->CanPerform(gameGlobal::action::PLAYER_FORWARD) ) forceDir += GetLook();
	if ( g_pKeyMap->CanPerform(gameGlobal::action::PLAYER_BACKWARD) ) forceDir += -GetLook();
	//if ( g_pInput->IsKeyDown(KEY_D) ) forceDir += m_orientation.GetRight();
	//if ( g_pInput->IsKeyDown(KEY_A) ) forceDir += -m_orientation.GetRight();
	if ( g_pKeyMap->CanPerform(gameGlobal::action::PLAYER_TURN_LEFT) ) turn += -1.0f;
	if ( g_pKeyMap->CanPerform(gameGlobal::action::PLAYER_TURN_RIGHT) ) turn += 1.0f;

	if ( !math::isZero(forceDir) )
	{
		forceDir = math::normalize( forceDir );
		math::Vec3 force = forceDir * m_movementForce;
		m_hPhysics->AddForce( force );	
	}

	if ( !math::isZero(turn) )
	{
		float yaw = turn * m_rotationSpeed * g_pCore->GetTimeSinceLastFrame();
		m_angle.yaw += yaw;
		m_angle.MapToOneRotation();
		
		m_orientation = math::matrixRotateEuler( m_angle );
		UpdateRenderRotationInfo();
	}

	if ( m_hCollision->Noclip() )
		m_hPhysics->GetData()->velocity *= m_hPhysics->GetData()->friction;

	UpdateRenderTransformInfo();
}
//-------------------------------------------------------------------
//	UpdateViewRotation
//-------------------------------------------------------------------
void Entity_Player::UpdateViewRotation()
{
	math::Euler angleAccel;

	if ( !g_pInput->IsKeyDown(KEY_CTRL) )
	{
		if ( g_pInput->IsKeyDown(KEY_UP) )
			angleAccel.pitch += -m_rotationSpeed;
		if ( g_pInput->IsKeyDown(KEY_DOWN) )
			angleAccel.pitch += m_rotationSpeed;
		if ( g_pInput->IsKeyDown(KEY_RIGHT) )
			angleAccel.yaw += m_rotationSpeed;
		if ( g_pInput->IsKeyDown(KEY_LEFT) )
			angleAccel.yaw += -m_rotationSpeed;
	}

	if ( !math::isZero(angleAccel.pitch) || !math::isZero(angleAccel.yaw) )
	{
		angleAccel.yaw *= g_pCore->GetTimeSinceLastFrame();
		angleAccel.pitch *= g_pCore->GetTimeSinceLastFrame();

		m_angle.yaw += angleAccel.yaw;
		if ( m_angle.yaw > 360.0f )
			m_angle.yaw -= 360.0f;
		else if ( m_angle.yaw < 0.0f )
			m_angle.yaw += 360.0f;

		m_angle.pitch += angleAccel.pitch;
		m_angle.pitch = math::clamp( m_angle.pitch, -89.0f, 89.0f );

		m_orientation = math::matrixRotateEuler( m_angle );
		UpdateRenderRotationInfo();
	}
}
//-------------------------------------------------------------------
//	TakeDamage
//-------------------------------------------------------------------
void Entity_Player::TakeDamage(const HashString& damageType)
{
	m_damageTimeEnd = g_pCore->GetTime() + 2.0f;
}
//-------------------------------------------------------------------
//	GetCameraTransform
//-------------------------------------------------------------------
math::CameraTransform Entity_Player::GetCameraTransform()const
{
	math::CameraTransform result;
	result.aspectRatio = g_pRenderer->GetWindowViewport().GetAspectRatio();
	result.fNear = 0.001f;
	result.fFar = 50.0f;
	result.fovDegs = 60.0f;
	result.look = GetLook();
	result.pos = m_hPhysics->GetPos() + m_cameraRelativePos;
	result.right = m_orientation.GetRight();
	result.up = m_orientation.GetUp();
	result.look = m_orientation.GetLook();
	result.proj = math::matrixPerspective( result.aspectRatio, result.fovDegs, result.fNear, result.fFar );
	result.view = math::matrixView( result.pos, result.right, result.up, result.look );

	return result;
}
//-------------------------------------------------------------------
//	GetLook
//-------------------------------------------------------------------
math::Vec3 Entity_Player::GetLook()const
{
	math::Vec3 result = m_orientation.GetLook();
	result = result - (result * math::AXIS_Y * math::AXIS_Y);
	result = math::normalize( result );
	return result;
}
//-------------------------------------------------------------------
//	SetScale
//-------------------------------------------------------------------
void Entity_Player::SetScale(const math::Vec3& scale)
{
	float cameraPosProportion = 0.75f;
	//m_pCollision->scale = scale;
	m_hCollision->GetCollider(true)->aabbScale = scale;
	//m_pCollision->aabbScale = math::Vec3( 0.5f, 1.5f, 0.5f );

	m_cameraRelativePos.y = m_hCollision->GetCollider()->aabbScale.y * cameraPosProportion;
}
//-------------------------------------------------------------------
//	ProcessCommand
//-------------------------------------------------------------------
void Entity_Player::ProcessCommand(const ScriptLine* pLine)
{
	VecGrid vecGrid;
	math::Vec3 direction;
	bool bData = true;

	if ( pLine->SetIntArray( command::GRID_POS, &vecGrid[0], 3 ) )
	{
		m_hPhysics->SetPos( Grid::ToVec3(vecGrid) );
		UpdateRenderTransformInfo();
	}
	else if ( pLine->SetFloatArray( command::DIRECTION, &direction[0], 3, false ) )
	{
		m_angle.yaw = -math::acosDegs( direction.x );

		if ( direction.z < 0.0f )
			m_angle.yaw *= -1.0f;

		m_angle.yaw += 90.0f;

		m_orientation = math::matrixRotateEuler( m_angle );
		UpdateRenderRotationInfo();
	}
	else if ( pLine->SetIntArray(command::GRID_SCALE, &vecGrid[0], 3) )
	{
		SetScale( Grid::ToVec3(vecGrid) );
	}
	else if ( pLine->SetBool(command::ACTIVE, bData) )
	{
		Active( bData );
	}
	else if ( !pLine->HasString( command::ENTITY_PLAYER_SPAWN ) )
	{
		ASSERT( "Invalid command" );
	}
}
