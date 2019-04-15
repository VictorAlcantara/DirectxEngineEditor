#include "../engine/Shader.h"
#include "../engine/String.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"
#include "../engine/Pointer.h"
#include "../engine/Vertex.h"
#include "../engine/Texture.h"
#include "../engine/RenderTarget.h"
#include "../engine/Path.h"

#include "api_ShaderUtil.h"


/****************************************************************************************
	Shader
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader::Shader(const StringW& vsFile, const StringW& psFile, const VertexLayout* pLayout)
{
	m_vsFile = vsFile;
	m_psFile = psFile;
	m_pVertexLayout = pLayout;

	m_initialized = false;
}
//-------------------------------------------------------------------
Shader::~Shader()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Shader::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( auto it = m_constantBuffers.GetIterator(); it; it++ )
		(*it)->Shutdown();

	m_shaderApi;

	platform::release( m_shaderApi );
	

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Shader::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Shader already initialized" );
		return;
	}

	platform::shader::createVertexShaderAndLayout( m_vsFile.AsWchar(), g_pRenderer->GetApi(), m_shaderApi, m_pVertexLayout );
	if ( m_psFile.Length() > 0 )
		platform::shader::createPixelShader( m_psFile.AsWchar(), g_pRenderer->GetApi(), m_shaderApi );
	
	for ( auto it = m_constantBuffers.GetIterator(); it; it++ )
		(*it)->Initialize();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	EndUse
//-------------------------------------------------------------------
void Shader::EndUse()
{
	platform::shader::pixelShaderClearResource();
}
//-------------------------------------------------------------------
//	GetApi
//-------------------------------------------------------------------
platform::ShaderApi& Shader::GetApi()
{
	return m_shaderApi;
}
//-------------------------------------------------------------------
//	GetVertexLayout
//-------------------------------------------------------------------
const VertexLayout& Shader::GetVertexLayout()const
{
	return *m_pVertexLayout;
}
//-------------------------------------------------------------------
//	Register
//-------------------------------------------------------------------
void Shader::Register(IConstantBuffer* pConstantBuffer)
{
	m_constantBuffers.AddLast( pConstantBuffer );
}

/****************************************************************************************
	Shader_Color
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_Color::Shader_Color() : Shader( path::shader(L"color_vs"), path::shader(L"color_ps"), &Vertex_P3C4::GetLayout() )
{
	Register( &m_cbWvp );
	Register( &m_cbColor );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_Color::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );

	m_cbColor.SetOnShader( 0, eShaderType::PixelShader );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_Color::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetColor
//-------------------------------------------------------------------
void Shader_Color::SetColor(const math::Vec4& color)
{
	auto& cbData = m_cbColor.GetData();
	cbData.color = color;
}


/****************************************************************************************
	Shader_Texture
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_Texture::Shader_Texture() : Shader( path::shader(L"texture_vs"), path::shader(L"texture_ps"), &Vertex_P3T2::GetLayout() )
{
	m_pTexture = nullptr;

	Register( &m_cbWvp );
	Register( &m_cbTextureTransform );
	Register( &m_cbColor );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_Texture::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbColor.SetOnShader( 0, eShaderType::PixelShader );

	if ( m_pTexture )
		m_pTexture->Bind( 0 );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_Texture::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetTexture
//-------------------------------------------------------------------
void Shader_Texture::SetTexture(ITexture* pTexture)
{
	m_pTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetTextureTransform
//-------------------------------------------------------------------
void Shader_Texture::SetTextureTransform(const math::Mtx33& transform)
{
	auto& cbTextureTransform = m_cbTextureTransform.GetData();
	cbTextureTransform.transform = transform;
}
//-------------------------------------------------------------------
//	SetColor
//-------------------------------------------------------------------
void Shader_Texture::SetColor(const math::Vec4& color)
{
	auto& cbColor = m_cbColor.GetData();
	cbColor.color = color;
}

/****************************************************************************************
	Shader_OneColor
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_OneColor::Shader_OneColor() : Shader( path::shader(L"oneColor_vs"), path::shader(L"oneColor_ps"), &Vertex_P3::GetLayout() )
{
	Register( &m_cbWvp );
	Register( &m_cbDepthBias );
	Register( &m_cbColor );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_OneColor::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbDepthBias.SetOnShader( 1, eShaderType::VertexShader );

	m_cbColor.SetOnShader( 0, eShaderType::PixelShader );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_OneColor::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetColor
//-------------------------------------------------------------------
void Shader_OneColor::SetColor(float r, float g, float b, float a)
{
	SetColor( math::Vec4(r, g, b, a) );
}
//-------------------------------------------------------------------
void Shader_OneColor::SetColor(const math::Vec4& color)
{
	auto& cbData = m_cbColor.GetData();
	cbData.color = color;
}
//-------------------------------------------------------------------
//	SetDepthBias
//-------------------------------------------------------------------
void Shader_OneColor::SetDepthBias(float depthBias)
{
	auto& cbData = m_cbDepthBias.GetData();
	cbData.depthBias = depthBias;
}

/****************************************************************************************
	Shader_ColorLightDir
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_ColorLightDir::Shader_ColorLightDir() : Shader( path::shader(L"colorLightDir_vs"), path::shader(L"colorLightDir_ps"), &Vertex_P3N3::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbLight );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_ColorLightDir::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );

	m_cbLight.SetOnShader( 0, eShaderType::PixelShader );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_ColorLightDir::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvpInvTransp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
	cbData.worldInvTransp = math::matrixNormalTransform( world );
}
//-------------------------------------------------------------------
//	SetAmbient
//-------------------------------------------------------------------
void Shader_ColorLightDir::SetAmbient(const math::Vec4& ambient)
{
	auto& cbData = m_cbLight.GetData();
	cbData.ambient = ambient;
}
//-------------------------------------------------------------------
//	SetDiffuse
//-------------------------------------------------------------------
void Shader_ColorLightDir::SetDiffuse(const math::Vec4& diffuse)
{
	auto& cbData = m_cbLight.GetData();
	cbData.diffuse = diffuse;
}
//-------------------------------------------------------------------
//	SetLightDirection
//-------------------------------------------------------------------
void Shader_ColorLightDir::SetLightDirection(const math::Vec3& lightDir)
{
	auto& cbData = m_cbLight.GetData();
	cbData.lightDir = lightDir;
}

/****************************************************************************************
	Shader_Blur
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_Blur::Shader_Blur() : Shader( path::shader(L"blur_vs"), path::shader(L"blur_ps"), &Vertex_P3T2::GetLayout() )
{
	m_pTexture = nullptr;
	Register( &m_cbWvp );
	Register( &m_cbTextureTransform );
	Register( &m_cbBlur );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_Blur::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbBlur.SetOnShader( 0, eShaderType::PixelShader );

	if ( m_pTexture )
		m_pTexture->Bind( 0 );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_Blur::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetBlurDir
//-------------------------------------------------------------------
void Shader_Blur::SetBlurDir(const math::Vec2& v)
{
	auto& cbData = m_cbBlur.GetData();
	cbData.blurDir.x = v.x;
	cbData.blurDir.y = v.y;
}
//-------------------------------------------------------------------
void Shader_Blur::SetBlurDir(float x, float y)
{
	SetBlurDir( math::Vec2(x, y) );
}
//-------------------------------------------------------------------
//	SetBlurDisplace
//-------------------------------------------------------------------
void Shader_Blur::SetBlurDisplace(float xy)
{
	SetBlurDisplace( math::Vec2(xy) );
}
//-------------------------------------------------------------------
void Shader_Blur::SetBlurDisplace(const math::Vec2& v)
{
	auto& cbData = m_cbBlur.GetData();
	cbData.blurDisplace.x = v.x;
	cbData.blurDisplace.y = v.y;
}
//-------------------------------------------------------------------
void Shader_Blur::SetBlurDisplace(float x, float y)
{
	SetBlurDisplace( math::Vec2(x, y) );
}
//-------------------------------------------------------------------
//	SetWeights
//-------------------------------------------------------------------
void Shader_Blur::SetWeights(const float* pValues, uint count, bool normalize)
{
	auto& cbData = m_cbBlur.GetData();

	CHECK( count <= engineDefault::shader::BLUR_WEIGHT_ARRAY_MAX );

	cbData.weightCount = math::min<uint>( count, engineDefault::shader::BLUR_WEIGHT_ARRAY_MAX );

	float weightsSum = 1.0f;
	if ( normalize )
	{
		weightsSum = 0.0f;
		for ( uint i = 0; i < cbData.weightCount; i++ )
			weightsSum += pValues[i];

		if ( math::isZero(weightsSum) )
			weightsSum = 1.0f;

		weightsSum = 1.0f / weightsSum;
	}

	for ( uint i = 0; i < cbData.weightCount; i++ )
		cbData.weights[i/4][i%4] = pValues[i] * weightsSum;
}
//-------------------------------------------------------------------
//	SetWeightBase
//-------------------------------------------------------------------
void Shader_Blur::SetBaseWeight(float f)
{
	CHECK( f >= 0.0f && f <= 1.0f );

	auto& cbData = m_cbBlur.GetData();
	cbData.baseWeight = f;
}
//-------------------------------------------------------------------
//	SetTextureTransform
//-------------------------------------------------------------------
void Shader_Blur::SetTextureTransform(const math::Mtx33& m)
{
	auto& cbData = m_cbTextureTransform.GetData();
	cbData.transform = m;
}
//-------------------------------------------------------------------
//	SetTargetDimension
//-------------------------------------------------------------------
void Shader_Blur::SetTargetDimension(const math::Vec2& dimension)
{
	auto& cbData = m_cbBlur.GetData();
	cbData.targetDimension = dimension;
}
//-------------------------------------------------------------------
//	SetTexture
//-------------------------------------------------------------------
void Shader_Blur::SetTexture(ITexture* pTexture)
{
	m_pTexture = pTexture;
}

/****************************************************************************************
	Shader_VertexPosTexture
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_VertexPosTexture::Shader_VertexPosTexture() : Shader( path::shader(L"vertexPosTexture_vs"), path::shader(L"vertexPosTexture_ps"), &Vertex_P3::GetLayout() )
{
	m_pTexture = nullptr;
	Register( &m_cbWvp );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_VertexPosTexture::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );

	if ( m_pTexture )
		m_pTexture->Bind( 0 );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_VertexPosTexture::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetTexture
//-------------------------------------------------------------------
void Shader_VertexPosTexture::SetTexture(ITexture* pTexture)
{
	m_pTexture = pTexture;
}

/****************************************************************************************
	Shader_MultiTexture
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_MultiTexture::Shader_MultiTexture() : Shader( path::shader(L"multiTexture_vs"), path::shader(L"multiTexture_ps"), &Vertex_P3T2::GetLayout() )
{
	Register( &m_cbWvp );
	Register( &m_cbMultiTexture );
	ClearTexture();
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_MultiTexture::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );

	m_cbMultiTexture.SetOnShader( 0, eShaderType::PixelShader );

	auto& data = m_cbMultiTexture.GetData();

	for ( uint i = 0; i < data.textureCount; i++ )
	{
		if ( m_textures[i] )
			m_textures[i]->Bind( i );
	}
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_MultiTexture::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	AddTexture
//-------------------------------------------------------------------
void Shader_MultiTexture::AddTexture(ITexture* pTexture, float weight)
{
	auto& data = m_cbMultiTexture.GetData();

	if ( data.textureCount < engineDefault::shader::MULTI_TEXTURE_ARRAY_COUNT )
	{
		m_textures[data.textureCount] = pTexture;
		data.weights[data.textureCount] = weight;
		data.textureCount++;
	}
	else
		ASSERT( "ITexture array filled" );
}
//-------------------------------------------------------------------
//	ClearTexture
//-------------------------------------------------------------------
void Shader_MultiTexture::ClearTexture()
{
	auto& data = m_cbMultiTexture.GetData();
	data.textureCount = 0;
	for ( uint i = 0; i < engineDefault::shader::MULTI_TEXTURE_ARRAY_COUNT; i++ )
		m_textures[i] = nullptr;
}
//-------------------------------------------------------------------
//	NormalizeWeights
//-------------------------------------------------------------------
void Shader_MultiTexture::NormalizeWeights()
{
	auto& data = m_cbMultiTexture.GetData();
	float weightSum = 0.0f;

	for ( uint i = 0; i < data.textureCount; i++ )
		weightSum = data.weights[i];
	for ( uint i = 0; i < data.textureCount; i++ )
		data.weights[i] /= weightSum;
}

/****************************************************************************************
	Shader_LightDir4
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_LightDir4::Shader_LightDir4() : Shader( path::shader(L"lightDir4_vs"), path::shader(L"lightDir4_ps"), &Vertex_P3N3T2::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbTextureTransform );
	Register( &m_cbLightDir4 );
	Register( &m_cbMaterial );
	m_pTexture = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_LightDir4::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbLightDir4.SetOnShader( 0, eShaderType::PixelShader );
	m_cbMaterial.SetOnShader( 1, eShaderType::PixelShader );

	m_pTexture->Bind( 0 );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_LightDir4::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvpInvTransp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
	cbData.worldInvTransp = math::matrixNormalTransform( world );
}
//-------------------------------------------------------------------
//	SetLightDir
//-------------------------------------------------------------------
void Shader_LightDir4::SetLightDir(uint index, const math::Vec3& normal)
{
	auto& cbData = m_cbLightDir4.GetData();

	if ( index < ARRAY_COUNT(cbData.lightDirs) )
	{
		cbData.lightDirs[index] = math::Vec4(normal, 0.0f);
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	SetLightColor
//-------------------------------------------------------------------
void Shader_LightDir4::SetLightColor(uint index, const math::Vec4& color)
{
	auto& cbData = m_cbLightDir4.GetData();

	if ( index < ARRAY_COUNT(cbData.lightColors) )
	{
		cbData.lightColors[index] = color, 0.0f;
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	SetAmbient
//-------------------------------------------------------------------
void Shader_LightDir4::SetAmbient(const math::Vec4& color)
{
	auto& cbData = m_cbMaterial.GetData();
	cbData.ambient = color;
}
//-------------------------------------------------------------------
//	SetDiffuse
//-------------------------------------------------------------------
void Shader_LightDir4::SetDiffuse(const math::Vec4& color)
{
	auto& cbData = m_cbMaterial.GetData();
	cbData.diffuse = color;
}
//-------------------------------------------------------------------
//	SetTexture
//-------------------------------------------------------------------
void Shader_LightDir4::SetTexture(ITexture* pTexture)
{
	m_pTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetTextureTransform
//-------------------------------------------------------------------
void Shader_LightDir4::SetTextureTransform(const math::Mtx33& m)
{
	auto& cbData = m_cbTextureTransform.GetData();
	cbData.transform = m;
}

/****************************************************************************************
	Shader_LightDir4Untex
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_LightDir4Untex::Shader_LightDir4Untex() : Shader( path::shader(L"lightDir4Untex_vs"), path::shader(L"lightDir4Untex_ps"), &Vertex_P3N3::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbLightDir4 );
	Register( &m_cbMaterial );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_LightDir4Untex::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbLightDir4.SetOnShader( 0, eShaderType::PixelShader );
	m_cbMaterial.SetOnShader( 1, eShaderType::PixelShader );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_LightDir4Untex::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvpInvTransp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
	cbData.worldInvTransp = math::matrixNormalTransform( world );
}
//-------------------------------------------------------------------
//	SetLightDir
//-------------------------------------------------------------------
void Shader_LightDir4Untex::SetLightDir(uint index, const math::Vec3& normal)
{
	auto& cbData = m_cbLightDir4.GetData();

	if ( index < ARRAY_COUNT(cbData.lightDirs) )
	{
		cbData.lightDirs[index] = math::Vec4(normal, 0.0f);
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	SetLightColor
//-------------------------------------------------------------------
void Shader_LightDir4Untex::SetLightColor(uint index, const math::Vec4& color)
{
	auto& cbData = m_cbLightDir4.GetData();

	if ( index < ARRAY_COUNT(cbData.lightColors) )
	{
		cbData.lightColors[index] = color, 0.0f;
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	SetAmbient
//-------------------------------------------------------------------
void Shader_LightDir4Untex::SetAmbient(const math::Vec4& color)
{
	auto& cbData = m_cbMaterial.GetData();
	cbData.ambient = color;
}
//-------------------------------------------------------------------
//	SetDiffuse
//-------------------------------------------------------------------
void Shader_LightDir4Untex::SetDiffuse(const math::Vec4& color)
{
	auto& cbData = m_cbMaterial.GetData();
	cbData.diffuse = color;
}

/****************************************************************************************
	Shader_Text2d
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_Text2d::Shader_Text2d() : Shader( path::shader(L"text2d_vs"), path::shader(L"text2d_ps"), &Vertex_P3T2::GetLayout() )
{
	Register( &m_cbWvp );
	Register( &m_cbTextureTransform );
	Register( &m_cbColor );

	m_pTexture = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_Text2d::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbColor.SetOnShader( 0, eShaderType::PixelShader );

	m_pTexture->Bind( 0 );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_Text2d::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetTextureTransform
//-------------------------------------------------------------------
void Shader_Text2d::SetTextureTransform(const math::Mtx33& transform)
{
	auto& cbTextureTransform = m_cbTextureTransform.GetData();
	cbTextureTransform.transform = transform;
}
//-------------------------------------------------------------------
//	SetColor
//-------------------------------------------------------------------
void Shader_Text2d::SetColor(const math::Vec4& color)
{
	auto& cbData = m_cbColor.GetData();
	cbData.color = color;
}
//-------------------------------------------------------------------
//	SetTexture
//-------------------------------------------------------------------
void Shader_Text2d::SetTexture(ITexture* pTexture)
{
	m_pTexture = pTexture;
}

/****************************************************************************************
	Shader_DepthAmbientEmissive
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_DepthAmbientEmissive::Shader_DepthAmbientEmissive() : Shader( path::shader(L"depthAmbientEmissive_vs"), path::shader(L"depthAmbientEmissive_ps"), &Vertex_P3T2::GetLayout() )
{
	Register( &m_cbWvp );
	Register( &m_cbTextureTransform );
	Register( &m_cbMaterial );
	Register( &m_cbAmbientLight );

	m_pTexture = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_DepthAmbientEmissive::Use()
{
	m_cbWvp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbMaterial.SetOnShader( 0, eShaderType::PixelShader );
	m_cbAmbientLight.SetOnShader( 1, eShaderType::PixelShader );

	m_pTexture->Bind( 0 );
}
//-------------------------------------------------------------------
//	SetWvp
//-------------------------------------------------------------------
void Shader_DepthAmbientEmissive::SetWvp(const math::Mtx44& world, const math::Mtx44& view, const math::Mtx44& proj)
{
	auto& cbData = m_cbWvp.GetData();

	cbData.world = world;
	cbData.view = view;
	cbData.proj = proj;
}
//-------------------------------------------------------------------
//	SetDiffuseTexture
//-------------------------------------------------------------------
void Shader_DepthAmbientEmissive::SetTexture(ITexture* pTexture)
{
	m_pTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetTextureTransform
//-------------------------------------------------------------------
void Shader_DepthAmbientEmissive::SetTextureTransform(const math::Mtx33& transform)
{
	auto& cbTextureTransform = m_cbTextureTransform.GetData();
	cbTextureTransform.transform = transform;
}
//-------------------------------------------------------------------
//	GetMaterial
//-------------------------------------------------------------------
ConstantBuffer_Material& Shader_DepthAmbientEmissive::GetMaterial()
{
	return m_cbMaterial.GetData();
}
//-------------------------------------------------------------------
//	GetAmbientLight
//-------------------------------------------------------------------
ConstantBuffer_AmbientLight& Shader_DepthAmbientEmissive::GetAmbientLight()
{
	return m_cbAmbientLight.GetData();
}

/****************************************************************************************
	Shader_PointLight
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_PointLight::Shader_PointLight() : Shader( path::shader(L"pointLight_vs"), path::shader(L"pointLight_ps"), &Vertex_P3N3Tg3Bn3T2::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbTextureTransform );
	Register( &m_cbMaterial );
	Register( &m_cbPointLight );
	Register( &m_cbCamera );
	
	m_pDiffuseTexture = nullptr;
	m_pSpecularTexture = nullptr;
	m_pNormalMap = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_PointLight::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbMaterial.SetOnShader( 0, eShaderType::PixelShader );
	m_cbPointLight.SetOnShader( 1, eShaderType::PixelShader );
	m_cbCamera.SetOnShader( 2, eShaderType::PixelShader );

	m_pDiffuseTexture->Bind( 0 );
	m_pSpecularTexture->Bind( 1 );
	m_pNormalMap->Bind( 2 );
}
//-------------------------------------------------------------------
//	GetTransform
//-------------------------------------------------------------------
ConstantBuffer_WvpInvTransp& Shader_PointLight::GetTransform()
{
	return m_cbWvpInvTransp.GetData();
}
//-------------------------------------------------------------------
//	GetTextureTransform
//-------------------------------------------------------------------
ConstantBuffer_TextureTransform& Shader_PointLight::GetTextureTransform()
{
	return m_cbTextureTransform.GetData();
}
//-------------------------------------------------------------------
//	GetMaterial
//-------------------------------------------------------------------
ConstantBuffer_Material& Shader_PointLight::GetMaterial()
{
	return m_cbMaterial.GetData();
}
//-------------------------------------------------------------------
//	GetPointLight
//-------------------------------------------------------------------
ConstantBuffer_PointLight& Shader_PointLight::GetPointLight()
{
	return m_cbPointLight.GetData();
}
//-------------------------------------------------------------------
//	GetCamera
//-------------------------------------------------------------------
ConstantBuffer_Camera& Shader_PointLight::GetCamera()
{
	return m_cbCamera.GetData();
}
//-------------------------------------------------------------------
//	SetDiffuseTexture
//-------------------------------------------------------------------
void Shader_PointLight::SetDiffuseTexture(ITexture* pTexture)
{
	m_pDiffuseTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetSpecularTexture
//-------------------------------------------------------------------
void Shader_PointLight::SetSpecularTexture(ITexture* pTexture)
{
	m_pSpecularTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetNormalMap
//-------------------------------------------------------------------
void Shader_PointLight::SetNormalMap(ITexture* pTexture)
{
	m_pNormalMap = pTexture;
}

/****************************************************************************************
	Shader_PointLightArray
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_PointLightArray::Shader_PointLightArray() : Shader( path::shader(L"pointLight_vs"), path::shader(L"pointLightArray_ps"), &Vertex_P3N3Tg3Bn3T2::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbTextureTransform );
	Register( &m_cbMaterial );
	Register( &m_cbPointLightArray );
	Register( &m_cbCamera );
	
	m_pDiffuseTexture = nullptr;
	m_pSpecularTexture = nullptr;
	m_pNormalMap = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_PointLightArray::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbMaterial.SetOnShader( 0, eShaderType::PixelShader );
	m_cbPointLightArray.SetOnShader( 1, eShaderType::PixelShader );
	m_cbCamera.SetOnShader( 2, eShaderType::PixelShader );

	m_pDiffuseTexture->Bind( 0 );
	m_pSpecularTexture->Bind( 1 );
	m_pNormalMap->Bind( 2 );
}
//-------------------------------------------------------------------
//	GetTransform
//-------------------------------------------------------------------
ConstantBuffer_WvpInvTransp& Shader_PointLightArray::GetTransform()
{
	return m_cbWvpInvTransp.GetData();
}
//-------------------------------------------------------------------
//	GetTextureTransform
//-------------------------------------------------------------------
ConstantBuffer_TextureTransform& Shader_PointLightArray::GetTextureTransform()
{
	return m_cbTextureTransform.GetData();
}
//-------------------------------------------------------------------
//	GetMaterial
//-------------------------------------------------------------------
ConstantBuffer_Material& Shader_PointLightArray::GetMaterial()
{
	return m_cbMaterial.GetData();
}
//-------------------------------------------------------------------
//	GetPointLight
//-------------------------------------------------------------------
ConstantBuffer_PointLight& Shader_PointLightArray::GetPointLight(uint index)
{
	index = (uint)math::clamp( index, 0, ARRAY_COUNT(m_cbPointLightArray.GetData().lights) - 1 );
	return m_cbPointLightArray.GetData().lights[index];
}
//-------------------------------------------------------------------
//	GetCamera
//-------------------------------------------------------------------
ConstantBuffer_Camera& Shader_PointLightArray::GetCamera()
{
	return m_cbCamera.GetData();
}
//-------------------------------------------------------------------
//	SetDiffuseTexture
//-------------------------------------------------------------------
void Shader_PointLightArray::SetDiffuseTexture(ITexture* pTexture)
{
	m_pDiffuseTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetSpecularTexture
//-------------------------------------------------------------------
void Shader_PointLightArray::SetSpecularTexture(ITexture* pTexture)
{
	m_pSpecularTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetNormalMap
//-------------------------------------------------------------------
void Shader_PointLightArray::SetNormalMap(ITexture* pTexture)
{
	m_pNormalMap = pTexture;
}

/****************************************************************************************
	Shader_SpotLight
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_SpotLight::Shader_SpotLight() : Shader( path::shader(L"spotLight_vs"), path::shader(L"spotLight_ps"), &Vertex_P3N3Tg3Bn3T2::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbTextureTransform );
	Register( &m_cbLightTransform );
	Register( &m_cbMaterial );
	Register( &m_cbSpotLight );
	Register( &m_cbCamera );

	m_pDiffuseTexture = nullptr;
	m_pSpecularTexture = nullptr;
	m_pNormalMap = nullptr;
	for ( uint i = 0; i < ARRAY_COUNT(m_shadowMaps); i++ )
		m_shadowMaps[i] = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_SpotLight::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );
	m_cbLightTransform.SetOnShader( 2, eShaderType::VertexShader );

	m_cbMaterial.SetOnShader( 0, eShaderType::PixelShader );
	m_cbSpotLight.SetOnShader( 1, eShaderType::PixelShader );
	m_cbCamera.SetOnShader( 2, eShaderType::PixelShader );

	m_pDiffuseTexture->Bind( 0 );
	m_pSpecularTexture->Bind( 1 );
	m_pNormalMap->Bind( 2 );

	for ( uint i = 0; i < ARRAY_COUNT(m_shadowMaps); i++ )
		m_shadowMaps[i]->Bind( 3 + i );
}
//-------------------------------------------------------------------
//	GetLightTransform
//-------------------------------------------------------------------
ConstantBuffer_LightTransformArray& Shader_SpotLight::GetLightTransform()
{
	return m_cbLightTransform.GetData();
}
//-------------------------------------------------------------------
//	GetTransform
//-------------------------------------------------------------------
ConstantBuffer_WvpInvTransp& Shader_SpotLight::GetTransform()
{
	return m_cbWvpInvTransp.GetData();
}
//-------------------------------------------------------------------
//	GetTextureTransform
//-------------------------------------------------------------------
ConstantBuffer_TextureTransform& Shader_SpotLight::GetTextureTransform()
{
	return m_cbTextureTransform.GetData();
}
//-------------------------------------------------------------------
//	GetMaterial
//-------------------------------------------------------------------
ConstantBuffer_Material& Shader_SpotLight::GetMaterial()
{
	return m_cbMaterial.GetData();
}
//-------------------------------------------------------------------
//	GetSpotLight
//-------------------------------------------------------------------
ConstantBuffer_SpotLight& Shader_SpotLight::GetSpotLight(uint index)
{
	index = FormatIndex( index );
	return m_cbSpotLight.GetData().lights[index];
}
//-------------------------------------------------------------------
//	GetCamera
//-------------------------------------------------------------------
ConstantBuffer_Camera& Shader_SpotLight::GetCamera()
{
	return m_cbCamera.GetData();
}
//-------------------------------------------------------------------
//	SetDiffuseTexture
//-------------------------------------------------------------------
void Shader_SpotLight::SetDiffuseTexture(ITexture* pTexture)
{
	m_pDiffuseTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetSpecularTexture
//-------------------------------------------------------------------
void Shader_SpotLight::SetSpecularTexture(ITexture* pTexture)
{
	m_pSpecularTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetNormalMap
//-------------------------------------------------------------------
void Shader_SpotLight::SetNormalMap(ITexture* pTexture)
{
	m_pNormalMap = pTexture;
}
//-------------------------------------------------------------------
//	SetShadowMap
//-------------------------------------------------------------------
void Shader_SpotLight::SetShadowMap(ITexture* pTexture, uint index)
{
	index = FormatIndex( index );
	m_shadowMaps[index] = pTexture;
}
//-------------------------------------------------------------------
//	FormatIndex
//-------------------------------------------------------------------
uint Shader_SpotLight::FormatIndex(uint index)const
{
	CHECK( index < ARRAY_COUNT(m_cbSpotLight.GetData().lights) );
	index = (uint)math::clamp( index, 0 , ARRAY_COUNT(m_cbSpotLight.GetData().lights) - 1 ); 
	return index;
}

/****************************************************************************************
	Shader_LightAll
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_LightAll::Shader_LightAll() : Shader( path::shader(L"light_vs"), path::shader(L"light_ps"), &Vertex_P3N3Tg3Bn3T2::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbTextureTransform );
	Register( &m_cbLightTransform );
	Register( &m_cbMaterial );
	Register( &m_cbLightAll );
	Register( &m_cbCamera );

	m_pDiffuseTexture = nullptr;
	m_pSpecularTexture = nullptr;
	m_pNormalMap = nullptr;
	for ( uint i = 0; i < ARRAY_COUNT(m_shadowMaps); i++ )
		m_shadowMaps[i] = nullptr;
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_LightAll::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );
	m_cbLightTransform.SetOnShader( 2, eShaderType::VertexShader );

	m_cbMaterial.SetOnShader( 0, eShaderType::PixelShader );
	m_cbLightAll.SetOnShader( 1, eShaderType::PixelShader );
	m_cbCamera.SetOnShader( 2, eShaderType::PixelShader );

	m_pDiffuseTexture->Bind( 0 );
	m_pSpecularTexture->Bind( 1 );
	m_pNormalMap->Bind( 2 );

	for ( uint i = 0; i < ARRAY_COUNT(m_shadowMaps); i++ )
		m_shadowMaps[i]->Bind( 3 + i );
}
//-------------------------------------------------------------------
//	GetLightTransform
//-------------------------------------------------------------------
ConstantBuffer_LightTransformArray& Shader_LightAll::GetLightTransform()
{
	return m_cbLightTransform.GetData();
}
//-------------------------------------------------------------------
//	GetTransform
//-------------------------------------------------------------------
ConstantBuffer_WvpInvTransp& Shader_LightAll::GetTransform()
{
	return m_cbWvpInvTransp.GetData();
}
//-------------------------------------------------------------------
//	GetTextureTransform
//-------------------------------------------------------------------
ConstantBuffer_TextureTransform& Shader_LightAll::GetTextureTransform()
{
	return m_cbTextureTransform.GetData();
}
//-------------------------------------------------------------------
//	GetMaterial
//-------------------------------------------------------------------
ConstantBuffer_Material& Shader_LightAll::GetMaterial()
{
	return m_cbMaterial.GetData();
}
//-------------------------------------------------------------------
//	GetPointLight
//-------------------------------------------------------------------
ConstantBuffer_PointLight& Shader_LightAll::GetPointLight(uint index)
{
	index = FormatIndex( index );
	return m_cbLightAll.GetData().pointLights[index];
}
//-------------------------------------------------------------------
//	GetSpotLight
//-------------------------------------------------------------------
ConstantBuffer_SpotLight& Shader_LightAll::GetSpotLight(uint index)
{
	index = FormatIndex( index );
	return m_cbLightAll.GetData().spotLights[index];
}
//-------------------------------------------------------------------
//	GetCamera
//-------------------------------------------------------------------
ConstantBuffer_Camera& Shader_LightAll::GetCamera()
{
	return m_cbCamera.GetData();
}
//-------------------------------------------------------------------
//	SetDiffuseTexture
//-------------------------------------------------------------------
void Shader_LightAll::SetDiffuseTexture(ITexture* pTexture)
{
	m_pDiffuseTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetSpecularTexture
//-------------------------------------------------------------------
void Shader_LightAll::SetSpecularTexture(ITexture* pTexture)
{
	m_pSpecularTexture = pTexture;
}
//-------------------------------------------------------------------
//	SetNormalMap
//-------------------------------------------------------------------
void Shader_LightAll::SetNormalMap(ITexture* pTexture)
{
	m_pNormalMap = pTexture;
}
//-------------------------------------------------------------------
//	SetShadowMap
//-------------------------------------------------------------------
void Shader_LightAll::SetShadowMap(ITexture* pTexture, uint index)
{
	index = FormatIndex( index );
	m_shadowMaps[index] = pTexture;
}
//-------------------------------------------------------------------
//	FormatIndex
//-------------------------------------------------------------------
uint Shader_LightAll::FormatIndex(uint index)const
{
	CHECK( index < ARRAY_COUNT(m_cbLightAll.GetData().pointLights) );
	index = math::clamp( index, 0 , ARRAY_COUNT(m_cbLightAll.GetData().pointLights) - 1 ); 
	return index;
}

/****************************************************************************************
	Shader_ShadowMap
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_ShadowMap::Shader_ShadowMap() : Shader( path::shader(L"shadowMap_vs"), path::shader(L"shadowMap_ps"), &Vertex_P3::GetLayout() )
{
	Register( &m_cbWorld );
	Register( &m_cbLightTransform );
}
//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_ShadowMap::Use()
{
	m_cbWorld.SetOnShader( 0, eShaderType::VertexShader );
	m_cbLightTransform.SetOnShader( 1, eShaderType::VertexShader );
}
//-------------------------------------------------------------------
//	SetModelWorld
//-------------------------------------------------------------------
void Shader_ShadowMap::SetModelWorld(const math::Mtx44& world)
{
	auto& data = m_cbWorld.GetData();
	data.world = world;
}
//-------------------------------------------------------------------
//	GetLightTransform
//-------------------------------------------------------------------
ConstantBuffer_LightTransform& Shader_ShadowMap::GetLightTransform()
{
	return m_cbLightTransform.GetData();
}

/****************************************************************************************
	Shader_Refraction
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Shader_Refraction::Shader_Refraction() : Shader( path::shader(L"refraction_vs"), path::shader(L"refraction_ps"), &Vertex_P3N3Tg3Bn3T2::GetLayout() )
{
	Register( &m_cbWvpInvTransp );
	Register( &m_cbTextureTransform );
	Register( &m_cbRefraction );

	m_pNormalMap = nullptr;
	m_pRefractionMap = nullptr;
}

//-------------------------------------------------------------------
//	Use
//-------------------------------------------------------------------
void Shader_Refraction::Use()
{
	m_cbWvpInvTransp.SetOnShader( 0, eShaderType::VertexShader );
	m_cbTextureTransform.SetOnShader( 1, eShaderType::VertexShader );

	m_cbRefraction.SetOnShader( 0, eShaderType::PixelShader );

	m_pNormalMap->Bind( 0 );
	m_pRefractionMap->Bind( 1 );
}
//-------------------------------------------------------------------
//	GetTransform
//-------------------------------------------------------------------
ConstantBuffer_WvpInvTransp& Shader_Refraction::GetTransform()
{
	return m_cbWvpInvTransp.GetData();
}
//-------------------------------------------------------------------
//	GetTextureTransform
//-------------------------------------------------------------------
ConstantBuffer_TextureTransform& Shader_Refraction::GetTextureTransform()
{
	return m_cbTextureTransform.GetData();
}
//-------------------------------------------------------------------
//	GetRefraction
//-------------------------------------------------------------------
ConstantBuffer_Refraction& Shader_Refraction::GetRefraction()
{
	return m_cbRefraction.GetData();
}
//-------------------------------------------------------------------
//	SetNormalMap
//-------------------------------------------------------------------
void Shader_Refraction::SetNormalMap(ITexture* pTexture)
{
	m_pNormalMap = pTexture;
}
//-------------------------------------------------------------------
//	SetRefractionMap
//-------------------------------------------------------------------
void Shader_Refraction::SetRefractionMap(ITexture* pTexture)
{
	m_pRefractionMap = pTexture;
}
