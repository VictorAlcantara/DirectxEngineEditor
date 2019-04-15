#pragma once

#include "Platform.h"
#include "RenderUtil.h"

namespace platform
{
	namespace depthStencilState
	{
		uint createDepthStencilState(DepthStencilStateApi** states);
		void setOperation(eDepthOperation, bool allowWrite, DepthStencilStateApi* states, uint count);
	};
};
