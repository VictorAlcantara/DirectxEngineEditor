#pragma once

#include "Platform.h"
#include "RenderUtil.h"
#include "../engine/Handle.h"

class ITexture;

class DepthStencil
{
	public:
		DepthStencil();
		~DepthStencil();

		void Initialize(uint width, uint height);
		void Shutdown();

		platform::DepthStencilApi GetApi();

		Handle<ITexture> GetTexture();

	private:
		static uint s_idControl;
		uint m_id;
		bool m_initialized;
		platform::DepthStencilApi m_api;
		Handle<ITexture> m_hTexture;
};
