//#include "Entity_Rocket.h"
//#include "RenderInfo_Entity.h"
//#include "../engine/Primitive.h"
//#include "../engine/CoreInterface.h"
//#include "../engine/Vertex.h"
//#include "GameCollision.h"
//
////-------------------------------------------------------------------
////	Cdtor
////-------------------------------------------------------------------
//Entity_Rocket::Entity_Rocket() : Entity(true)
//{
//	RegisterInheritance( GetClassTagStatic() );
//
//	//m_pCollision = nullptr;
//}
////-------------------------------------------------------------------
//Entity_Rocket::~Entity_Rocket()
//{
//}
////-------------------------------------------------------------------
////	Shutdown
////-------------------------------------------------------------------
//void Entity_Rocket::Shutdown()
//{
//	g_pScene->GetLightMngr()->RemovePointLight( m_hPointLight->GetName() );
//};
////-------------------------------------------------------------------
////	Initialize
////-------------------------------------------------------------------
//void Entity_Rocket::Initialize()
//{
//	CreateModel();
//
//	RenderInfo_Entity* pRenderInfo = new RenderInfo_Entity;
//	pRenderInfo->hModel = m_hModel;
//	pRenderInfo->hDiffuseTexture = primitive::texture::createPink();
//	pRenderInfo->material = eMaterial::Opaque;
//	pRenderInfo->emissiveIntensity = 0.5f;
//	pRenderInfo->transform.scale = math::matrixScale( 0.08f, 0.08f, 0.2f );
//	pRenderInfo->visible = false;
//	SetRenderInfo( (RenderInfo*)pRenderInfo );
//
//	//m_pCollision = UseCollision<CollisionInfo_Sphere>();
//	//m_pCollision->boundingSphereRadius = 0.08f;
//	//m_pCollision->hDebugBoundingSphere->visible = true;
//	//m_pCollision->aabbScale = math::Vec3(m_pCollision->boundingSphereRadius);
//	//m_pCollision->pfncClassify = collisionClassifyFunc_Projectile;
//	//m_pCollision->pfncIgnore = collisionIgnoreFunc_Projectile;
//	//m_pCollision->noclip = true;
//	
//	UsePhysics( true );
//};
////-------------------------------------------------------------------
////	Update
////-------------------------------------------------------------------
//void Entity_Rocket::Update()
//{
//	PhysicsInfo* pPhys = GetPhysicsInfo();
//
//	if ( m_hPointLight )
//	{
//		m_hPointLight->pos = pPhys->pos;
//
//		m_lightGradient.Update();
//		float progress = m_lightGradient.Get();
//		float rangeBase = 4.0f;
//		m_hPointLight->range = rangeBase + progress * rangeBase * 0.05f;
//		m_hPointLight->diffuseColor4 = math::Vec4( 1.0f, 0.41f + progress * 0.11f, 0.05f * (1.0f - progress), 1.0f );
//		m_hPointLight->specularColor4 = math::Vec3(1.0f).Xyzw(1.0f);
//	}
//
//	auto pRenderInfo = GetRenderInfoAs<RenderInfo_Entity>();
//	pRenderInfo->transform.translate[12] = pPhys->pos.x;
//	pRenderInfo->transform.translate[13] = pPhys->pos.y;
//	pRenderInfo->transform.translate[14] = pPhys->pos.z;
//	if ( !math::isZero(pPhys->velocity) )
//		pRenderInfo->transform.rotation = math::matrixOrthogonalLook( pPhys->velocity );
//};
////-------------------------------------------------------------------
////	Launch
////-------------------------------------------------------------------
//void Entity_Rocket::Launch(Handle<Entity> hOwner, const math::Vec3& pos, const math::Vec3& dir)
//{
//	m_hOwner = hOwner;
//
//	if ( !m_hPointLight )
//		m_hPointLight = g_pScene->GetLightMngr()->AddPointLight();
//	
//	m_hPointLight->range = 10.0f;
//	m_hPointLight->attenuation0 = 0.5f;
//	m_hPointLight->attenuation1 = 1.0f;
//	m_hPointLight->attenuation2 = 1.0f;
//
//	m_lightGradient.SetDuration( 0.3f );
//	m_lightGradient.SetInterval( 0.0f, 1.0f );
//	m_lightGradient.Reset();
//
//	//m_pCollision->noclip = false;
//
//	GetRenderInfoAs<RenderInfo_Entity>()->visible = true;
//	GetRenderInfoAs<RenderInfo_Entity>()->transform.translate = math::matrixTranslate( pos );
//
//	PhysicsInfo* pPhys = GetPhysicsInfo();
//	pPhys->pos = pos;
//	pPhys->velocity = dir * 0.1f;
//}
////-------------------------------------------------------------------
////	OnPhysicsUpdate
////-------------------------------------------------------------------
//void Entity_Rocket::OnPhysicsUpdate(Contact* contacts, uint contactCount)
//{
//	if ( contactCount > 0 )
//		g_pEntity->RemoveNextFrame( GetHandle() );
//}
////-------------------------------------------------------------------
////	GetOwner
////-------------------------------------------------------------------
//Handle<Entity> Entity_Rocket::GetOwner()
//{
//	return m_hOwner;
//}
////-------------------------------------------------------------------
////	CreateModel
////-------------------------------------------------------------------
//void Entity_Rocket::CreateModel()
//{
//	HashString name = "rocket";
//
//	if ( g_pResource->GetModelMngr()->HasModelByName( name ) )
//	{
//		m_hModel = g_pResource->GetModelMngr()->GetFormattedByName( name, Vertex_P3N3Tg3Bn3T2::GetLayout() );
//		return;
//	}
//
//	uint vertexCount = 18;
//	Vertex_P3N3Tg3Bn3T2* vertices = new Vertex_P3N3Tg3Bn3T2[vertexCount];
//
//	// Arrow
//	// Right up
//	uint idx = 0;
//
//	vertices[idx++].pos = math::Vec3(0.0f);
//	vertices[idx++].pos = math::Vec3( 1.0f, 0.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 0.0f, 1.0f, -1.0f );
//
//	// Right down
//	vertices[idx++].pos = math::Vec3(0.0f);
//	vertices[idx++].pos = math::Vec3( 0.0f, -1.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 1.0f, 0.0f, -1.0f );
//
//	// Left up
//	vertices[idx++].pos = math::Vec3(0.0f);
//	vertices[idx++].pos = math::Vec3( 0.0f, 1.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( -1.0f, 0.0f, -1.0f );
//
//	// Left down
//	vertices[idx++].pos = math::Vec3(0.0f);
//	vertices[idx++].pos = math::Vec3( -1.0f, 0.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 0.0f, -1.0f, -1.0f );
//
//	// Back
//	vertices[idx++].pos = math::Vec3( -1.0f, 0.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 0.0f, 1.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 0.0f, -1.0f, -1.0f );
//
//	vertices[idx++].pos = math::Vec3( 1.0f, 0.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 0.0f, -1.0f, -1.0f );
//	vertices[idx++].pos = math::Vec3( 0.0f, 1.0f, -1.0f );
//
//	CHECK( idx == vertexCount );
//
//	for ( uint i = 0; i < vertexCount; i += 3 )
//	{
//		math::Vec3 edge0 = vertices[i + 1].pos - vertices[i + 0].pos;
//		math::Vec3 edge1 = vertices[i + 2].pos - vertices[i + 0].pos;
//		math::Vec3 normal = math::normalize( math::cross( edge0, edge1 ) );
//		math::Vec3 tangent;
//		math::Vec3 binormal;
//
//		vertices[i + 0].normal = normal;
//		vertices[i + 0].texCoord = math::Vec2(0.5f, 0.0f);
//		vertices[i + 1].normal = normal;
//		vertices[i + 1].texCoord = math::Vec2(1.0f, 1.0f);
//		vertices[i + 2].normal = normal;
//		vertices[i + 2].texCoord = math::Vec2(0.0f, 1.0f);
//
//		math::calculateTangentBinormal( vertices[i + 0].pos, vertices[i + 1].pos, vertices[i + 2].pos, vertices[i + 0].texCoord, vertices[i + 1].texCoord, vertices[i + 2].texCoord, tangent, binormal );
//
//		vertices[i + 0].tangent = tangent;
//		vertices[i + 0].binormal = binormal;
//		vertices[i + 1].tangent = tangent;
//		vertices[i + 1].binormal = binormal;
//		vertices[i + 2].tangent = tangent;
//		vertices[i + 2].binormal = binormal;
//	}
//
//	m_hModel = g_pResource->GetModelMngr()->Create( name, eTopology::Triangle, vertices, vertexCount, nullptr, 0 );
//
//	safeArrayRelease( vertices );
//}
