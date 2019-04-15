#include "Rasterizer.h"
#include "../engine/Exception.h"
#include "../engine/Pointer.h"
#include "../engine/CoreInterface.h"


#ifdef DIRECTX11

//=============================================================================
//	Api specific helper function
//=============================================================================

D3D11_CULL_MODE cullModeToDx11CullMode(eCullMode);
D3D11_FILL_MODE fillModeToDx11FillMode(eFillMode);

//-------------------------------------------------------------------
//	dx11CreateRasterizerState
//-------------------------------------------------------------------
void dx11CreateRasterizerState(eCullMode cullMode, eFillMode fillMode, platform::RendererApi& rendererApi, platform::RasterizerApi& rasterizerApiArray)
{
	HRESULT hResult;
	D3D11_RASTERIZER_DESC rasterizerDesc = {};

	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = cullModeToDx11CullMode(cullMode);
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = fillModeToDx11FillMode(fillMode);
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hResult = rendererApi.pDevice->CreateRasterizerState( 
		&rasterizerDesc, 
		&rasterizerApiArray.pRasterizerState );

	if ( FAILED(hResult) )
	{
		THROW( "Failed creating rasterizer state" );
	}
}
//-------------------------------------------------------------------
//	cullModeToDx11CullMode
//-------------------------------------------------------------------
D3D11_CULL_MODE cullModeToDx11CullMode(eCullMode mode)
{
	switch ( mode )
	{
		case eCullMode::None:
			return D3D11_CULL_NONE;
		case eCullMode::Front:
			return D3D11_CULL_FRONT;
		case eCullMode::Back:
			return D3D11_CULL_BACK;
		default:
			THROW( "cullModeToDx11CullMode() invalid cull mode" );
	}
}
//-------------------------------------------------------------------
//	fillModeToDx11FillMode
//-------------------------------------------------------------------
D3D11_FILL_MODE fillModeToDx11FillMode(eFillMode mode)
{
	switch ( mode )
	{
		case eFillMode::Solid:
			return D3D11_FILL_SOLID;
		case eFillMode::Wireframe:
			return D3D11_FILL_WIREFRAME;
		default:
			THROW( "fillModeToDx11FillMode() invalid fill mode" );
	}
}

//=============================================================================
//	Rasterizer class
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Rasterizer::Rasterizer()
{
	m_initialized = false;
	m_currentFillMode = eFillMode::Solid;
}
//-------------------------------------------------------------------
Rasterizer::~Rasterizer()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Rasterizer::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( uint cull = 0; cull < (uint)eCullMode::Count; cull++ )
	{
		for ( uint fill = 0; fill < (uint)eFillMode::Count; fill++ )
		{
			if ( m_initialized )
				platform::release( m_rasterizerApiByMode[cull][fill] );
			m_rasterizerApiByMode[cull][fill] = platform::RasterizerApi();
		}
	}

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Rasterizer::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	for ( uint cull = 0; cull < (uint)eCullMode::Count; cull++ )
	{
		for ( uint fill = 0; fill < (uint)eFillMode::Count; fill++ )
		{
			dx11CreateRasterizerState( (eCullMode)cull, (eFillMode)fill, g_pRenderer->GetApi(), m_rasterizerApiByMode[cull][fill] );
		}
	}

	SetCullFillMode( eCullMode::None, eFillMode::Solid );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	SetCullMode
//-------------------------------------------------------------------
void Rasterizer::SetCullMode(eCullMode mode)
{
	SetCullFillMode( mode, m_currentFillMode );
}
//-------------------------------------------------------------------
//	SetFillMode
//-------------------------------------------------------------------
void Rasterizer::SetFillMode(eFillMode mode)
{
	SetCullFillMode( m_currentCullMode, mode );
}
//-------------------------------------------------------------------
//	SetFillMode
//-------------------------------------------------------------------
void Rasterizer::SetCullFillMode(eCullMode cullMode, eFillMode fillMode)
{
	if ( m_currentCullMode == cullMode && m_currentFillMode == fillMode )
		return;

	m_currentCullMode = cullMode;
	m_currentFillMode = fillMode;

	g_pRenderer->GetApi().pDeviceContext->RSSetState( m_rasterizerApiByMode[(uint)cullMode][(uint)fillMode].pRasterizerState );
}

#endif