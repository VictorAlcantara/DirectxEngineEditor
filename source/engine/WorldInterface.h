#pragma once

#include "../engine/Class.h"

struct WorldInfo
{
	String mapName;
};

class IWorld
{
	CLASS_TAG_INTERFACE;

	public:
		virtual ~IWorld() = default;
		virtual void Initialize(const WorldInfo*) = 0;
		virtual void Shutdown(WorldInfo* pOutInfo = nullptr) = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
};
