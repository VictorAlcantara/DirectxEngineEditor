#pragma once

#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/Class.h"
#include "LightManager.h"
#include "CoreModule.h"
#include "../engine/RenderTarget.h"
#include "../engine/Table.h"
#include "DebugRender.h"
#include "HandleRepository.h"
#include "../engine/EventListener.h"

class PartitionScene;
class Model;
class Entity;
class ITexture;
class Effect_DepthAmbientEmissive;
class Effect_PointLight;
class Effect_SpotLight;
struct EffectProp_Material;
struct RenderInfo;
class Effect_ShadowMap;
class Effect_VertexColor;
class Effect_Refraction;
class Component;
class Component_Render;
class Effect_LightAll;
struct EffectProp_PointLight;
struct EffectProp_SpotLight;

class Scene : public NonCopyable, public EventListener
{
	CORE_MODULE;

	private:
		struct SceneRenderInfo
		{
			bool writeToDepth = false;
			eDepthOperation depthOperation = eDepthOperation::Less;
		};

		struct ShadowCache
		{
			SmartPtr<RenderTarget> pRenderTargetShadowMap;
			SmartPtr<Table<HandleAs<Component, Component_Render>, uint>> objects;
			bool hasRendered = false;
		};

		template <class LIGHT_TYPE>
		struct SceneLight
		{
			const LIGHT_TYPE* pLight = nullptr;
			ShadowCache* pShadowCache = nullptr;
		};

		struct ObjectLightGroup
		{
			uint entityId = 0;
			RenderInfo* pRenderInfo = nullptr;
			SmartPtr<List<SceneLight<PointLight>>> pointLights;
			SmartPtr<List<SceneLight<SpotLight>>> spotLights;
		};

		struct DynamicObject
		{
			bool isRemoved = false;
			uint componentId = 0;
			HandleAs<Component, Component_Render> component;
		};

	public:
		~Scene();

		void Initialize();
		void Shutdown();

		LightManager* GetLightMngr();
		
		void RenderToTexture(const math::CameraTransform&);

		Handle<ITexture> GetSceneTexture();

		void SetClearColor(float x, float y, float z, float w = 1.0f) { m_clearColor = math::Vec4(x, y, z, w); }
		void Clear();

		void CreatePartition(const math::Vec3& pos, const math::Vec3& scale);

		void EnableLights(bool);
		void EnableShadow(bool);

		// Debug
		Handle<Debug_Line> Debug_AddLine();
		Handle<Debug_Cube> Debug_AddCube();
		Handle<Debug_Sphere> Debug_AddSphere();
		Handle<Debug_Cylinder> Debug_AddCylinder();
		void Debug_Remove(const Handle<Debug_Line>&);
		void Debug_Remove(const Handle<Debug_Cube>&);
		void Debug_Remove(const Handle<Debug_Sphere>&);
		void Debug_Remove(const Handle<Debug_Cylinder>&);
		void Debug_BreakOnRender(Handle<Entity>&);
		void Debug_ClearReport();
		void Debug_AddLightPerObject(uint lightCount, uint* arrayLightsPerObject, uint arrayCount);
		String Debug_Report();

		void HandleEvent(Event*)override final;

	private:
		Scene();

		void Render_Opaque(List<ObjectLightGroup>&, const math::CameraTransform&);
		void Render_Translucent(List<ObjectLightGroup>&, const math::CameraTransform&);

		void Pass_DepthEmissiveAmbient(const RenderInfo*, const SceneRenderInfo&, const math::CameraTransform&);
		void Pass_Light(ObjectLightGroup&, const SceneRenderInfo&, const math::CameraTransform&);
		void Pass_PointLight(ObjectLightGroup&, const math::CameraTransform&);
		void Pass_SpotLight(ObjectLightGroup&, const math::CameraTransform&);
		void Pass_LightAll(ObjectLightGroup&, const math::CameraTransform&);
		void Pass_Refraction(const RenderInfo*, const SceneRenderInfo&, const math::CameraTransform&);
		void Pass_ShadowMap(const RenderInfo*, const math::Mtx44& lightView, const math::Mtx44& lightProj);
		void Debug_Render(const math::CameraTransform& camera);

		ObjectLightGroup CreateLightGroup(RenderInfo*);
		ShadowCache* CreateShadowCache(uint lightId);
		void ClearShadowCache();
		void ShutdownShadowCache();

		void SetPointLightOnEffectProp(const RenderInfo*, const SceneLight<PointLight>*, EffectProp_PointLight*);
		void SetSpotLightOnEffectProp(const RenderInfo*, const SceneLight<SpotLight>*, EffectProp_SpotLight*);
		void SetMaterialOnEffect(EffectProp_Material*, const RenderInfo*);

		void AddToListOrderedFrontToBack(HandleAs<Component, Component_Render>&, RenderInfo*, List<ObjectLightGroup>&, const math::CameraTransform&);
		void AddToListOrderedBackToFront(HandleAs<Component, Component_Render>&, RenderInfo*, List<ObjectLightGroup>&, const math::CameraTransform&);

		bool m_initialized;
		math::Vec4 m_clearColor;
		bool m_allowShadow;
		bool m_allowLight;
		SimplePtr<PartitionScene> m_pPartition;
		Table<HandleAs<Component, Component_Render>, uint> m_toRenderTable;

		Handle<ITexture> m_hTextureWhite;
		Handle<ITexture> m_hTextureBlack;
		Handle<ITexture> m_hTexturePink;

		SimplePtr<LightManager> m_pLightMngr;

		SimplePtr<RenderTarget> m_pRenderTargetScene;
		Handle<ITexture> m_hRenderTargetCopy;
		SimplePtr<Effect_DepthAmbientEmissive> m_pEffectDepthEmissive;
		SimplePtr<Effect_PointLight> m_pEffectPointLight;
		SimplePtr<Effect_SpotLight> m_pEffectSpotLight;
		SimplePtr<Effect_LightAll> m_pEffectLightAll;
		SimplePtr<Effect_Refraction> m_pRefraction;

		Table<SmartPtr<ShadowCache>, uint> m_shadowCache;
		SimplePtr<Effect_ShadowMap> m_pEffectShadowMap;
		Table<DynamicObject, uint> m_dynamicObjects;
		Table<uint, uint> m_lightTransform;

		// Debug
		HandleRepository_Id<Debug_Line> m_repoDebugLine;
		HandleRepository_Id<Debug_Cube> m_repoDebugCube;
		HandleRepository_Id<Debug_Sphere> m_repoDebugSphere;
		HandleRepository_Id<Debug_Cylinder> m_repoDebugCylinder;
		uint m_debugBreakOnRender;
		uint m_debugPointLightsPerObject[10];
		uint m_debugSpotLightsPerObject[10];

		SimplePtr<Effect_VertexColor> m_pEffectVertexColor;
		Handle<Model> m_hSphereLine;
		Handle<Model> m_hCubeLine;
		Handle<Model> m_hLine;
		Handle<Model> m_hCylinder;

		uint m_debugPointLightRenderCount;
		uint m_debugSpotLightRenderCount;
		uint m_debugLightAllRenderCount;
		uint m_debugOpaqueCount;
		uint m_debugTransparentCount;
		uint m_debugLitOpaqueCount;
		uint m_debugLitOpaqueMax;
		float m_debugLitOpaqueAvrg;
		uint m_debugLitTransparentCount;
		uint m_debugLitTransparentMax;
		float m_debugLitTransparentAvrg;
};
