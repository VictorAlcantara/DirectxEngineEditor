#include "../engine/Texture.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"
#include "api_Texture.h"

/****************************************************************************************
	TextureBase
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
TextureBase::TextureBase()
{
	m_width = 0;
	m_height = 0;
	m_initialized = false;
}
//-------------------------------------------------------------------
TextureBase::~TextureBase()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void TextureBase::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	platform::release( m_api );

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Bind
//-------------------------------------------------------------------
void TextureBase::Bind(uint slot)
{
	platform::texture::bind( slot, m_api );
}
//-------------------------------------------------------------------
//	Copy
//-------------------------------------------------------------------
void TextureBase::Copy(ITexture* pTexture)
{
	CHECK( this != pTexture );

	platform::texture::copy( pTexture->GetApi(), m_api );
}
//-------------------------------------------------------------------
//	GetApi
//-------------------------------------------------------------------
platform::TextureApi& TextureBase::GetApi()
{
	return m_api;
}
//-------------------------------------------------------------------
//	SetName
//-------------------------------------------------------------------
void TextureBase::SetName(const HashString& name)
{
	m_name = name;
}
//-------------------------------------------------------------------
//	GetName
//-------------------------------------------------------------------
const HashString& TextureBase::GetName()const
{
	return m_name;
}

/****************************************************************************************
	Texture2d
*****************************************************************************************/

//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Texture2d::InitializeEmpty(uint width, uint height, eColorFormat format)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	platform::texture::createTexture2d( nullptr, width, height, format, m_api );

	m_width = width;
	m_height = height;

	m_initialized = true;
}
//-------------------------------------------------------------------
void Texture2d::Initialize(const uchar* pDataUnformatted, uint width, uint height, uint bitsPerPixel)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	uint size = width * height * 4;
	uchar* pData = new uchar[size];

	for ( uint i = 0; i < size / 4; i++ )
	{
		uint baseIndex = i * 4;
		uint baseIndexUnformatted = i * (bitsPerPixel / 8);

		pData[baseIndex + 0] = pDataUnformatted[baseIndexUnformatted + 0];
		pData[baseIndex + 1] = pDataUnformatted[baseIndexUnformatted + 1];
		pData[baseIndex + 2] = pDataUnformatted[baseIndexUnformatted + 2];
		pData[baseIndex + 3] = bitsPerPixel == 24 ? 255 : pDataUnformatted[baseIndexUnformatted + 3];
	}

	platform::texture::createTexture2d( pData, width, height, eColorFormat::Rgba_8_Uint, m_api );

	m_width = width;
	m_height = height;

	safeArrayRelease( pData );

	m_initialized = true;
}
//-------------------------------------------------------------------
void Texture2d::Initialize(const TgaInfo& info)
{
	Initialize( info.pData, info.width, info.height, info.bitsPerPixel );
}

/****************************************************************************************
	TextureRenderTarget
*****************************************************************************************/

//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void TextureRenderTarget::Initialize(uint width, uint height, eColorFormat format)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	platform::texture::createRenderTargetTexture(width, height, format, m_api);

	m_width = width;
	m_height = height;

	m_initialized = true;
}

/****************************************************************************************
	TextureDepthStencil
*****************************************************************************************/

//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void TextureDepthStencil::Initialize(uint width, uint height)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	platform::texture::createDepthStencilTexture(width, height, m_api);

	m_width = width;
	m_height = height;

	m_initialized = true;
}
