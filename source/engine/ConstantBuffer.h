#pragma once

#include "Platform.h"
#include "../engine/Class.h"
#include "Type.h"
#include "RenderUtil.h"
#include "../engine/Pointer.h"
#include "../engine/EngineGlobal.h"
#include "../engine/Math.h"

/******************************************************************************
	Constant buffer type declaration
*******************************************************************************/

struct ConstantBuffer_World
{
	math::Mtx44 world;
};

struct ConstantBuffer_Wvp
{
	math::Mtx44 world;
	math::Mtx44 view;
	math::Mtx44 proj;
};

struct ConstantBuffer_Color
{
	math::Vec4 color;

	ConstantBuffer_Color() : color(1.0f) {}
};

struct ConstantBuffer_WvpInvTransp
{
	math::Mtx44 world;
	math::Mtx44 view;
	math::Mtx44 proj;
	math::Mtx44 worldInvTransp;

	void SetWvp(const math::Mtx44& _world, const math::Mtx44& _view, const math::Mtx44& _proj)
	{
		world = _world;
		view = _view;
		proj = _proj;

		math::matrixInvert(world, worldInvTransp);
		math::matrixTranspose(world, worldInvTransp);
	}
};

struct ConstantBuffer_ColorLightDir
{
	math::Vec4 ambient;
	math::Vec4 diffuse;
	math::Vec3 lightDir;
	float padding;

	ConstantBuffer_ColorLightDir()
	{
		padding = 0.0f;
	}
};

struct ConstantBuffer_DepthBias
{
	float depthBias;
	math::Vec3 padding;

	ConstantBuffer_DepthBias() : depthBias(0.0f) {}
};

struct ConstantBuffer_ColorIntensity
{
	math::Vec4 color;
	float intensity;

	ConstantBuffer_ColorIntensity() : intensity(0.5f) {}
};

struct ConstantBuffer_TextureTransform
{
	math::Mtx44 transform;
};

struct ConstantBuffer_Viewport
{
	float width;
	float height;
	math::Vec2 padding;

	ConstantBuffer_Viewport() : width(0.0f), height(0.0f) {}
};

struct ConstantBuffer_Blur
{
	math::Vec2 blurDir;
	math::Vec2 blurDisplace;
	math::Vec4 weights[engineDefault::shader::BLUR_WEIGHT_ARRAY_SIZE];
	uint weightCount;
	float baseWeight;
	math::Vec2 targetDimension;


	ConstantBuffer_Blur() : weightCount(0), baseWeight(1.0f), targetDimension(1.0f)
	{
		memoryWrite( weights, ARRAY_COUNT(weights), math::Vec4() );
	}
};

struct ConstantBuffer_MultiTexture
{
	float weights[engineDefault::shader::MULTI_TEXTURE_ARRAY_COUNT];
	math::Vec4 color;
	uint textureCount;
	float padding[3];

	ConstantBuffer_MultiTexture() : color(1.0f), textureCount(0)
	{
		memoryWrite( weights, ARRAY_COUNT(weights), 0.0f );
		memoryWrite( padding, ARRAY_COUNT(padding), 0.0f );
	}
};

struct ConstantBuffer_LightDir4
{
	math::Vec4 lightDirs[4];
	math::Vec4 lightColors[4];
};

struct ConstantBuffer_LightDir4Material
{
	math::Vec4 ambient;
	math::Vec4 diffuse;
};

struct ConstantBuffer_Camera
{
	math::Vec3 cameraPos;
	float padding;

	ConstantBuffer_Camera() : padding(0.0f) {}
};

struct ConstantBuffer_Material
{
	math::Vec4 emissiveColor4;
	math::Vec4 ambientColor4;
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float specularPower;
	float padding[3];

	ConstantBuffer_Material() : emissiveColor4(0.0f), ambientColor4(0.0f), diffuseColor4(1.0f), specularColor4(1.0f), specularPower(128.0f) 
	{
		memoryWrite( padding, ARRAY_COUNT(padding), 0.0f );
	}
};

struct ConstantBuffer_AmbientLight
{
	math::Vec4 ambientColor4;
};

struct ConstantBuffer_PointLight
{
	math::Vec3 pos;
	float padding0;
	float range;
	float padding1[3];
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float attenuation0;
	float attenuation1;
	float attenuation2;
	float padding2;
	
	ConstantBuffer_PointLight() : range(5.0f), diffuseColor4(1.0f), specularColor4(1.0f), attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f) 
	{
		memoryWrite( padding1, ARRAY_COUNT(padding1), 0.0f );
		padding2 = 0.0f;
	}
};

struct ConstantBuffer_PointLightArray
{
	ConstantBuffer_PointLight lights[engineDefault::shader::MAX_LIGHTS];
};

struct ConstantBuffer_SpotLight
{
	math::Vec3 pos;
	float padding0;
	math::Vec3 direction;
	float range;
	float innerConeRads;
	float outerConeRads;
	float padding1[2];
	math::Vec4 diffuseColor4;
	math::Vec4 specularColor4;
	float attenuation0;
	float attenuation1;
	float attenuation2;
	float padding2;
	bool4 shadowEnabled;
	float shadowBias;
    float distanceAdjustment;
	float padding3;

	ConstantBuffer_SpotLight() : diffuseColor4(1.0f), specularColor4(1.0f), attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f), range(5.0f),
		innerConeRads(math::toRads(30.0f)), outerConeRads(math::toRads(15.0f)), shadowEnabled(bool4True), shadowBias(0.00001f), distanceAdjustment(0.0f)
	{
		memoryWrite( padding1, ARRAY_COUNT(padding1), 0.0f );
		padding2 = 0.0f;
		padding3 = 0.0f;
	}
};

struct ConstantBuffer_SpotLightArray
{
	ConstantBuffer_SpotLight lights[engineDefault::shader::MAX_LIGHTS];
};

struct ConstantBuffer_LightAll
{
	ConstantBuffer_PointLight pointLights[engineDefault::shader::MAX_LIGHTS];
	ConstantBuffer_SpotLight spotLights[engineDefault::shader::MAX_LIGHTS];
};

struct ConstantBuffer_LightTransform
{
	math::Mtx44 lightView;
	math::Mtx44 lightProj;

	void SetVp(const math::Mtx44& view, const math::Mtx44& proj)
	{
		lightView = view;
		lightProj = proj;
	}
};

struct ConstantBuffer_LightTransformArray
{
	math::Mtx44 lightViews[engineDefault::shader::MAX_LIGHTS];
	math::Mtx44 lightProjs[engineDefault::shader::MAX_LIGHTS];

	void SetVp(const math::Mtx44& view, const math::Mtx44& proj, uint index)
	{
		CHECK( index < ARRAY_COUNT(lightViews) );
		index = math::clamp( index, 0, ARRAY_COUNT(lightViews) - 1 );

		lightViews[index] = view;
		lightProjs[index] = proj;
	}
};

struct ConstantBuffer_Refraction
{
	math::Vec3 color3;
	float refractionIntensity;
	float colorSaturation;
	float colorSeparation;
	float padding[2];

	ConstantBuffer_Refraction() : refractionIntensity(0.1f), colorSaturation(0.0f), colorSeparation(0.0f) 
	{
		memoryWrite( padding, ARRAY_COUNT(padding), 0.0f );
	}
};

/******************************************************************************
	Constant buffer class logic
*******************************************************************************/

class IConstantBuffer
{
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
};

class ConstantBufferApiImplementation
{
	template<class> friend class ConstantBuffer;

	private:
		ConstantBufferApiImplementation();
		~ConstantBufferApiImplementation();

		void Initialize(uint sizeOfBufferType);
		void Shutdown();
		void SetOnShader(uint slot, eShaderType shaderType, const void* pData);

		bool m_initialized;
		platform::ConstantBufferApi m_cbApi;
		uint m_uiSizeOfBufferType;
};

template <class BUFFER_TYPE>
class ConstantBuffer : public NonCopyable, public IConstantBuffer
{
	public:
		ConstantBuffer();
		~ConstantBuffer();

		void Initialize()override;
		void Shutdown()override;

		void SetOnShader(uint slot, eShaderType);

		BUFFER_TYPE& GetData();
		const BUFFER_TYPE& GetData()const;

	private:
		bool m_initialized;
		BUFFER_TYPE m_data;
		ConstantBufferApiImplementation m_cbImplementation;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline ConstantBuffer<BUFFER_TYPE>::ConstantBuffer()
{
	m_initialized = false;
}
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline ConstantBuffer<BUFFER_TYPE>::~ConstantBuffer()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline void ConstantBuffer<BUFFER_TYPE>::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_cbImplementation.Shutdown();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline void ConstantBuffer<BUFFER_TYPE>::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_cbImplementation.Initialize( sizeof(BUFFER_TYPE) );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	SetOnShader
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline void ConstantBuffer<BUFFER_TYPE>::SetOnShader(uint slot, eShaderType shaderType)
{
	m_cbImplementation.SetOnShader(slot, shaderType, (void*)&m_data);
}
//-------------------------------------------------------------------
//	GetData
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline BUFFER_TYPE& ConstantBuffer<BUFFER_TYPE>::GetData()
{
	return m_data;
}
//-------------------------------------------------------------------
template<class BUFFER_TYPE>
inline const BUFFER_TYPE& ConstantBuffer<BUFFER_TYPE>::GetData()const
{
	return m_data;
}
