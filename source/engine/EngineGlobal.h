#pragma once

#include "Type.h"
#include "../engine/String.h"

enum class eTransformation
{
	None = 0,
	Collision,
	Physics,
	Render,
};

// TODO: change name to engineGlobal
namespace engineDefault
{
	const uint FPS_MAX = 10000;

	namespace samplerState
	{
		const uint SAMPLER_STATE_SLOT_MAX = 8;
	}

	namespace shader
	{
		// Would love to define these constants in a cpp file, however they are used for array size
		const uint BLUR_WEIGHT_ARRAY_TYPE_COUNT = 4;
		const uint BLUR_WEIGHT_ARRAY_SIZE = 16;
		const uint BLUR_WEIGHT_ARRAY_MAX = BLUR_WEIGHT_ARRAY_TYPE_COUNT * BLUR_WEIGHT_ARRAY_SIZE;

		const uint MULTI_TEXTURE_ARRAY_COUNT = 8;

		const uint MAX_LIGHTS = 8;
	}

	namespace entity
	{
		extern const uint ID_INVALID;
	}

	namespace model
	{
		extern const uint MODEL_MANAGER_TABLE_SIZE;
		extern const uint MODEL_HANDLE_MANAGER_SLOT_SIZE;
		extern const uint MODEL_REPOSITORY_TABLE_LAYOUT_SIZE;
	}
}
