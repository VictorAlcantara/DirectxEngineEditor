#pragma once

#include "Platform.h"
#include "RenderUtil.h"

class Renderer;

class BlendState
{
	friend class Renderer;

	public:
		~BlendState();

	private:
		BlendState();

		void Initialize();
		void Shutdown();

		void SetMode(eBlendMode);

		bool m_initialized;
		platform::BlendStateApi m_api[(uint)eBlendMode::Count];
};
