#pragma once

#include "Type.h"
#include "../engine/Math.h"
#include "../engine/Pointer.h"
#include "../engine/EngineGlobal.h"
#include "RenderUtil.h"
#include "../engine/Handle.h"

/****************************************************************************************
	Forward declaration
*****************************************************************************************/

template <class>
class Effect_Base;

class Model;
class ITexture;
class RenderTarget;

class Shader_Blur;
class Shader_Texture;
class Shader_MultiTexture;
class Shader_OneColor;
class Shader_Color;
class Shader_LightDir4;
class Shader_LightDir4Untex;
class Shader_Text2d;
class Shader_DepthAmbientEmissive;
class Shader_PointLight;
class Shader_PointLightArray;
class Shader_SpotLight;
class Shader_ShadowMap;
class Shader_Refraction;
class Shader_LightAll;
class SamplerState;

/****************************************************************************************
	Struct
*****************************************************************************************/

//-------------------------------------------------------------------
//	EffectProp_Material
//-------------------------------------------------------------------
struct EffectProp_Material
{
	math::Vec4 emissiveColor4;
	math::Vec4 ambientColor4;
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float specularPower;
	Handle<ITexture> hDiffuseTexture;
	Handle<ITexture> hSpecularTexture;
	Handle<ITexture> hNormalMap;
	math::Mtx33 textureTransform;
	TextureSampler sampler;

	EffectProp_Material() : emissiveColor4(math::Vec3(0.0f), 1.0f), ambientColor4(math::Vec3(0.1f), 1.0f), diffuseColor4(math::Vec3(0.7f), 1.0f),  
		specularPower(128.0f)
	{
		sampler.filter = eTextureFilter::Anisotropic;
	}
};
//-------------------------------------------------------------------
//	EffectProp_LightTransform
//-------------------------------------------------------------------
struct EffectProp_LightTransform
{
	math::Mtx44 view;
	math::Mtx44 proj;

	void Clear()
	{
		view = math::Mtx44::identity;
		proj = math::Mtx44::identity;
	};
};
//-------------------------------------------------------------------
//	EffectProp_PointLight
//-------------------------------------------------------------------
struct EffectProp_PointLight
{
	math::Vec3 pos;
	float range;
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float attenuation0;
	float attenuation1;
	float attenuation2;

	EffectProp_PointLight() : range(5.0f), diffuseColor4(math::Vec3(0.7f), 1.0f), specularColor4(math::Vec3(0.5f), 1.0f),
		attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f) {}

	void Clear()
	{
		pos = math::Vec3::zero;
		range = 0.0f;
		diffuseColor4 = math::Vec4::zero;
		specularColor4 = math::Vec4::zero;
		attenuation0 = 0.0f;
		attenuation1 = 0.0f;
		attenuation2 = 0.0f;
	};
};
//-------------------------------------------------------------------
//	EffectProp_SpotLight
//-------------------------------------------------------------------
struct EffectProp_SpotLight
{
	math::Vec3 pos;
	math::Vec3 direction;
	float range;
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float innerConeDegs;
	float outerConeDegs;
	float attenuation0;
	float attenuation1;
	float attenuation2;
	Handle<ITexture> hShadowMap;
	bool shadowEnabled = true;
	float shadowBias = 0.00001f;
	float distanceAdjustment = 0.0f;
	EffectProp_LightTransform transform;

	EffectProp_SpotLight() : direction( math::normalize(1.0f, -1.0f, 1.0f) ), range(5.0f), innerConeDegs(60.0f), outerConeDegs(20.0f),
		diffuseColor4(math::Vec3(0.7f), 1.0f), specularColor4(math::Vec3(0.5f), 1.0f), 
		attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f) {}

	void Clear()
	{
		pos = math::Vec3::zero;
		direction = math::Vec3::zero;
		range = 0.0f;
		diffuseColor4 = math::Vec4::zero;
		specularColor4 = math::Vec4::zero;
		innerConeDegs = 0.0f;
		outerConeDegs = 0.0f;
		attenuation0 = 0.0f;
		attenuation1 = 0.0f;
		attenuation2 = 0.0f;
		shadowEnabled = false;
		shadowBias = 0.0f;
		distanceAdjustment = 0.0f;

		hShadowMap.Invalidate();
		transform.Clear();
	};
};
//-------------------------------------------------------------------
//	EffectConfig_Renderer
//-------------------------------------------------------------------
struct EffectConfig_Renderer
{
	bool wireframe;
	eBlendMode blendMode;
	eDepthOperation depthOperation;
	eCullMode cullMode;
	bool enableDepthWrite;

	EffectConfig_Renderer() : wireframe(false), blendMode(eBlendMode::Disabled), depthOperation(eDepthOperation::Less), cullMode(eCullMode::Back), enableDepthWrite(true) {}
};
//-------------------------------------------------------------------
//	EffectConfig_Base
//-------------------------------------------------------------------
struct EffectConfig_Base
{
	EffectConfig_Renderer renderer;
};
//-------------------------------------------------------------------
//	EffectConfig_Blur
//-------------------------------------------------------------------
struct EffectConfig_Blur : public EffectConfig_Base
{
	friend class Effect_Blur;

	float displace;
	float baseWeight;
	math::Vec2 blurDirPass0;
	math::Vec2 blurDirPass1;
	Handle<ITexture> hTexture;
	math::Mtx33 textureTransform;

	EffectConfig_Blur() : displace(0.0f), baseWeight(0.0f), weightCount(0) 
	{
		uint weightCount = ARRAY_COUNT(weights);

		for ( uint i = 0; i < weightCount; i++ )
			weights[i] = 1.0f / weightCount;
	}

	void SetWeights(const float*, uint count);

	private:
	float weights[engineDefault::shader::BLUR_WEIGHT_ARRAY_MAX];
	uint weightCount;
};
//-------------------------------------------------------------------
//	EffectConfig_Texture
//-------------------------------------------------------------------
struct EffectConfig_Texture : public EffectConfig_Base
{
	Handle<ITexture> hTexture;
	math::Mtx33 textureTransform;
	math::Vec4 color4;

	EffectConfig_Texture() : color4(1.0f) {}
};
//-------------------------------------------------------------------
//	EffectConfig_Blur
//-------------------------------------------------------------------
struct EffectConfig_OneColor : public EffectConfig_Base
{
	math::Vec4 color4;
	float depthBias;

	EffectConfig_OneColor() : depthBias(0.0f) {}
};
//-------------------------------------------------------------------
//	EffectConfig_VertexColor
//-------------------------------------------------------------------
struct EffectConfig_VertexColor : public EffectConfig_Base
{
	math::Vec4 color4;
};
//-------------------------------------------------------------------
//	EffectConfig_LightDir4
//-------------------------------------------------------------------
struct EffectConfig_LightDir4 : public EffectConfig_Base
{
	public:
		math::Vec4 ambientColor4;
		math::Vec4 diffuseColor4;
		TextureSampler sampler;
		Handle<ITexture> hTexture;
		math::Mtx33 textureTranslate;

		EffectConfig_LightDir4() {}

		void SetLightDir(uint index, const math::Vec3& lightDir);
		void SetLightColor(uint index, const math::Vec4& lightColor);

		math::Vec3 GetLightDir(uint index)const;
		math::Vec4 GetLightColor(uint index)const;

		uint GetLightDirCount()const { return ARRAY_COUNT(lightDirs); }
		uint GetLightColorCount()const { return ARRAY_COUNT(lightColors); }

	private:
		math::Vec3 lightDirs[4];
		math::Vec4 lightColors[4];
};
//-------------------------------------------------------------------
//	EffectConfig_Text2d
//-------------------------------------------------------------------
struct EffectConfig_Text2d : public EffectConfig_Base
{
	math::Vec4 color4;
	Handle<ITexture> hTexture;
	TextureSampler sampler;
	math::Mtx33 textureTransform;

	EffectConfig_Text2d() : color4(1.0f) {}
};
//-------------------------------------------------------------------
//	EffectConfig_DepthEmissive
//-------------------------------------------------------------------
struct EffectConfig_DepthEmissive : public EffectConfig_Base
{
	EffectProp_Material material;
	math::Vec4 ambientLightColor4;

	EffectConfig_DepthEmissive() : ambientLightColor4(0.1f) {}
};
//-------------------------------------------------------------------
//	EffectConfig_PointLight
//-------------------------------------------------------------------
struct EffectConfig_PointLight : public EffectConfig_Base
{
	EffectProp_Material material;
	EffectProp_PointLight lights[engineDefault::shader::MAX_LIGHTS];
	math::Vec3 cameraPos;
};
//-------------------------------------------------------------------
//	EffectConfig_SpotLight
//-------------------------------------------------------------------
struct EffectConfig_SpotLight : public EffectConfig_Base
{
	EffectProp_Material material;
	EffectProp_SpotLight lights[engineDefault::shader::MAX_LIGHTS];
	math::Vec3 cameraPos;
};
//-------------------------------------------------------------------
//	EffectConfig_LightAll
//-------------------------------------------------------------------
struct EffectConfig_LightAll : public EffectConfig_Base
{
	EffectProp_Material material;
	EffectProp_PointLight pointLights[engineDefault::shader::MAX_LIGHTS];
	EffectProp_SpotLight spotLights[engineDefault::shader::MAX_LIGHTS];
	math::Vec3 cameraPos;
};
//-------------------------------------------------------------------
//	EffectConfig_ShadowMap
//-------------------------------------------------------------------
struct EffectConfig_ShadowMap : public EffectConfig_Base
{
	EffectProp_LightTransform lightTransform;
};
//-------------------------------------------------------------------
//	EffectConfig_Refraction
//-------------------------------------------------------------------
struct EffectConfig_Refraction : public EffectConfig_Base
{
	Handle<ITexture> hNormalMap;
	Handle<ITexture> hRefractionMap;
	math::Vec3 color3;
	float refractionIntensity;
	float colorSaturation;
	float colorSeparation;
	math::Mtx33 textureTransform;
	TextureSampler sampler;

	EffectConfig_Refraction() : color3(1.0f), refractionIntensity(0.2f), colorSaturation(0.0f), colorSeparation(0.0f) {}
};

/****************************************************************************************
	Effect class declaration
*****************************************************************************************/

//=============================================================================
//	IEffect
//=============================================================================

class IEffect
{
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
};

//=============================================================================
//	Effect_Base
//=============================================================================

class Effect_Base_Impl
{
	template <class>
	friend class Effect_Base;

	private:
		void ApplyRendererConfig(const EffectConfig_Renderer&);
};

template <class CONFIG_BASE>
class Effect_Base : public IEffect
{
	public:
		CONFIG_BASE& GetConfig() { return m_config; }
		const CONFIG_BASE& GetConfig()const { return m_config; }

		void Initialize()override final
		{
			if ( m_initialized )
			{
				ASSERT( "Already initialized" );
				return;
			}

			OnInitialize();

			m_initialized = true;
		}

		void Shutdown()override final
		{
			if ( !m_initialized )
			{
				ASSERT( "Not initialized" );
				return;
			}

			OnShutdown();

			m_initialized = false;
		}

	protected:
		Effect_Base() { m_initialized = false; }
		virtual ~Effect_Base() { CHECK(!m_initialized); }

		void ApplyRendererConfig() { m_impl.ApplyRendererConfig(m_config.renderer); }

		virtual void OnInitialize() = 0;
		virtual void OnShutdown() = 0;

		CONFIG_BASE m_config;

	private:
		bool m_initialized;
		Effect_Base_Impl m_impl;
};

//=============================================================================
//	Effect_Blur
//=============================================================================

class Effect_Blur : public Effect_Base<EffectConfig_Blur>
{
	public:
		virtual ~Effect_Blur() = default;

		void SetRenderTargetDimension(uint width, uint height);
		void SetRenderTargetDimension(const math::Vec2&);
		void SetRenderTargetProportion(float);
		void Render(RenderTarget*, const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<RenderTarget> m_pTargetBlurPass;
		SimplePtr<Shader_Blur> m_pShaderBlur;
};

//=============================================================================
//	Effect_Texture
//=============================================================================

class Effect_Texture : public Effect_Base<EffectConfig_Texture>
{
	public:
		virtual ~Effect_Texture() = default;

		void Render(const Handle<Model>&, const math::Mtx44 world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_Texture> m_pShaderTexture;
};

//=============================================================================
//	Effect_Color
//=============================================================================

class Effect_Color : public Effect_Base<EffectConfig_OneColor>
{
	public:
		virtual ~Effect_Color() = default;
		
		void Render(const Handle<Model>&, const math::Mtx44 world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_OneColor> m_pShaderOneColor;
};

//=============================================================================
//	Effect_VertexColor
//=============================================================================

class Effect_VertexColor : public Effect_Base<EffectConfig_VertexColor>
{
	public:
		virtual ~Effect_VertexColor() = default;

		void Render(const Handle<Model>&, const math::Mtx44 world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_Color> m_pShaderColor;
};

//=============================================================================
//	Effect_LightDir4
//=============================================================================

class Effect_LightDir4 : public Effect_Base<EffectConfig_LightDir4>
{
	public:
		virtual ~Effect_LightDir4() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		
	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_LightDir4> m_pShader;
		SimplePtr<Shader_LightDir4Untex> m_pShaderUntex;
};

//=============================================================================
//	Effect_Text2d
//=============================================================================

class Effect_Text2d : public Effect_Base<EffectConfig_Text2d>
{
	public:
		virtual ~Effect_Text2d() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_Text2d> m_pShader;
};

//=============================================================================
//	Effect_DepthAmbientEmissive
//=============================================================================

class Effect_DepthAmbientEmissive : public Effect_Base<EffectConfig_DepthEmissive>
{
	public:
		virtual ~Effect_DepthAmbientEmissive() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_DepthAmbientEmissive> m_pShader;
};

//=============================================================================
//	Effect_PointLight
//=============================================================================

class Effect_PointLight : public Effect_Base<EffectConfig_PointLight>
{
	public:
		virtual ~Effect_PointLight() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

		uint GetMaxLights()const { return ARRAY_COUNT(GetConfig().lights); }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_PointLightArray> m_pShader;
};

//=============================================================================
//	Effect_SpotLight
//=============================================================================

class Effect_SpotLight : public Effect_Base<EffectConfig_SpotLight>
{
	public:
		virtual ~Effect_SpotLight() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

		uint GetMaxLights()const { return ARRAY_COUNT(GetConfig().lights); }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_SpotLight> m_pShader;
};

//=============================================================================
//	Effect_LightAll
//=============================================================================

class Effect_LightAll : public Effect_Base<EffectConfig_LightAll>
{
	public:
		virtual ~Effect_LightAll() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

		uint GetMaxLights()const { return ARRAY_COUNT(GetConfig().pointLights); }

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_LightAll> m_pShader;
};


//=============================================================================
//	Effect_ShadowMap
//=============================================================================

class Effect_ShadowMap : public Effect_Base<EffectConfig_ShadowMap>
{
	public:
		virtual ~Effect_ShadowMap() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_ShadowMap> m_pShader;
};

//=============================================================================
//	Effect_Refraction
//=============================================================================

class Effect_Refraction : public Effect_Base<EffectConfig_Refraction>
{
	public:
		virtual ~Effect_Refraction() = default;

		void Render(const Handle<Model>&, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);

	protected:
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		SimplePtr<Shader_Refraction> m_pShader;
};
