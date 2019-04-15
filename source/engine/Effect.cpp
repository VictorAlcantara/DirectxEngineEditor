#include "../engine/Effect.h"
#include "../engine/Model.h"
#include "../engine/RenderTarget.h"
#include "../engine/Shader.h"
#include "../engine/CoreInterface.h"
#include "../engine/Texture.h"

/****************************************************************************************
	Helper function
*****************************************************************************************/

void setMaterialOnShader(const EffectProp_Material& effectMaterial, ConstantBuffer_Material& cbMaterial)
{
	cbMaterial.emissiveColor4 = effectMaterial.emissiveColor4;
	cbMaterial.ambientColor4 = effectMaterial.ambientColor4;
	cbMaterial.diffuseColor4 = effectMaterial.diffuseColor4;
	cbMaterial.specularColor4 = effectMaterial.specularColor4;
	cbMaterial.specularPower = effectMaterial.specularPower;
}

/****************************************************************************************
	Struct definition
*****************************************************************************************/

//=============================================================================
//	EffectConfig_Blur
//=============================================================================

void EffectConfig_Blur::SetWeights(const float* values, uint count)
{
	CHECK( count > 0 && count <= ARRAY_COUNT(weights) );
	count = (uint)math::clamp( (float)count, 0.0f, (float)ARRAY_COUNT(weights) );

	weightCount = count;

	float weightSum = 0.0f;
	for ( uint i = 0; i < count; i++ )
		weightSum += values[i];

	if ( math::isZero(weightSum) )
		weightSum = 1.0f;

	for ( uint i = 0; i < math::min<uint>(count, ARRAY_COUNT(weights)); i++ )
		weights[i] = values[i] / weightSum;
}

//=============================================================================
//	EffectConfig_LightDir4
//=============================================================================

//-------------------------------------------------------------------
//	SetLightDir
//-------------------------------------------------------------------
void EffectConfig_LightDir4::SetLightDir(uint index, const math::Vec3& lightDir)
{
	if ( index < ARRAY_COUNT(lightDirs) )
	{
		lightDirs[index] = lightDir;
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	SetLightColor
//-------------------------------------------------------------------
void EffectConfig_LightDir4::SetLightColor(uint index, const math::Vec4& lightColor)
{
	if ( index < ARRAY_COUNT(lightColors) )
	{
		lightColors[index] = lightColor;
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	GetLightDir
//-------------------------------------------------------------------
math::Vec3 EffectConfig_LightDir4::GetLightDir(uint index)const
{
	if ( index < ARRAY_COUNT(lightColors) )
		return lightDirs[index];

	ASSERT( "Index out of bounds" );
	return math::Vec3(0.0f);
}
//-------------------------------------------------------------------
//	GetLightColor
//-------------------------------------------------------------------
math::Vec4 EffectConfig_LightDir4::GetLightColor(uint index)const
{
	if ( index < ARRAY_COUNT(lightColors) )
		return lightColors[index];

	ASSERT( "Index out of bounds" );
	return math::Vec4(0.0f);
}

/****************************************************************************************
	Effect_Base
*****************************************************************************************/

//-------------------------------------------------------------------
//	ApplyRendererConfig
//-------------------------------------------------------------------
void Effect_Base_Impl::ApplyRendererConfig(const EffectConfig_Renderer& rendererConfig)
{
	g_pRenderer->SetBlendMode( rendererConfig.blendMode );
	g_pRenderer->SetFillMode( rendererConfig.wireframe ? eFillMode::Wireframe : eFillMode::Solid );
	g_pRenderer->SetCullMode( rendererConfig.wireframe ? eCullMode::None : rendererConfig.cullMode );
	g_pRenderer->SetDepthOperation( rendererConfig.depthOperation );
	g_pRenderer->EnableDepthWrite( rendererConfig.enableDepthWrite );
}

/****************************************************************************************
	Effect_Blur
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_Blur::OnShutdown()
{
	m_pShaderBlur->Shutdown();
	m_pShaderBlur = nullptr;

	m_pTargetBlurPass->Shutdown();
	m_pTargetBlurPass = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_Blur::OnInitialize()
{
	m_pShaderBlur = new Shader_Blur;
	m_pShaderBlur->Initialize();

	SetRenderTargetDimension( g_pRenderer->GetWindowViewport().GetDimension() );
}
//-------------------------------------------------------------------
//	SetRenderTargetDimension
//-------------------------------------------------------------------
void Effect_Blur::SetRenderTargetDimension(uint width, uint height)
{
	if ( m_pTargetBlurPass )
	{
		m_pTargetBlurPass->Shutdown();
		m_pTargetBlurPass = nullptr;
	}

	m_pTargetBlurPass = new RenderTarget;
	m_pTargetBlurPass->Initialize( width, height, eColorFormat::Rgba_32_Float );
}
//-------------------------------------------------------------------
void Effect_Blur::SetRenderTargetDimension(const math::Vec2& dimension)
{
	SetRenderTargetDimension( (uint)dimension.x, (uint)dimension.y );
}
//-------------------------------------------------------------------
//	SetRenderTargetProportion
//-------------------------------------------------------------------
void Effect_Blur::SetRenderTargetProportion(float f)
{
	math::Vec2 dimension = g_pRenderer->GetWindowViewport().GetDimension();
	SetRenderTargetDimension( dimension * f );
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_Blur::Render(RenderTarget* pRenderTarget, const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	m_pShaderBlur->SetBlurDisplace( m_config.displace );
	m_pShaderBlur->SetBaseWeight( m_config.baseWeight );
	m_pShaderBlur->SetWeights( m_config.weights, m_config.weightCount, false );

	m_pTargetBlurPass->Bind();
	m_pTargetBlurPass->Clear( 0.0f, 0.0f, 0.0f, 1.0f );
	{
		g_pRenderer->SetBlendMode( eBlendMode::Disabled );
		g_pRenderer->SetCullMode( eCullMode::Back );
		g_pRenderer->SetFillMode( eFillMode::Solid );
		g_pRenderer->SetSamplerState( eTextureAddress::Clamp, eTextureAddress::Clamp, eTextureFilter::Linear , 0);
		g_pRenderer->SetDepthOperation( eDepthOperation::Always );

		m_pShaderBlur->SetTargetDimension( m_pTargetBlurPass->GetTexture().Get().GetDimension() );
		m_pShaderBlur->SetWvp( math::Mtx44(), math::Mtx44(), math::matrixInvert(proj) );
		m_pShaderBlur->SetTexture( &m_config.hTexture.Get() );
		m_pShaderBlur->SetTextureTransform( m_config.textureTransform );
		m_pShaderBlur->SetBlurDir( m_config.blurDirPass0 );
		g_pResource->GetModelMngr()->Render( hModel, m_pShaderBlur.Get() );
	}
	m_pTargetBlurPass->Unbind();

	pRenderTarget->Bind();
	{
		ApplyRendererConfig();
		// Pass 1
		m_pShaderBlur->SetTargetDimension( m_pTargetBlurPass->GetTexture().Get().GetDimension() );
		m_pShaderBlur->SetTexture( &m_pTargetBlurPass->GetTexture().Get() );
		m_pShaderBlur->SetTextureTransform( math::Mtx33() );
		m_pShaderBlur->SetBlurDir( m_config.blurDirPass1 );
		g_pResource->GetModelMngr()->Render( hModel, m_pShaderBlur.Get() );
	}
	pRenderTarget->Unbind();
}

/****************************************************************************************
	Effect_Texture
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_Texture::OnShutdown()
{
	m_pShaderTexture->Shutdown();
	m_pShaderTexture = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_Texture::OnInitialize()
{
	m_pShaderTexture = new Shader_Texture;
	m_pShaderTexture->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_Texture::Render(const Handle<Model>& hModel, const math::Mtx44 world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	m_pShaderTexture->SetTextureTransform( m_config.textureTransform );
	m_pShaderTexture->SetColor( m_config.color4 );
	m_pShaderTexture->SetTexture( &m_config.hTexture.Get() );
	m_pShaderTexture->SetWvp(world, view, proj);

	g_pRenderer->SetSamplerState( eTextureAddress::Wrap, eTextureAddress::Wrap, eTextureFilter::Point, 0 );
	g_pResource->GetModelMngr()->Render( hModel, m_pShaderTexture.Get() );
}

/****************************************************************************************
	Effect_Color
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_Color::OnShutdown()
{
	m_pShaderOneColor->Shutdown();
	m_pShaderOneColor = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_Color::OnInitialize()
{
	m_pShaderOneColor = new Shader_OneColor;
	m_pShaderOneColor->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_Color::Render(const Handle<Model>& hModel, const math::Mtx44 world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();
	m_pShaderOneColor->SetWvp( world, view, proj );
	m_pShaderOneColor->SetColor( m_config.color4 );
	m_pShaderOneColor->SetDepthBias( m_config.depthBias );
	g_pResource->GetModelMngr()->Render( hModel, m_pShaderOneColor.Get() );
}

/****************************************************************************************
	Effect_VertexColor
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_VertexColor::OnShutdown()
{
	m_pShaderColor->Shutdown();
	m_pShaderColor= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_VertexColor::OnInitialize()
{
	m_pShaderColor = new Shader_Color;
	m_pShaderColor->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_VertexColor::Render(const Handle<Model>& hModel, const math::Mtx44 world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	m_pShaderColor->SetWvp( world, view, proj );
	m_pShaderColor->SetColor( m_config.color4 );
	g_pResource->GetModelMngr()->Render( hModel, m_pShaderColor.Get() );
}

/****************************************************************************************
	Effect_LightDir4
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_LightDir4::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;

	m_pShaderUntex->Shutdown();
	m_pShaderUntex= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_LightDir4::OnInitialize()
{
	m_pShader = new Shader_LightDir4;
	m_pShader->Initialize();

	m_pShaderUntex = new Shader_LightDir4Untex;
	m_pShaderUntex->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_LightDir4::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	g_pRenderer->SetSamplerState( m_config.sampler, 0 );

	if ( hModel.Get().GetVertexLayout().IsEqual(Vertex_P3N3T2::GetLayout()) )
	{
		m_pShader->SetWvp( world, view, proj );
		m_pShader->SetAmbient( m_config.ambientColor4 );
		m_pShader->SetDiffuse( m_config.diffuseColor4 );
		m_pShader->SetTexture( &m_config.hTexture.Get() );
		m_pShader->SetTextureTransform( m_config.textureTranslate );

		for ( uint i = 0; i < m_config.GetLightDirCount(); i++ )
		{
			m_pShader->SetLightDir( i, m_config.GetLightDir(i) );
			m_pShader->SetLightColor( i, m_config.GetLightColor(i) );
		}

		g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
	}
	else if ( hModel.Get().GetVertexLayout().IsEqual(Vertex_P3N3::GetLayout()) )
	{
		m_pShaderUntex->SetWvp( world, view, proj );
		m_pShaderUntex->SetAmbient( m_config.ambientColor4 );
		m_pShaderUntex->SetDiffuse( m_config.diffuseColor4 );

		for ( uint i = 0; i < m_config.GetLightDirCount(); i++ )
		{
			m_pShaderUntex->SetLightDir( i, m_config.GetLightDir(i) );
			m_pShaderUntex->SetLightColor( i, m_config.GetLightColor(i) );
		}

		g_pResource->GetModelMngr()->Render( hModel, m_pShaderUntex.Get() );
	}
	else
	{
		ASSERT( "Unrecognized vertex layout" );
	}
}

/****************************************************************************************
	Effect_Text2d
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_Text2d::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_Text2d::OnInitialize()
{
	m_pShader = new Shader_Text2d;
	m_pShader->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_Text2d::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	m_pShader->SetTexture( &GetConfig().hTexture.Get() );
	m_pShader->SetColor( GetConfig().color4 );
	m_pShader->SetTextureTransform( GetConfig().textureTransform );
	m_pShader->SetWvp( world, view, proj );

	g_pRenderer->SetSamplerState( GetConfig().sampler, 0 );
	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}

/****************************************************************************************
	Effect_DepthAmbientEmissive
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_DepthAmbientEmissive::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_DepthAmbientEmissive::OnInitialize()
{
	m_pShader = new Shader_DepthAmbientEmissive;
	m_pShader->Initialize();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_DepthAmbientEmissive::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	g_pRenderer->SetSamplerState( GetConfig().material.sampler, 0 );

	m_pShader->SetWvp( world, view, proj );
	m_pShader->SetTexture( &GetConfig().material.hDiffuseTexture.Get() );
	m_pShader->SetTextureTransform( GetConfig().material.textureTransform );
	m_pShader->GetAmbientLight().ambientColor4 = GetConfig().ambientLightColor4;
	setMaterialOnShader( GetConfig().material, m_pShader->GetMaterial() );

	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}

/****************************************************************************************
	Effect_PointLight
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_PointLight::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_PointLight::OnInitialize()
{
	m_pShader = new Shader_PointLightArray;
	m_pShader->Initialize();

	GetConfig().renderer.blendMode = eBlendMode::Additive;
	GetConfig().renderer.enableDepthWrite = false;
	GetConfig().renderer.depthOperation = eDepthOperation::Equal;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_PointLight::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	g_pRenderer->SetSamplerState( GetConfig().material.sampler, 0 );

	m_pShader->GetCamera().cameraPos = GetConfig().cameraPos;

	m_pShader->GetTransform().SetWvp( world, view, proj );
	m_pShader->SetDiffuseTexture( &GetConfig().material.hDiffuseTexture.Get() );
	m_pShader->SetSpecularTexture( &GetConfig().material.hSpecularTexture.Get() );
	m_pShader->SetNormalMap( &GetConfig().material.hNormalMap.Get() );
	m_pShader->GetTextureTransform().transform = GetConfig().material.textureTransform;

	setMaterialOnShader( GetConfig().material, m_pShader->GetMaterial() );

	for ( uint i = 0; i < GetMaxLights(); i++ )
	{
		m_pShader->GetPointLight(i).pos = GetConfig().lights[i].pos;
		m_pShader->GetPointLight(i).range = GetConfig().lights[i].range;
		m_pShader->GetPointLight(i).diffuseColor4 = GetConfig().lights[i].diffuseColor4;
		m_pShader->GetPointLight(i).specularColor4 = GetConfig().lights[i].specularColor4;
		m_pShader->GetPointLight(i).attenuation0 = GetConfig().lights[i].attenuation0;
		m_pShader->GetPointLight(i).attenuation1 = GetConfig().lights[i].attenuation1;
		m_pShader->GetPointLight(i).attenuation2 = GetConfig().lights[i].attenuation2;
	}
	
	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}

/****************************************************************************************
	Effect_SpotLight
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_SpotLight::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_SpotLight::OnInitialize()
{
	m_pShader = new Shader_SpotLight;
	m_pShader->Initialize();

	GetConfig().renderer.blendMode = eBlendMode::Additive;
	GetConfig().renderer.enableDepthWrite = false;
	GetConfig().renderer.depthOperation = eDepthOperation::Equal;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_SpotLight::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	g_pRenderer->SetSamplerState( GetConfig().material.sampler, 0 );
	g_pRenderer->SetSamplerState( eTextureAddress::Wrap, eTextureAddress::Wrap, eTextureFilter::Point, 1 );

	m_pShader->GetCamera().cameraPos = GetConfig().cameraPos;

	m_pShader->GetTransform().SetWvp( world, view, proj );
	m_pShader->SetDiffuseTexture( &GetConfig().material.hDiffuseTexture.Get() );
	m_pShader->SetSpecularTexture( &GetConfig().material.hSpecularTexture.Get() );
	m_pShader->SetNormalMap( &GetConfig().material.hNormalMap.Get() );
	m_pShader->GetTextureTransform().transform = GetConfig().material.textureTransform;

	setMaterialOnShader( GetConfig().material, m_pShader->GetMaterial() );

	for ( uint i = 0; i < GetMaxLights(); i++ )
	{
		m_pShader->GetSpotLight(i).pos = GetConfig().lights[i].pos;
		m_pShader->GetSpotLight(i).direction = GetConfig().lights[i].direction;
		m_pShader->GetSpotLight(i).range = GetConfig().lights[i].range;
		m_pShader->GetSpotLight(i).diffuseColor4 = GetConfig().lights[i].diffuseColor4;
		m_pShader->GetSpotLight(i).specularColor4 = GetConfig().lights[i].specularColor4;
		m_pShader->GetSpotLight(i).innerConeRads = math::toRads( GetConfig().lights[i].innerConeDegs );
		m_pShader->GetSpotLight(i).outerConeRads = math::toRads( GetConfig().lights[i].outerConeDegs );
		m_pShader->GetSpotLight(i).attenuation0 = GetConfig().lights[i].attenuation0;
		m_pShader->GetSpotLight(i).attenuation1 = GetConfig().lights[i].attenuation1;
		m_pShader->GetSpotLight(i).attenuation2 = GetConfig().lights[i].attenuation2;
		m_pShader->GetSpotLight(i).shadowEnabled = GetConfig().lights[i].shadowEnabled ? bool4True : bool4False;
		m_pShader->GetSpotLight(i).shadowBias = GetConfig().lights[i].shadowBias;
		m_pShader->GetSpotLight(i).distanceAdjustment = GetConfig().lights[i].distanceAdjustment;
		m_pShader->SetShadowMap( &GetConfig().lights[i].hShadowMap.Get(), i );
		m_pShader->GetLightTransform().SetVp( GetConfig().lights[i].transform.view, GetConfig().lights[i].transform.proj, i );
	}

	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}

/****************************************************************************************
	Effect_LightAll
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_LightAll::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_LightAll::OnInitialize()
{
	m_pShader = new Shader_LightAll;
	m_pShader->Initialize();

	GetConfig().renderer.blendMode = eBlendMode::Additive;
	GetConfig().renderer.enableDepthWrite = false;
	GetConfig().renderer.depthOperation = eDepthOperation::Equal;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_LightAll::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	g_pRenderer->SetSamplerState( GetConfig().material.sampler, 0 );
	g_pRenderer->SetSamplerState( eTextureAddress::Wrap, eTextureAddress::Wrap, eTextureFilter::Point, 1 );

	m_pShader->GetTransform().SetWvp( world, view, proj );

	m_pShader->GetCamera().cameraPos = GetConfig().cameraPos;
	m_pShader->SetDiffuseTexture( &GetConfig().material.hDiffuseTexture.Get() );
	m_pShader->SetSpecularTexture( &GetConfig().material.hSpecularTexture.Get() );
	m_pShader->SetNormalMap( &GetConfig().material.hNormalMap.Get() );
	m_pShader->GetTextureTransform().transform = GetConfig().material.textureTransform;

	setMaterialOnShader( GetConfig().material, m_pShader->GetMaterial() );

	auto& shaderLightTransform = m_pShader->GetLightTransform();
	for ( uint i = 0; i < GetMaxLights(); i++ )
	{
		// Point light
		auto pPointLight = &m_pShader->GetPointLight(i);
		const auto& configPointLight = GetConfig().pointLights[i];
		pPointLight->pos = configPointLight.pos;
		pPointLight->range = configPointLight.range;
		pPointLight->diffuseColor4 = configPointLight.diffuseColor4;
		pPointLight->specularColor4 = configPointLight.specularColor4;
		pPointLight->attenuation0 = configPointLight.attenuation0;
		pPointLight->attenuation1 = configPointLight.attenuation1;
		pPointLight->attenuation2 = configPointLight.attenuation2;

		// Spot light
		auto pSpotLight = &m_pShader->GetSpotLight(i);
		auto& configSpotLight = GetConfig().spotLights[i];
		pSpotLight->pos = configSpotLight.pos;
		pSpotLight->direction = configSpotLight.direction;
		pSpotLight->range = configSpotLight.range;
		pSpotLight->innerConeRads = math::toRads( configSpotLight.innerConeDegs );
		pSpotLight->outerConeRads = math::toRads( configSpotLight.outerConeDegs );
		pSpotLight->diffuseColor4 = configSpotLight.diffuseColor4;
		pSpotLight->specularColor4 = configSpotLight.specularColor4;
		pSpotLight->attenuation0 = configSpotLight.attenuation0;
		pSpotLight->attenuation1 = configSpotLight.attenuation1;
		pSpotLight->attenuation2 = configSpotLight.attenuation2;
		pSpotLight->shadowEnabled = configSpotLight.shadowEnabled ? bool4True : bool4False;
		pSpotLight->shadowBias = configSpotLight.shadowBias;
		pSpotLight->distanceAdjustment = configSpotLight.distanceAdjustment;
		m_pShader->SetShadowMap( configSpotLight.hShadowMap.GetPtr(), i );
		shaderLightTransform.SetVp( configSpotLight.transform.view, configSpotLight.transform.proj, i );
	}

	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}

/****************************************************************************************
	Effect_ShadowMap
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_ShadowMap::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_ShadowMap::OnInitialize()
{
	m_pShader = new Shader_ShadowMap;
	m_pShader->Initialize();

	GetConfig().renderer.blendMode = eBlendMode::Disabled;
	GetConfig().renderer.enableDepthWrite = true;
	GetConfig().renderer.depthOperation = eDepthOperation::Less;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_ShadowMap::Render(const Handle<Model>& hModel, const math::Mtx44& world)
{
	ApplyRendererConfig();

	m_pShader->SetModelWorld( world );
	m_pShader->GetLightTransform().lightView = GetConfig().lightTransform.view;
	m_pShader->GetLightTransform().lightProj = GetConfig().lightTransform.proj;

	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}

/****************************************************************************************
	Effect_Refraction
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Effect_Refraction::OnShutdown()
{
	m_pShader->Shutdown();
	m_pShader= nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Effect_Refraction::OnInitialize()
{
	m_pShader = new Shader_Refraction;
	m_pShader->Initialize();

	GetConfig().renderer.blendMode = eBlendMode::Disabled;
	GetConfig().renderer.enableDepthWrite = true;
	GetConfig().renderer.depthOperation = eDepthOperation::Less;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Effect_Refraction::Render(const Handle<Model>& hModel, const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	ApplyRendererConfig();

	g_pRenderer->SetSamplerState( GetConfig().sampler, 0 );
	m_pShader->GetTransform().SetWvp( world, view, proj );
	m_pShader->GetRefraction().color3 = GetConfig().color3;
	m_pShader->GetRefraction().refractionIntensity = GetConfig().refractionIntensity;
	m_pShader->GetRefraction().colorSaturation = GetConfig().colorSaturation;
	m_pShader->GetRefraction().colorSeparation = GetConfig().colorSeparation;
	m_pShader->GetTextureTransform().transform = GetConfig().textureTransform;
	m_pShader->SetNormalMap( GetConfig().hNormalMap.GetPtr() );
	m_pShader->SetRefractionMap( GetConfig().hRefractionMap.GetPtr() );

	g_pResource->GetModelMngr()->Render( hModel, m_pShader.Get() );
}
