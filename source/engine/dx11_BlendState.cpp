#include "api_BlendState.h"
#include "../engine/CoreInterface.h"

#ifdef WIN32_DX11

//-------------------------------------------------------------------
//	createDescForMode
//-------------------------------------------------------------------
D3D11_BLEND_DESC createDescForMode(eBlendMode mode)
{
	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	if ( mode == eBlendMode::Disabled )
	{
		desc.RenderTarget[0].BlendEnable = FALSE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else if ( mode == eBlendMode::Alpha )
	{
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else if ( mode == eBlendMode::Additive )
	{
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	return desc;
}
//-------------------------------------------------------------------
//	createBlendMode
//-------------------------------------------------------------------
void platform::blendState::createBlendMode(eBlendMode mode, BlendStateApi& api)
{
	HRESULT hResult = g_pRenderer->GetApi().pDevice->CreateBlendState( &createDescForMode(mode), &api.pBlendState );
	
	if ( FAILED(hResult) )
	{
		THROW( "BlendState::Initialize() Failed creating blend state" );
	}
}
//-------------------------------------------------------------------
//	setBlendMode
//-------------------------------------------------------------------
void platform::blendState::setBlendMode(BlendStateApi& api)
{
	g_pRenderer->GetApi().pDeviceContext->OMSetBlendState( api.pBlendState, nullptr, 0xffffffff );
}

#endif
