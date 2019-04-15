#pragma once

#include "Type.h"

namespace math
{
	struct Vec2;
	struct Vec3;
	struct Vec4;

	/************************************************************************************
		VEC2
	*************************************************************************************/

	struct Vec2
	{
		float x;
		float y;

		Vec2();
		explicit Vec2(float xy);
		explicit Vec2(float _x, float _y);

		math::Vec3 Xyz(float z = 0.0f)const;

		Vec2 operator-()const;
		Vec2& operator+=(const Vec2&);
		Vec2& operator-=(const Vec2&);
		Vec2& operator*=(float value);
		float& operator[](uint);
		const float& operator[](uint)const;

		static uint GetComponentCount() { return 2; }
	};

	/************************************************************************************
		VEC3
	*************************************************************************************/

	struct Vec3
	{
		float x;
		float y;
		float z;

		Vec3();
		explicit Vec3(float xyz);
		explicit Vec3(const Vec2&, float _z);
		explicit Vec3(float _x, float _y, float _z);

		Vec2 Xy()const;
		Vec4 Xyzw(float w)const;
		math::Vec3 Abs()const;
		math::Vec3 Inv()const;

		Vec3 operator-()const;
		Vec3& operator-=(const Vec3&);
		Vec3& operator+=(const Vec3&);
		Vec3& operator*=(float value);
		Vec3& operator/=(float value);
		float& operator[](uint);
		const float& operator[](uint)const;

		static uint GetComponentCount() { return 3; }

		static const Vec3 zero;
	};

	/************************************************************************************
		VEC4
	*************************************************************************************/

	struct Vec4
	{
		float x;
		float y;
		float z;
		float w;

		Vec4();
		explicit Vec4(float xyzw);
		explicit Vec4(float _x, float _y, float _z, float _w);
		explicit Vec4(const Vec3& v, float _w);

		Vec2 Xy()const;
		Vec3 Xyz()const;

		Vec4& operator+=(const Vec4&);
		Vec4& operator*=(float);
		float& operator[](uint);
		const float& operator[](uint)const;

		static uint GetComponentCount() { return 4; }

		static const Vec4 zero;
	};

	/************************************************************************************
		HELPER FUNCTIONS
	*************************************************************************************/

	//=========================================================================
	//	Vec2 helper function
	//=========================================================================

	Vec2 operator*(const Vec2&, float);
	float operator*(const Vec2&, const Vec2&);
	Vec2 operator/(const Vec2&, float);
	Vec2 operator+(const Vec2&, const Vec2&);
	Vec2 operator-(const Vec2&, const Vec2&);

	//=========================================================================
	//	Vec3 helper function
	//=========================================================================

	Vec3 operator*(const Vec3&, float);
	Vec3 operator*(float, const Vec3&);
	float operator*(const Vec3&, const Vec3&);
	Vec3 operator/(const Vec3&, float);
	Vec3 operator+(const Vec3&, const Vec3&);
	Vec3 operator-(const Vec3&, const Vec3&);

	//=========================================================================
	//	Vec4 helper function
	//=========================================================================

	Vec4 operator*(const Vec4&, float);
	Vec4 operator*(float, const Vec4&);
	Vec4 operator/(const Vec4&, float);
	Vec4 operator+(const Vec4&, const Vec4&);
	Vec4 operator-(const Vec4&, const Vec4&);
}