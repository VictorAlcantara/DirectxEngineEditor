#include "../engine/RenderTarget.h"
#include "../engine/CoreInterface.h"
#include "../engine/Exception.h"
#include "../engine/Pointer.h"
#include "DepthStencil.h"
#include "../engine/Texture.h"
#include "../engine/CoreInterface.h"

#ifdef WIN32_DX11

uint RenderTarget::s_idControl = 1;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
RenderTarget::RenderTarget()
{
	m_id = s_idControl++;
	m_initialized = false;
}
//-------------------------------------------------------------------
RenderTarget::~RenderTarget()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void RenderTarget::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_pDepthStencil->Shutdown();
	m_pDepthStencil = nullptr;

	if ( m_hTexture )
		g_pResource->GetTextureMngr()->Remove( m_hTexture->GetName() );

	platform::release( m_api );

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void RenderTarget::Initialize(uint width, uint height, eColorFormat format)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_hTexture = g_pResource->GetTextureMngr()->CreateRenderTargetTexture( "RenderTarget_" + string::intToStr(m_id), width, height, format );

	HRESULT hResult = g_pRenderer->GetApi().pDevice->CreateRenderTargetView( m_hTexture.Get().GetApi().pTexture, nullptr, &m_api.pRenderTargetView );
	if ( FAILED(hResult) )
	{
		THROW( "RenderTarget::Initialize() Failed creating render target view" );
	}

	m_pDepthStencil = new DepthStencil;
	m_pDepthStencil->Initialize( width, height );

	m_viewport.width = width;
	m_viewport.height = height;

	m_initialized = true;
}
//-------------------------------------------------------------------
void RenderTarget::Initialize(const math::Vec2& dimension, eColorFormat format)
{
	Initialize( (uint)dimension.x, (uint)dimension.y, format );
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void RenderTarget::Clear(float r, float g, float b, float a)
{
	float color[4] = { r, g, b, a };

	if ( m_api.pRenderTargetView )
		g_pRenderer->GetApi().pDeviceContext->ClearRenderTargetView( m_api.pRenderTargetView, color );
	g_pRenderer->GetApi().pDeviceContext->ClearDepthStencilView( m_pDepthStencil->GetApi().pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}
//-------------------------------------------------------------------
//	Bind
//-------------------------------------------------------------------
void RenderTarget::Bind()
{
	g_pRenderer->GetApi().pDeviceContext->OMSetRenderTargets( 1, &m_api.pRenderTargetView, m_pDepthStencil->GetApi().pDepthStencilView );
	g_pRenderer->SetViewport( m_viewport );
}
//-------------------------------------------------------------------
//	Unbind
//-------------------------------------------------------------------
void RenderTarget::Unbind()
{
	g_pRenderer->UseDefaultRenderTargetAndViewport();
}
//-------------------------------------------------------------------
//	GetTexture
//-------------------------------------------------------------------
Handle<ITexture> RenderTarget::GetTexture()
{
	return m_hTexture;
}
//-------------------------------------------------------------------
//	GetApi
//-------------------------------------------------------------------
platform::RenderTargetApi& RenderTarget::GetApi()
{
	return m_api;
}

#endif
