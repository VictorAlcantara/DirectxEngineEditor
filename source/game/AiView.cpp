#include "AiView.h"
#include "../engine/Entity.h"
#include "../engine/CoreInterface.h"
#include "AiState.h"
#include "Entity_Monster.h"
#include "../engine/TableHashFunction.h"
#include "GameGlobal.h"
#include "Entity_MonsterProjectile.h"
#include "../engine/Component_Render.h"

/****************************************************************************************
	AiView
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiView::AiView(Handle<Entity> hOwner) : m_tableData(100, tableHashFunc_HashString)
{
	m_hOwner = hOwner;
}
//-------------------------------------------------------------------
//	GetPos
//-------------------------------------------------------------------
math::Vec3 AiView::GetPos()const
{
	return entityGet_pos( m_hOwner );
}
//-------------------------------------------------------------------
//	GetOrientation
//-------------------------------------------------------------------
const math::Mtx44& AiView::GetOrientation()const
{
	auto pCmpRender = m_hOwner->GetComponentPtrAs<Component_Render>();

	if ( pCmpRender && pCmpRender->GetData() )
		return pCmpRender->GetData()->GetTransform().rotation;

	return math::Mtx44::identity;
}
//-------------------------------------------------------------------
//	SetTarget
//-------------------------------------------------------------------
void AiView::SetTarget(Handle<Entity> hTarget)
{
	CHECK( !hTarget && !m_hOwner || (hTarget->GetId() != m_hOwner->GetId()) );

	m_hTarget = hTarget;
}
//-------------------------------------------------------------------
//	GetTarget
//-------------------------------------------------------------------
Handle<Entity> AiView::GetTarget()
{
	return m_hTarget;
}
//-------------------------------------------------------------------
//	SetData
//-------------------------------------------------------------------
void AiView::SetData(const HashString& name, float value)
{
	m_tableData.AddOrChange( value, name );
}
//-------------------------------------------------------------------
//	GetData
//-------------------------------------------------------------------
float AiView::GetData(const HashString& name, float defaultValue)const
{
	auto pFind = m_tableData.Find( name );
	return pFind ? *pFind : defaultValue;
}

/****************************************************************************************
	AiView_Monster
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiView_Monster::AiView_Monster(Handle<Entity> hOwner) : AiView(hOwner)
{
	m_action = eAction::Idle;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void AiView_Monster::Update()
{
	if ( m_hOwner )
	{
		if ( m_action == eAction::MoveTo )
			Update_MoveTo();
	}
}
//-------------------------------------------------------------------
//	Update_MoveTo
//-------------------------------------------------------------------
void AiView_Monster::Update_MoveTo()
{
	Entity_Monster* pMonster = (Entity_Monster*)m_hOwner.GetPtr();

	math::Vec3 toTarget = m_targetPos - entityGet_pos( m_hOwner );
	float force = GetData(gameGlobal::ai::monster::FORCE_MAX, 50.0f);

	pMonster->HeadTo( toTarget, force );
}
//-------------------------------------------------------------------
//	MoveTo
//-------------------------------------------------------------------
void AiView_Monster::MoveTo(const math::Vec3& targetPos)
{
	m_action = eAction::MoveTo;
	m_targetPos = targetPos;
}
//-------------------------------------------------------------------
//	Throw
//-------------------------------------------------------------------
void AiView_Monster::Throw(Handle<Entity>& hEntity, const math::Vec3& startPos, const math::Vec3& velocity)
{
	m_action = eAction::Throw;

	auto pProjectile = Entity::Convert<Entity_MonsterProjectile>( hEntity );
	
	if ( pProjectile )
	{
		auto pCmpPhysics = pProjectile->GetComponentPtrAs<Component_Physics>();
		pCmpPhysics->SetPos( startPos );
		pCmpPhysics->GetData()->velocity = velocity;
	}
}
//-------------------------------------------------------------------
//	OnStateChange
//-------------------------------------------------------------------
void AiView_Monster::OnStateChange(AiState* pState)
{
	if ( !pState )
		return;

	if ( pState->GetClassRelationship_Is<AiState_Rest>() )
	{
		AiState_Rest* pRest = (AiState_Rest*)pState;
		pRest->SetDuration( math::random(1.0f, 2.5f) );
	}
	else if ( pState->GetClassRelationship_Is<AiState_Chase>() )
	{
		float lookTimeModifier = 0.0f;

		if ( m_hTarget )
		{
			float cosLookToTarget = GetOrientation().GetLook() * math::normalize(entityGet_pos(m_hTarget) - entityGet_pos(m_hOwner));
			const float cosTolerance = math::cosDegs( 30.0f );
		
			if ( cosLookToTarget > cosTolerance )
			{
				float chaseTurnTimeMin = GetData(gameGlobal::ai::monster::CHASE_TIME_TURN_MIN);
				float chaseTurnTimeMax = GetData(gameGlobal::ai::monster::CHASE_TIME_TURN_MAX);
				lookTimeModifier += math::random(chaseTurnTimeMin,chaseTurnTimeMax);
			}
		}

		float chaseTimeMin = GetData(gameGlobal::ai::monster::CHASE_TIME_MIN);
		float chaseTimeMax = GetData(gameGlobal::ai::monster::CHASE_TIME_MAX);
		AiState_Chase* pChase = (AiState_Chase*)pState;
		pChase->SetDuration( math::random(chaseTimeMin, chaseTimeMax) + lookTimeModifier );
	}

	m_action = eAction::Idle;
}
