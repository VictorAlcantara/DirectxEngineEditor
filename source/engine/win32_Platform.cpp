#include "Platform.h"

#ifdef WIN32_DX11

//-------------------------------------------------------------------
//	release
//-------------------------------------------------------------------
void platform::release(RendererApi& api)
{
	safeComRelease( api.pRenderTargetView );
	safeComRelease( api.pDeviceContext );
	safeComRelease( api.pDevice );
	safeComRelease( api.pSwapChain );
}
//-------------------------------------------------------------------
void platform::release(RasterizerApi& api)
{
	safeComRelease( api.pRasterizerState );
}
//-------------------------------------------------------------------
void platform::release(DepthStencilApi& api)
{
	safeComRelease( api.pDepthStencilView );
}
//-------------------------------------------------------------------
void platform::release(DepthStencilStateApi& api)
{
	safeComRelease( api.pDepthStencilState );
}
//-------------------------------------------------------------------
void platform::release(ModelApi& api)
{
	safeComRelease( api.pVertexBuffer );
	safeComRelease( api.pIndexBuffer );
}
//-------------------------------------------------------------------
void platform::release(SamplerStateApi& api)
{
	safeComRelease( api.pSamplerState );
}
//-------------------------------------------------------------------
void platform::release(TextureApi& api)
{
	safeComRelease( api.pTextureView );
	safeComRelease( api.pTexture );
}
//-------------------------------------------------------------------
void platform::release(RenderTargetApi& api)
{
	safeComRelease( api.pRenderTargetView );
}
//-------------------------------------------------------------------
void platform::release(ShaderApi& api)
{
	safeComRelease( api.pLayout );
	safeComRelease( api.pPixelShader );
	safeComRelease( api.pVertexShader );
}
//-------------------------------------------------------------------
void platform::release(ConstantBufferApi& api)
{
	safeComRelease( api.pConstantBuffer );
}
//-------------------------------------------------------------------
void platform::release(BlendStateApi& api)
{
	safeComRelease( api.pBlendState );
}

#endif