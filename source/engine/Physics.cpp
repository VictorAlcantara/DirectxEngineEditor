#include "Physics.h"
#include "../engine/TableHashFunction.h"
#include "../engine/CoreInterface.h"
#include "../engine/CollisionInfo.h"
#include "../engine/PhysicsInfo.h"
#include "../engine/Debug.h"
#include "../engine/Component_Physics.h"
#include "EventEngine.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Physics::Physics() : m_components(1000, tableHashFunc_Uint)
{
	m_initialized = false;
}
//-------------------------------------------------------------------
Physics::~Physics()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Physics::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_components.Clear();

	ShutdownListener();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Physics::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	SetGravity( math::Vec3(0.0f, -100.0f, 0.0f) );
	InitializeListener();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Physics::Update()
{
	for ( auto it = m_components.GetIterator(); it; it++ )
	{
		uint idOwner = (*it)->GetOwner()->GetId();

		float dt = g_pCore->GetTimeSinceLastFrame();
		auto pCmpPhys = *it;
		PhysicsData* pInfo = pCmpPhys->GetData();

		math::Vec3 accel = pInfo->acceleration;

		if ( pInfo->mass > 0.0f )
			accel += pCmpPhys->GetForce() / pInfo->mass;

		pInfo->velocity += accel * dt;

		if ( math::isZero(math::lengthSq(accel), 0.00001f) )
			continue;

		pInfo->velocity = math::clampLength( pInfo->velocity, 0.0f, pInfo->maxVelocity );
		math::Vec3 newVelocity = pInfo->velocity;
		Contact contacts[50];
		uint contactCount = g_pCollision->CalculateContacts( pCmpPhys->GetOwner(), newVelocity, dt, contacts, ARRAY_COUNT(contacts) );
		bool applyFriction = false;

		CHECK( !math::isNan(newVelocity) );

		for ( uint i = 0; i < contactCount; i++ )
		{
			pCmpPhys->SetPos( contacts[i].newPos );

			float frictionForAngle = contacts[i].surfaceNormal * math::AXIS_Y;
			const float frictionCosMax = math::cosDegs(10.0f);

			if ( frictionForAngle > frictionCosMax )
				applyFriction = true;
		}
		
		pInfo->velocity = newVelocity;

		if ( applyFriction )
			pInfo->velocity *= 1.0f - pInfo->friction;

		pInfo->velocity = math::clampLength( pInfo->velocity, 0.0f, pInfo->maxVelocity );

		pCmpPhys->AddPos( pInfo->velocity * dt );

		CHECK( !math::isNan( pCmpPhys->GetPos() ) );

		pCmpPhys->ClearForce();
		pCmpPhys->OnPhysicsUpdate( contacts, contactCount );
	}
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void Physics::HandleEvent(Event* pEvent)
{
	Event_Component* pEventComponent = nullptr;
	Event_EntityManager* pEventEntityMngr = nullptr;
	Event_ComponentPropertyChange* pEventChangeProp = nullptr;

	if ( isEvent(pEvent, &pEventComponent) )
	{
		if ( pEventComponent->hComponent->GetClassRelationship_IsOrHas<Component_Physics>() )
		{
			if ( pEventComponent->action == eComponentAction::Add )
			{
				m_components.Add( HandleAs<Component, Component_Physics>(pEventComponent->hComponent), pEventComponent->hComponent->GetId() );
			}
			else if ( pEventComponent->action == eComponentAction::Remove )
			{
				m_components.Remove( pEventComponent->hComponent->GetId() );
			}
		}
	}
	else if ( isEvent(pEvent, &pEventEntityMngr) )
	{
		if ( pEventEntityMngr->action == eEventEntity::RemoveAll )
			m_components.Clear();
	}
	else if ( isEvent(pEvent, &pEventChangeProp) )
	{
		if ( pEventChangeProp->hComponent->GetClassRelationship_Is<Component_Physics>() )
		{
			if ( pEventChangeProp->hComponent.GetAs<Component_Physics>().IsUpdateEnabled() )
				m_components.AddIfNotExists( HandleAs<Component, Component_Physics>(pEventChangeProp->hComponent), pEventChangeProp->hComponent->GetId() );
			else
				m_components.Remove( pEventChangeProp->hComponent->GetId() );
		}
	}
}
