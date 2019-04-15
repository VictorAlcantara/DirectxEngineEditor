#include "Entity_MonsterProjectile.h"
#include "RenderInfo_Entity.h"
#include "../engine/Primitive.h"
#include "../engine/CoreInterface.h"
#include "GameCollision.h"
#include "../engine/Path.h"
#include "../engine/Vertex.h"
#include "Entity_Player.h"
#include "GameGlobal.h"
#include "Entity_Monster.h"
#include "../engine/Component_Render.h"

const float entityTime = 15.0f;
const float scaleNormal = 0.8f;
const float scaleMini = scaleNormal * 0.25f;
const math::Vec4 color = math::normalizeRgb(0, 193, 34, 200);

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Collision_MonsterProjectile::Component_Collision_MonsterProjectile()
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	Ignore
//-------------------------------------------------------------------
bool Component_Collision_MonsterProjectile::Ignore(const Handle<Entity>& hOther)const
{
	return hOther->GetClassRelationship_Is<Entity_Monster>() || hOther->GetClassRelationship_Is<Entity_MonsterProjectile>();
}

/****************************************************************************************
	Component_Physics_MonsterProjectile
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Component_Physics_MonsterProjectile::Component_Physics_MonsterProjectile()
{
	RegisterInheritance( GetClassTagStatic() );
}
//-------------------------------------------------------------------
//	OnPhysicsUpdate
//-------------------------------------------------------------------
void Component_Physics_MonsterProjectile::OnPhysicsUpdate(Contact* contacts, uint contactCount)
{
	CHECK( GetOwner()->GetClassRelationship_Is<Entity_MonsterProjectile>() );

	auto pEntity = &GetOwner().GetAs<Entity_MonsterProjectile>();

	if ( pEntity->m_isShrinking )
		return;

	for ( uint i = 0; i < contactCount; i++ )
	{
		auto pPlayer = Entity::Convert<Entity_Player>( contacts[i].hEntityContact );

		if ( pPlayer )
		{
			if ( pEntity->m_isMini )
				pPlayer->TakeDamage( gameGlobal::ai::monster::damage::PROJECTILE_MINI );
			else
				pPlayer->TakeDamage( gameGlobal::ai::monster::damage::PROJECTILE );

			//m_isShrinking = true;

			break;
		}
		else if ( pEntity->m_isMini )
		{
			pEntity->m_isShrinking = true;
		}
	}
}


/****************************************************************************************
	Entity_MonsterProjectile
*****************************************************************************************/

//---------------------------------------------------------------------------------------
//	Cdtor
//---------------------------------------------------------------------------------------
Entity_MonsterProjectile::Entity_MonsterProjectile() : Entity(true)
{
	RegisterInheritance( GetClassTagStatic() );
	m_isShrinking = false;
	m_scaleBase = scaleNormal;
	m_scaleFactor = 1.0f;
	m_shrinkTime = 1.0f;
	m_shrinkCurrentTime = 0.0f;
	m_entityCurrentTime = 0.0f;
	m_isMini = false;
}
//---------------------------------------------------------------------------------------
//	OnShutdown
//---------------------------------------------------------------------------------------
void Entity_MonsterProjectile::OnShutdown()
{
}
//---------------------------------------------------------------------------------------
//	OnInitialize
//---------------------------------------------------------------------------------------
void Entity_MonsterProjectile::OnInitialize()
{
	auto pRenderInfo = SmartPtr<RenderInfo_Entity>::New();
	pRenderInfo->SetModel( primitive::geometry::createBlock() );
	//pRenderInfo->hModel = g_pResource->GetModelMngr()->LoadObj<Vertex_P3N3Tg3Bn3T2>( path::model("monkey") );
	pRenderInfo->hDiffuseMap = primitive::texture::createPink();
	pRenderInfo->material = eMaterial::Transparent;
	pRenderInfo->emissiveIntensity = 1.0f;
	pRenderInfo->ambientIntensity = 1.0f;
	pRenderInfo->diffuseIntensity = 1.0f;
	pRenderInfo->color4 = color;
	pRenderInfo->visible = true;

	auto hCmpRender = AddComponentAs<Component_Render>();
	hCmpRender->SetData( pRenderInfo.As<RenderInfo>() );

	m_hCollision = AddComponentAs<Component_Collision_MonsterProjectile>();
	m_hCollision->SetCollider(eColliderInfo::Aabb);
	
	m_hPhysics = AddComponentAs<Component_Physics_MonsterProjectile>();
	m_hPhysics->GetData()->acceleration = math::Vec3( 0.0f, -9.8f, 0.0f );
	m_hPhysics->GetData()->maxVelocity = 50.0f;

	UpdateScale();
}
//---------------------------------------------------------------------------------------
//	Update
//---------------------------------------------------------------------------------------
void Entity_MonsterProjectile::Update()
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	if ( m_isShrinking )
	{
		m_shrinkCurrentTime += g_pCore->GetTimeSinceLastFrame();

		float percentage = m_shrinkCurrentTime / (m_shrinkTime * (m_isMini ? 0.5f : 1.0f));

		if ( percentage < 1.0f )
			m_scaleFactor = math::smoothstep2( 1.0f, 0.0f, percentage );
		else
			g_pEntity->RemoveNextFrame( GetHandle() );
	}
	else
	{
		m_entityCurrentTime += g_pCore->GetTimeSinceLastFrame();

		if ( m_entityCurrentTime > entityTime )
			m_isShrinking = true;
		else
		{
			float saturate = math::smoothstep2( 0.1f, 0.7f, m_entityCurrentTime / entityTime );
			pRenderInfo->color4 =  math::saturateRgb( color, saturate );
		}
	}

	UpdateScale();

	math::Vec3 pos = m_hPhysics->GetPos();
	pRenderInfo->SetTranslation( pos );
}
//---------------------------------------------------------------------------------------
//	UpdateScale
//---------------------------------------------------------------------------------------
void Entity_MonsterProjectile::UpdateScale()
{
	auto pRenderInfo = GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_Entity>(true);

	float scale = m_scaleBase * m_scaleFactor;

	pRenderInfo->SetScale( math::Vec3(scale) );
	m_hCollision->GetCollider(true)->aabbScale = pRenderInfo->GetModel()->GetAabb().GetScale() * scale;
}
//-------------------------------------------------------------------
//	SetIsMini
//-------------------------------------------------------------------
void Entity_MonsterProjectile::SetIsMini(bool b)
{
	m_isMini = b;

	if ( m_isMini )
		m_scaleBase = scaleMini;
	else
		m_scaleBase = scaleNormal;
}
