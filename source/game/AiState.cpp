#include "AiState.h"
#include "../engine/Math.h"
#include "../engine/CoreInterface.h"
#include "../engine/Entity.h"
#include "AiStateMachine.h"
#include "AiView.h"
#include "Entity_Player.h"
#include "GameGlobal.h"
#include "Entity_MonsterProjectile.h"

/****************************************************************************************
	AiState
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiState::AiState()
{
	m_pOwner = nullptr;
	RegisterInheritance( GetClassTagStatic() );
}

/****************************************************************************************
	AiState_Rest
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiState_Rest::AiState_Rest()
{
	RegisterInheritance( GetClassTagStatic() );
	m_time = 0.0f;
	m_duration = 1.0f;
}
//-------------------------------------------------------------------
//	OnExit
//-------------------------------------------------------------------
void AiState_Rest::OnExit()
{
}
//-------------------------------------------------------------------
//	OnEnter
//-------------------------------------------------------------------
void AiState_Rest::OnEnter()
{
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void AiState_Rest::Update()
{
	m_time += g_pCore->GetTimeSinceLastFrame();

	if ( m_time > m_duration )
	{
		m_pOwner->ScheduleChangeState( new AiState_Chase );
	}
}

/****************************************************************************************
	AiState_Chase
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiState_Chase::AiState_Chase()
{
	RegisterInheritance( GetClassTagStatic() );
	m_duration = 2.0f;
	m_time = 0.0f;
}
//-------------------------------------------------------------------
//	OnExit
//-------------------------------------------------------------------
void AiState_Chase::OnExit()
{
}
//-------------------------------------------------------------------
//	OnEnter
//-------------------------------------------------------------------
void AiState_Chase::OnEnter()
{
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void AiState_Chase::Update()
{
	m_time += g_pCore->GetTimeSinceLastFrame();

	AiView* pView = m_pOwner->GetView();
	Handle<Entity> hTarget = pView->GetTarget();
	bool isFacingTarget = false;

	if ( hTarget )
	{
		float range = pView->GetData(gameGlobal::ai::monster::SLASH_RANGE) * 0.9f;
		math::Vec3 monsterPos = pView->GetPos();
		math::Vec3 targetPos = entityGet_pos(hTarget);
		float facing = pView->GetOrientation().GetLook() * math::normalize(targetPos - monsterPos);
		const float facingTolerance = math::cosDegs( 20.0f );
		isFacingTarget = facing > facingTolerance;

		if (isFacingTarget && math::distSq(monsterPos, targetPos) < range * range && isFacingTarget)
			m_pOwner->ScheduleChangeState( new AiState_Slash );
		else
			pView->MoveTo( targetPos );
	}

	if ( m_time > m_duration )
	{
		float chanceThrowProjectile = 0.5f;

		if ( math::random() < chanceThrowProjectile && isFacingTarget )
			m_pOwner->ScheduleChangeState( new AiState_ProjectileThrow );
		else
			m_pOwner->ScheduleChangeState( new AiState_Rest );
	}
}

/****************************************************************************************
	AiState_Slash
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiState_Slash::AiState_Slash()
{
	RegisterInheritance( GetClassTagStatic() );

	m_time = 0.0f;
	m_duration = 1.0f;
}
//-------------------------------------------------------------------
//	OnExit
//-------------------------------------------------------------------
void AiState_Slash::OnExit()
{
	g_pScene->Debug_Remove( m_hLine );
}
//-------------------------------------------------------------------
//	OnEnter
//-------------------------------------------------------------------
void AiState_Slash::OnEnter()
{
	m_hLine = g_pScene->Debug_AddLine();
	m_hLine->color4 = math::normalizeRgb( 54, 215, 20 );
	m_hLine->visible = true;

	m_duration = m_pOwner->GetView()->GetData( gameGlobal::ai::monster::SLASH_DURATION, m_duration );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void AiState_Slash::Update()
{
	m_time += g_pCore->GetTimeSinceLastFrame();
	bool changeState = false;

	if ( m_time < m_duration )
	{
		AiView* pView = m_pOwner->GetView();
		float progress = math::lerp( 0.0f, 1.0f, m_time / m_duration );
		float range = pView->GetData( gameGlobal::ai::monster::SLASH_RANGE );

		math::Vec3 slashPoint0 = pView->GetPos();
		math::Vec3 slashDir = math::AXIS_X;
		math::Mtx44 yawProgress = math::matrixYawDegs( -90.0f * progress );
		math::Mtx44 monsterRotation = pView->GetOrientation();

		slashDir = slashDir * yawProgress * monsterRotation;
		RaycastInfo contacts[5];
		uint contactCount =  g_pCollision->Raycast( slashPoint0, slashDir, contacts, ARRAY_COUNT(contacts), range );
		Handle<Entity> hTarget = pView->GetTarget();

		CHECK( hTarget->GetClassRelationship_Is<Entity_Player>() );

		m_hLine->point0 = slashPoint0;
		m_hLine->point1 = slashPoint0 + slashDir * range;

		for ( uint i = 0; i < contactCount; i++ )
		{
			if ( contacts[i].hEntity->GetId() == hTarget->GetId() )
			{
				Entity_Player* pPlayer = (Entity_Player*)hTarget.GetPtr();
				pPlayer->TakeDamage( gameGlobal::ai::monster::damage::SLASH );
				//changeState = true;
				break;
			}
		}
	}
	else
		changeState = true;
	
	if ( changeState )
		m_pOwner->ScheduleChangeState( new AiState_Rest );
}

/****************************************************************************************
	AiState_ProjectileThrow
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiState_ProjectileThrow::AiState_ProjectileThrow()
{
	RegisterInheritance( GetClassTagStatic() );
	m_hasThrown = false;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void AiState_ProjectileThrow::Update()
{
	if ( m_hasThrown )
	{
		m_pOwner->ScheduleChangeState( new AiState_Rest );
		return;
	}

	AiView* pView = m_pOwner->GetView();

	const float toleranceAngle = math::cosDegs( 30.0f );

	Handle<Entity> hTarget = pView->GetTarget();
	math::Vec3 targetPos = entityGet_pos(hTarget);
	math::Vec3 ownerPos = pView->GetPos();
	math::Vec3 ownerLook = pView->GetOrientation().GetLook();
	math::Vec3 startPos = ownerPos + ownerLook * 1.5f + math::Vec3( 0.0f, 0.8f, 0.0f );

	math::Vec3 toTarget = targetPos - ownerPos;
	float toTargetLen = math::length( toTarget );

	if ( !math::isZero(toTargetLen) )
	{
		const float speed = 20.0f;
		math::Vec3 toTargetDir = toTarget / toTargetLen;
		uint miniProjectileCount = (uint)math::random(4.0f, 12.0f);

		// Mini projectile
		for ( uint i = 0; i < miniProjectileCount; i++ )
		{
			const float randomInterval = 0.5f;
			math::Vec3 throwDir = toTargetDir;

			throwDir.x *= math::random( 1.0f - randomInterval, 1.0f + randomInterval );
			throwDir.y += math::random(0.0f, math::min(toTargetLen * 0.05f, 0.6f));
			throwDir.z *= math::random( 1.0f - randomInterval, 1.0f + randomInterval );
			//throwDir = math::normalize( throwDir );
			
			auto pProjectile = g_pEntity->CreateGetPtr<Entity_MonsterProjectile>();
			pProjectile->SetIsMini( true );
			pView->Throw( pProjectile->GetHandle(), startPos, throwDir * speed * math::random(1.0f, 1.0f + randomInterval) );
		}
		
		// Main projectile
		math::Vec3 throwDir = toTargetDir;
		//throwDir.x *= math::random(0.5f, 1.5f);
		throwDir.y += 0.1f;
		//throwDir.y *= math::random( 1.0f, 1.2f );
		//throwDir.z *= math::random(0.5f, 1.5f);
		throwDir = math::normalize( throwDir );

		Entity_MonsterProjectile* pProjectile = g_pEntity->CreateGetPtr<Entity_MonsterProjectile>();
		pView->Throw( pProjectile->GetHandle(), startPos, throwDir * speed );

		m_hasThrown = true;
	}
}
