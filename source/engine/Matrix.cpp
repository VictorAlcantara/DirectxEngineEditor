#include "Matrix.h"
#include "Debug.h"
#include "Exception.h"

#ifdef DEBUG_MATRIX_INDEX_CHECK
	#define CHECK_INDEX(matrix, index) CHECK( (index) < matrix::s_size );
	#define CHECK_ROW_COLUMN(matrix, row, col) CHECK( (row) < matrix::s_width && (col) < matrix::s_width );
#else
	#define CHECK_INDEX(matrix, index)
	#define CHECK_ROW_COLUMN(matrix, row, col)
#endif

/****************************************************************************************
	Helper functions declaration
*****************************************************************************************/

template <class MTX> bool matrixIsInRow(const MTX&, uint row, uint index);
template <class MTX> bool matrixIsInColumn(const MTX&, uint column, uint index);
template <class MTX_RESULT, class MTX> MTX_RESULT matrixEliminate(const MTX&, uint row, uint column);

/****************************************************************************************
	Mtx22
*****************************************************************************************/

const uint math::Mtx22::s_width = 2;
const uint math::Mtx22::s_size = 4;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
math::Mtx22::Mtx22()
{
	m_data[0] = 1.0f; m_data[1] = 0.0f;
	m_data[2] = 0.0f; m_data[3] = 1.0f;
}
//-------------------------------------------------------------------
math::Mtx22::Mtx22(float _00, float _01, float _02, float _03)
{
	m_data[0] = _00; m_data[1] = _01;
	m_data[2] = _02; m_data[3] = _03;
}
//-------------------------------------------------------------------
//	operator []
//-------------------------------------------------------------------
float& math::Mtx22::operator[](uint index)
{
	return m_data[index];
}
//-------------------------------------------------------------------
const float& math::Mtx22::operator[](uint index)const
{
	return m_data[index];
}
//-------------------------------------------------------------------
//	operator ()
//-------------------------------------------------------------------
float& math::Mtx22::operator()(uint row, uint col)
{
	return m_data[row * s_width + col];
}
//-------------------------------------------------------------------
const float& math::Mtx22::operator()(uint row, uint col)const
{
	return m_data[row * s_width + col];
}

/****************************************************************************************
	Mtx33
*****************************************************************************************/

const uint math::Mtx33::s_width = 3;
const uint math::Mtx33::s_size = 9;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
math::Mtx33::Mtx33()
{
	m_data[0] = 1.0f; m_data[1] = 0.0f; m_data[2] = 0.0f;
	m_data[3] = 0.0f; m_data[4] = 1.0f; m_data[5] = 0.0f;
	m_data[6] = 0.0f; m_data[7] = 0.0f; m_data[8] = 1.0f;
}
//-------------------------------------------------------------------
math::Mtx33::Mtx33(
	float _00, float _01, float _02,
	float _03, float _04, float _05,
	float _06, float _07, float _08)
{
	m_data[0] = _00; m_data[1] = _01; m_data[2] = _02;
	m_data[3] = _03; m_data[4] = _04; m_data[5] = _05;
	m_data[6] = _06; m_data[7] = _07; m_data[8] = _08;
}
//-------------------------------------------------------------------
math::Mtx33::Mtx33(const Vec3& row0, const Vec3& row1, const Vec3& row2)
{
	m_data[0] = row0.x; m_data[1] = row0.y; m_data[2] = row0.z;
	m_data[3] = row1.x; m_data[4] = row1.y; m_data[5] = row1.z;
	m_data[6] = row2.x; m_data[7] = row2.y; m_data[8] = row2.z;
}
//-------------------------------------------------------------------
//	operator []
//-------------------------------------------------------------------
float& math::Mtx33::operator[](uint index)
{
	CHECK_INDEX( Mtx33, index );

	return m_data[index];
}
//-------------------------------------------------------------------
const float& math::Mtx33::operator[](uint index)const
{
	CHECK_INDEX( Mtx33, index );

	return m_data[index];
}
//-------------------------------------------------------------------
//	operator ()
//-------------------------------------------------------------------
float& math::Mtx33::operator()(uint row, uint col)
{
	CHECK_ROW_COLUMN( Mtx33, row, col );

	return m_data[row * s_width + col];
}
//-------------------------------------------------------------------
const float& math::Mtx33::operator()(uint row, uint col)const
{
	CHECK_ROW_COLUMN( Mtx33, row, col );
	
	return m_data[row * s_width + col];
}
//-------------------------------------------------------------------
//	Eliminate
//-------------------------------------------------------------------
math::Mtx22 math::Mtx33::Eliminate(uint index)const
{
	CHECK_INDEX( Mtx33, index );

	return Eliminate( index / s_width, index % s_width );
}
//-------------------------------------------------------------------
math::Mtx22 math::Mtx33::Eliminate(uint row, uint col)const
{
	CHECK_ROW_COLUMN( Mtx33, row, col );

	return matrixEliminate<math::Mtx22>( *this, row, col );
}
//-------------------------------------------------------------------
//	IsInRow
//-------------------------------------------------------------------
bool math::Mtx33::IsInRow(uint row, uint index)const
{
	CHECK_ROW_COLUMN( Mtx33, row, 0 );
	CHECK_INDEX( Mtx33, index );

	return matrixIsInRow( *this, row, index );
}
//-------------------------------------------------------------------
//	IsInColumn
//-------------------------------------------------------------------
bool math::Mtx33::IsInColumn(uint column, uint index)const
{
	CHECK_ROW_COLUMN( Mtx33, 0, column );
	CHECK_INDEX( Mtx33, index );

	return matrixIsInColumn( *this, column, index );
}

/****************************************************************************************
	Mtx44
*****************************************************************************************/

const uint math::Mtx44::s_width = 4;
const uint math::Mtx44::s_size = 16;
const math::Mtx44 math::Mtx44::identity = math::Mtx44();

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
math::Mtx44::Mtx44() : Mtx44(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
)
{
}
//-------------------------------------------------------------------
math::Mtx44::Mtx44(const Mtx44& m)
{
	*this = m;
}
//-------------------------------------------------------------------
math::Mtx44::Mtx44(
	float _00, float _01, float _02, float _03,
	float _04, float _05, float _06, float _07,
	float _08, float _09, float _10, float _11,
	float _12, float _13, float _14, float _15)
{
	Set( 
		_00, _01, _02, _03,
		_04, _05, _06, _07,
		_08, _09, _10, _11,
		_12, _13, _14, _15
	);
}
//-------------------------------------------------------------------
math::Mtx44::Mtx44(
	Vec3 row0, float _03,
	Vec3 row1, float _13,
	Vec3 row2, float _23,
	Vec3 row3, float _33) 
	: Mtx44 (
		row0.x, row0.y, row0.z, _03,
		row1.x, row1.y, row1.z, _13,
		row2.x, row2.y, row2.z, _23,
		row3.x, row3.y, row3.z, _33
	)
{
}
//-------------------------------------------------------------------
math::Mtx44::Mtx44(const Mtx33& m, float _33) : Mtx44(
	m[0], m[1], m[2], 0.0f,
	m[3], m[4], m[5], 0.0f,
	m[6], m[7], m[8], 0.0f,
	0.0f, 0.0f, 0.0f, _33
)
{
}
//-------------------------------------------------------------------
//	GetRow
//-------------------------------------------------------------------
math::Vec4 math::Mtx44::GetRow(uint row)const
{
	CHECK_ROW_COLUMN( Mtx44, row, 0 );

	uint index = row * s_width;

	return math::Vec4(
		m_data[index],
		m_data[index + 1],
		m_data[index + 2],
		m_data[index + 3]
	);
}
//-------------------------------------------------------------------
//	Set
//-------------------------------------------------------------------
void math::Mtx44::Set(
	float _00, float _01, float _02, float _03,
	float _04, float _05, float _06, float _07,
	float _08, float _09, float _10, float _11,
	float _12, float _13, float _14, float _15)
{
	m_data[ 0] = _00; m_data[ 1] = _01; m_data[ 2] = _02; m_data[ 3] = _03;
	m_data[ 4] = _04; m_data[ 5] = _05; m_data[ 6] = _06; m_data[ 7] = _07;
	m_data[ 8] = _08; m_data[ 9] = _09; m_data[10] = _10; m_data[11] = _11;
	m_data[12] = _12; m_data[13] = _13; m_data[14] = _14; m_data[15] = _15;
}
//-------------------------------------------------------------------
//	SetRow
//-------------------------------------------------------------------
void math::Mtx44::SetRow(uint row, float x, float y, float z, float w)
{
	CHECK_ROW_COLUMN( Mtx44, row, 0 );

	uint index = row * s_width;

	m_data[index + 0] = x;
	m_data[index + 1] = y;
	m_data[index + 2] = z;
	m_data[index + 3] = w;
}
//-------------------------------------------------------------------
void math::Mtx44::SetRow(uint row, const Vec4& value)
{
	return SetRow( row, value.x, value.y, value.z, value.w );
}
//-------------------------------------------------------------------
void math::Mtx44::SetRow(uint row, const Vec3& value, float w)
{
	return SetRow( row, value.x, value.y, value.z, w );
}
//-------------------------------------------------------------------
//	GetColumn
//-------------------------------------------------------------------
math::Vec4 math::Mtx44::GetColumn(uint column)const
{
	CHECK_ROW_COLUMN( Mtx44, 0, column );

	return math::Vec4(
		m_data[column + s_width * 0],
		m_data[column + s_width * 1],
		m_data[column + s_width * 2],
		m_data[column + s_width * 3]
	);
}
//-------------------------------------------------------------------
//	Eliminate
//-------------------------------------------------------------------
math::Mtx33 math::Mtx44::Eliminate(uint index)const
{
	CHECK_INDEX( Mtx44, index );

	return Eliminate( index / s_width, index % s_width );
}
//-------------------------------------------------------------------
math::Mtx33 math::Mtx44::Eliminate(uint row, uint col)const
{
	CHECK_ROW_COLUMN( Mtx44, row, col );

	return matrixEliminate<math::Mtx33>( *this, row, col );
}
//-------------------------------------------------------------------
//	IsInRow
//-------------------------------------------------------------------
bool math::Mtx44::IsInRow(uint row, uint index)const
{
	CHECK_ROW_COLUMN( Mtx44, row, 0 );
	CHECK_INDEX( Mtx44, index );

	return matrixIsInRow( *this, row, index );
}
//-------------------------------------------------------------------
//	IsInColumn
//-------------------------------------------------------------------
bool math::Mtx44::IsInColumn(uint column, uint index)const
{
	CHECK_ROW_COLUMN( Mtx44, 0, column );
	CHECK_INDEX( Mtx44, index );

	return matrixIsInColumn( *this, column, index );
}
//-------------------------------------------------------------------
//	GetPos
//-------------------------------------------------------------------
math::Vec3 math::Mtx44::GetPos()const
{
	return GetRow(3).Xyz();
}
//-------------------------------------------------------------------
//	GetRight
//-------------------------------------------------------------------
math::Vec3 math::Mtx44::GetRight()const
{
	return GetRow(0).Xyz();
}
//-------------------------------------------------------------------
//	GetUp
//-------------------------------------------------------------------
math::Vec3 math::Mtx44::GetUp()const
{
	return GetRow(1).Xyz();
}
//-------------------------------------------------------------------
//	GetLook
//-------------------------------------------------------------------
math::Vec3 math::Mtx44::GetLook()const
{
	return GetRow(2).Xyz();
}
//-------------------------------------------------------------------
//	operator[]
//-------------------------------------------------------------------
float& math::Mtx44::operator[](uint index)
{
	CHECK_INDEX( Mtx44, index );

	return m_data[index];
}
//-------------------------------------------------------------------
const float& math::Mtx44::operator[](uint index)const
{
	CHECK_INDEX( Mtx44, index );

	return m_data[index];
}
//-------------------------------------------------------------------
//	operator()
//-------------------------------------------------------------------
float& math::Mtx44::operator()(uint row, uint col)
{
	CHECK_ROW_COLUMN( Mtx44, row, col );

	return m_data[row * s_width + col];
}
//-------------------------------------------------------------------
const float& math::Mtx44::operator()(uint row, uint col)const
{
	CHECK_ROW_COLUMN( Mtx44, row, col );

	return m_data[row * s_width + col];
}
//-------------------------------------------------------------------
//	operator*=
//-------------------------------------------------------------------
math::Mtx44& math::Mtx44::operator*=(float value)
{
	m_data[ 0] *= value; m_data[ 1] *= value; m_data[ 2] *= value; m_data[ 3] *= value;
	m_data[ 4] *= value; m_data[ 5] *= value; m_data[ 6] *= value; m_data[ 7] *= value;
	m_data[ 8] *= value; m_data[ 9] *= value; m_data[10] *= value; m_data[11] *= value;
	m_data[12] *= value; m_data[13] *= value; m_data[14] *= value; m_data[15] *= value;

	return *this;
}
//-------------------------------------------------------------------
//	operator=
//-------------------------------------------------------------------
math::Mtx44& math::Mtx44::operator=(const math::Mtx44& m)
{
	if ( this != &m )
	{
		m_data[ 0] = m[ 0]; m_data[ 1] = m[ 1]; m_data[ 2] = m[ 2]; m_data[ 3] = m[ 3];
		m_data[ 4] = m[ 4]; m_data[ 5] = m[ 5]; m_data[ 6] = m[ 6]; m_data[ 7] = m[ 7];
		m_data[ 8] = m[ 8]; m_data[ 9] = m[ 9]; m_data[10] = m[10]; m_data[11] = m[11];
		m_data[12] = m[12]; m_data[13] = m[13]; m_data[14] = m[14]; m_data[15] = m[15];
	}

	return *this;
}

/****************************************************************************************
	Helper functions definition
*****************************************************************************************/

//-------------------------------------------------------------------
//	matrixIsInRow
//-------------------------------------------------------------------
template<class MTX>
bool matrixIsInRow(const MTX& m, uint row, uint index)
{
	return index >= row * MTX::s_width && index < (row * MTX::s_width + MTX::s_width);
}
//-------------------------------------------------------------------
//	matrixIsInColumn
//-------------------------------------------------------------------
template<class MTX>
bool matrixIsInColumn(const MTX& m, uint column, uint index)
{
	for ( uint row = 0; row < MTX::s_width; row++ )
	{
		if ( row * MTX::s_width + column == index )
			return true;
	}

	return false;
}

//-------------------------------------------------------------------
//	matrixEliminate
//-------------------------------------------------------------------
template <class MTX_RESULT, class MTX>
inline MTX_RESULT matrixEliminate(const MTX& m, uint row, uint col)
{
	MTX_RESULT result;

	uint resultIndex = 0;

	for ( uint i = 0; i < MTX::s_size; i++ )
	{
		if ( matrixIsInRow(m, row, i) )
			i += MTX::s_width - 1;
		else if ( !matrixIsInColumn(m, col, i) )
			result[resultIndex++] = (m)[i];
	}

	return result;
}
//-------------------------------------------------------------------
//	operator *
//-------------------------------------------------------------------
math::Mtx33 math::operator*(const math::Mtx33& m1, const math::Mtx33& m2)
{
	math::Mtx33 result;

	result[0] = m1[0] * m2[0]  +  m1[1] * m2[3]  +  m1[2] * m2[6];
	result[1] = m1[0] * m2[1]  +  m1[1] * m2[4]  +  m1[2] * m2[7];
	result[2] = m1[0] * m2[2]  +  m1[1] * m2[5]  +  m1[2] * m2[8];

	result[3] = m1[3] * m2[0]  +  m1[4] * m2[3]  +  m1[5] * m2[6];
	result[4] = m1[3] * m2[1]  +  m1[4] * m2[4]  +  m1[5] * m2[7];
	result[5] = m1[3] * m2[2]  +  m1[4] * m2[5]  +  m1[5] * m2[8];

	result[6] = m1[6] * m2[0]  +  m1[7] * m2[3]  +  m1[8] * m2[6];
	result[7] = m1[6] * m2[1]  +  m1[7] * m2[4]  +  m1[8] * m2[7];
	result[8] = m1[6] * m2[2]  +  m1[7] * m2[5]  +  m1[8] * m2[8];

	return result;
}
//-------------------------------------------------------------------
math::Mtx33 math::operator*(const math::Mtx33& m, float f)
{
	math::Mtx33 result = m;

	result[0] *= f; result[1] *= f; result[2] *= f;
	result[3] *= f; result[4] *= f; result[5] *= f;
	result[6] *= f; result[7] *= f; result[8] *= f;

	return result;
}
//-------------------------------------------------------------------
math::Mtx33 math::operator*(float f, const math::Mtx33& m)
{
	return m * f;
}
//-------------------------------------------------------------------
math::Vec3 math::operator*(const math::Vec3& u, const math::Mtx33& m)
{
	return Vec3(
		u.x * m(0, 0) + u.y * m(1, 0) + u.z * m(2, 0),
		u.x * m(0, 1) + u.y * m(1, 1) + u.z * m(2, 1),
		u.x * m(0, 2) + u.y * m(1, 2) + u.z * m(2, 2)
	);
}
//-------------------------------------------------------------------
//	operator /
//-------------------------------------------------------------------
math::Mtx33 math::operator/(const math::Mtx33& m, float f)
{
	return m * (1.0f / f);
}
//-------------------------------------------------------------------
//	operator *
//-------------------------------------------------------------------
math::Mtx44 math::operator*(const math::Mtx44& m1, const math::Mtx44& m2)
{
	math::Mtx44 result;

	result[ 0] = m1[ 0] * m2[ 0]  +  m1[ 1] * m2[ 4]  +  m1[ 2] * m2[ 8]  +  m1[ 3] * m2[12];
	result[ 1] = m1[ 0] * m2[ 1]  +  m1[ 1] * m2[ 5]  +  m1[ 2] * m2[ 9]  +  m1[ 3] * m2[13];
	result[ 2] = m1[ 0] * m2[ 2]  +  m1[ 1] * m2[ 6]  +  m1[ 2] * m2[10]  +  m1[ 3] * m2[14];
	result[ 3] = m1[ 0] * m2[ 3]  +  m1[ 1] * m2[ 7]  +  m1[ 2] * m2[11]  +  m1[ 3] * m2[15];

	result[ 4] = m1[ 4] * m2[ 0]  +  m1[ 5] * m2[ 4]  +  m1[ 6] * m2[ 8]  +  m1[ 7] * m2[12];
	result[ 5] = m1[ 4] * m2[ 1]  +  m1[ 5] * m2[ 5]  +  m1[ 6] * m2[ 9]  +  m1[ 7] * m2[13];
	result[ 6] = m1[ 4] * m2[ 2]  +  m1[ 5] * m2[ 6]  +  m1[ 6] * m2[10]  +  m1[ 7] * m2[14];
	result[ 7] = m1[ 4] * m2[ 3]  +  m1[ 5] * m2[ 7]  +  m1[ 6] * m2[11]  +  m1[ 7] * m2[15];

	result[ 8] = m1[ 8] * m2[ 0]  +  m1[ 9] * m2[ 4]  +  m1[10] * m2[ 8]  +  m1[11] * m2[12];
	result[ 9] = m1[ 8] * m2[ 1]  +  m1[ 9] * m2[ 5]  +  m1[10] * m2[ 9]  +  m1[11] * m2[13];
	result[10] = m1[ 8] * m2[ 2]  +  m1[ 9] * m2[ 6]  +  m1[10] * m2[10]  +  m1[11] * m2[14];
	result[11] = m1[ 8] * m2[ 3]  +  m1[ 9] * m2[ 7]  +  m1[10] * m2[11]  +  m1[11] * m2[15];

	result[12] = m1[12] * m2[ 0]  +  m1[13] * m2[ 4]  +  m1[14] * m2[ 8]  +  m1[15] * m2[12];
	result[13] = m1[12] * m2[ 1]  +  m1[13] * m2[ 5]  +  m1[14] * m2[ 9]  +  m1[15] * m2[13];
	result[14] = m1[12] * m2[ 2]  +  m1[13] * m2[ 6]  +  m1[14] * m2[10]  +  m1[15] * m2[14];
	result[15] = m1[12] * m2[ 3]  +  m1[13] * m2[ 7]  +  m1[14] * m2[11]  +  m1[15] * m2[15];

	return result;
}
//-------------------------------------------------------------------
math::Mtx44 math::operator*(const math::Mtx44& m, float f)
{
	math::Mtx44 result = m;

	result[ 0] *= f; result[ 1] *= f; result[ 2] *= f; result[ 3] *= f;
	result[ 4] *= f; result[ 5] *= f; result[ 6] *= f; result[ 7] *= f;
	result[ 8] *= f; result[ 9] *= f; result[10] *= f; result[11] *= f;
	result[12] *= f; result[13] *= f; result[14] *= f; result[15] *= f;

	return result;
}
//-------------------------------------------------------------------
math::Mtx44 math::operator*(float f, const math::Mtx44& m)
{
	return m * f;
}
//-------------------------------------------------------------------
math::Vec3 math::operator*(const math::Vec3& v, const math::Mtx44& m)
{
	math::Vec3 result;

	result.x = v.x * m[ 0]  +  v.y * m[ 4]  +  v.z * m[ 8]  + m[12];
	result.y = v.x * m[ 1]  +  v.y * m[ 5]  +  v.z * m[ 9]  + m[13];
	result.z = v.x * m[ 2]  +  v.y * m[ 6]  +  v.z * m[10]  + m[14];

	return result;
}
//-------------------------------------------------------------------
math::Vec4 math::operator*(const math::Vec4& v, const math::Mtx44& m)
{
	math::Vec4 result;

	result.x = v.x * m[ 0]  +  v.y * m[ 4]  +  v.z * m[ 8]  +  v.w * m[12];
	result.y = v.x * m[ 1]  +  v.y * m[ 5]  +  v.z * m[ 9]  +  v.w * m[13];
	result.z = v.x * m[ 2]  +  v.y * m[ 6]  +  v.z * m[10]  +  v.w * m[14];
	result.w = v.x * m[ 3]  +  v.y * m[ 7]  +  v.z * m[11]  +  v.w * m[15];
		 
	return result;
}
//-------------------------------------------------------------------
//	operator /
//-------------------------------------------------------------------
math::Mtx44 math::operator/(const math::Mtx44& m, float f)
{
	return m * (1.0f / f);
}
