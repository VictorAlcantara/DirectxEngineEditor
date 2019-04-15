#pragma once

#include "Platform.h"
#include "RenderUtil.h"

namespace platform
{
	namespace texture
	{
		void createTexture2d(const uchar* pData, uint width, uint height, eColorFormat, TextureApi&);
		void createRenderTargetTexture(uint width, uint height, eColorFormat, TextureApi&);
		void createDepthStencilTexture(uint width, uint height, TextureApi&);
		void bind(uint slot, TextureApi&);
		void copy(TextureApi& src, TextureApi& dest);
	};
};
