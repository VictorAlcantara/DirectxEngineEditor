#include "api_DepthStencil.h"

#ifdef WIN32_DX11

#include "../engine/CoreInterface.h"

//-------------------------------------------------------------------
//	createDepthStencil
//-------------------------------------------------------------------
void platform::depthStencil::createDepthStencil(platform::TextureApi& textureApi, platform::DepthStencilApi& api)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HRESULT hResult = g_pRenderer->GetApi().pDevice->CreateDepthStencilView( textureApi.pTexture, &depthStencilViewDesc, &api.pDepthStencilView );
	if ( FAILED(hResult) )
	{
		THROW( "createDepthStencil() failed creating view for depth stencil" );
	}
}

#endif
