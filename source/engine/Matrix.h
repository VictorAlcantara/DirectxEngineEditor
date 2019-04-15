#pragma once

#include "Type.h"
#include "Vector.h"

/****************************************************************************************
	Row major
	0 1
	2 3

	0 1 2
	3 4 5
	6 7 8

	 0   1   2   3
	 4   5   6   7
	 8   9  10  11
	12  13  14  15

	Transformation
	Rx Ry Rz 0
	Ux Uy Uz 0
	Lx Ly Lz 0
	Tx Ty Tz 1
*****************************************************************************************/

namespace math
{
	class Mtx22
	{
		public:
			Mtx22();
			Mtx22(
				float _00, float _01,
				float _10, float _11);

			static const uint s_width;
			static const uint s_size;

			float& operator[](uint);
			const float& operator[](uint)const;

			float& operator()(uint row, uint column);
			const float& operator()(uint row, uint column)const;

		private:
			float m_data[4];
	};

	class Mtx33
	{
		public:
			Mtx33();
			Mtx33(
				float _00, float _01, float _02,
				float _10, float _11, float _12,
				float _20, float _21, float _22);
			Mtx33(
				const Vec3&,
				const Vec3&,
				const Vec3&);

			static const uint s_width;
			static const uint s_size;

			float& operator[](uint);
			const float& operator[](uint)const;

			float& operator()(uint row, uint column);
			const float& operator()(uint row, uint column)const;

			Mtx22 Eliminate(uint index)const;
			Mtx22 Eliminate(uint row, uint column)const;

			bool IsInRow(uint row, uint index)const;
			bool IsInColumn(uint column, uint index)const;

		private:
			float m_data[9];
	};

	class Mtx44
	{
		public:
			Mtx44();
			Mtx44(const Mtx44&);
			Mtx44(
				float _00, float _01, float _02, float _03,
				float _10, float _11, float _12, float _13,
				float _20, float _21, float _22, float _23,
				float _30, float _31, float _32, float _33);
			Mtx44(
				Vec3 row0, float _03,
				Vec3 row1, float _13,
				Vec3 row2, float _23,
				Vec3 row3, float _33);
			Mtx44(const Mtx33&, float _33 = 1.0f);

			static const uint s_width;
			static const uint s_size;
			static const Mtx44 identity;

			Vec4 GetRow(uint row)const;
			void Set(
				float _00, float _01, float _02, float _03,
				float _10, float _11, float _12, float _13,
				float _20, float _21, float _22, float _23,
				float _30, float _31, float _32, float _33);
			void SetRow(uint row, float x, float y, float z, float w);
			void SetRow(uint row, const Vec4&);
			void SetRow(uint row, const Vec3&, float w);
			Vec4 GetColumn(uint column)const;

			Mtx33 Eliminate(uint index)const;
			Mtx33 Eliminate(uint row, uint column)const;

			bool IsInRow(uint row, uint index)const;
			bool IsInColumn(uint column, uint index)const;

			Vec3 GetPos()const;
			Vec3 GetRight()const;
			Vec3 GetUp()const;
			Vec3 GetLook()const;

			float& operator[](uint);
			const float& operator[](uint)const;

			float& operator()(uint row, uint column);
			const float& operator()(uint row, uint column)const;

			Mtx44& operator*=(float);

			Mtx44& operator=(const math::Mtx44&);

		private:
			float m_data[16];
	};

	Mtx33 operator*(const Mtx33&, const Mtx33&);
	Mtx33 operator*(const Mtx33&, float);
	Mtx33 operator*(float, const Mtx33&);
	Vec3 operator*(const Vec3&, const Mtx33&);
	Mtx33 operator/(const Mtx33&, float);

	Mtx44 operator*(const Mtx44& m1, const Mtx44& m2);
	Mtx44 operator*(const Mtx44&, float);
	Mtx44 operator*(float, const Mtx44&);
	Vec3 operator*(const Vec3&, const Mtx44&);
	Vec4 operator*(const Vec4&, const Mtx44&);
	Mtx44 operator/(const Mtx44&, float);
}

