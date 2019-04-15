#include "api_Texture.h"

#ifdef DIRECTX11

#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"

/****************************************************************************************
	Helper
*****************************************************************************************/

//-------------------------------------------------------------------
//	createTexture2dAndView
//-------------------------------------------------------------------
void createTexture2dAndView(D3D11_TEXTURE2D_DESC& desc, D3D11_SUBRESOURCE_DATA* pInitialData, platform::TextureApi& api)
{
	bool hasResourceViewDesc = false;
	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	
	HRESULT hResult = g_pRenderer->GetApi().pDevice->CreateTexture2D( &desc, pInitialData, &api.pTexture );

	if ( FAILED(hResult) )
		THROW( "createTexture2dAndView() failed creating texture" );

	if ( desc.MiscFlags == D3D11_RESOURCE_MISC_GENERATE_MIPS )
	{
		resourceViewDesc.Format = desc.Format;
		resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Texture2D.MostDetailedMip = 0;
		resourceViewDesc.Texture2D.MipLevels = -1;
		hasResourceViewDesc = true;
	}

	hResult = g_pRenderer->GetApi().pDevice->CreateShaderResourceView( api.pTexture, hasResourceViewDesc ? &resourceViewDesc : nullptr, &api.pTextureView );

	if ( FAILED(hResult) )
		THROW( "createTexture2dAndView() failed creating texture view" );
}
//-------------------------------------------------------------------
//	formatToDxFormat
//-------------------------------------------------------------------
DXGI_FORMAT formatToDxFormat(eColorFormat format)
{
	switch ( format )
	{
		case eColorFormat::Rgba_8_Uint: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case eColorFormat::Rgb_32_Float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case eColorFormat::Rgba_32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case eColorFormat::Rg_32_Float: return DXGI_FORMAT_R32G32_FLOAT;
		default:
			THROW( "formatToDxFormat() unrecognized format" );
	}
}

/****************************************************************************************
	Namespace implementation
*****************************************************************************************/

//-------------------------------------------------------------------
//	bind
//-------------------------------------------------------------------
void platform::texture::bind(uint slot, TextureApi& api)
{
	g_pRenderer->GetApi().pDeviceContext->PSSetShaderResources( slot, 1, &api.pTextureView );
}
//-------------------------------------------------------------------
//	copy
//-------------------------------------------------------------------
void platform::texture::copy(TextureApi& src, TextureApi& dest)
{
	#ifdef DEBUG_MODE
		D3D11_TEXTURE2D_DESC srcDesc;
		D3D11_TEXTURE2D_DESC destDesc;
		src.pTexture->GetDesc( &srcDesc );
		dest.pTexture->GetDesc( &destDesc );
	#endif

	g_pRenderer->GetApi().pDeviceContext->CopyResource( dest.pTexture, src.pTexture );
}
//-------------------------------------------------------------------
//	createTexture2d
//-------------------------------------------------------------------
void platform::texture::createTexture2d(const uchar* pData, uint width, uint height, eColorFormat format, TextureApi& api)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = formatToDxFormat(format);
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	auto* pTextureData = pData ? new D3D11_SUBRESOURCE_DATA : nullptr;

	if ( pData )
	{
		pTextureData->SysMemSlicePitch = 0;
		pTextureData->pSysMem = (const void**)pData;
		pTextureData->SysMemPitch = width * 4;
	}

	createTexture2dAndView( textureDesc, pTextureData, api );

	g_pRenderer->GetApi().pDeviceContext->GenerateMips( api.pTextureView );
}
//-------------------------------------------------------------------
//	createRenderTargetTexture
//-------------------------------------------------------------------
void platform::texture::createRenderTargetTexture(uint width, uint height, eColorFormat format, TextureApi& api)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = 0;
	desc.Format = formatToDxFormat(format);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	createTexture2dAndView( desc, nullptr, api );
}
//-------------------------------------------------------------------
//	createDepthStencilTexture
//-------------------------------------------------------------------
void platform::texture::createDepthStencilTexture(uint width, uint height, TextureApi& api)
{
	D3D11_TEXTURE2D_DESC bufferDesc = {};
	bufferDesc.ArraySize = 1;
	bufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // | (usage == eDepthStencilUsage::DepthTexture ? D3D11_BIND_SHADER_RESOURCE : 0);
	bufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.MipLevels = 1;
	bufferDesc.MiscFlags = 0;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;

	HRESULT hResult = g_pRenderer->GetApi().pDevice->CreateTexture2D( &bufferDesc, nullptr, &api.pTexture );
	if ( FAILED(hResult) )
	{
		THROW( "createDepthStencilTexture() failed creating depth stencil texture" );
	}

	//if ( usage == eDepthStencilUsage::DepthTexture )
	//{
	//	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	//	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//	shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32_TYPELESS;
	//	shaderResourceViewDesc.Texture2D.MipLevels = bufferDesc.MipLevels;
	//	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	//	hResult = g_pRenderer->GetApi().pDevice->CreateShaderResourceView( api.pTexture, &shaderResourceViewDesc, &api.pTextureView );

	//	if ( FAILED(hResult) )
	//	{
	//		THROW( "createDepthStencilTexture() failed creating shader resource view" );
	//	}
	//}
}

#endif
