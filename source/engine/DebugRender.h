#pragma once

#include "../engine/Math.h"
#include "../engine/Class.h"

struct DebugRender : public NonCopyable
{
	DebugRender();

	bool visible;
	math::Vec4 color4;
	String msg;
	
	uint GetId()const { return id; }

	private:
		uint id;
		static uint s_idControl;
};

struct Debug_Line : public DebugRender
{
	math::Vec3 point0;
	math::Vec3 point1;

	Debug_Line() {}
};

struct Debug_Cube : public DebugRender
{
	math::Vec3 pos;
	math::Vec3 scale;
	math::Mtx44 rotation;

	Debug_Cube() : scale(1.0f) {}
};

struct Debug_Sphere : public DebugRender
{
	math::Vec3 pos;
	math::Vec3 scale;

	Debug_Sphere() : scale(1.0f) {}
};

struct Debug_Cylinder : public DebugRender
{
	math::Vec3 pos;
	math::Vec3 scale;

	Debug_Cylinder() {}
};
