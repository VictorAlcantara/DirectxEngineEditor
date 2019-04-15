#include "CollisionManager.h"
#include "../engine/Math.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"
#include "../engine/TableHashFunction.h"
#include "../engine/PhysicsInfo.h"
#include "../engine/CoreInterface.h"
#include "EventEngine.h"
#include "../engine/Component_Collision.h"
#include "../engine/Component_Trigger.h"
#include "../engine/Component_Physics.h"
#include "Partition.h"
#include "../engine/Debug.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
CollisionManager::CollisionManager() : m_collisions(200, tableHashFunc_Uint), 
	m_triggerCollisionCandidate(25, tableHashFunc_Uint), 
	m_triggers(1000, tableHashFunc_Uint), 
	m_triggerIdContacts(500, tableHashFunc_Uint)
{
	m_currentNormal = 0;
	m_initialized = false;
	m_isQuerying = false;
}
//-------------------------------------------------------------------
CollisionManager::~CollisionManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void CollisionManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_pPartition->Shutdown();
	m_pPartition = nullptr;

	ShutdownListener();

	for ( uint i = 0; i < ARRAY_COUNT(m_normals); i++ )
	{
		if ( m_normals[i] )
			g_pScene->Debug_Remove( m_normals[i] );
	}

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void CollisionManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_pPartition = new PartitionCollision;
	m_pPartition->Initialize();

	InitializeListener();

	#ifdef DEBUG_SHOW_CONTACT_NORMALS
		for ( uint i = 0; i < ARRAY_COUNT(m_normals); i++ )
		{
			m_normals[i] = g_pScene->Debug_AddLine();
			float c[] = { 0.2f, 0.5f, 0.7f, 1.0f };
			m_normals[i]->color4 = math::Vec4( math::randomArray(c, ARRAY_COUNT(c)), math::randomArray(c, ARRAY_COUNT(c)), math::randomArray(c, ARRAY_COUNT(c)), 1.0f );
		}
	#endif

	HideNormals();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void CollisionManager::Update()
{
	for ( auto itTrigger = m_triggers.GetIterator(); itTrigger; itTrigger++ )
	{
		auto pCmpTrigger = *itTrigger;
		auto pCldrTrigger = pCmpTrigger->GetCollider();

		bool hasTriggerContact = false;

		if ( pCmpTrigger->Noclip() )
			continue;

		uint triggerId = pCmpTrigger->GetId();
		math::Vec3 triggerScale = pCldrTrigger->aabbScale;
		math::Vec3 triggerPos = entityGet_pos( pCmpTrigger->GetOwner() );

		for ( auto itCmpCollision = m_triggerCollisionCandidate.GetIterator(); itCmpCollision; itCmpCollision++ )
		{
			CHECK( (*itCmpCollision)->GetId() != triggerId );

			auto pCmpCollision = *itCmpCollision;
			auto pCldrEntity = pCmpCollision->GetCollider();

			if ( pCmpCollision->Noclip() || pCmpCollision->Ignore(pCmpTrigger->GetOwner()) || pCmpTrigger->Ignore(pCmpCollision->GetOwner()) )
				continue;

			math::Vec3 entityPos = entityGet_pos(pCmpCollision->GetOwner());
			math::Vec3 entityScale = pCldrEntity->aabbScale;

			//float distSq = math::distSq( triggerPos, entityPos );
			//float radiusSq = triggerScale * triggerScale + entityScale * entityScale;

			//if ( distSq > radiusSq )
			//	continue;

			if ( math::intersectAabb3(triggerPos + math::Aabb3(triggerScale), entityPos + math::Aabb3(entityScale)).collision )
			{
				if ( m_triggerIdContacts.Find(triggerId) )
				{
					pCmpTrigger->OnTrigger_Update( pCmpCollision->GetOwner() );
				}
				else
				{
					m_triggerIdContacts.Add( triggerId, triggerId );
					pCmpTrigger->OnTrigger_Enter( pCmpCollision->GetOwner() );
				}
			}
			else
			{
				m_triggerIdContacts.Remove( triggerId );
				pCmpTrigger->OnTrigger_Exit( pCmpCollision->GetOwner() );
			}
		}
	}
}
//-------------------------------------------------------------------
//	CalculateContacs
//-------------------------------------------------------------------
uint CollisionManager::CalculateContacts(Handle<Entity> hEntity, math::Vec3& inOutVelocity, float dt, Contact* outContacts, uint maxContacts)
{
	uint result = 0;
	const float velocityLengthMin = 0.0001f;
	math::Vec3 oldPos = entityGet_pos(hEntity);
	List<Contact> outContactQueue;

	QueryAndSortContacts( hEntity, inOutVelocity, dt, maxContacts, outContactQueue );

	for ( auto itContactQueue = outContactQueue.GetIterator(); itContactQueue; itContactQueue++ )
	{
		Contact* pContact = &(*itContactQueue);
		Handle<Entity> hEntityContact = pContact->hEntityContact;

		if ( math::isZero(inOutVelocity * inOutVelocity, velocityLengthMin * velocityLengthMin) )
		{
			inOutVelocity = math::Vec3(0.0f);
			break;
		}

		Contact contact = CalculateContact( hEntity, oldPos, hEntityContact, inOutVelocity - entityGet_velocity(hEntityContact), dt );

		if ( contact.collisionType != eCollision::None )
		{
			if ( contact.surfaceNormal * inOutVelocity < 0.0f )
				inOutVelocity = inOutVelocity - inOutVelocity * contact.surfaceNormal * contact.surfaceNormal;
		}
		else
			continue;

		oldPos = contact.newPos;
		outContacts[result++] = contact;
	}

	return math::min( result, maxContacts );
}
//-------------------------------------------------------------------
//	QueryAndSortContacts
//-------------------------------------------------------------------
void CollisionManager::QueryAndSortContacts(Handle<Entity> hEntity1, const math::Vec3& entityVelocity, float dt, uint maxContacts, List<Contact>& outContacts)
{
	m_isQuerying = true;

	auto pCol1 = hEntity1->GetComponentPtrAs<Component_Collision>();

	if ( !pCol1 || pCol1->Noclip() )
		return;

	math::Vec3 pos1 = entityGet_pos(hEntity1);

	m_collisions.Clear();
	m_pPartition->GetFromAabb( math::Aabb3(pCol1->GetCollider()->aabbScale) + pos1, m_collisions, false );

	//#ifdef DEBUG_MODE
	//	static float t = 0.0f;

	//	if ( t < 0.0f )
	//	{
	//		auto ent1Tag = hEntity1->GetClassTag();
	//		static HashString entPlayer( "entity_player" );
	//		if ( ent1Tag == entPlayer )
	//			debugToOutput( (int)m_collisions.Count(), "collisions: " );
	//		t = 1.0f;
	//	}
	//	else
	//		t -= g_pCore->GetTimeSinceLastFrame();
	//#endif

	for ( auto itEnts = m_collisions.GetIterator(); itEnts; itEnts++ )
	{
		Handle<Entity> hEntity2 = (*itEnts)->GetOwner();

		if ( hEntity1->GetId() == hEntity2->GetId() )
			continue;

		auto pCol2 = hEntity2->GetComponentPtrAs<Component_Collision>();

		if ( !pCol2 || pCol2->Noclip() )
			continue;

		if ( pCol1->Ignore(hEntity2) || pCol2->Ignore(hEntity1) )
			continue;

		math::Vec3 velocityRelative = entityVelocity - entityGet_velocity(hEntity2);
		Contact contact = CalculateContact( hEntity1, pos1, hEntity2, velocityRelative, dt );

		if ( contact.collisionType != eCollision::None )
		{
			auto pNode = outContacts.GetFirst();

			for ( ; pNode; pNode = pNode->GetNext() )
			{
				uint iContactCollisionType = (uint)contact.collisionType;
				uint iNodeCollisionType = (uint)pNode->GetValue().collisionType;

				if ( iContactCollisionType > iNodeCollisionType )
					continue;

				if ( contact.distSq < pNode->GetValue().distSq || iContactCollisionType < iNodeCollisionType )
				{
					pNode->AddPrev( contact );
					break;
				}
			}

			if ( !pNode )
				outContacts.AddLast( contact );
		}

		if ( outContacts.Count() >= maxContacts )
		{
			ASSERT( "Exceeded max contacts" );
			break;
		}
	}

	// Not good...
	// Anyway, delete contacts that rest on same plane
	// Start from last, since those are the ones the furthest away
	const float tolerance = 0.01f;
	for ( auto pNodeLast = outContacts.GetLast(); pNodeLast; )
	{
		bool onSamePlane = false;

		for ( auto pNodeCompare = pNodeLast->GetPrev(); pNodeCompare; pNodeCompare = pNodeCompare->GetPrev() )
		{
			math::Vec3 lastPoint = pNodeLast->GetValue().surfacePoint;
			math::Vec3 compareNormal = pNodeCompare->GetValue().surfaceNormal;
			math::Vec3 comparePoint = pNodeCompare->GetValue().surfacePoint;
			float dot = (lastPoint - comparePoint) * compareNormal;

			if ( math::isZero( dot, tolerance ) )
			{
				onSamePlane = true;
				break;
			}
		}

		auto pLastPrev = pNodeLast->GetPrev();
		
		if ( onSamePlane )
			pNodeLast->Remove();
		
		pNodeLast = pLastPrev;
	}

	m_isQuerying = false;
}
//-------------------------------------------------------------------
//	CalculateContacs
//-------------------------------------------------------------------
Contact CollisionManager::CalculateContact(Handle<Entity> hEntity1, const math::Vec3& entity1Pos, Handle<Entity> hEntity2, const math::Vec3& velocity, float dt)
{
	Contact result;

	auto pCol1 = hEntity1->GetComponentPtrAs<Component_Collision>();
	auto pCol2 = hEntity2->GetComponentPtrAs<Component_Collision>();

	eColliderInfo entity1CollisionType;
	eColliderInfo entity2CollisionType = pCol2->GetCollider()->GetType();

	pCol1->ClassifyCollider( hEntity2, entity1CollisionType, entity2CollisionType );

	if ( entity1CollisionType == eColliderInfo::Aabb && entity2CollisionType == eColliderInfo::Aabb )
	{
		result = CalculateContact_Aabb_Aabb( hEntity1, entity1Pos, hEntity2, velocity, dt );
	}
	else if ( entity1CollisionType == eColliderInfo::Sphere && entity2CollisionType == eColliderInfo::Aabb )
	{
		result = CalculateContact_Sphere_Aabb( hEntity1, entity1Pos, hEntity2, velocity, dt );
	}
	else if ( entity1CollisionType == eColliderInfo::Aabb && entity2CollisionType == eColliderInfo::Sphere )
	{
		result = CalculateContact_Aabb_Sphere( hEntity1, entity1Pos, hEntity2, velocity, dt );
	}
	else if ( entity1CollisionType == eColliderInfo::Ellipse && entity2CollisionType == eColliderInfo::Aabb )
	{
		result = CalculateContact_Ellipse_Aabb( hEntity1, entity1Pos, hEntity2, velocity, dt );
	}
	else
	{
		ASSERT( "Collision pair not implemented" );
	}

	return result;
}
//-------------------------------------------------------------------
//	Raycast
//-------------------------------------------------------------------
uint CollisionManager::Raycast(const math::Vec3& rayPos, const math::Vec3& rayDir, RaycastInfo* raycasts, uint maxRaycasts, float rayMaxLength)
{
	uint result = 0;

	math::Vec3 rayLength = rayDir;

	if ( rayMaxLength == 0.0f )
		rayLength *= 10000.0f; // Should be enough... SHOULD
	else
		rayLength *= rayMaxLength;

	#ifdef DEBUG_MODE
		CHECK( !math::isNan( rayLength ) );
	#endif

	math::Vec3 ray0 = rayPos;
	math::Vec3 ray1 = rayPos + rayLength;
	math::Vec3 rayDiff = ray1 - ray0;

	const float aabbAdjustment = 0.1f;
	for ( uint i = 0; i < 3; i++ )
	{
		if ( math::isZero(rayDiff[i], aabbAdjustment * 0.1f) )
		{
			ray0[i] -= aabbAdjustment;
			ray1[i] += aabbAdjustment;
		}
	}

	math::Aabb3 rayAabb = math::calculateAabb3( ray0, ray1 );

	m_collisions.Clear();
	m_pPartition->GetFromAabb( rayAabb, m_collisions, true );

	for ( auto it = m_collisions.GetIterator(); it; it++ )
	{
		auto pCol = (*it)->GetCollider();
		math::Vec3 pos = entityGet_pos( (*it)->GetOwner() );

		//float dist = math::length(rayPos - pos);
		//debugToOutput( (*it)->GetOwner()->GetClassTag().GetString().AsChar() );

		auto intersect  = math::intersectRayAabb( rayPos, rayDir, math::Aabb3(pCol->aabbScale) + pos );

		if ( intersect.collisionFlag & math::COLLISION_FRONT && (rayMaxLength == 0.0f || intersect.t < rayMaxLength) )
		{
			raycasts[result].intersect = intersect;
			raycasts[result].hEntity = (*it)->GetOwner();
			result++;
		}

		if ( result == maxRaycasts )
		{
			ASSERT( "Max raycasts reached" );
			break;
		}
	}

	return result;
}
//-------------------------------------------------------------------
//	CalculateContact_Aabb_Aabb
//-------------------------------------------------------------------
Contact CollisionManager::CalculateContact_Aabb_Aabb(Handle<Entity> hEntityAabb0, const math::Vec3& entity0Pos, Handle<Entity> hEntityAabb1, const math::Vec3& velocity, float dt)
{
	Contact result;
	result.oldPos = entity0Pos;

	math::Vec3 pos0 = entity0Pos;
	auto pColAabb0 = (ColliderInfo_Aabb*)hEntityAabb0->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 pos1 = entityGet_pos(hEntityAabb1);
	auto pColAabb1 = (ColliderInfo_Aabb*)hEntityAabb1->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 collisionPoint;

	// Static
	{
		auto intersectStatic = math::intersectAabb3( pos0, pColAabb0->aabbScale, pos1, pColAabb1->aabbScale );

		if ( intersectStatic.collision )
		{
			result.collisionType = eCollision::Static;
			result.surfaceNormal = intersectStatic.resolveNormal;
			result.collisionAmount = intersectStatic.resolveAmount;
			result.newPos = pos0 + intersectStatic.resolveNormal * intersectStatic.resolveAmount;
			collisionPoint = intersectStatic.collisionPoint;
		}
	}

	// Dynamic
	if ( result.collisionType == eCollision::None )
	{
		auto intersectDynamic = math::intersectAabb3Dynamic( pos0, pColAabb0->aabbScale, velocity, pos1 , pColAabb1->aabbScale );

		if ( intersectDynamic.collision && intersectDynamic.t < dt )
		{
			collisionPoint = intersectDynamic.collisionPoint;

			result.collisionType = eCollision::Dynamic;
			result.surfaceNormal = intersectDynamic.normal;
			result.collisionAmount = velocity * (1.0f - intersectDynamic.t) * -result.surfaceNormal;
			result.newPos = pos0 + velocity * intersectDynamic.t;
		}
	}

	if ( result.collisionType != eCollision::None )
	{
		result.distSq = math::distSq(pos0, collisionPoint);
		result.surfacePoint = collisionPoint;
		result.hEntityContact = hEntityAabb1;

		ShowNormal( collisionPoint, collisionPoint + result.surfaceNormal * 2.5f );
	}

	return result;
}
//-------------------------------------------------------------------
//	CalculateContact_Sphere_Aabb
//-------------------------------------------------------------------
Contact CollisionManager::CalculateContact_Sphere_Aabb(Handle<Entity> hEntitySphere, const math::Vec3& entity1Pos, Handle<Entity> hEntity2Aabb, const math::Vec3& velocity, float dt)
{
	Contact result;
	result.oldPos = entity1Pos;

	auto pCol1 = (ColliderInfo_Sphere*)hEntitySphere->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 pos1 = entity1Pos;
	float radius1 = pCol1->boundingSphereRadius;
	auto pCol2 = (ColliderInfo_Aabb*)hEntity2Aabb->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 pos2 = entityGet_pos(hEntity2Aabb);
	math::Vec3 aabb2 = pCol2->aabbScale;

	math::Vec3 collisionPoint;

	// Static
	{
		auto intersectStatic = math::intersectSphereAabb( pos1, radius1, pos2, aabb2 );

		if ( intersectStatic.collision )
		{
			collisionPoint = intersectStatic.collisionPoint;

			result.collisionType = eCollision::Static;
			result.surfaceNormal = intersectStatic.normal;
			result.collisionAmount = intersectStatic.t;
			result.newPos = pos1 + result.surfaceNormal * intersectStatic.t;
		}
	}

	// Dynamic
	if ( result.collisionType == eCollision::None )
	{
		auto intersectDynamic = math::intersectSphereAabbDynamic( pos1,
			radius1,
			velocity,
			pos2,
			aabb2
		);

		if ( intersectDynamic.collision && intersectDynamic.t < dt )
		{
			collisionPoint = intersectDynamic.collisionPoint;

			result.collisionType = eCollision::Dynamic;
			result.surfaceNormal = intersectDynamic.normal;
			result.collisionAmount = velocity * (1.0f - intersectDynamic.t) * -result.surfaceNormal;
			result.newPos = pos1 + velocity * intersectDynamic.t;
		}
	}

	if ( result.collisionType != eCollision::None )
	{
		result.distSq = math::distSq(pos1, collisionPoint);
		result.surfacePoint = collisionPoint;
		result.hEntityContact = hEntity2Aabb;

		ShowNormal( collisionPoint, collisionPoint + result.surfaceNormal * 2.5f );
	}

	return result;
}
//-------------------------------------------------------------------
//	CalculateContact_Aabb_Sphere
//-------------------------------------------------------------------
Contact CollisionManager::CalculateContact_Aabb_Sphere(Handle<Entity> hEntity1Aabb, const math::Vec3& entity1Pos, Handle<Entity> hEntity2, const math::Vec3& velocity, float dt)
{
	Contact result;
	result.oldPos = entity1Pos;

	auto pColAabb = (ColliderInfo_Aabb*)hEntity1Aabb->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 posAabb = entity1Pos;
	math::Vec3 aabb1 = pColAabb->aabbScale;
	float radius2 = pColAabb->boundingSphereRadius;
	auto pColSphere = (ColliderInfo_Sphere*)hEntity2->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 posSphere = entityGet_pos(hEntity2);

	math::Vec3 collisionPoint;

	{
		auto intersectStatic = math::intersectSphereAabb( posAabb, radius2, posSphere, aabb1 );

		if ( intersectStatic.collision )
		{
			collisionPoint = intersectStatic.collisionPoint;

			result.collisionType = eCollision::Static;
			result.surfaceNormal = intersectStatic.normal;
			result.collisionAmount = intersectStatic.t;
			result.newPos = posAabb + result.surfaceNormal * intersectStatic.t;
		}
	}

	if ( result.collisionType == eCollision::None )
	{
		auto intersectDynamic = math::intersectSphereAabbDynamic( posAabb,
			radius2,
			velocity,
			posSphere,
			aabb1
		);

		if ( intersectDynamic.collision && intersectDynamic.t < dt )
		{
			collisionPoint = intersectDynamic.collisionPoint;

			result.collisionType = eCollision::Dynamic;
			result.surfaceNormal = intersectDynamic.normal;
			result.collisionAmount = velocity * (1.0f - intersectDynamic.t) * -result.surfaceNormal;
			result.newPos = posAabb + velocity * intersectDynamic.t;
		}
	}

	if ( result.collisionType != eCollision::None )
	{
		result.distSq = math::distSq(posAabb, collisionPoint);
		result.surfacePoint = collisionPoint;
		result.hEntityContact = hEntity2;

		ShowNormal( collisionPoint, collisionPoint + result.surfaceNormal * 2.5f );
	}

	return result;
}
//-------------------------------------------------------------------
//	CalculateContact_Ellipse_Aabb
//-------------------------------------------------------------------
Contact CollisionManager::CalculateContact_Ellipse_Aabb(Handle<Entity> hEntity1Ellipse, const math::Vec3& entity1Pos, Handle<Entity> hEntity2Aabb, const math::Vec3& velocity, float dt)
{
	Contact result;
	result.oldPos = entity1Pos;

	auto pCol1 = (ColliderInfo_Ellipse*)hEntity1Ellipse->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 pos1 = entity1Pos;
	auto pCol2 = (ColliderInfo_Aabb*)hEntity2Aabb->GetComponentPtrAs<Component_Collision>()->GetCollider();
	math::Vec3 pos2 = entityGet_pos(hEntity2Aabb);

	// Ellipse space (es)
	math::Vec3 toEs = pCol1->scale;
	math::Vec3 esPos1 = math::scaleInv( pos1, toEs );
	float esRadius1 = 1.0f;
	math::Vec3 esPos2 = math::scaleInv( pos2, toEs );
	math::Vec3 esScale2 = math::scaleInv( pCol2->aabbScale, toEs );
	math::Vec3 esVelocity = math::scaleInv( velocity, toEs );
	math::Vec3 collisionPoint;
	
	{
		auto intersectStatic = math::intersectSphereAabb( esPos1, esRadius1, esPos2, esScale2 );

		if ( intersectStatic.collision )
		{
			intersectStatic.collisionPoint = math::scale( intersectStatic.collisionPoint, toEs );
			collisionPoint = intersectStatic.collisionPoint;

			result.collisionType = eCollision::Static;
			result.surfaceNormal = intersectStatic.normal;
			result.collisionAmount = intersectStatic.t;
			result.newPos = pos1 + math::scale( result.surfaceNormal * intersectStatic.t, toEs );
		}
	}

	if ( result.collisionType == eCollision::None )
	{
		auto intersectDynamic = math::intersectSphereAabbDynamic( esPos1,
			esRadius1,
			esVelocity,
			esPos2,
			esScale2
		);

		if ( intersectDynamic.collision && intersectDynamic.t < dt )
		{
			intersectDynamic.collisionPoint = math::scale( intersectDynamic.collisionPoint, toEs );
			collisionPoint = intersectDynamic.collisionPoint;

			result.collisionType = eCollision::Dynamic;
			result.surfaceNormal = intersectDynamic.normal;
			result.collisionAmount = velocity * (1.0f - intersectDynamic.t) * -result.surfaceNormal;
			result.newPos = pos1 + velocity * intersectDynamic.t;
		}
	}

	if ( result.collisionType != eCollision::None )
	{
		result.distSq = math::distSq(pos1, collisionPoint);
		result.surfacePoint = collisionPoint;
		result.hEntityContact = hEntity2Aabb;

		ShowNormal( collisionPoint, collisionPoint + result.surfaceNormal * 2.5f );
	}

	return result;
}
//-------------------------------------------------------------------
//	CreatePartition
//-------------------------------------------------------------------
void CollisionManager::CreatePartition(const math::Vec3& pos, const math::Vec3& scale, bool debugAddEntity, const String& debugEntityContact)
{
	m_pPartition->AddBlock( pos, scale, debugAddEntity, debugEntityContact );
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void CollisionManager::HandleEvent(Event* pEvent)
{
	Event_EntityManager* pEventEntityMngr = nullptr;
	Event_Component* pEventComponent = nullptr;
	Event_TransformChange* pEventTransform = nullptr;
	Event_ComponentPropertyChange* pEventChangeProp = nullptr;

	if ( isEvent(pEvent, &pEventComponent) )
	{
		Table<Component, uint>* pTable = nullptr;

		if ( pEventComponent->action == eComponentAction::Add )
		{
			if ( pEventComponent->hComponent->GetClassRelationship_IsOrHas<Component_Collision>() )
				m_pPartition->Add( HandleAs<Component, Component_Collision>(pEventComponent->hComponent) );
			else if ( pEventComponent->hComponent->GetClassRelationship_IsOrHas<Component_Trigger>() )
				m_triggers.Add( HandleAs<Component, Component_Trigger>(pEventComponent->hComponent), pEventComponent->hComponent->GetId() );
		}
		else if ( pEventComponent->action == eComponentAction::Remove )
		{
			if ( pEventComponent->hComponent->GetClassRelationship_IsOrHas<Component_Collision>() )
			{
				uint componentId = pEventComponent->hComponent->GetId();
				m_pPartition->Remove( componentId );
				m_triggerCollisionCandidate.Remove( componentId );
			}
			else if ( pEventComponent->hComponent->GetClassRelationship_IsOrHas<Component_Trigger>() )
				m_triggers.Remove( pEventComponent->hComponent->GetId() );
		}
	}
	else if ( isEvent(pEvent, &pEventEntityMngr) )
	{
		if ( pEventEntityMngr->action == eEventEntity::RemoveAll )
		{
			m_pPartition->Clear();
			m_collisions.Clear();
			m_triggers.Clear();
			m_triggerCollisionCandidate.Clear();
		}
	}
	else if ( isEvent(pEvent, &pEventTransform) )
	{
		if ( pEventTransform->flag != eTransformation::None )
		{
			auto hCollision = pEventTransform->hEntity->GetComponentAs<Component_Collision>();
			if ( hCollision )
				m_pPartition->OnChange( hCollision );
		}
	}
	else if ( isEvent(pEvent, &pEventChangeProp) )
	{
		bool isComponentCollision = pEventChangeProp->hComponent->GetClassRelationship_IsOrHas<Component_Collision>();

		if ( isComponentCollision )
		{
			HandleAs<Component, Component_Collision> hComponentCollision = HandleAs<Component, Component_Collision>(pEventChangeProp->hComponent);

			if ( hComponentCollision->CanActivateTrigger() )
				m_triggerCollisionCandidate.AddIfNotExists( hComponentCollision, hComponentCollision->GetId() );
			else
				m_triggerCollisionCandidate.Remove( hComponentCollision->GetId() );
		}
	}
}
//-------------------------------------------------------------------
//	HideNormals
//-------------------------------------------------------------------
void CollisionManager::HideNormals()
{
	for ( uint i = 0; i < ARRAY_COUNT(m_normals); i++ )
	{
		if ( m_normals[i] )
			m_normals[i]->visible = false;
	}

	m_currentNormal = 0;
}
//-------------------------------------------------------------------
//	ShowNormal
//-------------------------------------------------------------------
void CollisionManager::ShowNormal(const math::Vec3& p0, const math::Vec3& p1)
{
	m_currentNormal = m_currentNormal % ARRAY_COUNT( m_normals );

	if ( !m_normals[m_currentNormal] )
		return;

	m_normals[m_currentNormal]->visible = true;
	m_normals[m_currentNormal]->point0 = p0;
	m_normals[m_currentNormal]->point1 = p1;

	CHECK( !math::isNan(p0) && !math::isNan(p1) );

	m_currentNormal++;
}
