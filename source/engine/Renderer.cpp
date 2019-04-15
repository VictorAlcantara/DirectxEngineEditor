#include "Renderer.h"
#include "../engine/Exception.h"
#include "../engine/Model.h"
#include "../engine/Shader.h"
#include "Rasterizer.h"
#include "DepthStencilState.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "DepthStencil.h"
#include "../engine/Vertex.h"

#ifdef WIN32_DX11

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Renderer::Renderer()
{
	m_initialized = false;
	m_uiClientHeight = 0;
	m_uiClientWidth = 0;
}
//-------------------------------------------------------------------
Renderer::~Renderer()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Renderer::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	if ( m_api.pSwapChain )
		m_api.pSwapChain->SetFullscreenState( FALSE, nullptr );

	m_pSamplerState->Shutdown();
	m_pSamplerState = nullptr;
	m_pRasterizer->Shutdown();
	m_pRasterizer = nullptr;
	m_pDepthStencilState->Shutdown();
	m_pDepthStencilState = nullptr;
	m_pBlendState->Shutdown();
	m_pBlendState = nullptr;
	m_pDepthStencil->Shutdown();
	m_pDepthStencil = nullptr;

	platform::release( m_api );

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Renderer::Initialize(platform::ApplicationApi& applicationApi)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	HRESULT hResult;
	RECT clientArea;
	HWND hWnd = applicationApi.hWnd;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	D3D_FEATURE_LEVEL featureLevel = {};
	ID3D11Texture2D* pBackBuffer;
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	GetClientRect( hWnd, &clientArea );

	m_uiClientWidth = clientArea.right - clientArea.left;
	m_uiClientHeight = clientArea.bottom - clientArea.top;

	// 1. Create device and swap chain
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_uiClientWidth;
	swapChainDesc.BufferDesc.Height = m_uiClientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	hResult = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
		D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_api.pSwapChain, &m_api.pDevice, nullptr, &m_api.pDeviceContext );	
	if ( FAILED(hResult) )
	{
		THROW( "Renderer::Initialize() Failed creating DirectX 11 device" );
	}

	pDevice = m_api.pDevice;
	pDeviceContext = m_api.pDeviceContext;

	// 2. Create backbuffer
	hResult = m_api.pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer );
	if ( FAILED(hResult) )
	{
		THROW( "Renderer::Initialize() Failed getting texture from swap chain" );
	}

	hResult = pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_api.pRenderTargetView );
	if ( FAILED(hResult) )
	{
		THROW( "Renderer::Initialize() Failed creating render target view" );
	}

	platform::safeComRelease( pBackBuffer );

	m_pDepthStencil = new DepthStencil;
	m_pDepthStencil->Initialize( m_uiClientWidth, m_uiClientHeight );

	UseDefaultRenderTargetAndViewport();

	m_pDepthStencilState = new DepthStencilState;
	m_pDepthStencilState->Initialize();
	m_pDepthStencilState->SetDepthOperation( eDepthOperation::Less );

	// 3. Create rasterizer state
	m_pRasterizer = new Rasterizer;
	m_pRasterizer->Initialize();
	m_pRasterizer->SetCullFillMode( eCullMode::None, eFillMode::Solid );	

	m_pSamplerState = new SamplerState;
	m_pSamplerState->Initialize();

	m_pBlendState = new BlendState;
	m_pBlendState->Initialize();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	EnableDepthWrite
//-------------------------------------------------------------------
void Renderer::EnableDepthWrite(bool b)
{
	m_pDepthStencilState->EnableDepthWrite( b );
}
//-------------------------------------------------------------------
//	SetDepthOperation
//-------------------------------------------------------------------
void Renderer::SetDepthOperation(eDepthOperation depthOp)
{
	m_pDepthStencilState->SetDepthOperation( depthOp );
}
//-------------------------------------------------------------------
//	SetCullMode
//-------------------------------------------------------------------
void Renderer::SetCullMode(eCullMode mode)
{
	m_pRasterizer->SetCullMode( mode );
}
//-------------------------------------------------------------------
//	SetFillMode
//-------------------------------------------------------------------
void Renderer::SetFillMode(eFillMode mode)
{
	m_pRasterizer->SetFillMode( mode );
}
//-------------------------------------------------------------------
//	BeginScene
//-------------------------------------------------------------------
void Renderer::BeginScene(float red, float green, float blue)
{
	float color[4] = { red, green, blue, 1.0f };

	m_api.pDeviceContext->ClearRenderTargetView( m_api.pRenderTargetView, color );
	m_api.pDeviceContext->ClearDepthStencilView( m_pDepthStencil->GetApi().pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}
//-------------------------------------------------------------------
//	EndScene
//-------------------------------------------------------------------
void Renderer::EndScene()
{
	m_api.pSwapChain->Present( 0, 0 );
}
//-------------------------------------------------------------------
//	UseDefaultRenderTargetAndViewport
//-------------------------------------------------------------------
void Renderer::UseDefaultRenderTargetAndViewport()
{
	m_api.pDeviceContext->OMSetRenderTargets( 1, &m_api.pRenderTargetView, m_pDepthStencil->GetApi().pDepthStencilView );
	SetViewport( GetWindowViewport() );
}
//-------------------------------------------------------------------
//	SetSamplerState
//-------------------------------------------------------------------
void Renderer::SetSamplerState(eTextureAddress addressU, eTextureAddress addressV, eTextureFilter filter, uint slot)
{
	m_pSamplerState->SetSamplerState( addressU, addressV, filter, slot );
}
//-------------------------------------------------------------------
void Renderer::SetSamplerState(const TextureSampler& sampler, uint slot)
{
	SetSamplerState( sampler.u, sampler.v, sampler.filter, slot );
}
//-------------------------------------------------------------------
//	SetBlendMode
//-------------------------------------------------------------------
void Renderer::SetBlendMode(eBlendMode mode)
{
	m_pBlendState->SetMode( mode );
}
//-------------------------------------------------------------------
//	SetViewport
//-------------------------------------------------------------------
void Renderer::SetViewport(const math::ViewportInfo& viewport)
{
	D3D11_VIEWPORT d3dViewport = {};
	d3dViewport.Width = (float)viewport.width;
	d3dViewport.Height = (float)viewport.height;
	d3dViewport.MinDepth = 0.0f;
	d3dViewport.MaxDepth = 1.0f;
	d3dViewport.TopLeftX = (float)viewport.left;
	d3dViewport.TopLeftY = (float)viewport.top;

	m_api.pDeviceContext->RSSetViewports( 1, &d3dViewport );
}
//-------------------------------------------------------------------
//	GetApi
//-------------------------------------------------------------------
platform::RendererApi& Renderer::GetApi()
{
	return m_api;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Renderer::Render(Model* pModel, Shader* pShader)
{
	CHECK( pModel->GetVertexLayout().IsEqual(pShader->GetVertexLayout()) );

	uint stride = pModel->GetVertexTypeSize();
	uint offset = 0;
	auto pDeviceContext = m_api.pDeviceContext;

	pDeviceContext->IASetVertexBuffers( 0, 1, &pModel->GetApi().pVertexBuffer, &stride, &offset );

	if ( pModel->UsesIndex() )
		pDeviceContext->IASetIndexBuffer( pModel->GetApi().pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	pDeviceContext->IASetPrimitiveTopology( pModel->GetApi().topology );

	pShader->Use();

	pDeviceContext->IASetInputLayout( pShader->GetApi().pLayout );
	pDeviceContext->VSSetShader( pShader->GetApi().pVertexShader, nullptr, 0 );
	pDeviceContext->PSSetShader( pShader->GetApi().pPixelShader, nullptr, 0 );

	if (pModel->UsesIndex())
		pDeviceContext->DrawIndexed(pModel->GetIndexCount(), 0, 0);
	else
		pDeviceContext->Draw( pModel->GetVertexCount(), 0 );

	pShader->EndUse();
}
//-------------------------------------------------------------------
//	GetWindowViewport
//-------------------------------------------------------------------
math::ViewportInfo Renderer::GetWindowViewport()const
{
	math::ViewportInfo result;
	
	result.width = m_uiClientWidth;
	result.height = m_uiClientHeight;
	
	return result;
}



#endif