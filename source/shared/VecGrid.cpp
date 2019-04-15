#include "../shared/VecGrid.h"

//=============================================================================
//	VecGrid
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
VecGrid::VecGrid() : VecGrid(0)
{
}
//-------------------------------------------------------------------
VecGrid::VecGrid(int xyz) : VecGrid(xyz, xyz, xyz)
{
}
//-------------------------------------------------------------------
VecGrid::VecGrid(int x, int y, int z) : x(x), y(y), z(z)
{
}
//-------------------------------------------------------------------
VecGrid::VecGrid(const math::Vec3& v) : VecGrid((int)v.x, (int)v.y, (int)v.z)
{
}
//-------------------------------------------------------------------
//	AsVec3
//-------------------------------------------------------------------
math::Vec3 VecGrid::AsVec3()const
{
	return math::Vec3( (float)x, (float)y, (float)z );
}
//-------------------------------------------------------------------
//	AsString
//-------------------------------------------------------------------
String VecGrid::AsString()const
{
	return string::intToStr(x) + " " +
		string::intToStr(y) + " " +
		string::intToStr(z);
}
//-------------------------------------------------------------------
//	operator+=
//-------------------------------------------------------------------
VecGrid& VecGrid::operator+=(const VecGrid& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
//-------------------------------------------------------------------
//	operator-=
//-------------------------------------------------------------------
VecGrid& VecGrid::operator-=(const VecGrid& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
//-------------------------------------------------------------------
//	operator*=
//-------------------------------------------------------------------
VecGrid& VecGrid::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
	return *this;
}
//-------------------------------------------------------------------
//	operator/=
//-------------------------------------------------------------------
VecGrid& VecGrid::operator/=(int i)
{
	x /= i;
	y /= i;
	z /= i;
	return *this;
}
//-------------------------------------------------------------------
//	operator[]
//-------------------------------------------------------------------
int& VecGrid::operator[](uint index)
{
	return (&x)[index];
}
//-------------------------------------------------------------------
const int& VecGrid::operator[](uint index)const
{
	return (&x)[index];
}

//=============================================================================
//	Helper function
//=============================================================================

//-------------------------------------------------------------------
//	operator+
//-------------------------------------------------------------------
VecGrid operator+(const VecGrid& u, const VecGrid& v)
{
	VecGrid result = u;
	result += v;
	return result;
}
//-------------------------------------------------------------------
//	operator-
//-------------------------------------------------------------------
VecGrid operator-(const VecGrid& u, const VecGrid& v)
{
	VecGrid result = u;
	result -= v;
	return result;
}
//-------------------------------------------------------------------
//	operator*
//-------------------------------------------------------------------
VecGrid operator*(const VecGrid& u, int v)
{
	VecGrid result = u;
	result *= v;
	return result;
}
//-------------------------------------------------------------------
//	operator/
//-------------------------------------------------------------------
VecGrid operator/(const VecGrid& u, int v)
{
	VecGrid result = u;
	result /= v;
	return result;
}
//-------------------------------------------------------------------
//	strToVecGrid
//-------------------------------------------------------------------
VecGrid strToVecGrid(const String& s)
{
	VecGrid result;
	int* vector[] = { &result.x, &result.y, &result.z };
	string::strToIntArray( s, vector, 3 );
	return result;
}
