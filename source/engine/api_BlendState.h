#pragma once

#include "Platform.h"
#include "RenderUtil.h"

namespace platform
{
	namespace blendState
	{
		void createBlendMode(eBlendMode, BlendStateApi&);
		void setBlendMode(BlendStateApi&);
	};
};
