#pragma once

#include "Platform.h"
#include "../engine/Vertex.h"

namespace platform
{
	namespace shader
	{
		void createVertexShaderAndLayout(const wchar* file, RendererApi& rendererApi, ShaderApi& shaderApi, const VertexLayout*);
		void createPixelShader(const wchar* file, RendererApi& rendererApi, ShaderApi& shaderApi);
		void vertexShaderClearResource();
		void pixelShaderClearResource();
	};
}
