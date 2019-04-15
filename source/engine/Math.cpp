#include "../engine/Math.h"
#include <math.h>
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"
#include "../engine/Debug.h"
#include "../engine/Random.h"
#include "../engine/String.h"

/****************************************************************************************
	CameraTransform
*****************************************************************************************/

//-------------------------------------------------------------------
//	GetFrustumAabbGlobal
//-------------------------------------------------------------------
math::Aabb3 math::CameraTransform::GetFrustumAabbGlobal()const
{
	float d = fFar;
	float y =  tanf(toRads(fovDegs * 0.5f)) * d;
	float x = y * aspectRatio; 

	math::Vec3 aabbTransformed[8];
	math::Mtx44 m;

	m.SetRow( 0, right, 0.0f );
	m.SetRow( 1, up, 0.0f );
	m.SetRow( 2, look, 0.0f );
	m.SetRow( 3, pos, 1.0f );

	//                   Front
	aabbTransformed[0] = math::Vec3( -x, -y, 0.0f ) * m;
	aabbTransformed[1] = math::Vec3( -x,  y, 0.0f ) * m;
	aabbTransformed[2] = math::Vec3(  x,  y, 0.0f ) * m;
	aabbTransformed[3] = math::Vec3(  x, -y, 0.0f ) * m;
	//                   Back
	aabbTransformed[4] = math::Vec3( -x, -y, d ) * m;
	aabbTransformed[5] = math::Vec3( -x,  y, d ) * m;
	aabbTransformed[6] = math::Vec3(  x,  y, d ) * m;
	aabbTransformed[7] = math::Vec3(  x, -y, d ) * m;

	Aabb3 result;
	result.min = aabbTransformed[0];
	result.max = aabbTransformed[0];

	for ( uint i = 1; i < ARRAY_COUNT(aabbTransformed); i++ )
	{
		setMinMax( aabbTransformed[i].x, result.min.x, result.max.x );
		setMinMax( aabbTransformed[i].y, result.min.y, result.max.y );
		setMinMax( aabbTransformed[i].z, result.min.z, result.max.z );
	}

	return result;
}

/****************************************************************************************
	GENERAL FUNCTIONS
*****************************************************************************************/

//-------------------------------------------------------------------
//	abs
//-------------------------------------------------------------------
int math::abs(int i)
{
	return i < 0 ? -i : i;
}
//-------------------------------------------------------------------
float math::abs(float f)
{
	return f < 0.0f ? -f : f;
}
//-------------------------------------------------------------------
//	sign
//-------------------------------------------------------------------
int math::sign(int i)
{
	return i < 0 ? -1 : 1;
}
//-------------------------------------------------------------------
float math::sign(float f)
{
	return f < 0.0f ? -1.0f : 1.0f;
}
//-------------------------------------------------------------------
//	isEqual
//-------------------------------------------------------------------
bool math::isEqual(float value1, float value2, float epsilon)
{
	return isZero( value1 - value2, epsilon );
}
//-------------------------------------------------------------------
//	isEqualLess
//-------------------------------------------------------------------
bool math::isEqualLess(float value1, float value2, float epsilon)
{
	return value1 < value2 || isZero( value1 - value2, epsilon );
}
//-------------------------------------------------------------------
//	isZero
//-------------------------------------------------------------------
bool math::isZero(float value, float epsilon)
{
	return fabs(value) < epsilon;
}
//-------------------------------------------------------------------
//	isPair
//-------------------------------------------------------------------
bool math::isPair(int value)
{
	return value % 2 == 0;
}
//-------------------------------------------------------------------
bool math::isPair(uint value)
{
	return value % 2 == 0;
}
//-------------------------------------------------------------------
//	clamp
//-------------------------------------------------------------------
uint math::clamp(uint value, uint minValue, uint maxValue)
{
	if ( value < minValue )
		return minValue;
	if ( value > maxValue )
		return maxValue;

	return value;
}
//-------------------------------------------------------------------
float math::clamp(float value, float minValue, float maxValue)
{
	if ( value < minValue )
		return minValue;
	if ( value > maxValue )
		return maxValue;

	return value;
}
//-------------------------------------------------------------------
math::Rect math::clamp(const Rect& value, float minValue, float maxValue)
{
	Rect result;
	
	result.left = clamp( value.left, minValue, maxValue );
	result.top = clamp( value.top, minValue, maxValue );
	result.right = clamp( value.right, minValue, maxValue );
	result.bottom = clamp( value.bottom, minValue, maxValue );

	return result;
}
//-------------------------------------------------------------------
//	clampMin
//-------------------------------------------------------------------
float math::clampMin(float value, float minValue)
{
	if ( value < minValue )
		return minValue;
	return value;
}
//-------------------------------------------------------------------
//	sq
//-------------------------------------------------------------------
float math::sq(float f)
{
	return f * f;
}
//-------------------------------------------------------------------
//	sqrt
//-------------------------------------------------------------------
float math::sqrt(float f)
{
	return sqrtf( f );
}
//-------------------------------------------------------------------
//	random
//-------------------------------------------------------------------
float math::random()
{
	static Random random(RANDOM_SEED);

	return random.Get();
}
//-------------------------------------------------------------------
float math::random(float val1, float val2)
{
	static Random random(RANDOM_SEED);
	
	return random.GetRange( val1, val2 );
}
//-------------------------------------------------------------------
//	bhaskara
//-------------------------------------------------------------------
bool math::bhaskara(float a, float b, float c, float& root0, float& root1)
{
	float discriminant = b * b - 4.0f * a * c;

	if ( discriminant < 0.0f )
		return false;

	float discrSqrt = sqrtf(discriminant);
	float div = 1.0f / (2.0f * a);

	root0 = (-b + discrSqrt) * div;
	root1 = (-b - discrSqrt) * div;

	if ( root0 > root1 )
		swap( root0, root1 );

	return true;
}
//-------------------------------------------------------------------
//	remainder
//-------------------------------------------------------------------
float math::remainder(float divident, float divisor)
{
	int quotient = (int)(divident / divisor);
	return divident - (quotient * divisor);
}
//-------------------------------------------------------------------
//	pow
//-------------------------------------------------------------------
float math::pow(float value, uint power)
{
	if ( power == 0 )
		return 1.0f;
	return value * math::pow( value, power - 1 );
}
//-------------------------------------------------------------------
//	invertGrowth
//-------------------------------------------------------------------
math::eGrowth math::invertGrowth(eGrowth g)
{
	if ( g == eGrowth::INCREASE )
		return eGrowth::DECREASE;
	return eGrowth::INCREASE;
}
//-------------------------------------------------------------------
//	sin
//-------------------------------------------------------------------
float math::sin(float value)
{
	return sinf( value );
}
//-------------------------------------------------------------------
//	cos
//-------------------------------------------------------------------
float math::cos(float value)
{
	return cosf( value );
}
//-------------------------------------------------------------------
//	tan
//-------------------------------------------------------------------
float math::tan(float value)
{
	return tanf( value );
}
//-------------------------------------------------------------------
//	sinDegs
//-------------------------------------------------------------------
float math::sinDegs(float degs)
{
	return math::sin( toRads(degs) );
}
//-------------------------------------------------------------------
//	cosDegs
//-------------------------------------------------------------------
float math::cosDegs(float degs)
{
	return math::cos( toRads(degs) );
}
//-------------------------------------------------------------------
//	tanDegs
//-------------------------------------------------------------------
float math::tanDegs(float degs)
{
	return math::tan( toRads(degs) );
}
//-------------------------------------------------------------------
//	asin
//-------------------------------------------------------------------
float math::asin(float s)
{
	return asinf( s );
}
//-------------------------------------------------------------------
float math::asinDegs(float s)
{
	return toDegs( asin( s ) );
}
//-------------------------------------------------------------------
//	acos
//-------------------------------------------------------------------
float math::acos(float s)
{
	return acosf( s );
}
//-------------------------------------------------------------------
float math::acosDegs(float s)
{
	return toDegs( acos( s ) );
}
//-------------------------------------------------------------------
//	isNan
//-------------------------------------------------------------------
bool math::isNan(float f)
{
	return f != f;
}
//-------------------------------------------------------------------
//	setMinMax
//-------------------------------------------------------------------
void math::setMinMax(float value, float& refMin, float& refMax)
{
	if ( value < refMin )
		refMin = value;
	if ( value > refMax )
		refMax = value;
}
//-------------------------------------------------------------------
//	getAabb3Points
//-------------------------------------------------------------------
void math::getAabb3Points(const Aabb3& aabb, Vec3* outPoints, uint pointCount)
{
	CHECK( pointCount == 8 );

	math::Vec3 aabbScale = aabb.GetScale();
	math::Vec3 aabbPos = aabb.GetPos();

	// Front
	outPoints[0] = aabbPos + Vec3( -aabbScale.x, -aabbScale.y, -aabbScale.z );
	outPoints[1] = aabbPos + Vec3( -aabbScale.x,  aabbScale.y, -aabbScale.z );
	outPoints[2] = aabbPos + Vec3(  aabbScale.x,  aabbScale.y, -aabbScale.z );
	outPoints[3] = aabbPos + Vec3(  aabbScale.x, -aabbScale.y, -aabbScale.z );
	// Back
	outPoints[4] = aabbPos + Vec3( -aabbScale.x, -aabbScale.y,  aabbScale.z );
	outPoints[5] = aabbPos + Vec3( -aabbScale.x,  aabbScale.y,  aabbScale.z );
	outPoints[6] = aabbPos + Vec3(  aabbScale.x,  aabbScale.y,  aabbScale.z );
	outPoints[7] = aabbPos + Vec3(  aabbScale.x, -aabbScale.y,  aabbScale.z );
}
//-------------------------------------------------------------------
//	normalizePlane
//-------------------------------------------------------------------
math::Plane math::normalizePlane(const Plane& p)
{
	Plane result = p;
	float l = length( math::Vec3(p.a, p.b, p.c) );

	result.a /= l;
	result.b /= l;
	result.c /= l;
	result.d /= l;

	return result;
}

/****************************************************************************************
	ANGLE
*****************************************************************************************/

//-------------------------------------------------------------------
//	toRads
//-------------------------------------------------------------------
float math::toRads(float degs)
{
	return (PI / 180.0f) * degs;
}
//-------------------------------------------------------------------
//	toDegs
//-------------------------------------------------------------------
float math::toDegs(float rads)
{
	return (180.0f / PI) * rads;
}

/****************************************************************************************
	VECTOR
*****************************************************************************************/

//-------------------------------------------------------------------
//	abs
//-------------------------------------------------------------------
math::Vec3 math::abs(const math::Vec3& v)
{
	return Vec3(
		abs(v.x),
		abs(v.y),
		abs(v.z)
	);
}
//-------------------------------------------------------------------
//	sign
//-------------------------------------------------------------------
math::Vec3 math::sign(const math::Vec3& v)
{
	return Vec3(
		sign(v.x),
		sign(v.y),
		sign(v.z)
	);
}
//-------------------------------------------------------------------
//	parallel
//-------------------------------------------------------------------
math::Vec2 math::parallel(const math::Vec2& u)
{
	return math::Vec2(
		-u.y,
		u.x
	);
}
//-------------------------------------------------------------------
//	scaleInv
//-------------------------------------------------------------------
math::Vec2 math::scaleInv(const Vec2& u, const Vec2& v)
{
	return Vec2(
		u.x / v.x,
		u.y / v.y
	);
}
//-------------------------------------------------------------------
math::Vec3 math::scaleInv(const Vec3& u, const Vec3& v)
{
	return Vec3(
		u.x / v.x,
		u.y / v.y,
		u.z / v.z
	);
}
//-------------------------------------------------------------------
//	scale
//-------------------------------------------------------------------
math::Vec2 math::scale(const Vec2& u, const Vec2& v)
{
	return Vec2(
		u.x * v.x,
		u.y * v.y
	);
}
//-------------------------------------------------------------------
math::Vec3 math::scale(const Vec3& u, const Vec3& v)
{
	return Vec3(
		u.x * v.x,
		u.y * v.y,
		u.z * v.z
	);
}
//-------------------------------------------------------------------
math::Vec4 math::scale(const Vec4& u, const Vec4& v)
{
	return Vec4(
		u.x * v.x,
		u.y * v.y,
		u.z * v.z,
		u.w * v.w
	);
}
//-------------------------------------------------------------------
//	mulRgb
//-------------------------------------------------------------------
math::Vec4 math::mulRgb(const Vec4& color4, float f)
{
	return Vec4( 
		color4.x * f, 
		color4.y * f,
		color4.z * f,
		color4.w );
}
//-------------------------------------------------------------------
//	min
//-------------------------------------------------------------------
float math::min(const Vec2& v)
{
	return min( v.x, v.y );
}
//-------------------------------------------------------------------
float math::min(const Vec3& v)
{
	return min( v.x, min ( v.y, v.z ) );
}
//-------------------------------------------------------------------
float math::min(const Vec4& v)
{
	return min( v.x, min ( v.y, min ( v.z, v.w ) ) );
}
//-------------------------------------------------------------------
//	max
//-------------------------------------------------------------------
float math::max(const Vec2& v)
{
	return max( v.x, v.y );
}
//-------------------------------------------------------------------
float math::max(const Vec3& v)
{
	return max( v.x, max ( v.y, v.z ) );
}
//-------------------------------------------------------------------
float math::max(const Vec4& v)
{
	return max( v.x, max ( v.y, max ( v.z, v.w ) ) );
}
//-------------------------------------------------------------------
//	compMin
//-------------------------------------------------------------------
math::Vec3 math::compMin(const Vec3& u, const Vec3& v)
{
	return Vec3(
		min( u.x, v.x ),
		min( u.y, v.y ),
		min( u.z, v.z )
	);
}
//-------------------------------------------------------------------
//	compMax
//-------------------------------------------------------------------
math::Vec3 math::compMax(const Vec3& u, const Vec3& v)
{
	return Vec3(
		max( u.x, v.x ),
		max( u.y, v.y ),
		max( u.z, v.z )
	);
}
//-------------------------------------------------------------------
//	clamp
//-------------------------------------------------------------------
math::Vec3 math::clamp(const Vec3& value, const Vec3& minValue, const Vec3& maxValue)
{
	return Vec3(
		clamp( value[0], minValue[0], maxValue[0] ),
		clamp( value[1], minValue[1], maxValue[1] ),
		clamp( value[2], minValue[2], maxValue[2] )
	);
}
//-------------------------------------------------------------------
math::Vec4 math::clamp(const Vec4& value, const Vec4& minValue, const Vec4& maxValue)
{
	return Vec4(
		clamp( value[0], minValue[0], maxValue[0] ),
		clamp( value[1], minValue[1], maxValue[1] ),
		clamp( value[2], minValue[2], maxValue[2] ),
		clamp( value[3], minValue[3], maxValue[3] )
	);
}
//-------------------------------------------------------------------
//	clampMin
//-------------------------------------------------------------------
math::Vec3 math::clampMin(const Vec3& value, const Vec3& minValue)
{
	return Vec3(
		clampMin( value[0], minValue[0] ),
		clampMin( value[1], minValue[1] ),
		clampMin( value[2], minValue[2] )
	);
}
//-------------------------------------------------------------------
//	clampUnit
//-------------------------------------------------------------------
math::Vec3 math::clampUnit(const Vec3& value)
{
	return math::clamp( value, math::Vec3(0.0f), math::Vec3(1.0f) );
}
//-------------------------------------------------------------------
math::Vec4 math::clampUnit(const Vec4& value)
{
	return math::clamp( value, math::Vec4(0.0f), math::Vec4(1.0f) );
}
//-------------------------------------------------------------------
//	clampLength
//-------------------------------------------------------------------
math::Vec3 math::clampLength(const Vec3& value, float minLength, float maxLength)
{
	float valueLengthSq = value * value;
	
	if ( math::isZero(valueLengthSq) )
		return math::Vec3(0.0f);

	if ( valueLengthSq < minLength * minLength )
		return math::normalize(value) * minLength;
	
	if ( valueLengthSq > maxLength * maxLength )
		return math::normalize(value) * maxLength;

	return value;
}
//-------------------------------------------------------------------
//	length
//-------------------------------------------------------------------
float math::length(const Vec2& v)
{
	return sqrtf( v * v );
}
//-------------------------------------------------------------------
float math::length(const Vec3& v)
{
	return sqrtf( v * v );
}
//-------------------------------------------------------------------
float math::length(float x, float y, float z)
{
	Vec3 v = Vec3(x, y, z);
	return sqrtf( v * v );
}
//-------------------------------------------------------------------
//	lengthSq
//-------------------------------------------------------------------
float math::lengthSq(const Vec2& v)
{
	return v * v;
}
//-------------------------------------------------------------------
float math::lengthSq(const Vec3& v)
{
	return v * v;
}
//-------------------------------------------------------------------
//	dist
//-------------------------------------------------------------------
float math::dist(const Vec3& u, const Vec3& v)
{
	return length( u - v );
}
//-------------------------------------------------------------------
//	distSq
//-------------------------------------------------------------------
float math::distSq(const Vec3& u, const Vec3& v)
{
	return lengthSq( u - v );
}
//-------------------------------------------------------------------
//	normalize
//-------------------------------------------------------------------
math::Vec3 math::normalize(const Vec3& v)
{
	float len = length( v );

	if ( isZero(len) )
		return Vec3( 0.0f, 0.0f, 0.0f );

	return v / len;
}
//-------------------------------------------------------------------
math::Vec3 math::normalize(float x, float y, float z)
{
	return normalize( math::Vec3(x, y, z) );
}
//-------------------------------------------------------------------
math::Vec3 math::normalize(const Vec3& v, float& outLength)
{
	outLength = length( v );

	if ( isZero(outLength) )
	{
		outLength = 0.0f;
		return Vec3( 0.0f, 0.0f, 0.0f );
	}

	return v / outLength;
}
//-------------------------------------------------------------------
math::Vec3 math::normalize(float x, float y, float z, float& outLength)
{
	return normalize( math::Vec3(x, y, z), outLength );
}
//-------------------------------------------------------------------
math::Vec2 math::normalize(const Vec2& v)
{
	float  len = length( v );

	if ( isZero(len) )
		return Vec2();

	return v / len;
}
//-------------------------------------------------------------------
math::Vec2 math::normalize(float x, float y)
{
	return normalize( math::Vec2(x, y) );
}
//-------------------------------------------------------------------
//	cross
//-------------------------------------------------------------------
math::Vec3 math::cross(const Vec3& u, const Vec3& v)
{
	return Vec3( 
		u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x
	);
}
//-------------------------------------------------------------------
//	screenToWorld
//-------------------------------------------------------------------
math::Vec3 math::screenToWorld(const Vec2& mousePos, uint clientWidth, uint clientHeight, const Mtx44& view, const Mtx44& proj)
{
	Vec2 viewport( (float)clientWidth, (float)clientHeight );
	Vec3 rayNdc( (mousePos.x / viewport.x) * 2.0f - 1.0f, -(mousePos.y / viewport.y) * 2.0f + 1.0f, 1.0f );
	Vec4 rayClip( rayNdc, 1.0f );
	Vec4 rayEye = rayClip * matrixInvert( proj );
	rayEye.z = 1.0f;
	rayEye.w = 0.0f;
	Vec3 rayWorld = ( rayEye * matrixInvert(view) ).Xyz();
	return normalize( rayWorld );
}
//-------------------------------------------------------------------
math::Vec3 math::screenToWorld(const Vec2& mousePos, const ViewportInfo& viewport, const CameraTransform& camera)
{
	return screenToWorld( mousePos, viewport.width, viewport.height, camera.view, camera.proj );
}
//-------------------------------------------------------------------
//	mousePosToWorld
//-------------------------------------------------------------------
math::Vec3 math::mousePosToWorld(const CameraTransform& camera)
{
	return screenToWorld( g_pInput->GetMousePos(), g_pRenderer->GetWindowViewport(), camera );
}
//-------------------------------------------------------------------
//	normalizeRgb
//-------------------------------------------------------------------
math::Vec4 math::normalizeRgb(ushort rgb, ushort a)
{
	return normalizeRgb( rgb, rgb, rgb, a );
}
//-------------------------------------------------------------------
math::Vec4 math::normalizeRgb(ushort r, ushort g, ushort b, ushort a)
{
	return Vec4( (float)r, (float)g, (float)b, (float)a ) / 255.0f;
}
//-------------------------------------------------------------------
//	saturateRgb
//-------------------------------------------------------------------
math::Vec4 math::saturateRgb(const Vec4& color, float saturation)
{
	math::Vec4 result = saturate( color.Xyz(), saturation ).Xyzw( color.w );

	return result;
}
//-------------------------------------------------------------------
math::Vec4 math::saturateRgb(const Vec3& color, float saturation, float alpha)
{
	return saturateRgb( color.Xyzw(alpha), saturation );
}
//-------------------------------------------------------------------
//	isEqual
//-------------------------------------------------------------------
bool math::isEqual(const Vec3& u, const Vec3& v, float epsilon)
{
	return isEqual(u.x, v.x, epsilon) && isEqual(u.y, v.y, epsilon) && isEqual(u.z, v.z, epsilon);
}
//-------------------------------------------------------------------
//	screenCoordToCenter
//-------------------------------------------------------------------
math::Vec2 math::screenCoordToCenter(const Vec2& pos, const Vec2& viewportDimension)
{
	return Vec2(
		 pos.x + -viewportDimension.x * 0.5f,
		-pos.y +  viewportDimension.y * 0.5f
	);
}
//-------------------------------------------------------------------
math::Vec2 math::screenCoordToCenter(float x, float y, const Vec2& viewportDimension)
{
	return screenCoordToCenter( math::Vec2(x, y), viewportDimension );
}
//-------------------------------------------------------------------
//	centerToScreenCoord
//-------------------------------------------------------------------
math::Vec2 math::centerToScreenCoord(const Vec2& pos, const Vec2& viewportDimension)
{
	return Vec2(
		 pos.x + viewportDimension.x * 0.5f,
		-pos.y + viewportDimension.y * 0.5f
	);
}
//-------------------------------------------------------------------
//	screenCoordToNdc
//-------------------------------------------------------------------
math::Vec2 math::screenCoordToNdc(const Vec2& screenCoord, const ViewportInfo& viewport)
{
	return Vec2(
		 2.0f * screenCoord.x / (float)viewport.width - 1.0f,
		-2.0f * screenCoord.y / (float)viewport.height + 1.0f
	);
}
//-------------------------------------------------------------------
//	screenNormalizedToNdc
//-------------------------------------------------------------------
math::Vec2 math::screenNormalizedToNdc(const Vec2& screenCoord)
{
	ViewportInfo viewport;
	viewport.width = 1;
	viewport.height = 1;
	return screenCoordToNdc( screenCoord, viewport );
}
//-------------------------------------------------------------------
math::Vec2 math::screenNormalizedToNdc(float left, float top)
{
	ViewportInfo viewport;
	viewport.width = 1;
	viewport.height = 1;
	return screenCoordToNdc( math::Vec2(left, top), viewport );
}
//-------------------------------------------------------------------
//	ndcToScreenNormalized
//-------------------------------------------------------------------
math::Vec2 math::ndcToScreenNormalized(const math::Vec2& ndc)
{
	math::Vec2 result = ndc * 0.5f;

	result.x += 0.5f;
	result.y = -result.y + 0.5f;

	return result;
}
//-------------------------------------------------------------------
math::Vec2 math::ndcToScreenNormalized(float x, float y)
{
	return ndcToScreenNormalized( math::Vec2(x, y) );
}
//-------------------------------------------------------------------
//	calculateTangentBinormal
//-------------------------------------------------------------------
void math::calculateTangentBinormal(const Vec3& edge0, const Vec3& edge1, const Vec3& edge2, const Vec2& texCoord0, const Vec2& texCoord1, const Vec2& texCoord2, Vec3& outTangent, Vec3& outBinormal)
{
	// http://ogldev.atspace.co.uk/www/tutorial26/tutorial26.html
	// https://learnopengl.com/Advanced-Lighting/Normal-Mapping

	Vec3 e1 = edge1 - edge0;
	Vec3 e2 = edge2 - edge0;
	float u1Delta = texCoord1.x - texCoord0.x;
	float v1Delta = texCoord1.y - texCoord0.y;
	float u2Delta = texCoord2.x - texCoord0.x;
	float v2Delta = texCoord2.y - texCoord0.y;
	float invDet = 1.0f / (u1Delta*v2Delta - u2Delta*v1Delta);

	outTangent.x = invDet * ( v2Delta * e1.x - v1Delta * e2.x );
	outTangent.y = invDet * ( v2Delta * e1.y - v1Delta * e2.y );
	outTangent.z = invDet * ( v2Delta * e1.z - v1Delta * e2.z );
	outTangent = normalize( outTangent );

	outBinormal.x = invDet * ( -u2Delta * e1.x + u1Delta * e2.x );
	outBinormal.y = invDet * ( -u2Delta * e1.y + u1Delta * e2.y );
	outBinormal.z = invDet * ( -u2Delta * e1.z + u1Delta * e2.z );
	outBinormal = normalize( outBinormal );
}
//-------------------------------------------------------------------
//	calculateAabb3
//-------------------------------------------------------------------
math::Aabb3 math::calculateAabb3(const Vec3* points, uint count)
{
	Aabb3 result;

	CHECK( count > 1 );

	result.min = points[0];
	result.max = points[0];

	for ( uint i = 1; i < count; i++ )
	{
		math::setMinMax( points[i].x, result.min.x, result.max.x );
		math::setMinMax( points[i].y, result.min.y, result.max.y );
		math::setMinMax( points[i].z, result.min.z, result.max.z );
	}

	return result;
}
//-------------------------------------------------------------------
math::Aabb3 math::calculateAabb3(const Vec3& p0, const Vec3& p1)
{
	math::Aabb3 result;

	for ( uint i = 0; i < 3; i++ )
	{
		if ( p0[i] < p1[i] )
		{
			result.min[i] = p0[i];
			result.max[i] = p1[i];
		}
		else
		{
			result.min[i] = p1[i];
			result.max[i] = p0[i];
		}
	}

	return result;
}
//-------------------------------------------------------------------
//	isZero
//-------------------------------------------------------------------
bool math::isZero(const Vec3& v)
{
	return isZero(v.x) && isZero(v.y) && isZero(v.z);
}
//-------------------------------------------------------------------
bool math::isZero(const Vec4& v)
{
	return isZero(v.x) && isZero(v.y) && isZero(v.z) && isZero(v.w);
}
//-------------------------------------------------------------------
//	isNan
//-------------------------------------------------------------------
bool math::isNan(const Vec3& v)
{
	return isNan(v.x) || isNan(v.y) || isNan(v.z);
}
//-------------------------------------------------------------------
//	projectPointOnLine
//-------------------------------------------------------------------
float math::projectPointOnLine(const Vec3& point, const Vec3& line0, const Vec3& line1)
{
	float length = 0.0f;
	math::Vec3 lineDir = normalize( line1 - line0, length );

	if ( length == 0.0f )
		return 0.0f;

	float proj = (point - line0) * lineDir;
	return proj / length;
}

/****************************************************************************************
	MATRIX
*****************************************************************************************/

//-------------------------------------------------------------------
//	matrixPerspective
//-------------------------------------------------------------------
math::Mtx44 math::matrixPerspective(float aspectRatio, float fovDegs, float _near, float _far)
{
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205350(v=vs.85).aspx
	float yScale = tanf( toRads(fovDegs * 0.5f) );
	yScale = 1.0f / yScale;
	float xScale = yScale / aspectRatio;

	return Mtx44(
		xScale, 0.0f  , 0.0f                          , 0.0f,
		0.0f  , yScale, 0.0f                          , 0.0f,
		0.0f  , 0.0f  , _far / (_far - _near)         , 1.0f,
		0.0f  , 0.0   , -_near * _far / (_far - _near), 0.0f 
	);
}
//-------------------------------------------------------------------
//	matrixOrtho
//-------------------------------------------------------------------
math::Mtx44 math::matrixOrtho(float width, float height, float _near, float _far)
{
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205346(v=vs.85).aspx
	return Mtx44(
		2.0f / width, 0.0f         , 0.0f                   , 0.0f,
		0.0f        , 2.0f / height, 0.0f                   , 0.0f,
		0.0f        , 0.0f         , 1.0f / (_far - _near)  , 0.0f,
		0.0f        , 0.0f         , -_near / (_far - _near), 1.0f
	);
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixOrtho(float aspectRatio)
{
	float _near = 0.00001f;
	math::Mtx44 result = matrixOrtho( 2.0f * aspectRatio, 2.0f, _near, 1.0f + _near );
	
	return result;
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixOrtho(const Vec2& dimension)
{
	float _near = 0.00001f;
	math::Mtx44 result = matrixOrtho( dimension.x, dimension.y, _near, 1.0f + _near );

	return result;
}
//-------------------------------------------------------------------
//	matrixTranslate
//-------------------------------------------------------------------
math::Mtx44 math::matrixTranslate(float x, float y, float z)
{
	Mtx44 result;
	result[12] = x;
	result[13] = y;
	result[14] = z;

	return result;
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixTranslate(const Vec3& v)
{
	return matrixTranslate( v.x, v.y, v.z );
}
//-------------------------------------------------------------------
math::Mtx33 math::matrixTranslate(const Vec2& v)
{
	return matrixTranslate( v.x, v.y );
}
//-------------------------------------------------------------------
math::Mtx33 math::matrixTranslate(float x, float y)
{
	Mtx33 result;
	result[6] = x;
	result[7] = y;
	return result;
}
//-------------------------------------------------------------------
//	matrixScale
//-------------------------------------------------------------------
math::Mtx33 math::matrixScale(float x, float y)
{
	Mtx33 result;
	result(0, 0) = x;
	result(1, 1) = y;
	return result;
}
//-------------------------------------------------------------------
math::Mtx33 math::matrixScale(const Vec2& u)
{
	return matrixScale( u.x, u.y );
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixScale(float x, float y, float z)
{
	Mtx44 result;
	result(0, 0) = x;
	result(1, 1) = y;
	result(2, 2) = z;
	return result;
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixScale(const math::Vec3& v)
{
	return matrixScale( v.x, v.y, v.z );
}
//-------------------------------------------------------------------
//	matrixTexTransform
//-------------------------------------------------------------------
math::Mtx33 math::matrixTexTransform(const Vec2& translate, const Vec2& scale, float rotateRads, const Vec2& originTranslate)
{
	Mtx33 originTransform = math::matrixTranslate( originTranslate );
	Mtx33 undoOriginTransform = math::matrixTranslate( -originTranslate );

	return originTransform * math::matrixScale(scale) * math::matrixRotate2d(rotateRads) * undoOriginTransform * matrixTranslate(translate);
}
math::Mtx33 math::matrixTexTransformDegs(const math::Vec2& translate, const math::Vec2& scale, float rotateDegs, const math::Vec2& originTranslate)
{
	return matrixTexTransform( translate, scale, toRads(rotateDegs), originTranslate );
}

//-------------------------------------------------------------------
//	matrixTranspose
//-------------------------------------------------------------------
math::Mtx33 math::matrixTranspose(const Mtx33& m)
{
	return Mtx33(
		m[0], m[3], m[6],
		m[1], m[4], m[7],
		m[2], m[5], m[8]
	);
}
//-------------------------------------------------------------------
void math::matrixTranspose(const Mtx44& m, Mtx44& outResult)
{
	outResult.Set(
		m[ 0], m[ 4], m[ 8], m[12],
		m[ 1], m[ 5], m[ 9], m[13],
		m[ 2], m[ 6], m[10], m[14],
		m[ 3], m[ 7], m[11], m[15]
	);
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixTranspose(const Mtx44& m)
{
	Mtx44 result;
	matrixTranspose( m, result );
	return result;
}
//-------------------------------------------------------------------
//	matrixPitch
//-------------------------------------------------------------------
math::Mtx44 math::matrixPitch(float rads)
{
	Mtx44 result;
	float cosine = cosf( rads );
	float sine = sinf( rads );

	result[ 5] = cosine;
	result[ 6] = sine;
	result[ 9] = -sine;
	result[10] = cosine;

	return result;
}
//-------------------------------------------------------------------
//	matrixPitchDegs
//-------------------------------------------------------------------
math::Mtx44 math::matrixPitchDegs(float degs)
{
	return matrixPitch( toRads(degs) );
}
//-------------------------------------------------------------------
//	matrixYaw
//-------------------------------------------------------------------
math::Mtx44 math::matrixYaw(float rads)
{
	Mtx44 result;
	float cosine = cos( rads );
	float sine = sin( rads );

	result[ 0] = cosine;
	result[ 2] = -sine;
	result[ 8] = sine;
	result[10] = cosine;

	return result;
}
//-------------------------------------------------------------------
//	matrixYawDegs
//-------------------------------------------------------------------
math::Mtx44 math::matrixYawDegs(float degs)
{
	return matrixYaw( toRads(degs) );
}
//-------------------------------------------------------------------
//	matrixRoll
//-------------------------------------------------------------------
math::Mtx44 math::matrixRoll(float rads)
{
	Mtx44 result;
	float cosine = cos( rads );
	float sine = sin( rads );

	result[0] = cosine;
	result[1] = sine;
	result[4] = -sine;
	result[5] = cosine;

	return result;
}
//-------------------------------------------------------------------
//	matrixRollDegs
//-------------------------------------------------------------------
math::Mtx44 math::matrixRollDegs(float degs)
{
	return matrixRoll( toRads(degs) );
}
//-------------------------------------------------------------------
//	matrixYawPitchRoll
//-------------------------------------------------------------------
math::Mtx44 math::matrixYawPitchRoll(const Euler& angle)
{
	Euler e = angle.AsRads();
	return matrixYawPitchRoll( e.yaw, e.pitch, e.roll );
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixYawPitchRoll(float yaw, float pitch, float roll)
{
	return 
		matrixRoll( roll ) *
		matrixPitch( pitch ) *
		matrixYaw( yaw );
}
//-------------------------------------------------------------------
//	matrixYawPitchRollDegs
//-------------------------------------------------------------------
math::Mtx44 math::matrixYawPitchRollDegs(float yaw, float pitch, float roll)
{
	return matrixYawPitchRoll( toRads(yaw), toRads(pitch), toRads(roll) );
}
//-------------------------------------------------------------------
//	matrixRotate
//-------------------------------------------------------------------
math::Mtx44 math::matrixRotateAxis(float x, float y, float z, float angle)
{
	// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

	Mtx44 result;

	float c = cos( angle );
	float s = sin( angle );
	float one_cos = 1 - c; 

	result(0, 0) = c + x * x * one_cos;
	result(0, 1) = x * y * one_cos + z * s;
	result(0, 2) = x * z * one_cos - y * s;

	result(1, 0) = x * y * one_cos - z * s;
	result(1, 1) = c + y * y * one_cos;
	result(1, 2) = y * z * one_cos + x * s;

	result(2, 0) = x * z * one_cos + y * s;
	result(2, 1) = y * z * one_cos - x * s;
	result(2, 2) = c + z * z * one_cos;

	return result;
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixRotateAxis(const Vec3& axis, float angle)
{
	return matrixRotateAxis( axis.x, axis.y, axis.z, angle );
}
//-------------------------------------------------------------------
//	matrixRotateDegs
//-------------------------------------------------------------------
math::Mtx44 math::matrixRotateDegs(float x, float y, float z, float angle)
{
	return matrixRotateAxis( x, y, z, toRads(angle) );
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixRotateDegs(const Vec3& axis, float angle)
{
	return matrixRotateAxis( axis.x, axis.y, axis.z, toRads(angle) );
}
//-------------------------------------------------------------------
//	matrixRotateEuler
//-------------------------------------------------------------------
math::Mtx44 math::matrixRotateEuler(const Euler& e)
{
	Euler angle = e.AsRads();
	return math::matrixYawPitchRoll( angle.yaw, angle.pitch, angle.roll );
}
//-------------------------------------------------------------------
//	matrixRotate2d
//-------------------------------------------------------------------
math::Mtx33 math::matrixRotate2d(float rads)
{
	float c = cos( rads );
	float s = sin( rads );

	return Mtx33(
		c   , -s  , 0.0f,
		s   ,  c  , 0.0f,
		0.0f, 0.0f, 1.0f
	);
}
//-------------------------------------------------------------------
//	matrixRotate2dDegs
//-------------------------------------------------------------------
math::Mtx33 math::matrixRotate2dDegs(float degs)
{
	return matrixRotate2d( toRads(degs) );
}
//-------------------------------------------------------------------
//	matrixOrthogonalUp
//-------------------------------------------------------------------
math::Mtx44 math::matrixOrthogonalUp(const Vec3& up)
{
	Vec3 axis1 = AXIS_X;
	Vec3 axis2 = normalize( up );

	if ( isEqual( abs(axis1 * axis2), 1.0f ) )
	{
		axis1 = normalize(AXIS_Z);
	}

	Vec3 axis3 = normalize( cross( axis1, axis2 ) );
	axis1 = cross( axis2, axis3 );

	#ifdef DEBUG_MATH_CHECK_MATRIX_ORTHOGONAL
		CHECK( isZero(axis1 * axis2) );
		CHECK( isZero(axis1 * axis3) );
		CHECK( isZero(axis2 * axis3) );
	#endif

	Mtx44 result;
	result.SetRow( 0, Vec4( axis1, 0.0f ) );
	result.SetRow( 1, Vec4( axis2, 0.0f ) );
	result.SetRow( 2, Vec4( axis3, 0.0f ) );

	return result;
}
//-------------------------------------------------------------------
//	matrixOrthogonalLook
//-------------------------------------------------------------------
math::Mtx44 math::matrixOrthogonalLook(const Vec3& look)
{
	Vec3 axis1 = AXIS_X;
	Vec3 axis3 = normalize( look );

	if ( isEqual( abs(axis1 * axis3), 1.0f ) )
	{
		axis1 = normalize(AXIS_Y);
	}

	Vec3 axis2 = normalize( cross( axis3, axis1 ) );
	axis1 = cross( axis2, axis3 );

	#ifdef DEBUG_MATH_CHECK_MATRIX_ORTHOGONAL
		CHECK( math::isEqual( axis1 * axis1, 1.0f ) || math::isZero( axis1 * axis1 ) );
		CHECK( math::isEqual( axis2 * axis2, 1.0f ) || math::isZero( axis2 * axis2 ) );
		CHECK( math::isEqual( axis3 * axis3, 1.0f ) || math::isZero( axis3 * axis3 ) );
		CHECK( isZero(axis1 * axis2) );
		CHECK( isZero(axis1 * axis3) );
		CHECK( isZero(axis2 * axis3) );
	#endif

	Mtx44 result;
	result.SetRow( 0, Vec4( axis1, 0.0f ) );
	result.SetRow( 1, Vec4( axis2, 0.0f ) );
	result.SetRow( 2, Vec4( axis3, 0.0f ) );

	return result;
}
//-------------------------------------------------------------------
//	matrixOrthogonalLookUp
//-------------------------------------------------------------------
math::Mtx44 math::matrixOrthogonalLookUp(const Vec3& look, const Vec3& up)
{
	Mtx44 result;
	Vec3 zAxis = normalize( look );
	Vec3 xAxis = normalize( cross(up, zAxis) );
	Vec3 yAxis = cross( zAxis, xAxis );

	#ifdef DEBUG_MATH_CHECK_MATRIX_ORTHOGONAL
		CHECK( math::isEqual( xAxis * xAxis, 1.0f ) || math::isZero( xAxis * xAxis ) );
		CHECK( math::isEqual( yAxis * yAxis, 1.0f ) || math::isZero( yAxis * yAxis ) );
		CHECK( math::isEqual( zAxis * zAxis, 1.0f ) || math::isZero( zAxis * zAxis ) );
		CHECK( isZero(xAxis * yAxis) );
		CHECK( isZero(xAxis * zAxis) );
		CHECK( isZero(yAxis * zAxis) );
	#endif

	result.SetRow( 0, Vec4( xAxis, 0.0f ) );
	result.SetRow( 1, Vec4( yAxis, 0.0f ) );
	result.SetRow( 2, Vec4( zAxis, 0.0f ) );

	return result;
}
//-------------------------------------------------------------------
//	matrixDeterminant
//-------------------------------------------------------------------
template <class MATRIX>
float matrixDeterminant(const MATRIX& m)
{
	// NOTE: Very unoptimized... =/ But cute ^^
	// https://en.wikipedia.org/wiki/Laplace_expansion
	float result = 0.0f;

	for ( uint i = 0; i < MATRIX::s_width; i++ )
	{
		uint row;
		uint col;
		math::matrixIndexToRowColumn<MATRIX>( i, row, col );

		float sign = math::isPair(row + col) ? 1.0f : -1.0f;
		result += sign * m(0, i) * matrixDeterminant( m.Eliminate(0, i) );
	}

	return result;
}
//-------------------------------------------------------------------
template <>
float matrixDeterminant<math::Mtx22>(const math::Mtx22& m)
{
	return m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1);
}
//-------------------------------------------------------------------
float math::matrixDeterminant(const Mtx22& m)
{
	return ::matrixDeterminant( m );
}
//-------------------------------------------------------------------
float math::matrixDeterminant(const Mtx33& m)
{
	return ::matrixDeterminant( m );
}
//-------------------------------------------------------------------
float math::matrixDeterminant(const Mtx44& m)
{
	// https://www.mathsisfun.com/algebra/matrix-determinant.html
	float det00 = m[ 0] * ( m[ 5]*(m[10]*m[15]-m[14]*m[11]) - m[ 6]*(m[ 9]*m[15]-m[13]*m[11]) + m[ 7]*(m[ 9]*m[14]-m[13]*m[10]) );
	float det01 = m[ 1] * ( m[ 4]*(m[10]*m[15]-m[14]*m[11]) - m[ 6]*(m[ 8]*m[15]-m[12]*m[11]) + m[ 7]*(m[ 8]*m[14]-m[12]*m[10]) );
	float det02 = m[ 2] * ( m[ 4]*(m[ 9]*m[15]-m[13]*m[11]) - m[ 5]*(m[ 8]*m[15]-m[12]*m[11]) + m[ 7]*(m[ 8]*m[13]-m[12]*m[ 9]) );
	float det03 = m[ 3] * ( m[ 4]*(m[ 9]*m[14]-m[13]*m[10]) - m[ 5]*(m[ 8]*m[14]-m[12]*m[10]) + m[ 6]*(m[ 8]*m[13]-m[12]*m[ 9]) );

	float result = det00 - det01 + det02 - det03;

	return result;

	//return ::matrixDeterminant( m );
}
//-------------------------------------------------------------------
//	matrixCofactor
//-------------------------------------------------------------------
template <class MATRIX>
MATRIX matrixCofactor(const MATRIX& m)
{
	MATRIX result;

	for ( uint i = 0; i < MATRIX::s_size; i++ )
	{
		uint row;
		uint col;
		math::matrixIndexToRowColumn<MATRIX>( i, row, col );

		float sign = math::isPair(row + col) ? 1.0f : -1.0f;
		float det = math::matrixDeterminant( m.Eliminate(i) );
		float value = sign * det;
		result[i] = value;
	}

	return result;
}
//-------------------------------------------------------------------
math::Mtx33 math::matrixCofactor(const math::Mtx33& m)
{
	return ::matrixCofactor( m );
}
//-------------------------------------------------------------------
void math::matrixCofactor(const Mtx44& m, Mtx44& outResult)
{
	outResult[ 0] = +( m[ 5]*(m[10]*m[15]-m[14]*m[11]) - m[ 6]*(m[ 9]*m[15]-m[13]*m[11]) + m[ 7]*(m[ 9]*m[14]-m[13]*m[10]) );
	outResult[ 1] = -( m[ 4]*(m[10]*m[15]-m[14]*m[11]) - m[ 6]*(m[ 8]*m[15]-m[12]*m[11]) + m[ 7]*(m[ 8]*m[14]-m[12]*m[10]) );
	outResult[ 2] = +( m[ 4]*(m[ 9]*m[15]-m[13]*m[11]) - m[ 5]*(m[ 8]*m[15]-m[12]*m[11]) + m[ 7]*(m[ 8]*m[13]-m[12]*m[ 9]) );
	outResult[ 3] = -( m[ 4]*(m[ 9]*m[14]-m[13]*m[10]) - m[ 5]*(m[ 8]*m[14]-m[12]*m[10]) + m[ 6]*(m[ 8]*m[13]-m[12]*m[ 9]) );

	outResult[ 4] = -( m[ 1]*(m[10]*m[15]-m[14]*m[11]) - m[ 2]*(m[ 9]*m[15]-m[13]*m[11]) + m[ 3]*(m[ 9]*m[14]-m[13]*m[10]) );
	outResult[ 5] = +( m[ 0]*(m[10]*m[15]-m[14]*m[11]) - m[ 2]*(m[ 8]*m[15]-m[12]*m[11]) + m[ 3]*(m[ 8]*m[14]-m[12]*m[10]) );
	outResult[ 6] = -( m[ 0]*(m[ 9]*m[15]-m[13]*m[11]) - m[ 1]*(m[ 8]*m[15]-m[12]*m[11]) + m[ 3]*(m[ 8]*m[13]-m[12]*m[ 9]) );
	outResult[ 7] = +( m[ 0]*(m[ 9]*m[14]-m[13]*m[10]) - m[ 1]*(m[ 8]*m[14]-m[12]*m[10]) + m[ 2]*(m[ 8]*m[13]-m[12]*m[ 9]) );

	outResult[ 8] = +( m[ 1]*(m[ 6]*m[15]-m[14]*m[ 7]) - m[ 2]*(m[ 5]*m[15]-m[13]*m[ 7]) + m[ 3]*(m[ 5]*m[14]-m[13]*m[ 6]) );
	outResult[ 9] = -( m[ 0]*(m[ 6]*m[15]-m[14]*m[ 7]) - m[ 2]*(m[ 4]*m[15]-m[12]*m[ 7]) + m[ 3]*(m[ 4]*m[14]-m[12]*m[ 6]) );
	outResult[10] = +( m[ 0]*(m[ 5]*m[15]-m[13]*m[ 7]) - m[ 1]*(m[ 4]*m[15]-m[12]*m[ 7]) + m[ 3]*(m[ 4]*m[13]-m[12]*m[ 5]) );
	outResult[11] = -( m[ 0]*(m[ 5]*m[14]-m[13]*m[ 6]) - m[ 1]*(m[ 4]*m[14]-m[12]*m[ 6]) + m[ 2]*(m[ 4]*m[13]-m[12]*m[ 5]) );

	outResult[12] = -( m[ 1]*(m[ 6]*m[11]-m[10]*m[ 7]) - m[ 2]*(m[ 5]*m[11]-m[ 9]*m[ 7]) + m[ 3]*(m[ 5]*m[10]-m[ 9]*m[ 6]) );
	outResult[13] = +( m[ 0]*(m[ 6]*m[11]-m[10]*m[ 7]) - m[ 2]*(m[ 4]*m[11]-m[ 8]*m[ 7]) + m[ 3]*(m[ 4]*m[10]-m[ 8]*m[ 6]) );
	outResult[14] = -( m[ 0]*(m[ 5]*m[11]-m[ 9]*m[ 7]) - m[ 1]*(m[ 4]*m[11]-m[ 8]*m[ 7]) + m[ 3]*(m[ 4]*m[ 9]-m[ 8]*m[ 5]) );
	outResult[15] = +( m[ 0]*(m[ 5]*m[10]-m[ 9]*m[ 6]) - m[ 1]*(m[ 4]*m[10]-m[ 8]*m[ 6]) + m[ 2]*(m[ 4]*m[ 9]-m[ 8]*m[ 5]) );
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixCofactor(const Mtx44& m)
{
	Mtx44 result;
	matrixCofactor( m, result );

	return result;

	//return ::matrixCofactor( m );
}
//-------------------------------------------------------------------
//	matrixInvert
//-------------------------------------------------------------------
void math::matrixInvert(const Mtx44& m, Mtx44& outResult)
{
	float determinant = matrixDeterminant( m );

	//if ( isZero(determinant) )
	if ( determinant > -EPSILON && determinant < EPSILON )
	{
		outResult = math::Mtx44();
		return;
	}
	
	matrixCofactor( m, outResult );
	matrixTranspose( outResult, outResult );
	outResult *= 1.0f / determinant;
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixInvert(const Mtx44& m)
{
	// https://en.wikipedia.org/wiki/Invertible_matrix#In_relation_to_its_adjugate

	float determinant = matrixDeterminant( m );

	if ( isZero(determinant) )
	{
		return Mtx44();
	}

	Mtx44 cofactor = matrixCofactor( m );
	Mtx44 result = matrixTranspose( cofactor ) / determinant;

	return result;
}
//-------------------------------------------------------------------
//	matrixNormalTransform
//-------------------------------------------------------------------
math::Mtx44 math::matrixNormalTransform(const Mtx44& world)
{
	Mtx44 result = world;
	result.SetRow( 3, math::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
	result = matrixTranspose( matrixInvert(result) );

	return result;
}
//-------------------------------------------------------------------
//	matrixView
//-------------------------------------------------------------------
math::Mtx44 math::matrixView(const Vec3& pos, const Vec3& right, const Vec3& up, const Vec3& look)
{
	math::Mtx44 rotate(
		right           , 0.0f,
		up              , 0.0f,
		look            , 0.0f,
		math::Vec3(0.0f), 1.0f
	);

	return matrixTranslate(-pos) * matrixTranspose( rotate );
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixViewLook(const Vec3& pos, const Vec3& look)
{
	Mtx44 rotate = matrixOrthogonalLook( look );
	return matrixView( pos,
		rotate.GetRow(0).Xyz(), rotate.GetRow(1).Xyz(), rotate.GetRow(2).Xyz() );
}
//-------------------------------------------------------------------
math::Mtx44 math::matrixViewLookUp(const Vec3& pos, const Vec3& look, const Vec3& up)
{
	Mtx44 rotate = matrixOrthogonalLookUp( look, up );
	return matrixView( pos,
		rotate.GetRow(0).Xyz(), rotate.GetRow(1).Xyz(), rotate.GetRow(2).Xyz() );
}
//-------------------------------------------------------------------
//	extractViewFrustum
// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
//-------------------------------------------------------------------
math::Frustum math::extractViewFrustum(const Mtx44& m)
{
	Frustum result;

	result.front.a = m(0, 2);
	result.front.b = m(1, 2);
	result.front.c = m(2, 2);
	result.front.d = m(3, 2);

	result.back.a = m(0, 3) - m(0, 2);
	result.back.b = m(1, 3) - m(1, 2);
	result.back.c = m(2, 3) - m(2, 2);
	result.back.d = m(3, 3) - m(3, 2);

	result.left.a = m(0, 3) + m(0, 0);
	result.left.b = m(1, 3) + m(1, 0);
	result.left.c = m(2, 3) + m(2, 0);
	result.left.d = m(3, 3) + m(3, 0);

	result.right.a = m(0, 3) - m(0, 0);
	result.right.b = m(1, 3) - m(1, 0);
	result.right.c = m(2, 3) - m(2, 0);
	result.right.d = m(3, 3) - m(3, 0);

	result.top.a = m(0, 3) - m(0, 1);
	result.top.b = m(1, 3) - m(1, 1);
	result.top.c = m(2, 3) - m(2, 1);
	result.top.d = m(3, 3) - m(3, 1);

	result.bottom.a = m(0, 3) + m(0, 1);
	result.bottom.b = m(1, 3) + m(1, 1);
	result.bottom.c = m(2, 3) + m(2, 1);
	result.bottom.d = m(3, 3) + m(3, 1);

	result.front = normalizePlane( result.front );
	result.back = normalizePlane( result.back );
	result.left = normalizePlane( result.left );
	result.right = normalizePlane( result.right );
	result.top = normalizePlane( result.top );
	result.bottom = normalizePlane( result.bottom );

	return result;
}

/****************************************************************************************
	INTERSECTION
*****************************************************************************************/

//-------------------------------------------------------------------
//	classifyCollision
//-------------------------------------------------------------------
uint classifyCollision(float t)
{
	return t > 0.0f ? math::COLLISION_FRONT : math::COLLISION_BACK;
}
//-------------------------------------------------------------------
uint classifyCollision(float& t0, float& t1)
{
	uint result = 0;

	if ( t0 > t1 )
		math::swap( t0, t1 );

	if ( t1 < 0.0f )
		result = math::COLLISION_BACK;
	else if ( t0 > 0.0f )
		result = math::COLLISION_FRONT;
	else
		result = math::COLLISION_BACK | math::COLLISION_FRONT;

	return result;
}
//-------------------------------------------------------------------
//	pointClosestAxis
//-------------------------------------------------------------------
math::Vec3 math::pointClosestAxis(const math::Vec3& point, const math::Vec3& scale)
{
	math::Vec3 pointAbs = point.Abs();

	float distAxes[] = {
		scale.x - pointAbs.x,
		scale.y - pointAbs.y,
		scale.z - pointAbs.z
	};
	float closestDist = distAxes[0];
	math::Vec3 result = AXIS_X;

	for ( uint i = 1; i < 3; i++ )
	{
		if ( (math::isZero(distAxes[i]) || distAxes[i] > 0.0f) && distAxes[i] < closestDist )
		{
			closestDist = distAxes[i];
			result = AXES[i];
		}
	}

	return result * math::sign(point * result);
}
//-------------------------------------------------------------------
//	pointOnQuad
//-------------------------------------------------------------------
bool math::pointOnQuad(const math::Vec2& point, const Vec2& squareMin, const Vec2& squareMax)
{
	return between( point.x, squareMin.x, squareMax.x ) && 
		between( point.y, squareMin.y, squareMax.y );
}
//-------------------------------------------------------------------
bool math::pointOnQuad(const math::Vec2& point, const Aabb2& aabb)
{
	return pointOnQuad( point, aabb.min, aabb.max );
}
//-------------------------------------------------------------------
//	pointInsideAabb
//-------------------------------------------------------------------
bool math::pointInsideAabb(const math::Vec3& point, const Aabb3& aabb)
{
	return !(
		point.x > aabb.max.x || point.x < aabb.min.x ||
		point.y > aabb.max.y || point.y < aabb.min.y ||
		point.z > aabb.max.z || point.z < aabb.min.z
	);
}
//-------------------------------------------------------------------
//	intersectRaySphere
//-------------------------------------------------------------------
math::IntersectRaySphereInfo math::intersectRaySphere(const math::Vec3& rayOrigin, const math::Vec3& dir, const math::Vec3& circlePos, float radius)
{
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection

	IntersectRaySphereInfo result;
	Vec3 toPoint = rayOrigin - circlePos;
	
	float b = 2.0f * dir * toPoint;
	float c = toPoint * toPoint - radius * radius;

	if ( bhaskara( 1.0f, b, c, result.t0, result.t1 ) )
	{
		result.collisionFlag = classifyCollision(result.t0, result.t1);
	}

	return result;
}
//-------------------------------------------------------------------
//	intersectRayPlane
//-------------------------------------------------------------------
math::IntersectRayPlaneInfo math::intersectRayPlane(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& planePoint, const Vec3& normal)
{
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
	IntersectRayPlaneInfo result;

	float denominator = rayDir * normal;

	if ( !math::isZero(denominator) )
	{
		result.t = (planePoint - rayOrigin) * normal / denominator;
		result.collisionFlag = classifyCollision(result.t);
	}

	return result;
}
//-------------------------------------------------------------------
//	intersectRayCircle
//-------------------------------------------------------------------
math::IntersectRayCircleInfo math::intersectRayCircle(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& circlePoint, const Vec3& circleNormal, float radius)
{
	IntersectRayCircleInfo result;

	auto intersectPlane = intersectRayPlane( rayOrigin, rayDirection, circlePoint, circleNormal );
	result.t = intersectPlane.t;

	if ( distSq(rayOrigin + rayDirection * result.t, circlePoint) < radius * radius )
		result.collisionFlag = classifyCollision( result.t );

	return result;
}
//-------------------------------------------------------------------
//	intersectRayAabb
//-------------------------------------------------------------------
math::IntersectRayAabbInfo math::intersectRayAabb(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& boxMin, const Vec3& boxMax)
{
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

	IntersectRayAabbInfo result;

	math::Vec3 invDir(1.0f, 1.0f, 1.0f);
	invDir.x /= rayDir.x;
	invDir.y /= rayDir.y;
	invDir.z /= rayDir.z;

	float tmin = (boxMin.x - rayOrigin.x) * invDir.x;
	float tmax = (boxMax.x - rayOrigin.x) * invDir.x;

	if (tmin > tmax) 
		swap(tmin, tmax);

	float tymin = (boxMin.y - rayOrigin.y) * invDir.y;
	float tymax = (boxMax.y - rayOrigin.y) * invDir.y;

	if (tymin > tymax)
		swap(tymin, tymax);

	if (tmin > tymax || tymin > tmax)
		return result;

	tmin = max(tmin, tymin);
	tmax = min(tmax, tymax);

	float tzmin = (boxMin.z - rayOrigin.z) * invDir.z;
	float tzmax = (boxMax.z - rayOrigin.z) * invDir.z;

	if (tzmin > tzmax)
		swap(tzmin, tzmax);

	if (tmin > tzmax || tzmin > tmax)
		return result;

	tmin = max( tmin, tzmin );
	tmax = min( tmax, tzmax );

	result.t = tmin;
	result.collisionFlag = classifyCollision(result.t);

	return result;
}
//-------------------------------------------------------------------
math::IntersectRayAabbInfo math::intersectRayAabb(const Vec3& rayOrigin, const Vec3& rayDir, const Aabb3& aabb)
{
	return intersectRayAabb( rayOrigin, rayDir, aabb.min, aabb.max );
}
//-------------------------------------------------------------------
//	intersectRayTriangle
//-------------------------------------------------------------------
math::IntersectRayTriangleInfo math::intersectRayTriangle(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& e0, const Vec3& e1, const Vec3& e2)
{
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection

	IntersectRayTriangleInfo result;

	// Matrix for Cramer's rule: | -rayDir E1 E2 |
	// Where on tutorial E1 and E2, here j, k, respectively
	Vec3 t = rayOrigin - e0;
	Vec3 j = e1 - e0;
	Vec3 k = e2 - e0;
	// Using triple product https://en.wikipedia.org/wiki/Triple_product
	float detM = -rayDir * cross(j, k);

	if ( !isZero(detM) )
	{
		float invDetM = 1.0f * detM;
		float detT = t * cross(j, k);
		float detU = -rayDir * cross(t, k);
		float detV = -rayDir * cross(j, t);

		result.t = detT * invDetM;
		result.u = detU * invDetM;
		result.v = detV * invDetM;

		if ( result.u > 0.0f && result.v > 0.0f && (result.u + result.v) < 1.0f )
			result.collisionFlag = classifyCollision(result.t);
	}

	return result;
}
//-------------------------------------------------------------------
//	intersectRayQuad
//-------------------------------------------------------------------
math::IntersectRayQuadInfo math::intersectRayQuad(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& e0, const Vec3& e1, const Vec3& e2)
{
	// https://stackoverflow.com/questions/21114796/3d-ray-quad-intersection-test-in-java

	IntersectRayQuadInfo result;

	auto intersectPlane = intersectRayPlane( rayOrigin, rayDir, e0, cross(e2 - e0, e1 - e0) );

	if ( intersectPlane.collisionFlag & COLLISION_NONE )
		return result;

	result.t = intersectPlane.t;
	Vec3 rayOnPlane = rayOrigin + rayDir * result.t;
	Vec3 u = e1 - e0;
	Vec3 v = e2 - e0;
	Vec3 rayToE0 = rayOnPlane - e0;
	float rayOnU = u * rayToE0;
	float rayOnV = v * rayToE0;

	if ( rayOnU < 0.0f || rayOnV < 0.0f || rayOnU > lengthSq(u) || rayOnV > lengthSq(v) )
		return result;

	result.collisionFlag = classifyCollision(result.t);

	return result;
}
//-------------------------------------------------------------------
//	intersectRayCylinder
//-------------------------------------------------------------------
math::IntersectRayCylinderInfo math::intersectRayCylinder(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& cylinderStart, const Vec3& cylinderEnd, float radius)
{
	// http://hugi.scene.org/online/hugi24/coding%20graphics%20chris%20dragan%20raytracing%20shapes.htm
	IntersectRayCylinderInfo result;

	uint collisionFrontBack = COLLISION_FRONT | COLLISION_BACK;
	Vec3 d = rayDir;
	Vec3 n = normalize(cylinderEnd - cylinderStart);
	Vec3 x = rayOrigin - cylinderStart;
	float nd = d * n;
	float rr = radius * radius;
	float dx = d * x;
	float nx = n * x;

	float a = d*d - nd*nd;
	float b = 2.0f * (dx - nd*nx);
	float c = x*x - nx*nx -rr;

	if ( bhaskara(a, b, c, result.t0, result.t1) )
	{
		Vec3 t0point = rayOrigin + rayDir * result.t0;
		Vec3 t1point = rayOrigin + rayDir * result.t1;

		Vec3 tpoint[2] = { t0point, t1point };
		Vec3 cylinderPoint[] = { cylinderStart, cylinderEnd };
		Vec3 circleNormal[] = { -n, n };
		uint* collisionFlag[2] = { &result.t0collisionFlag, &result.t1collisionFlag };
		float* ptrTvalue[2] = { &result.t0, &result.t1 };

		for ( uint i = 0; i < 2; i++ )
		{
			IntersectRayCylinderInfo intersectCylinder;

			if ( classifyPointPlane(tpoint[i], cylinderStart, n) & PLANE_FRONT && classifyPointPlane(tpoint[i], cylinderEnd, n) & PLANE_BACK )
				*collisionFlag[i] = classifyCollision(*ptrTvalue[i]);

			auto intersectCircle = intersectRayCircle( rayOrigin, rayDir, cylinderPoint[i], circleNormal[i], radius );

			if ( intersectCircle.collisionFlag != COLLISION_NONE )
			{
				bool noCylinderCollision = *collisionFlag[i] == COLLISION_NONE;

				if ( noCylinderCollision || intersectCircle.t < *collisionFlag[i] )
				{
					*ptrTvalue[i] = intersectCircle.t;
					*collisionFlag[i] = intersectCircle.collisionFlag;
				}
			}
		}

		if ( result.t0collisionFlag & COLLISION_NONE )
		{
			swap( result.t0, result.t1 );
			swap( result.t0collisionFlag, result.t1collisionFlag );
		}
		else if ( result.t1collisionFlag != COLLISION_NONE )
		{
			if ( result.t0 > result.t1 )
			{
				swap( result.t0, result.t1 );
				swap( result.t0collisionFlag, result.t1collisionFlag );
			}
		}

		result.collision = result.t0collisionFlag & collisionFrontBack || result.t1collisionFlag & collisionFrontBack;
	}

	return result;
}
//-------------------------------------------------------------------
//	intersectAabb3Frustum
//-------------------------------------------------------------------
bool math::intersectAabb3Frustum(const Aabb3& aabb, const Frustum& frustum)
{
	Vec3 points[8];
	const Plane* frustumPlanes[6] = { 
		&frustum.front, 
		&frustum.back, 
		&frustum.left, 
		&frustum.right,
		&frustum.top, 
		&frustum.bottom,
	};

	getAabb3Points( aabb, points, ARRAY_COUNT(points) );

	for ( uint i = 0; i < ARRAY_COUNT(frustumPlanes); i++ )
	{
		bool allPointsOutsidePlane = true;

		for ( uint j = 0; j < ARRAY_COUNT(points); j++ )
		{
			if ( classifyPointPlane(points[j], *frustumPlanes[i]) == PLANE_FRONT )
			{
				allPointsOutsidePlane = false;
				break;
			}
		}

		if ( allPointsOutsidePlane )
			return false;
	}

	return true;
}
//-------------------------------------------------------------------
//	intersectAabb3
//-------------------------------------------------------------------
math::IntersectAabb3Info math::intersectAabb3(const Vec3& pos0, const Vec3& scale0, const Vec3& pos1, const Vec3& scale1)
{
	IntersectAabb3Info result;
	math::Vec3 scaleSum = scale1 + scale0;
	math::Vec3 toPos0 = pos0 - pos1;
	math::Vec3 absToPos0 = math::abs( toPos0 );
	math::Vec3 collisionAmount = scaleSum - absToPos0;

	uint minAmountAxis = 0;
	float minAmount = collisionAmount[minAmountAxis];

	if ( minAmount < 0.0f )
		return result;

	for ( uint i = 1; i < 3; i++ )
	{
		if ( collisionAmount[i] < 0.0f )
			return result;

		if ( collisionAmount[i] < minAmount )
		{
			minAmountAxis = i;
			minAmount = collisionAmount[i];
		}
	}

	Vec3 aabb1Min = pos1 - scale1;
	Vec3 aabb1Max = pos1 + scale1;

	result.collision = true;
	result.resolveAmount = collisionAmount[minAmountAxis];
	result.resolveNormal = AXES[minAmountAxis] * sign(toPos0[minAmountAxis]);
	result.collisionPoint.x = math::clamp( pos0.x, aabb1Min.x, aabb1Max.x );
	result.collisionPoint.y = math::clamp( pos0.y, aabb1Min.y, aabb1Max.y );
	result.collisionPoint.z = math::clamp( pos0.z, aabb1Min.z, aabb1Max.z );

	return result;
}
//-------------------------------------------------------------------
math::IntersectAabb3Info math::intersectAabb3(const Aabb3& aabb0, const Aabb3& aabb1)
{
	return intersectAabb3( aabb0.GetPos(), aabb0.GetScale(), aabb1.GetPos(), aabb1.GetScale() );
}
//-------------------------------------------------------------------
//	intersectAabb3Dynamic
//-------------------------------------------------------------------
math::IntersectAabb3DynamicInfo math::intersectAabb3Dynamic(const Vec3& pos0, const Vec3& scale0, const Vec3& relVel0, const Vec3& pos1, const Vec3& scale1)
{
	// A lambda on November 19th... s2
	auto calculateMinDistToAxis = [](const Vec3& pointOnAabbLocal, const Vec3& aabbScale, uint axisIndex)->float
	{
		return abs(pointOnAabbLocal[axisIndex] - aabbScale[axisIndex]);
	};

	IntersectAabb3DynamicInfo result;
	Vec3 scaleSum = scale1 + scale0;

	auto intersect = intersectRayAabb( pos0, relVel0, pos1 + math::Aabb3(scaleSum) );

	if ( intersect.collisionFlag != COLLISION_FRONT || intersect.t < 0.0f || intersect.t > 1.0f || math::isNan(intersect.t) )
		return result;

	Vec3 collisionPointScaleSum = pos0 + relVel0 * intersect.t;
	Vec3 collisionPointScaleSumLocal = collisionPointScaleSum - pos1;
	Vec3 collisionPointProportion = scaleInv( collisionPointScaleSumLocal, scaleSum );
	Vec3 collisionPointLocal = scale( scale1, collisionPointProportion );

	result.collision = true;
	result.t = intersect.t;
	result.collisionPoint = pos1 + collisionPointLocal;

	Vec3 collisionPointLocalAbs = math::abs( collisionPointLocal );

	uint axisIndex = 0;
	float minDistToAxis = calculateMinDistToAxis( collisionPointLocalAbs, scale1, 0 );

	for ( uint i = 1; i < 3; i++ )
	{
		float minDistToAxisCurrent = calculateMinDistToAxis( collisionPointLocalAbs, scale1, i );

		if ( minDistToAxisCurrent < minDistToAxis )
		{
			axisIndex = i;
			minDistToAxis = minDistToAxisCurrent;
		}
	}

	result.normal = AXES[axisIndex] * sign(collisionPointLocal[axisIndex]);

	return result;
}
//-------------------------------------------------------------------
//	intersectSphereAabb
//-------------------------------------------------------------------
math::IntersectSphereAabbInfo math::intersectSphereAabb(const math::Vec3& posSphere, float radius, const math::Vec3& posAabb, const math::Vec3& scaleAabb)
{
	IntersectSphereAabbInfo result;

	Vec3 toSphere = posSphere - posAabb;
	Vec3 pointClosestAabb = clamp( toSphere, -scaleAabb, scaleAabb );

	if ( distSq(toSphere, pointClosestAabb) > radius * radius )
		return result;

	result.collision = true;
	result.collisionPoint = posAabb + pointClosestAabb;
	result.normal = normalize( posSphere - result.collisionPoint );
	result.t = length( result.collisionPoint - (posSphere - result.normal * radius) );

	return result;
}
//-------------------------------------------------------------------
//	intersectSphereAabbDynamic
//-------------------------------------------------------------------
math::IntersectSphereAabbInfo math::intersectSphereAabbDynamic(const math::Vec3& posSphere, float radius, const math::Vec3& relVelSphere, const math::Vec3& posAabb, const math::Vec3& scaleAabb)
{
	IntersectSphereAabbInfo result;

	Vec3 radiusVec = math::Vec3(radius);
	auto intersectAabbRadius = intersectRayAabb( posSphere, relVelSphere, posAabb + math::Aabb3( scaleAabb + radiusVec ) );

	if ( intersectAabbRadius.collisionFlag == COLLISION_NONE || intersectAabbRadius.t > 1.0f || intersectAabbRadius.t < 0.0f || math::isNan(intersectAabbRadius.t) )
		return result;

	Vec3 pointAabbRadius = posSphere + relVelSphere * intersectAabbRadius.t - posAabb;
	Vec3 pointOnAabb = clamp( pointAabbRadius, -scaleAabb, scaleAabb );

	math::Vec3 sphereAtIntersectPoint = posSphere + relVelSphere * intersectAabbRadius.t;
	math::Vec3 collisionPoint = pointOnAabb + posAabb;

	if ( math::distSq(sphereAtIntersectPoint, collisionPoint) > radius * radius )
		return result;

	result.collision = true;
	result.t = intersectAabbRadius.t;
	result.collisionPoint = collisionPoint;
	result.normal = normalize( pointAabbRadius - pointOnAabb );

	return result;
}
//-------------------------------------------------------------------
//	classifyPointPlane
//-------------------------------------------------------------------
math::ePlane math::classifyPointPlane(const Vec3& point, const Vec3& planePoint, const Vec3& planeNormal)
{
	float dot = (point - planePoint) * planeNormal;
	if ( dot > 0.0f )
		return PLANE_FRONT;
	if ( dot < 0.0f )
		return PLANE_BACK;
	return PLANE_ON;
}
//-------------------------------------------------------------------
math::ePlane math::classifyPointPlane(const Vec3& point, const Plane& plane)
{
	float dot = point * Vec3(plane.a, plane.b, plane.c) + plane.d;
	if ( dot > 0.0f )
		return PLANE_FRONT;
	if ( dot < 0.0f )
		return PLANE_BACK;
	return PLANE_ON;
}
