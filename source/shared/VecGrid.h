#pragma once

#include "../engine/Math.h"
#include "../engine/String.h"

struct VecGrid
{
	int x;
	int y;
	int z;

	VecGrid();
	explicit VecGrid(int xyz);
	explicit VecGrid(int x, int y, int z);
	VecGrid(const math::Vec3&);

	math::Vec3 AsVec3()const;
	String AsString()const;

	VecGrid& operator+=(const VecGrid&);
	VecGrid& operator-=(const VecGrid&);
	VecGrid& operator*=(int);
	VecGrid& operator/=(int);

	int& operator[](uint);
	const int& operator[](uint)const;
};

VecGrid operator+(const VecGrid&, const VecGrid&);
VecGrid operator-(const VecGrid&, const VecGrid&);
VecGrid operator*(const VecGrid&, int);
VecGrid operator/(const VecGrid&, int);

const VecGrid AXES_GRID[3] = {
	VecGrid(1, 0, 0),
	VecGrid(0, 1, 0),
	VecGrid(0, 0, 1),
};

VecGrid strToVecGrid(const String&);

inline bool operator==(const VecGrid& v0, const VecGrid& v1)
{
	return 
		v0.x == v1.x &&
		v0.y == v1.y &&
		v0.z == v1.z;
}

inline bool operator!=(const VecGrid& v0, const VecGrid& v1)
{
	return !(v0 == v1);
}
