#pragma once

#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/Entity.h"

struct GridRenderData
{
	math::Vec3 pos;
	math::Euler angle;
	math::Vec4 color;
	bool visible;

	GridRenderData() : visible(true) {}
};

struct GridLockActionInfo
{
	Handle<Entity> hEntity;
	math::eAxis axisEnum;
	math::eSign axisSign;
	bool isShowingGrid;

	GridLockActionInfo() : axisEnum(math::eAxis::X), axisSign(math::POSITIVE), isShowingGrid(false) {}
};

