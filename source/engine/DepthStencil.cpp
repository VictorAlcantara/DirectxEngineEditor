#include "DepthStencil.h"
#include "../engine/CoreInterface.h"
#include "../engine/Exception.h"
#include "api_DepthStencil.h"

uint DepthStencil::s_idControl = 1;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
DepthStencil::DepthStencil()
{
	m_initialized = false;
	m_id = s_idControl++;
}
//-------------------------------------------------------------------
DepthStencil::~DepthStencil()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void DepthStencil::Shutdown()
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
//	Initialize
//-------------------------------------------------------------------
void DepthStencil::Initialize(uint width, uint height)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_hTexture = g_pResource->GetTextureMngr()->CreateDepthStencilTexture( "depthStencil_" + string::intToStr(m_id), width, height );

	platform::depthStencil::createDepthStencil( m_hTexture->GetApi(), m_api );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	GetApi
//-------------------------------------------------------------------
platform::DepthStencilApi DepthStencil::GetApi()
{
	return m_api;
}
//-------------------------------------------------------------------
//	GetTexture
//-------------------------------------------------------------------
Handle<ITexture> DepthStencil::GetTexture()
{
	ASSERT( "Texture is probably not bound as shader resource" );
	return m_hTexture;
}
