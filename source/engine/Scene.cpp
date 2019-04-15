#include "../engine/Scene.h"
#include "../engine/Effect.h"
#include "../engine/Entity.h"
#include "../engine/RenderInfo.h"
#include "../engine/CoreInterface.h"
#include "../engine/Texture.h"
#include "../engine/Light.h"
#include "../engine/Primitive.h"
#include "../engine/TableHashFunction.h"
#include "Partition.h"
#include "../engine/Component_Render.h"
#include "EventEngine.h"

const uint SHADOW_MAP_RESOLUTION = 1024;

#ifdef DEBUG_MODE
	#define DEBUG_BREAK_ON_RENDER(_entityId) BREAKPOINT_IF( _entityId == m_debugBreakOnRender );
#else
	#define DEBUG_BREAK_ON_RENDER
#endif

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Scene::Scene() : m_toRenderTable(250, tableHashFunc_Uint),
	m_shadowCache(50, tableHashFunc_Uint),
	m_lightTransform(10, tableHashFunc_Uint),
	m_dynamicObjects(50, tableHashFunc_Uint),
	m_repoDebugLine(1000, 300), 
	m_repoDebugCube(1000, 300),
	m_repoDebugSphere(1000, 300),
	m_repoDebugCylinder(1000, 300)
{
	m_initialized = false;
	Clear();
}
//-------------------------------------------------------------------
Scene::~Scene()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Scene::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	Clear();

	m_pPartition->Shutdown();
	m_pPartition = nullptr;

	m_repoDebugSphere.Clear();
	m_repoDebugLine.Clear();

	m_pRenderTargetScene->Shutdown();
	m_pRenderTargetScene = nullptr;

	ShutdownShadowCache();

	m_pEffectVertexColor->Shutdown();
	m_pEffectVertexColor = nullptr;

	m_pEffectShadowMap->Shutdown();
	m_pEffectShadowMap = nullptr;

	m_pEffectSpotLight->Shutdown();
	m_pEffectSpotLight = nullptr;

	m_pEffectPointLight->Shutdown();
	m_pEffectPointLight = nullptr;

	m_pEffectDepthEmissive->Shutdown();
	m_pEffectDepthEmissive = nullptr;

	m_pEffectLightAll->Shutdown();
	m_pEffectLightAll = nullptr;

	m_pRefraction->Shutdown();
	m_pRefraction = nullptr;

	m_pLightMngr->Shutdown();
	m_pLightMngr = nullptr;

	g_pResource->GetTextureMngr()->Remove( m_hRenderTargetCopy );

	ShutdownListener();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Scene::Initialize()
{
	if ( m_initialized )
	{
		ASSERT(  "Already initialized" );
		return;
	}

	m_allowShadow = true;
	m_allowLight = true;

	InitializeListener();

	m_pPartition = new PartitionScene;
	m_pPartition->Initialize();

	m_pLightMngr = new LightManager;
	m_pLightMngr->Initialize();

	m_pEffectDepthEmissive = new Effect_DepthAmbientEmissive;
	m_pEffectDepthEmissive->Initialize();
	m_pEffectDepthEmissive->GetConfig().renderer.blendMode = eBlendMode::Alpha;

	m_pEffectPointLight = new Effect_PointLight;
	m_pEffectPointLight->Initialize();

	m_pEffectSpotLight = new Effect_SpotLight;
	m_pEffectSpotLight->Initialize();

	m_pEffectLightAll = new Effect_LightAll;
	m_pEffectLightAll->Initialize();

	m_pEffectShadowMap = new Effect_ShadowMap;
	m_pEffectShadowMap->Initialize();

	m_pEffectVertexColor = new Effect_VertexColor;
	m_pEffectVertexColor->Initialize();

	m_pRefraction = new Effect_Refraction;
	m_pRefraction->Initialize();

	m_pRenderTargetScene = new RenderTarget;
	m_pRenderTargetScene->Initialize( g_pRenderer->GetWindowViewport().GetDimension(), eColorFormat::Rgba_32_Float );

	{
		math::ViewportInfo viewport = g_pRenderer->GetWindowViewport();
		m_hRenderTargetCopy = g_pResource->GetTextureMngr()->CreateEmpty( "sceneRenderTargetCopy", viewport.width, viewport.height, eColorFormat::Rgba_32_Float );
	}

	m_hLine = primitive::geometry::createLine();
	m_hCubeLine = primitive::geometry::createCubeLine();
	m_hSphereLine = primitive::geometry::createSphereLine( 16, 8 );
	m_hCylinder = primitive::geometry::createCylinderLine(3);

	m_hTextureWhite = primitive::texture::createWhite();
	m_hTextureBlack = primitive::texture::createBlack();
	m_hTexturePink = primitive::texture::createPink();

	m_debugBreakOnRender = 0;

	m_dynamicObjects.Debug_SlotDepthCheck( 100 );
	m_lightTransform.Debug_SlotDepthCheck( 100 );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void Scene::Clear()
{
	if ( m_pLightMngr )
		m_pLightMngr->Clear();
	if ( m_pPartition )
		m_pPartition->Clear();

	m_clearColor = math::Vec4( 0.10f, 0.13f, 0.19f, 1.0f );
	m_allowShadow = true;
	m_allowLight = true;
	m_debugBreakOnRender = 0;

	ShutdownShadowCache();
	Debug_ClearReport();
}
//-------------------------------------------------------------------
//	CreatePartition
//-------------------------------------------------------------------
void Scene::CreatePartition(const math::Vec3& pos, const math::Vec3& scale)
{
	m_pPartition->AddBlock( pos, scale );
}
//-------------------------------------------------------------------
//	EnableShadow
//-------------------------------------------------------------------
void Scene::EnableShadow(bool b)
{
	m_allowShadow = b;
}
//-------------------------------------------------------------------
//	EnableLight
//-------------------------------------------------------------------
void Scene::EnableLights(bool b)
{
	m_allowLight = b;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Scene::RenderToTexture(const math::CameraTransform& camera)
{
	//PROFILE_START( "renderPrepare" );
	Debug_ClearReport();

	CHECK( g_pCore->GetState() == eCoreState::Render );

	m_toRenderTable.Clear();

	m_pRenderTargetScene->Bind();
	m_pRenderTargetScene->Clear( m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w );

	List<ObjectLightGroup> opaques;
	List<ObjectLightGroup> translucents;

	math::Frustum frustum = math::extractViewFrustum( camera.view * camera.proj);
	math::Aabb3 frustumAabb = camera.GetFrustumAabbGlobal();

	m_pPartition->GetFromFrustum( frustum, frustumAabb, m_toRenderTable );

	// Sort rendering data
	for ( auto it = m_toRenderTable.GetIterator(); it; it++ )
	{
		auto pRenderInfo = (*it)->GetData();

		DEBUG_BREAK_ON_RENDER( (*it)->GetOwner()->GetId() );

		if ( !pRenderInfo || !pRenderInfo->visible )
			continue;

		auto renderInfoAabb = pRenderInfo->GetAabbGlobal();
		if ( !math::intersectAabb3(renderInfoAabb, frustumAabb).collision || !math::intersectAabb3Frustum(renderInfoAabb, frustum) )
		{
			continue;
		}

		if ( pRenderInfo->material == eMaterial::Opaque )
		{
			AddToListOrderedFrontToBack( *it, pRenderInfo, opaques, camera );
		}
		else if ( pRenderInfo->material == eMaterial::Transparent || pRenderInfo->material == eMaterial::Refraction )
		{
			AddToListOrderedBackToFront( *it, pRenderInfo, translucents, camera );
		}
		else
			ASSERT( "Unknown material type" );
	}
	//PROFILE_END( "renderPrepare" );

	// Render opaque
	Render_Opaque( opaques, camera );

	//PROFILE_START( "renderTransparent" );
	// Render translucent
	Render_Translucent( translucents, camera );
	//PROFILE_END( "renderTransparent" );

	PROFILE_START( "renderDebug" );
	Debug_Render( camera );
	PROFILE_END( "renderDebug" );

	m_dynamicObjects.Clear();
	m_lightTransform.Clear();

	if ( m_debugLitOpaqueCount )
		m_debugLitOpaqueAvrg /= (float)m_debugLitOpaqueCount;

	if ( m_debugLitTransparentCount )
		m_debugLitTransparentAvrg /= (float)m_debugLitTransparentCount;
}
//-------------------------------------------------------------------
//	Render_Opaque
//-------------------------------------------------------------------
void Scene::Render_Opaque(List<ObjectLightGroup>& objects, const math::CameraTransform& camera)
{
	SceneRenderInfo info;
	info.writeToDepth = true;
	info.depthOperation = eDepthOperation::Less;

	m_debugOpaqueCount = objects.Count();
	for ( auto it = objects.GetIterator(); it; it++ )
	{
		DEBUG_BREAK_ON_RENDER( (*it).entityId );
		auto pRenderInfo = (*it).pRenderInfo;
		Pass_DepthEmissiveAmbient( pRenderInfo, info, camera );
	}

	info.depthOperation = eDepthOperation::Equal;

	for ( auto it = objects.GetIterator(); it; it++ )
	{
		uint pointLightsCount = (*it).pointLights ? (*it).pointLights->Count() : 0;
		uint spotLightsCount = (*it).spotLights ? (*it).spotLights->Count() : 0;
		m_debugLitOpaqueAvrg += pointLightsCount + spotLightsCount;
		m_debugLitOpaqueCount++;
		m_debugLitOpaqueMax = math::max( m_debugLitOpaqueMax, math::max( pointLightsCount, spotLightsCount ) );
		DEBUG_BREAK_ON_RENDER( (*it).entityId );
		Pass_Light( *it, info, camera );
	}
}
//-------------------------------------------------------------------
//	Render_Translucent
//-------------------------------------------------------------------
void Scene::Render_Translucent(List<ObjectLightGroup>& objects, const math::CameraTransform& camera)
{
	SceneRenderInfo info;
	info.writeToDepth = true;
	info.depthOperation = eDepthOperation::LessEqual;

	for ( auto it = objects.GetIterator(); it; it++ )
	{
		DEBUG_BREAK_ON_RENDER( (*it).entityId );

		auto pRenderInfo = (*it).pRenderInfo;

		CHECK( pRenderInfo->material == eMaterial::Transparent || pRenderInfo->material == eMaterial::Refraction );

		if ( pRenderInfo->material == eMaterial::Transparent )
		{
			uint pointLightsCount = (*it).pointLights ? (*it).pointLights->Count() : 0;
			uint spotLightsCount = (*it).spotLights ? (*it).spotLights->Count() : 0;
			m_debugLitTransparentAvrg += pointLightsCount + spotLightsCount;
			m_debugTransparentCount++;
			m_debugLitTransparentCount++;
			m_debugLitTransparentMax = math::max( m_debugLitTransparentMax, math::max( pointLightsCount, spotLightsCount ) );
			Pass_DepthEmissiveAmbient( pRenderInfo, info, camera );
			Pass_Light( *it, info, camera );
		}
		else
		{
			Pass_Refraction( pRenderInfo, info, camera );
		}
	}
}
//-------------------------------------------------------------------
//	Pass_DepthEmissiveAmbient
//-------------------------------------------------------------------
void Scene::Pass_DepthEmissiveAmbient(const RenderInfo* pInfo, const SceneRenderInfo& renderInfo, const math::CameraTransform& camera)
{
	Handle<Model> hModel = pInfo->modelGroup.Get( (renderFlag)eRenderFlagModel::Primary );
	uint modelId = hModel->GetId();
	math::Mtx44 world = pInfo->GetTransform().GetWorld();

	SetMaterialOnEffect( &m_pEffectDepthEmissive->GetConfig().material, pInfo );

	m_pEffectDepthEmissive->GetConfig().ambientLightColor4 = m_allowLight ? GetLightMngr()->GetAmbientLight() : math::Vec3( 0.7f ).Xyzw(1.0f);
	m_pEffectDepthEmissive->GetConfig().renderer.cullMode = pInfo->cullMode;
	m_pEffectDepthEmissive->GetConfig().renderer.wireframe = pInfo->fillMode == eFillMode::Wireframe;
	m_pEffectDepthEmissive->GetConfig().renderer.depthOperation = renderInfo.depthOperation;
	m_pEffectDepthEmissive->GetConfig().renderer.enableDepthWrite = renderInfo.writeToDepth;

	m_pEffectDepthEmissive->Render( hModel, world, camera.view, camera.proj );
}
//-------------------------------------------------------------------
//	Pass_Light
//-------------------------------------------------------------------
void Scene::Pass_Light(ObjectLightGroup& olg, const SceneRenderInfo& renderInfo, const math::CameraTransform& camera)
{
	m_pEffectLightAll->GetConfig().renderer.depthOperation = renderInfo.depthOperation;
	m_pEffectLightAll->GetConfig().renderer.enableDepthWrite = renderInfo.writeToDepth;

	Pass_LightAll( olg, camera );

	//m_pEffectPointLight->GetConfig().renderer.depthOperation = renderInfo.depthOperation;
	//m_pEffectPointLight->GetConfig().renderer.enableDepthWrite = renderInfo.writeToDepth;

	//Pass_PointLight( olg, camera );

	//m_pEffectSpotLight->GetConfig().renderer.depthOperation = renderInfo.depthOperation;
	//m_pEffectSpotLight->GetConfig().renderer.enableDepthWrite = renderInfo.writeToDepth;

	//Pass_SpotLight( olg, camera );
}
//-------------------------------------------------------------------
//	Pass_LightAll
//-------------------------------------------------------------------
void Scene::Pass_LightAll(ObjectLightGroup& olg, const math::CameraTransform& camera)
{
	PROFILE_START( "lightAllPrepare" );

	auto pInfo = olg.pRenderInfo;
	Handle<Model> hModel = pInfo->modelGroup.Get( (renderFlag)eRenderFlagModel::Primary );
	math::Mtx44 world = pInfo->GetTransform().GetWorld();

	SetMaterialOnEffect( &m_pEffectLightAll->GetConfig().material, pInfo );

	const auto& material = m_pEffectLightAll->GetConfig().material;
	if ( math::isZero(material.diffuseColor4.Xyz()) )
	{
		PROFILE_END( "lightAllPrepare" );
		return;
	}

	// Point light
	{
		auto it = olg.pointLights->GetIterator();
		for ( uint i = 0; i < m_pEffectLightAll->GetMaxLights(); i++ )
		{
			auto pLight = it ? &(*it) : nullptr;
			SetPointLightOnEffectProp( pInfo, pLight, &m_pEffectLightAll->GetConfig().pointLights[i] );
			it++;
		}
	}

	// Spot light
	{
		auto it = olg.spotLights->GetIterator();
		for ( uint i = 0; i < m_pEffectLightAll->GetMaxLights(); i++ )
		{
			auto pLight = it ? &(*it) : nullptr;
			SetSpotLightOnEffectProp( pInfo, pLight, &m_pEffectLightAll->GetConfig().spotLights[i] );
			it++;
		}
	}
	PROFILE_END( "lightAllPrepare" );

	// Render
			
	m_pEffectLightAll->GetConfig().cameraPos = camera.pos;
	m_pEffectLightAll->GetConfig().renderer.cullMode = pInfo->cullMode;
	m_pEffectLightAll->GetConfig().renderer.wireframe = pInfo->fillMode == eFillMode::Wireframe;

	//PROFILE_START( "lightAllRender" );
	m_pEffectLightAll->Render( hModel, world, camera.view, camera.proj );
	//PROFILE_END( "lightAllRender" );

	m_debugLightAllRenderCount++;
}
//-------------------------------------------------------------------
//	SetPointLightOnEffectProp
//-------------------------------------------------------------------
void Scene::SetPointLightOnEffectProp(const RenderInfo* pRenderInfo, const SceneLight<PointLight>* pSceneLight, EffectProp_PointLight* pPropLight)
{
	if ( !pSceneLight )
	{
		pPropLight->Clear();
		return;
	}

	auto pLight = pSceneLight->pLight;

	pPropLight->pos = pLight->pos;
	pPropLight->range = pLight->range;
	pPropLight->diffuseColor4 = pLight->diffuseColor4;
	pPropLight->specularColor4 = pLight->specularColor4;
	pPropLight->attenuation0 = pLight->attenuation0;
	pPropLight->attenuation1 = pLight->attenuation1;
	pPropLight->attenuation2 = pLight->attenuation2;
}
//-------------------------------------------------------------------
//	SetSpotLightOnEffectProp
//-------------------------------------------------------------------
void Scene::SetSpotLightOnEffectProp(const RenderInfo* pRenderInfo, const SceneLight<SpotLight>* pSceneLight, EffectProp_SpotLight* pPropLight)
{
	if ( !pSceneLight )
	{
		pPropLight->Clear();
		pPropLight->hShadowMap = m_hTextureBlack;
		return;
	}

	auto pLight = pSceneLight->pLight;

	math::Mtx44 lightView;
	math::Mtx44 lightProj;

	//PROFILE_START( "shadowRender" );
	// Shadow pass
	if ( pSceneLight->pShadowCache )
	{
		pLight->GetViewProj( lightView, lightProj );

		if ( !pSceneLight->pShadowCache->hasRendered )
		{
			pSceneLight->pShadowCache->pRenderTargetShadowMap->Bind();
			pSceneLight->pShadowCache->pRenderTargetShadowMap->Clear( 0.0f, 0.0f, 0.0f, 1.0f );

			for ( auto it = pSceneLight->pShadowCache->objects->GetIterator(); it; it++ )
				Pass_ShadowMap( (*it)->GetData(), lightView, lightProj );

			m_pRenderTargetScene->Bind();

			pSceneLight->pShadowCache->hasRendered = true;
		}
		
		pPropLight->shadowEnabled = true;
		pPropLight->shadowBias = pLight->shadowBias;
		pPropLight->distanceAdjustment = pLight->shadowDistanceAdjustment;
		pPropLight->hShadowMap = pSceneLight->pShadowCache->pRenderTargetShadowMap->GetTexture();
	}
	else
	{
		pPropLight->shadowEnabled = false;
		pPropLight->hShadowMap = m_hTextureWhite;
	}
	//PROFILE_END( "shadowRender" );

	// Spot light
	math::Vec3 lightPos = pLight->GetPos();
	math::Vec3 lightDir = pLight->GetDirection();
	float lightRange = pLight->GetRange();
	float lightInnerCone = pLight->GetInnerConeDegs();
	float lightOuterCone = pLight->GetOuterConeDegs();

	pPropLight->pos = lightPos;
	pPropLight->direction = lightDir;
	pPropLight->range = lightRange;
	pPropLight->innerConeDegs = lightInnerCone;
	pPropLight->outerConeDegs = lightOuterCone;
	pPropLight->diffuseColor4 = pLight->diffuseColor4;
	pPropLight->specularColor4 = pLight->specularColor4;
	pPropLight->attenuation0 = pLight->attenuation0;
	pPropLight->attenuation1 = pLight->attenuation1;
	pPropLight->attenuation2 = pLight->attenuation2;
	pPropLight->transform.view = lightView;
	pPropLight->transform.proj = lightProj;
}
//-------------------------------------------------------------------
//	Pass_PointLightPass_PointLight
//-------------------------------------------------------------------
void Scene::Pass_PointLight(ObjectLightGroup& olg, const math::CameraTransform& camera)
{
	Debug_AddLightPerObject( olg.pointLights->Count(), m_debugPointLightsPerObject, ARRAY_COUNT(m_debugPointLightsPerObject) );

	if ( olg.pointLights->Count() == 0 )
		return;

	auto pInfo = olg.pRenderInfo;

	Handle<Model> hModel = pInfo->modelGroup.Get( (renderFlag)eRenderFlagModel::Primary );
	math::Mtx44 world = pInfo->GetTransform().GetWorld();

	auto it = olg.pointLights->GetIterator();
	for ( uint i = 0; i < m_pEffectPointLight->GetMaxLights(); i++ )
	{
		auto pLight = it ? &(*it) : nullptr;
		SetPointLightOnEffectProp( pInfo, pLight, &m_pEffectPointLight->GetConfig().lights[i] );
		it++;
	}

	SetMaterialOnEffect( &m_pEffectPointLight->GetConfig().material, pInfo );

	m_pEffectPointLight->GetConfig().cameraPos = camera.pos;
	m_pEffectPointLight->GetConfig().renderer.cullMode = pInfo->cullMode;
	m_pEffectPointLight->GetConfig().renderer.wireframe = pInfo->fillMode == eFillMode::Wireframe;
	m_pEffectPointLight->Render( hModel, world, camera.view, camera.proj );

	m_debugPointLightRenderCount++;
}
//-------------------------------------------------------------------
//	Pass_SpotLight
//-------------------------------------------------------------------
void Scene::Pass_SpotLight(ObjectLightGroup& olg, const math::CameraTransform& camera)
{
	Debug_AddLightPerObject( olg.spotLights->Count(), m_debugSpotLightsPerObject, ARRAY_COUNT(m_debugSpotLightsPerObject) );

	if ( olg.spotLights->Count() == 0 )
		return;

	auto pInfo = olg.pRenderInfo;
	Handle<Model> hModel = pInfo->modelGroup.Get( (renderFlag)eRenderFlagModel::Primary );
	math::Mtx44 world = pInfo->GetTransform().GetWorld();

	auto it = olg.spotLights->GetIterator();
	for ( uint i = 0; i < m_pEffectSpotLight->GetMaxLights(); i++ )
	{
		auto pLight = it ? &(*it) : nullptr;
		SetSpotLightOnEffectProp( pInfo, pLight, &m_pEffectSpotLight->GetConfig().lights[i] );
		it++;
	}

	m_pEffectSpotLight->GetConfig().cameraPos = camera.pos;
			
	SetMaterialOnEffect( &m_pEffectSpotLight->GetConfig().material, pInfo );

	m_pEffectSpotLight->GetConfig().renderer.cullMode = pInfo->cullMode;
	m_pEffectSpotLight->GetConfig().renderer.wireframe = pInfo->fillMode == eFillMode::Wireframe;
	m_pEffectSpotLight->Render( hModel, world, camera.view, camera.proj );

	m_debugSpotLightRenderCount++;
}
//-------------------------------------------------------------------
//	Pass_Refraction
//-------------------------------------------------------------------
void Scene::Pass_Refraction(const RenderInfo* pRenderInfo, const SceneRenderInfo& renderInfo, const math::CameraTransform& camera)
{
	//m_pRenderTargetScene->Unbind();
	m_hRenderTargetCopy->Copy( GetSceneTexture().GetPtr() );
	//m_pRenderTargetScene->Bind();

	Handle<Model> hModel = pRenderInfo->modelGroup.Get( (renderFlag)eRenderFlagModel::Primary );
	math::Mtx44 world = pRenderInfo->GetTransform().GetWorld();

	m_pRefraction->GetConfig().hNormalMap = pRenderInfo->textureGroup.Get( (renderFlag)eRenderFlagTexture::NormalMap );
	m_pRefraction->GetConfig().hRefractionMap = m_hRenderTargetCopy;

	if ( !m_pRefraction->GetConfig().hNormalMap )
		m_pRefraction->GetConfig().hNormalMap = m_hTextureBlack;

	m_pRefraction->GetConfig().textureTransform = pRenderInfo->matrix3Group.Get( (renderFlag)eRenderFlagMatrix3::TextureTransform );
	m_pRefraction->GetConfig().sampler = pRenderInfo->textureSamplerGroup.Get( (renderFlag)eRenderFlagTexture::Ambient | (renderFlag)eRenderFlagTexture::Diffuse );
	m_pRefraction->GetConfig().refractionIntensity = pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::RefractionIntensity );
	m_pRefraction->GetConfig().colorSaturation = pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::ColorSaturation );
	m_pRefraction->GetConfig().colorSeparation = pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::ColorSeparation );
	m_pRefraction->GetConfig().color3 = pRenderInfo->vec4Group.Get( (renderFlag)eRenderFlagVec4::Color ).Xyz();
	m_pRefraction->GetConfig().renderer.cullMode = pRenderInfo->cullMode;
	m_pRefraction->GetConfig().renderer.wireframe = pRenderInfo->fillMode == eFillMode::Wireframe;
	m_pRefraction->GetConfig().renderer.depthOperation = renderInfo.depthOperation;
	m_pRefraction->GetConfig().renderer.enableDepthWrite = renderInfo.writeToDepth;

	m_pRefraction->Render( hModel, world, camera.view, camera.proj );
}
//-------------------------------------------------------------------
//	Pass_ShadowMap
//-------------------------------------------------------------------
void Scene::Pass_ShadowMap(const RenderInfo* pInfo, const math::Mtx44& lightView, const math::Mtx44& lightProj)
{
	if ( !pInfo->castShadow || !pInfo->visible )
		return;

	m_pEffectShadowMap->GetConfig().lightTransform.view = lightView;
	m_pEffectShadowMap->GetConfig().lightTransform.proj = lightProj;

	Handle<Model> hModel = pInfo->modelGroup.Get( (renderFlag)eRenderFlagModel::Primary );

	m_pEffectShadowMap->GetConfig().renderer.cullMode = pInfo->cullMode;
	m_pEffectShadowMap->Render( hModel, pInfo->GetTransform().GetWorld() );
}
//-------------------------------------------------------------------
//	SetMaterialOnEffect
//-------------------------------------------------------------------
void Scene::SetMaterialOnEffect(EffectProp_Material* pMaterial, const RenderInfo* pRenderInfo)
{
	CHECK( pMaterial && pRenderInfo );

	pMaterial->hDiffuseTexture = pRenderInfo->textureGroup.Get( (renderFlag)eRenderFlagTexture::Diffuse );
	pMaterial->hSpecularTexture = pRenderInfo->textureGroup.Get( (renderFlag)eRenderFlagTexture::Specular );
	pMaterial->hNormalMap = pRenderInfo->textureGroup.Get( (renderFlag)eRenderFlagTexture::NormalMap );

	if ( !pMaterial->hDiffuseTexture )
		pMaterial->hDiffuseTexture = m_hTexturePink;
	if ( !pMaterial->hSpecularTexture )
		pMaterial->hSpecularTexture = m_hTextureWhite;
	if ( !pMaterial->hNormalMap )
		pMaterial->hNormalMap = m_hTextureBlack;

	pMaterial->textureTransform = pRenderInfo->matrix3Group.Get( (renderFlag)eRenderFlagMatrix3::TextureTransform );
	pMaterial->sampler = pRenderInfo->textureSamplerGroup.Get( (renderFlag)eRenderFlagTexture::Ambient | (renderFlag)eRenderFlagTexture::Diffuse );
	math::Vec4 color4 = pRenderInfo->vec4Group.Get( (renderFlag)eRenderFlagVec4::Color );
	pMaterial->emissiveColor4 = math::mulRgb( color4, pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::EmissiveIntensity ) );
	pMaterial->ambientColor4 = math::mulRgb( color4, pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::AmbientIntensity ) );
	pMaterial->diffuseColor4 = math::mulRgb( color4, pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::DiffuseIntensity ) );

	float specularIntensity = pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::SpecularIntensity );

	// What is this thing? oO
	math::Vec3 specularColor3 = math::clampUnit( color4 * 1.5f ).Xyz();

	if ( specularIntensity < 0.5f )
	{
		specularColor3 = math::lerp( math::Vec3(0.0f), specularColor3, specularIntensity / 0.5f );
	}
	else
	{
		specularColor3 = math::lerp( specularColor3, math::Vec3(1.0f), 1.0f - (1.0f - specularIntensity) / 0.5f );
	}

	pMaterial->specularColor4 = math::Vec4(specularColor3, color4.w);
	pMaterial->specularPower = pRenderInfo->floatGroup.Get( (renderFlag)eRenderFlagFloat::SpecularPower );
	pMaterial->specularPower *= pMaterial->specularPower;
}
//-------------------------------------------------------------------
//	AddToListOrderedFrontToBack
//-------------------------------------------------------------------
void Scene::AddToListOrderedFrontToBack(HandleAs<Component, Component_Render>& hCmp, RenderInfo* pRenderInfo, List<ObjectLightGroup>& groups, const math::CameraTransform& camera)
{
	ObjectLightGroup group = CreateLightGroup( pRenderInfo );

	PROFILE_START( "renderOrder" )
	math::Vec3 posCurrent = pRenderInfo->GetTransform().GetPos();
	float distSqCurrent = math::distSq(camera.pos, posCurrent);

	#ifdef DEBUG_MODE
		group.entityId = hCmp->GetOwner()->GetId();
	#endif

	for ( auto pNode = groups.GetLast(); pNode; pNode = pNode->GetPrev() )
	{
		math::Vec3 posNode = pNode->GetValue().pRenderInfo->GetTransform().GetPos();
		float distSqNode = math::distSq(camera.pos, posNode);

		if ( distSqCurrent > distSqNode )
		{
			pNode->AddNext( group );
			PROFILE_END( "renderOrder" )
			return;
		}
	}

	PROFILE_END( "renderOrder" )
	groups.AddFirst( group );
}
//-------------------------------------------------------------------
//	AddToListOrderedBackToFront
//-------------------------------------------------------------------
void Scene::AddToListOrderedBackToFront(HandleAs<Component, Component_Render>& hCmp, RenderInfo* pRenderInfo, List<ObjectLightGroup>& groups, const math::CameraTransform& camera)
{
	ObjectLightGroup group = CreateLightGroup( pRenderInfo );

	PROFILE_START( "renderOrder" )
	math::Vec3 posCurrent = pRenderInfo->GetTransform().GetPos();
	float distSqCurrent = math::distSq(camera.pos, posCurrent);

	#ifdef DEBUG_MODE
		group.entityId = hCmp->GetOwner()->GetId();
	#endif

	for ( auto pNode = groups.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		math::Vec3 posNode = pNode->GetValue().pRenderInfo->GetTransform().GetPos();
		float distSqNode = math::distSq(camera.pos, posNode);

		if ( distSqCurrent > distSqNode )
		{
			pNode->AddPrev( group );
			PROFILE_END( "renderOrder" )
			return;
		}
	}

	PROFILE_END( "renderOrder" )
	groups.AddLast( group );
}
//-------------------------------------------------------------------
//	CreateLightGroup
//-------------------------------------------------------------------
Scene::ObjectLightGroup Scene::CreateLightGroup(RenderInfo* pRenderInfo)
{
	ObjectLightGroup result;
	result.pRenderInfo = pRenderInfo;
	result.pointLights = new List<SceneLight<PointLight>>();
	result.spotLights = new List<SceneLight<SpotLight>>();

	if ( !m_allowLight )
		return result;

	PROFILE_START( "lightGroupPoint" );
	for ( auto it = m_pLightMngr->GetAllPointLights(); it; it++ )
	{
		if ( !(*it)->enabled )
			continue;

		if ( !math::intersectAabb3( (*it)->GetAabbGlobal(), pRenderInfo->GetAabbGlobal() ).collision )
			continue;

		SceneLight<PointLight> sceneLight;
		sceneLight.pLight = (*it).GetPtr();
		result.pointLights->AddLast( sceneLight );
	}
	PROFILE_END( "lightGroupPoint" );

	PROFILE_START( "lightGroupSpot" );
	static uint spots;
	spots = 0;
	for ( auto it = m_pLightMngr->GetAllSpotLights(); it; it++ )
	{
		if ( !(*it)->enabled )
			continue;

		if ( !math::intersectAabb3( (*it)->GetAabbGlobal(), pRenderInfo->GetAabbGlobal() ).collision )
			continue;

		SceneLight<SpotLight> sceneLight;
		sceneLight.pLight = (*it).GetPtr();
		sceneLight.pShadowCache = CreateShadowCache( (*it)->GetId() );

		result.spotLights->AddLast( sceneLight );
		spots++;
	}
	PROFILE_END( "lightGroupSpot" );

	return result;
}
//-------------------------------------------------------------------
//	CreateShadowCache
//-------------------------------------------------------------------
Scene::ShadowCache* Scene::CreateShadowCache(uint lightId)
{
	//PROFILE_START( "shadowCache" );
	if ( !m_allowShadow )
	{
		//PROFILE_END( "shadowCache" );
		return nullptr;
	}

	SmartPtr<ShadowCache> pResult;
	SpotLight* pLight = m_pLightMngr->GetSpotLight(lightId).GetPtrOrNull();

	if ( pLight && pLight->shadowEnabled )
	{
		pResult = m_shadowCache.Get( lightId );

		if ( !pResult )
		{
			pResult = new ShadowCache;
			pResult->pRenderTargetShadowMap = new RenderTarget;
			pResult->pRenderTargetShadowMap->Initialize( SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, eColorFormat::Rgba_32_Float );
			pResult->objects = new Table<HandleAs<Component, Component_Render>, uint>( 100, tableHashFunc_Uint );
			pResult->objects->Debug_SlotDepthCheck( 10 );

			m_shadowCache.Add( pResult, lightId );
		}

		// Has an object from this cache moved?
		if ( pResult->objects->Count() > 0 )
		{
			for ( auto it = m_dynamicObjects.GetIterator(); it; it++ )
			{
				auto hCmpRender = pResult->objects->Get( (*it).componentId );
				if ( !((*it).component) || (*it).isRemoved )
				{
					pResult->hasRendered = false;
					break;
				}
				else if ( hCmpRender )
				{
					auto pRenderInfo = hCmpRender->GetData();
					if ( pRenderInfo->visible && pRenderInfo->castShadow )
					{
						pResult->hasRendered = false;
						break;
					}
				}
				else if ( math::intersectAabb3( pLight->GetAabbGlobal(), (*it).component->GetData()->GetAabbGlobal() ).collision )
				{
					
					pResult->hasRendered = false;
					break;
				}
			}
		}

		if ( pResult->hasRendered && m_lightTransform.Count() != 0 && m_lightTransform.Find(pLight->GetId()) )
		{
			pResult->hasRendered = false;
			m_lightTransform.Remove( pLight->GetId() );
		}

		if ( !pResult->hasRendered || pResult->objects->Count() == 0 )
		{
			pResult->objects->Clear();
			m_pPartition->GetFromAabb( pLight->GetAabbGlobal(), **pResult->objects, true );
		}
	}

	//PROFILE_END( "shadowCache" );

	return pResult.Get();
}
//-------------------------------------------------------------------
//	ClearShadowCache
//-------------------------------------------------------------------
void Scene::ClearShadowCache()
{
	for ( auto it = m_shadowCache.GetIterator(); it; it++ )
	{
		(*it)->hasRendered = false;
		(*it)->objects->Clear();
	}
}
//-------------------------------------------------------------------
//	ShutdownShadowCache
//-------------------------------------------------------------------
void Scene::ShutdownShadowCache()
{
	for ( auto it = m_shadowCache.GetIterator(); it; it++ )
	{
		if ( (*it)->pRenderTargetShadowMap )
			(*it)->pRenderTargetShadowMap->Shutdown();
	}

	m_shadowCache.Clear();
	m_lightTransform.Clear();
	m_dynamicObjects.Clear();
}
//-------------------------------------------------------------------
//	GetLightMngr
//-------------------------------------------------------------------
LightManager* Scene::GetLightMngr()
{
	return m_pLightMngr.Get();
}
//-------------------------------------------------------------------
//	GetTexture
//-------------------------------------------------------------------
Handle<ITexture> Scene::GetSceneTexture()
{
	return m_pRenderTargetScene->GetTexture();
}

//=============================================================================
//	Debug
//=============================================================================

//-------------------------------------------------------------------
//	Debug_Add
//-------------------------------------------------------------------
Handle<Debug_Line> Scene::Debug_AddLine()
{
	return m_repoDebugLine.Create();
}
//-------------------------------------------------------------------
Handle<Debug_Cube> Scene::Debug_AddCube()
{
	return m_repoDebugCube.Create();
}
//-------------------------------------------------------------------
Handle<Debug_Sphere> Scene::Debug_AddSphere()
{
	return m_repoDebugSphere.Create();
}
//-------------------------------------------------------------------
Handle<Debug_Cylinder> Scene::Debug_AddCylinder()
{
	return m_repoDebugCylinder.Create();
}
//-------------------------------------------------------------------
//	Debug_Remove
void Scene::Debug_Remove(const Handle<Debug_Line>& handle)
{
	m_repoDebugLine.Remove( handle );
}
//-------------------------------------------------------------------
void Scene::Debug_Remove(const Handle<Debug_Cube>& handle)
{
	m_repoDebugCube.Remove( handle );
}
//-------------------------------------------------------------------
void Scene::Debug_Remove(const Handle<Debug_Sphere>& handle)
{
	m_repoDebugSphere.Remove( handle );
}
//-------------------------------------------------------------------
void Scene::Debug_Remove(const Handle<Debug_Cylinder>& handle)
{
	m_repoDebugCylinder.Remove( handle );
}
//-------------------------------------------------------------------
//	Debug_Render
//-------------------------------------------------------------------
void Scene::Debug_Render(const math::CameraTransform& camera)
{
	math::Mtx44 view = camera.view;
	math::Mtx44 proj = camera.proj;

	// Debug_Line
	for ( auto it = m_repoDebugLine.GetIterator(); it; it++ )
	{
		Debug_Line* pLine = (*it).GetPtr();

		if ( !pLine->visible )
			continue;

		float lineLength = math::length( pLine->point1 - pLine->point0 );

		if ( math::isZero(lineLength) )
			continue;

		math::Vec3 lineDir = ( pLine->point1 - pLine->point0 ) / lineLength;
		math::Mtx44 world = math::matrixScale( math::Vec3(lineLength) ) * math::matrixOrthogonalLook(lineDir) * math::matrixTranslate(pLine->point0);

		m_pEffectVertexColor->GetConfig().color4 = pLine->color4;
		m_pEffectVertexColor->Render( m_hLine, world, view, proj );
	}

	// Debug_Cube
	for ( auto it = m_repoDebugCube.GetIterator(); it; it++ )
	{
		Debug_Cube* pCube = (*it).GetPtr();

		if ( !pCube->visible )
			continue;

		math::Mtx44 world = math::matrixScale(pCube->scale) * pCube->rotation * math::matrixTranslate(pCube->pos);

		m_pEffectVertexColor->GetConfig().color4 = pCube->color4;
		m_pEffectVertexColor->Render( m_hCubeLine, world, view, proj );
	}

	// Debug_Sphere
	for ( auto it = m_repoDebugSphere.GetIterator(); it; it++ )
	{
		Debug_Sphere* pSphere = (*it).GetPtr();

		if ( !pSphere->visible )
			continue;

		math::Mtx44 world = math::matrixScale(pSphere->scale) * math::matrixTranslate(pSphere->pos);

		m_pEffectVertexColor->GetConfig().color4 = pSphere->color4;
		m_pEffectVertexColor->Render( m_hSphereLine, world, view, proj );
	}

	// Debug_Cylinder
	for ( auto it = m_repoDebugCylinder.GetIterator(); it; it++ )
	{
		Debug_Cylinder* pCylinder = (*it).GetPtr();

		if ( !pCylinder->visible )
			continue;

		math::Mtx44 world = math::matrixScale( pCylinder->scale ) * math::matrixTranslate( pCylinder->pos );

		m_pEffectVertexColor->GetConfig().color4 = pCylinder->color4;
		m_pEffectVertexColor->Render( m_hCylinder, world, view, proj );
	}
}
//-------------------------------------------------------------------
//	Debug_BreakOnRender
//-------------------------------------------------------------------
void Scene::Debug_BreakOnRender(Handle<Entity>& hEntity)
{
	m_debugBreakOnRender = hEntity ? hEntity->GetId() : 0;
}
//-------------------------------------------------------------------
//	Debug_AddLightPerObject
//-------------------------------------------------------------------
void Scene::Debug_AddLightPerObject(uint lightCount, uint* arrayLightsPerObject, uint arrayCount)
{
	if ( arrayCount > lightCount )
		arrayLightsPerObject[lightCount]++;
}
//-------------------------------------------------------------------
//	Debug_ClearReport
//-------------------------------------------------------------------
void Scene::Debug_ClearReport()
{
	m_debugPointLightRenderCount = 0;
	m_debugSpotLightRenderCount = 0;
	m_debugLightAllRenderCount = 0;
	m_debugOpaqueCount = 0;
	m_debugTransparentCount = 0;
	m_debugLitOpaqueCount = 0;
	m_debugLitOpaqueMax = 0;
	m_debugLitOpaqueAvrg = 0.0f;
	m_debugLitTransparentCount = 0;
	m_debugLitTransparentMax = 0;
	m_debugLitTransparentAvrg = 0.0f;

	memoryWrite( m_debugPointLightsPerObject, ARRAY_COUNT(m_debugPointLightsPerObject), (uint)0 );
	memoryWrite( m_debugSpotLightsPerObject, ARRAY_COUNT(m_debugSpotLightsPerObject), (uint)0 );
}
//-------------------------------------------------------------------
//	Debug_Report
//-------------------------------------------------------------------
String Scene::Debug_Report()
{
	auto printLightPerObject = [](String& r, const uint* lightsPerObject, uint count)
	{
		for ( uint i = 0; i < count; i++ )
		{
			uint value = lightsPerObject[i];
			if ( value > 0 )
				r += "    " + string::intToStr((int)i) + ": " + string::intToStr((int)value)+ "\n";
		}
	};

	String result;

	result += 
		      "ALL LIGHTS PASS COUNT: " + string::intToStr(m_debugLightAllRenderCount) + "\n" +
		      //"POINT PASS COUNT: " + string::intToStr(m_debugPointLightRenderCount) + "\n" +
		      //"SPOT  PASS COUNT: " + string::intToStr(m_debugSpotLightRenderCount) + "\n" +
	       //   "OPAQUES         : " + string::intToStr(m_debugOpaqueCount) + "\n" +
		      //"LIT OPAQUES     : " + string::intToStr(m_debugLitOpaqueCount) + "\n" +
		      //"LIT OPAQUES MAX : " + string::intToStr(m_debugLitOpaqueMax) + "\n" +
		      //"LIT OPAQUES AVRG: " + string::floatToStr(m_debugLitOpaqueAvrg) + "\n" +
		      //"TRANSPARENTS    : " + string::intToStr(m_debugTransparentCount) + "\n" +
		      //"LIT TRANSPARENTS: " + string::intToStr(m_debugLitTransparentCount) + "\n" +
		      //"LIT TRANSPS MAX : " + string::intToStr(m_debugLitTransparentMax) + "\n" +
		      //"LIT TRANSPS AVRG: " + string::floatToStr(m_debugLitTransparentAvrg) + "\n" +
	"";

	//result += "POINT LIGHTS PER OBJECT\n";
	//printLightPerObject( result, m_debugPointLightsPerObject, ARRAY_COUNT(m_debugPointLightsPerObject) );
	//result += "SPOT LIGHTS PER OBJECT\n";
	//printLightPerObject( result, m_debugSpotLightsPerObject, ARRAY_COUNT(m_debugSpotLightsPerObject) );
	
	return result;
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void Scene::HandleEvent(Event* pEvent)
{
	Event_EntityManager* pEventEntityMngr = nullptr;
	Event_Component* pEventComponent = nullptr;
	Event_TransformChange* pEventTransform = nullptr;
	Event_LightTransform* pEventLight = nullptr;

	if ( isEvent<Event_Component>(pEvent, &pEventComponent) )
	{
		if ( pEventComponent->hComponent->GetClassRelationship_IsOrHas<Component_Render>() )
		{
			if ( pEventComponent->action == eComponentAction::Add )
			{
				HandleAs<Component, Component_Render> hCmpRender(pEventComponent->hComponent);
				m_pPartition->Add( hCmpRender );

				DynamicObject dynamicObj;
				dynamicObj.component = hCmpRender;
				dynamicObj.componentId = hCmpRender->GetId();
				m_dynamicObjects.AddIfNotExists( dynamicObj, dynamicObj.componentId );
			}

			else if ( pEventComponent->action == eComponentAction::Remove )
			{
				uint idComponent = pEventComponent->hComponent->GetId();
				m_pPartition->Remove( idComponent );

				DynamicObject dynamicObj;
				dynamicObj.componentId = idComponent;
				dynamicObj.isRemoved = true;
				m_dynamicObjects.AddIfNotExists( dynamicObj, dynamicObj.componentId );
			}
		}
	}
	else if ( isEvent<Event_EntityManager>(pEvent, &pEventEntityMngr) )
	{
		if ( pEventEntityMngr->action == eEventEntity::RemoveAll )
		{
			m_pPartition->Clear();
			m_dynamicObjects.Clear();
			m_lightTransform.Clear();
		}
	}
	else if ( isEvent<Event_TransformChange>(pEvent, &pEventTransform) )
	{
		if ( pEventTransform->flag == eTransformation::Render )
		{
			auto hCmpRender = pEventTransform->hEntity->GetComponentAs<Component_Render>();

			const String& classTag = hCmpRender->GetOwner()->GetClassTag().GetString();

			m_pPartition->OnChange( hCmpRender );

			DynamicObject dynamicObj;
			dynamicObj.componentId = hCmpRender->GetId();
			m_dynamicObjects.AddIfNotExists( dynamicObj, dynamicObj.componentId );
		}
	}
	else if ( isEvent<Event_LightTransform>(pEvent, &pEventLight) )
	{
		m_lightTransform.AddIfNotExists( pEventLight->lightId, pEventLight->lightId );
	}
}
