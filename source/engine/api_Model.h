#pragma once

#include "Platform.h"
#include "RenderUtil.h"

namespace platform
{
	namespace model
	{
		struct ModelInitializationResult
		{
			bool initialized;
			uint vertexCount;
			uint indexCount;
			uint vertexTypeSize;
			bool usesIndex;

			ModelInitializationResult() : initialized(false), vertexCount(0), indexCount(0), vertexTypeSize(0), usesIndex(false) {}
		};

		ModelInitializationResult initializeModel(ModelApi&, eTopology, const void* pVertices, uint vertexTypeSize, uint vertexCount, uint* pIndices, uint indexCount);
	}
}
