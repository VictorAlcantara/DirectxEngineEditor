#pragma once

#include "Platform.h"
#include "../engine/Class.h"
#include "RenderUtil.h"

class Renderer;

class Rasterizer : public NonCopyable
{
	friend class Renderer;

	public:
		~Rasterizer();

	private:
		Rasterizer();

		void Initialize();
		void Shutdown();

		void SetCullMode(eCullMode);
		void SetFillMode(eFillMode);
		void SetCullFillMode(eCullMode, eFillMode);

		bool m_initialized;
		platform::RasterizerApi m_rasterizerApiByMode[(uint)eCullMode::Count][(uint)eFillMode::Count];
		eCullMode m_currentCullMode;
		eFillMode m_currentFillMode;
};
