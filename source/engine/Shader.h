#pragma once

#include "Platform.h"
#include "../engine/Class.h"
#include "Matrix.h"
#include "../engine/String.h"
#include "ConstantBuffer.h"
#include "../engine/List.h"
#include "../engine/String.h"
#include "../engine/Vertex.h"

class ITexture;

// TODO: the shader's setters are horrible. They should be on the constant buffer struct

//=============================================================================
//	Shader base class
//=============================================================================

class Shader : public NonCopyable
{
	public:
		virtual ~Shader();

		void Initialize();
		void Shutdown();
		virtual void Use() = 0;
		virtual void EndUse();

		platform::ShaderApi& GetApi();

		const VertexLayout& GetVertexLayout()const;

	protected:
		Shader(const StringW& vsFile, const StringW& psFile, const VertexLayout*);

		void Register(IConstantBuffer*);

	private:
		Shader() = delete;

		bool m_initialized;
		platform::ShaderApi m_shaderApi;
		StringW m_vsFile;
		StringW m_psFile;
		const VertexLayout* m_pVertexLayout;
		List<IConstantBuffer*> m_constantBuffers;
};

/****************************************************************************************
	Shader declaration
*****************************************************************************************/

//=============================================================================
//	Shader_Color
//=============================================================================

class Shader_Color : public Shader
{
	public:
		Shader_Color();
		virtual ~Shader_Color() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetColor(const math::Vec4&);

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ConstantBuffer<ConstantBuffer_Color> m_cbColor;
};

//=============================================================================
//	Shader_Texture
//=============================================================================

class Shader_Texture : public Shader
{
	public:
		Shader_Texture();
		virtual ~Shader_Texture() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetTexture(ITexture*);
		void SetTextureTransform(const math::Mtx33&);
		void SetColor(const math::Vec4&);

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_Color> m_cbColor;
		ITexture* m_pTexture;
};

//=============================================================================
//	Shader_OneColor
//=============================================================================

class Shader_OneColor : public Shader
{
	public:
		Shader_OneColor();
		virtual ~Shader_OneColor() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetColor(float red, float green, float blue, float alpha);
		void SetColor(const math::Vec4&);
		void SetDepthBias(float);

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ConstantBuffer<ConstantBuffer_DepthBias> m_cbDepthBias;
		ConstantBuffer<ConstantBuffer_Color> m_cbColor;
};

//=============================================================================
//	Shader_ColorLightDir
//=============================================================================

class Shader_ColorLightDir : public Shader
{
	public:
		Shader_ColorLightDir();
		virtual ~Shader_ColorLightDir() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetAmbient(const math::Vec4&);
		void SetDiffuse(const math::Vec4&);
		void SetLightDirection(const math::Vec3&);

	private:
		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_ColorLightDir> m_cbLight;
};

//=============================================================================
//	Shader_Blur
//=============================================================================

class Shader_Blur : public Shader
{
	public:
		Shader_Blur();
		virtual ~Shader_Blur() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetBlurDir(const math::Vec2&);
		void SetBlurDir(float x, float y);
		void SetBlurDisplace(float xy);
		void SetBlurDisplace(const math::Vec2&);
		void SetBlurDisplace(float x, float y);
		void SetWeights(const float*, uint count, bool normalize);
		void SetBaseWeight(float);
		void SetTextureTransform(const math::Mtx33&);
		void SetTargetDimension(const math::Vec2&);

		void SetTexture(ITexture*);

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_Blur> m_cbBlur;
		ITexture* m_pTexture;
};

//=============================================================================
//	Shader_VertexPosTexture
//=============================================================================

class Shader_VertexPosTexture : public Shader
{
	public:
		Shader_VertexPosTexture();
		virtual ~Shader_VertexPosTexture() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetTexture(ITexture*);

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ITexture* m_pTexture;
};

//=============================================================================
//	Shader_MultiTexture
//=============================================================================

class Shader_MultiTexture : public Shader
{
	public:
		Shader_MultiTexture();
		virtual ~Shader_MultiTexture() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void AddTexture(ITexture*, float weight);
		void ClearTexture();
		void NormalizeWeights();

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ConstantBuffer<ConstantBuffer_MultiTexture> m_cbMultiTexture;
		ITexture* m_textures[engineDefault::shader::MULTI_TEXTURE_ARRAY_COUNT];
};

//=============================================================================
//	Shader_LightDir4
//=============================================================================

class Shader_LightDir4 : public Shader
{
	public:
		Shader_LightDir4();
		virtual ~Shader_LightDir4() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetLightDir(uint index, const math::Vec3& lightDir);
		void SetLightColor(uint index, const math::Vec4&);
		void SetAmbient(const math::Vec4&);
		void SetDiffuse(const math::Vec4&);
		void SetTexture(ITexture*);
		void SetTextureTransform(const math::Mtx33&);

	private:
		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_LightDir4> m_cbLightDir4;
		ConstantBuffer<ConstantBuffer_LightDir4Material> m_cbMaterial;
		ITexture* m_pTexture;
};

//=============================================================================
//	Shader_LightDir4Untex
//=============================================================================

class Shader_LightDir4Untex : public Shader
{
	public:
		Shader_LightDir4Untex();
		virtual ~Shader_LightDir4Untex() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetLightDir(uint index, const math::Vec3& lightDir);
		void SetLightColor(uint index, const math::Vec4&);
		void SetAmbient(const math::Vec4&);
		void SetDiffuse(const math::Vec4&);

	private:
		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_LightDir4> m_cbLightDir4;
		ConstantBuffer<ConstantBuffer_LightDir4Material> m_cbMaterial;
};

//=============================================================================
//	Shader_Text2d
//=============================================================================

class Shader_Text2d : public Shader
{
	public:
		Shader_Text2d();
		virtual ~Shader_Text2d() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetTextureTransform(const math::Mtx33&);
		void SetColor(const math::Vec4&);
		void SetTexture(ITexture*);

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;
		ConstantBuffer<ConstantBuffer_Color> m_cbColor;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ITexture* m_pTexture;
};

//=============================================================================
//	Shader_DepthAmbientEmissive
//=============================================================================

class Shader_DepthAmbientEmissive : public Shader
{
	public:
		Shader_DepthAmbientEmissive();
		virtual ~Shader_DepthAmbientEmissive() = default;

		void Use()override final;
		void SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj);
		void SetTexture(ITexture*);
		void SetTextureTransform(const math::Mtx33&);
		ConstantBuffer_Material& GetMaterial();
		ConstantBuffer_AmbientLight& GetAmbientLight();

	private:
		ConstantBuffer<ConstantBuffer_Wvp> m_cbWvp;		
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_Material> m_cbMaterial;
		ConstantBuffer<ConstantBuffer_AmbientLight> m_cbAmbientLight;
		ITexture* m_pTexture;
};

//=============================================================================
//	Shader_PointLight
//=============================================================================

class Shader_PointLight : public Shader
{
	public:
		Shader_PointLight();
		virtual ~Shader_PointLight() = default;

		void Use()override final;

		ConstantBuffer_WvpInvTransp& GetTransform();
		ConstantBuffer_TextureTransform& GetTextureTransform();
		ConstantBuffer_Material& GetMaterial();
		ConstantBuffer_PointLight& GetPointLight();
		ConstantBuffer_Camera& GetCamera();

		void SetDiffuseTexture(ITexture*);
		void SetSpecularTexture(ITexture*);
		void SetNormalMap(ITexture*);

	private:
		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_Material> m_cbMaterial;
		ConstantBuffer<ConstantBuffer_PointLight> m_cbPointLight;
		ConstantBuffer<ConstantBuffer_Camera> m_cbCamera;
		ITexture* m_pDiffuseTexture;
		ITexture* m_pSpecularTexture;
		ITexture* m_pNormalMap;
};

//=============================================================================
//	Shader_PointLightArray
//=============================================================================

class Shader_PointLightArray : public Shader
{
	public:
		Shader_PointLightArray();
		virtual ~Shader_PointLightArray() = default;

		void Use()override final;

		ConstantBuffer_WvpInvTransp& GetTransform();
		ConstantBuffer_TextureTransform& GetTextureTransform();
		ConstantBuffer_Material& GetMaterial();
		ConstantBuffer_PointLight& GetPointLight(uint);
		ConstantBuffer_Camera& GetCamera();

		void SetDiffuseTexture(ITexture*);
		void SetSpecularTexture(ITexture*);
		void SetNormalMap(ITexture*);

	private:
		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_Material> m_cbMaterial;
		ConstantBuffer<ConstantBuffer_PointLightArray> m_cbPointLightArray;
		ConstantBuffer<ConstantBuffer_Camera> m_cbCamera;
		ITexture* m_pDiffuseTexture;
		ITexture* m_pSpecularTexture;
		ITexture* m_pNormalMap;
};

//=============================================================================
//	Shader_SpotLight
//=============================================================================

class Shader_SpotLight : public Shader
{
	public:
		Shader_SpotLight();
		virtual ~Shader_SpotLight() = default;

		void Use()override final;

		ConstantBuffer_LightTransformArray& GetLightTransform();
		ConstantBuffer_WvpInvTransp& GetTransform();
		ConstantBuffer_TextureTransform& GetTextureTransform();
		ConstantBuffer_Material& GetMaterial();
		ConstantBuffer_SpotLight& GetSpotLight(uint);
		ConstantBuffer_Camera& GetCamera();

		void SetDiffuseTexture(ITexture*);
		void SetSpecularTexture(ITexture*);
		void SetNormalMap(ITexture*);
		void SetShadowMap(ITexture*, uint index);

	private:
		uint FormatIndex(uint index)const;

		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_LightTransformArray> m_cbLightTransform;
		ConstantBuffer<ConstantBuffer_Material> m_cbMaterial;
		ConstantBuffer<ConstantBuffer_SpotLightArray> m_cbSpotLight;
		ConstantBuffer<ConstantBuffer_Camera> m_cbCamera;
		ITexture* m_pDiffuseTexture;
		ITexture* m_pSpecularTexture;
		ITexture* m_pNormalMap;
		ITexture* m_shadowMaps[engineDefault::shader::MAX_LIGHTS];
};

//=============================================================================
//	Shader_LightAll
//=============================================================================

class Shader_LightAll : public Shader
{
	public:
		Shader_LightAll();
		virtual ~Shader_LightAll() = default;

		void Use()override final;

		ConstantBuffer_LightTransformArray& GetLightTransform();
		ConstantBuffer_WvpInvTransp& GetTransform();
		ConstantBuffer_TextureTransform& GetTextureTransform();
		ConstantBuffer_Material& GetMaterial();
		ConstantBuffer_PointLight& GetPointLight(uint);
		ConstantBuffer_SpotLight& GetSpotLight(uint);
		ConstantBuffer_Camera& GetCamera();

		void SetDiffuseTexture(ITexture*);
		void SetSpecularTexture(ITexture*);
		void SetNormalMap(ITexture*);
		void SetShadowMap(ITexture*, uint index);

	private:
		uint FormatIndex(uint index)const;

		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_LightTransformArray> m_cbLightTransform;
		ConstantBuffer<ConstantBuffer_Material> m_cbMaterial;
		ConstantBuffer<ConstantBuffer_LightAll> m_cbLightAll;
		ConstantBuffer<ConstantBuffer_Camera> m_cbCamera;
		ITexture* m_pDiffuseTexture;
		ITexture* m_pSpecularTexture;
		ITexture* m_pNormalMap;
		ITexture* m_shadowMaps[engineDefault::shader::MAX_LIGHTS];
};

//=============================================================================
//	Shader_ShadowMap
//=============================================================================

class Shader_ShadowMap : public Shader
{
	public:
		Shader_ShadowMap();
		virtual ~Shader_ShadowMap() = default;

		void Use()override final;

		void SetModelWorld(const math::Mtx44&);
		ConstantBuffer_LightTransform& GetLightTransform();

	private:
		ConstantBuffer<ConstantBuffer_World> m_cbWorld;
		ConstantBuffer<ConstantBuffer_LightTransform> m_cbLightTransform;
};

//=============================================================================
//	Shader_Refraction
//=============================================================================

class Shader_Refraction : public Shader
{
	public:
		Shader_Refraction();
		virtual ~Shader_Refraction() = default;

		void Use()override final;

		ConstantBuffer_WvpInvTransp& GetTransform();
		ConstantBuffer_TextureTransform& GetTextureTransform();
		ConstantBuffer_Refraction& GetRefraction();

		void SetNormalMap(ITexture*);
		void SetRefractionMap(ITexture*);

	private:
		ConstantBuffer<ConstantBuffer_WvpInvTransp> m_cbWvpInvTransp;
		ConstantBuffer<ConstantBuffer_TextureTransform> m_cbTextureTransform;
		ConstantBuffer<ConstantBuffer_Refraction> m_cbRefraction;
		ITexture* m_pNormalMap;
		ITexture* m_pRefractionMap;
};
