#pragma once

#include "../engine/Light.h"
#include "../engine/Table.h"
#include "../engine/Class.h"
#include "../engine/String.h"
#include "../engine/List.h"
#include "../engine/Exception.h"
#include "../engine/Handle.h"
#include "../engine/TableHashFunction.h"

class Scene;

class LightManager : public NonCopyable
{
	friend class Scene;

	private:

		template<class LIGHT_TYPE>
		struct LightRepository
		{
			Table<Handle<LIGHT_TYPE>, uint> table;
			HandleManager<LIGHT_TYPE> handles;
			
			LightRepository() : table(50, tableHashFunc_Uint), handles(200) {}
		};

	public:
		~LightManager();

		// Generic light
		Light_Base* GetLightPtr(uint lightId);
		const Light_Base* GetLightPtr(uint lightId)const;

		// Ambient light
		void SetAmbientLight(ushort red, ushort green, ushort blue, ushort alpha = 255) { m_ambientLightColor4 = math::normalizeRgb(red, green ,blue, alpha); }
		void SetAmbientLight(const math::Vec4& ambientColor4) { m_ambientLightColor4 = ambientColor4; }
		const math::Vec4& GetAmbientLight()const { return m_ambientLightColor4; }

		// Point light
		Handle<PointLight> AddPointLight();
		void RemovePointLight(uint idLight);

		const Handle<PointLight> GetPointLight(uint idLight)const;
		Handle<PointLight> GetPointLight(uint idLight);

		TableIteratorConst<Handle<PointLight>> GetAllPointLights()const;

		// Spot light
		Handle<SpotLight> AddSpotLight();
		void RemoveSpotLight(uint idLight);

		const Handle<SpotLight> GetSpotLight(uint idLight)const;
		Handle<SpotLight> GetSpotLight(uint idLight);

		TableIteratorConst<Handle<SpotLight>> GetAllSpotLights()const;

		void Clear();

	private:
		LightManager();
		
		void Initialize();
		void Shutdown();

		template <class LIGHT_TYPE>
		Handle<LIGHT_TYPE> AddLight(LightRepository<LIGHT_TYPE>&);
		template <class LIGHT_TYPE>
		void RemoveLight(LightRepository<LIGHT_TYPE>&, uint idLight);
		template <class LIGHT_TYPE>
		Handle<LIGHT_TYPE> GetLight(LightRepository<LIGHT_TYPE>&, uint idLight);
		template <class LIGHT_TYPE>
		const Handle<LIGHT_TYPE> GetLight(const LightRepository<LIGHT_TYPE>&, uint idLight)const;

		bool m_initialized;
		math::Vec4 m_ambientLightColor4;

		Table<Light_Base*, uint> m_allLights;
		LightRepository<PointLight> m_repoPointLight;
		LightRepository<SpotLight> m_repoSpotLight;
};

//=============================================================================
//	LightManager template definition
//=============================================================================

//-------------------------------------------------------------------
//	AddLight
//-------------------------------------------------------------------
template <class LIGHT_TYPE>
inline Handle<LIGHT_TYPE> LightManager::AddLight(LightRepository<LIGHT_TYPE>& repo)
{
	Handle<LIGHT_TYPE> hLight = repo.handles.Create();
	repo.table.Add( hLight, hLight->GetId() );

	auto pLight = (Light_Base*)hLight.GetPtr();
	m_allLights.Add( pLight, hLight->GetId() );

	return hLight;
}
//-------------------------------------------------------------------
//	RemoveLight
//-------------------------------------------------------------------
template <class LIGHT_TYPE>
inline void LightManager::RemoveLight(LightRepository<LIGHT_TYPE>& repo, uint idLight)
{
	Handle<LIGHT_TYPE> hLight = GetLight(repo, idLight);

	if ( hLight )
	{
		m_allLights.Remove( hLight->GetId() );
		repo.handles.Remove( hLight );
		repo.table.Remove( idLight );
	}
}
//-------------------------------------------------------------------
//	GetLight
//-------------------------------------------------------------------
template <class LIGHT_TYPE>
inline Handle<LIGHT_TYPE> LightManager::GetLight(LightRepository<LIGHT_TYPE>& repo, uint idLight)
{
	return repo.table.Get( idLight );
}
//-------------------------------------------------------------------
template <class LIGHT_TYPE>
inline const Handle<LIGHT_TYPE> LightManager::GetLight(const LightRepository<LIGHT_TYPE>& repo, uint idLight)const
{
	return repo.table.Get( idLight );
}
