#include "LightManager.h"
#include "../engine/TableHashFunction.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
LightManager::LightManager() : m_allLights(1000, tableHashFunc_Uint)
{
	m_initialized = false;
}
//-------------------------------------------------------------------
LightManager::~LightManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void LightManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	Clear();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void LightManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_ambientLightColor4 = math::Vec3(0.0f).Xyzw(1.0f);
	m_initialized = true;
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void LightManager::Clear()
{
	m_allLights.Clear();

	m_repoSpotLight.handles.Clear();
	m_repoSpotLight.table.Clear();

	m_repoPointLight.handles.Clear();
	m_repoPointLight.table.Clear();
}

//=============================================================================
//	Generic light
//=============================================================================

//-------------------------------------------------------------------
//	GetLightPtr
//-------------------------------------------------------------------
Light_Base* LightManager::GetLightPtr(uint lightId)
{
	auto pResult = m_allLights.Find( lightId );
	return pResult ? *pResult : nullptr;
}
//-------------------------------------------------------------------
const Light_Base* LightManager::GetLightPtr(uint lightId)const
{
	auto pResult = m_allLights.Find( lightId );
	return pResult ? *pResult : nullptr;
}

//=============================================================================
//	PointLight
//=============================================================================

//-------------------------------------------------------------------
//	AddPointLight
//-------------------------------------------------------------------
Handle<PointLight> LightManager::AddPointLight()
{
	return AddLight( m_repoPointLight );
}
//-------------------------------------------------------------------
//	RemovePointLight
//-------------------------------------------------------------------
void LightManager::RemovePointLight(uint idLight)
{
	RemoveLight( m_repoPointLight, idLight );
}
//-------------------------------------------------------------------
//	GetPointLight
//-------------------------------------------------------------------
Handle<PointLight> LightManager::GetPointLight(uint idLight)
{
	return GetLight( m_repoPointLight, idLight );
}
//-------------------------------------------------------------------
const Handle<PointLight> LightManager::GetPointLight(uint idLight)const
{
	return GetLight( m_repoPointLight, idLight );
}
//-------------------------------------------------------------------
//	GetAllPointLights
//-------------------------------------------------------------------
TableIteratorConst<Handle<PointLight>> LightManager::GetAllPointLights()const
{
	return m_repoPointLight.table.GetIterator();
}

//=============================================================================
//	SpotLight
//=============================================================================

//-------------------------------------------------------------------
//	AddSpotLight
//-------------------------------------------------------------------
Handle<SpotLight> LightManager::AddSpotLight()
{
	return AddLight( m_repoSpotLight );
}
//-------------------------------------------------------------------
//	RemoveSpotLight
//-------------------------------------------------------------------
void LightManager::RemoveSpotLight(uint idLight)
{
	RemoveLight( m_repoSpotLight, idLight );
}
//-------------------------------------------------------------------
//	GetSpotLight
//-------------------------------------------------------------------
Handle<SpotLight> LightManager::GetSpotLight(uint idLight)
{
	return GetLight( m_repoSpotLight, idLight );
}
//-------------------------------------------------------------------
const Handle<SpotLight> LightManager::GetSpotLight(uint idLight)const
{
	return GetLight( m_repoSpotLight, idLight );
}
//-------------------------------------------------------------------
//	GetAllSpotLights
//-------------------------------------------------------------------
TableIteratorConst<Handle<SpotLight>> LightManager::GetAllSpotLights()const
{
	return m_repoSpotLight.table.GetIterator();
}
