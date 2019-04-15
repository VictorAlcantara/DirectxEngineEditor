#pragma once

#include "Platform.h"
#include "CoreModule.h"
#include "../engine/Pointer.h"
#include "RenderUtil.h"
#include "../engine/Math.h"

class Model;
class Shader;
class Rasterizer;
class DepthStencilState;
class SamplerState;
class BlendState;
class DepthStencil;

// TODO: decouple api call from class definition :|
class Renderer
{
	CORE_MODULE;

	public:
		~Renderer();

		void Initialize(platform::ApplicationApi&);
		void Shutdown();

		void EnableDepthWrite(bool);
		void SetDepthOperation(eDepthOperation);
		void SetCullMode(eCullMode);
		void SetFillMode(eFillMode);
		void BeginScene(float red, float green, float blue);
		void EndScene();
		void UseDefaultRenderTargetAndViewport();
		void SetSamplerState(eTextureAddress addressU, eTextureAddress addressV, eTextureFilter, uint slot);
		void SetSamplerState(const TextureSampler&, uint slot);
		void SetBlendMode(eBlendMode);
		void SetViewport(const math::ViewportInfo&);

		void Render(Model*, Shader*);

		math::ViewportInfo GetWindowViewport()const;

		platform::RendererApi& GetApi();

	private:
		Renderer();

		bool m_initialized;
		platform::RendererApi m_api;
		SimplePtr<Rasterizer> m_pRasterizer;
		SimplePtr<DepthStencilState> m_pDepthStencilState;
		SimplePtr<SamplerState> m_pSamplerState;
		SimplePtr<BlendState> m_pBlendState;
		SimplePtr<DepthStencil> m_pDepthStencil;

		uint m_uiClientWidth;
		uint m_uiClientHeight;
};
