#pragma once

#include "Platform.h"
#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "RenderUtil.h"

class DepthStencil;
class ITexture;

class RenderTarget
{
	public:
		RenderTarget();
		~RenderTarget();

		void Initialize(uint width, uint height, eColorFormat);
		void Initialize(const math::Vec2& dimension, eColorFormat);
		void Shutdown();

		void Clear(float red, float green, float blue, float alpha = 1.0f);
		void Bind();
		void Unbind();

		Handle<ITexture> GetTexture();

		platform::RenderTargetApi& GetApi();

	private:
		static uint s_idControl;

		bool m_initialized;
		uint m_id;
		platform::RenderTargetApi m_api;
		math::ViewportInfo m_viewport;
		SimplePtr<DepthStencil> m_pDepthStencil;
		Handle<ITexture> m_hTexture;
};
