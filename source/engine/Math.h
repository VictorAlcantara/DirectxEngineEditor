#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Angle.h"

#define QUAD(x) (x)*(x);

namespace math
{
	template<class> struct Aabb;

	typedef Aabb<Vec2> Aabb2;
	typedef Aabb<Vec3> Aabb3;

	/************************************************************************************
		ENUMS
	*************************************************************************************/

	enum eAxis
	{
		X = 0,
		Y,
		Z,
	};

	enum eSign
	{
		POSITIVE = 1,
		NEGATIVE = -1,
	};

	enum eFace 
	{
		FACE_FRONT = 0,
		FACE_BACK,
		FACE_RIGHT,
		FACE_LEFT,
		FACE_UP,
		FACE_DOWN,

		FACE_COUNT // Must always be last one! (used for indexing)
	};

	enum ePlane
	{
		PLANE_ON = 1 << 0,
		PLANE_FRONT = 1 << 1,
		PLANE_BACK = 1 << 2,
	};

	enum eCollision
	{
		COLLISION_NONE = 1 << 0,
		COLLISION_FRONT = 1 << 1,
		COLLISION_BACK = 1 << 2,
	};

	enum eGrowth
	{
		INCREASE = 1,
		DECREASE = -1,
	};

	/************************************************************************************
		CONSTANTS
	*************************************************************************************/

	const float PI = 3.14159265f;
	const float HALF_PI = PI * 0.5f;
	const float TWO_PI = 2.0f * PI;
	const float EPSILON = 0.0001f;
	const float FLOAT_MAX = 99999999.0f;
	const Vec3 AXIS_X(1.0f, 0.0f, 0.0f);
	const Vec3 AXIS_Y(0.0f, 1.0f, 0.0f);
	const Vec3 AXIS_Z(0.0f, 0.0f, 1.0f);
	const Vec3 AXES[3] = { AXIS_X, AXIS_Y, AXIS_Z };
	const eAxis AXES_ENUM[3] = { X, Y, Z };
	const uint RANDOM_SEED = 0;

	/************************************************************************************
		STRUCTS
	*************************************************************************************/

	struct AxisInfo
	{
		eAxis axis;
		eSign sign;

		AxisInfo() : axis(eAxis::X), sign(eSign::POSITIVE) {}
		AxisInfo(eAxis ax, eSign si) : axis(ax), sign(si) {}
	};

	struct Transform
	{
		Mtx44 scale;
		Mtx44 rotation;
		Mtx44 translate;

		Mtx44 GetWorld()const
		{
			return scale * rotation * translate;
		};

		Vec3 GetScale()const
		{
			return math::Vec3(scale(0, 0), scale(1, 1), scale(2, 2));
		};

		Vec3 GetPos()const
		{
			return translate.GetRow(3).Xyz();
		}
	};

	struct CameraTransform
	{
		Mtx44 view;
		Mtx44 proj;

		Vec3 pos;
		Vec3 right;
		Vec3 up;
		Vec3 look;

		float fovDegs;
		float fNear;
		float fFar;
		float aspectRatio;

		CameraTransform() : fovDegs(90.0f), fNear(0.0001f), fFar(1000.0f), aspectRatio(1.0f) {}

		math::Aabb3 GetFrustumAabbGlobal()const;
	};

	struct ViewportInfo
	{
		int left;
		int top;
		uint width;
		uint height;

		ViewportInfo() : left(0), top(0), width(0), height(0) {}

		float GetAspectRatio()const { return (float)width / (float)height; }
		Vec2 GetDimension()const { return Vec2((float)width, (float)height); }
		Vec2 GetDimensionScaled(float xScale, float yScale)const { return Vec2((float)width * xScale, (float)height * yScale); }
	};

	struct IntersectRaySphereInfo
	{
		uint collisionFlag;
		float t0;
		float t1;

		IntersectRaySphereInfo() : collisionFlag(COLLISION_NONE), t0(0.0f), t1(0.0f) {}
	};

	struct IntersectRayPlaneInfo
	{
		uint collisionFlag;
		float t;

		IntersectRayPlaneInfo() : collisionFlag(COLLISION_NONE), t(0.0f) {}
	};

	struct IntersectRayCircleInfo
	{
		uint collisionFlag;
		float t;

		IntersectRayCircleInfo() : collisionFlag(COLLISION_NONE), t(0.0f) {}
	};

	struct IntersectRayAabbInfo
	{
		uint collisionFlag;
		float t;

		IntersectRayAabbInfo() : collisionFlag(COLLISION_NONE), t(0.0f) {}
	};

	struct IntersectRayTriangleInfo
	{
		uint collisionFlag;
		float t;
		float u;
		float v;

		IntersectRayTriangleInfo() : collisionFlag(COLLISION_NONE), t(0.0f), u(0.0f), v(0.0f) {}
	};

	struct IntersectRayQuadInfo
	{
		uint collisionFlag;
		float t;

		IntersectRayQuadInfo() : collisionFlag(COLLISION_NONE), t(0.0f) {}
	};

	struct IntersectRayCylinderInfo
	{
		bool collision;
		uint t0collisionFlag;
		float t0;
		uint t1collisionFlag;
		float t1;

		IntersectRayCylinderInfo() : collision(false), t0collisionFlag(COLLISION_NONE), t1collisionFlag(COLLISION_NONE), t0(0.0f), t1(0.0f) {}
	};

	struct IntersectAabb3Info
	{
		bool collision;
		float resolveAmount;
		Vec3 resolveNormal;
		Vec3 collisionPoint;
		
		IntersectAabb3Info() : collision(false), resolveAmount(0.0f) {}
	};

	struct IntersectAabb3DynamicInfo
	{
		bool collision;
		float t;
		Vec3 collisionPoint;
		Vec3 normal;

		IntersectAabb3DynamicInfo() : collision(false), t(0.0f) {}
	};

	struct IntersectSphereAabbInfo
	{
		bool collision;
		float t;
		Vec3 collisionPoint;
		Vec3 normal;

		IntersectSphereAabbInfo() : collision(false), t(0.0f) {}
	};

	template <class VEC_TYPE>
	struct Aabb
	{
		VEC_TYPE min;
		VEC_TYPE max;

		Aabb() {}
		Aabb(const VEC_TYPE& scale) : min(-scale), max(scale) {}
		Aabb(const VEC_TYPE& min, const VEC_TYPE& max) : min(min), max(max) {}
		VEC_TYPE GetScale()const { return (max - min) * 0.5f; }
		VEC_TYPE GetPos()const { return (max + min) * 0.5f; }
	};

	struct Rect
	{
		float left;
		float top;
		float right;
		float bottom;

		Rect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
	};

	struct Plane
	{
		// ax + by + cz + d = 0
		float a;
		float b;
		float c;
		float d;

		Plane() : a(0.0f), b(0.0f), c(0.0f), d(0.0f) {}
	};

	struct Frustum
	{
		Plane front;
		Plane back;
		Plane left;
		Plane right;
		Plane top;
		Plane bottom;
	};

	/************************************************************************************
		GENERAL FUNCTIONS
	*************************************************************************************/

	int abs(int);
	float abs(float);
	int sign(int);
	float sign(float);
	bool isEqual(float, float, float epsilon = EPSILON);
	bool isEqualLess(float, float, float epsilon = EPSILON);
	bool isZero(float, float epsilon = EPSILON);
	bool isPair(int);
	bool isPair(uint);
	uint clamp(uint value, uint minValue, uint maxValue);
	float clamp(float value, float minValue, float maxValue);
	Rect clamp(const Rect&, float minValue, float maxValue);
	float clampMin(float value, float minValue);
	float sq(float);
	float sqrt(float);
	float random();
	float random(float val1, float v2);
	bool bhaskara(float a, float b, float c, float& root0, float& root1); // return true if the discriminant is >= 0
	float remainder(float divident, float divisor);
	float pow(float value, uint power);
	eGrowth invertGrowth(eGrowth);
	float sin(float);
	float cos(float);
	float tan(float);
	float sinDegs(float);
	float cosDegs(float);
	float tanDegs(float);
	float asin(float);
	float acos(float);
	float asinDegs(float);
	float acosDegs(float);
	bool isNan(float);
	void setMinMax(float value, float& refMin, float& refMax);
	void getAabb3Points(const Aabb3&, Vec3* outPoints, uint pointCount);
	Plane normalizePlane(const Plane&);

	/************************************************************************************
		ANGLE
	*************************************************************************************/
	
	float toRads(float degs);
	float toDegs(float rads);

	/************************************************************************************
		VECTOR
	*************************************************************************************/

	Vec3 abs(const Vec3&);
	Vec3 sign(const Vec3&);
	Vec2 parallel(const Vec2&);
	Vec2 scaleInv(const Vec2&, const Vec2&); // component division
	Vec3 scaleInv(const Vec3&, const Vec3&); // component division
	Vec2 scale(const Vec2&, const Vec2&); // Component multiplication
	Vec3 scale(const Vec3&, const Vec3&); // Component multiplication
	Vec4 scale(const Vec4&, const Vec4&); // Component multiplication
	Vec4 mulRgb(const Vec4&, float);
	float min(const Vec2&);
	float min(const Vec3&);
	float min(const Vec4&);
	float max(const Vec2&);
	float max(const Vec3&);
	float max(const Vec4&);
	Vec3 compMin(const Vec3&, const Vec3&);
	Vec3 compMax(const Vec3&, const Vec3&);
	Vec3 clamp(const Vec3& value, const Vec3& minValue, const Vec3& maxValue);
	Vec4 clamp(const Vec4& value, const Vec4& minValue, const Vec4& maxValue);
	Vec3 clampMin(const Vec3& value, const Vec3& minValue);
	Vec3 clampUnit(const Vec3& value);
	Vec4 clampUnit(const Vec4& value);
	Vec3 clampLength(const Vec3&, float minLength, float maxLength);
	float length(const Vec2&);
	float length(const Vec3&);
	float length(float x, float y, float z);
	float lengthSq(const Vec2&);
	float lengthSq(const Vec3&);
	float dist(const Vec3&, const Vec3&);
	float distSq(const Vec3&, const Vec3&);
	Vec3 normalize(const Vec3&);
	Vec3 normalize(float  x, float y, float z);
	Vec3 normalize(const Vec3&, float& outLength);
	Vec3 normalize(float  x, float y, float z, float& outLength);
	Vec2 normalize(const Vec2&);
	Vec2 normalize(float  x, float y);
	Vec3 cross(const Vec3&, const Vec3&);
	Vec3 screenToWorld(const Vec2& mousePos, uint clientWidth, uint clientHeight, const Mtx44& view, const Mtx44& proj);
	Vec3 screenToWorld(const Vec2& mousePos, const ViewportInfo& clientViewport, const CameraTransform&);
	Vec3 mousePosToWorld(const CameraTransform&);
	Vec4 normalizeRgb(ushort rgb, ushort alpha = 255);
	Vec4 normalizeRgb(ushort red, ushort green, ushort blue, ushort alpha = 255);
	Vec4 saturateRgb(const Vec4&, float saturation);
	Vec4 saturateRgb(const Vec3&, float saturation, float alpha = 1.0f);
	bool isEqual(const Vec3&, const Vec3&, float epsilon = EPSILON);
	Vec2 screenCoordToCenter(const Vec2& pos, const Vec2& viewportDimension);
	Vec2 screenCoordToCenter(float x, float y, const Vec2& viewportDimension);
	Vec2 centerToScreenCoord(const Vec2& pos, const Vec2& viewportDimension);
	Vec2 screenCoordToNdc(const Vec2&, const ViewportInfo&);
	Vec2 screenNormalizedToNdc(const Vec2&);
	Vec2 screenNormalizedToNdc(float left, float top);
	Vec2 ndcToScreenNormalized(const Vec2&);
	Vec2 ndcToScreenNormalized(float x, float y);
	void calculateTangentBinormal(const Vec3& edge0, const Vec3& edge1, const Vec3& edge2, const Vec2& texCoord0, const Vec2& texCoord1, const Vec2& texCoord2, Vec3& outTangent, Vec3& outBinormal);
	Aabb3 calculateAabb3(const Vec3* points, uint pointCount);
	Aabb3 calculateAabb3(const Vec3& point0, const Vec3& point1);
	bool isZero(const Vec3&);
	bool isZero(const Vec4&);
	bool isNan(const Vec3&);
	float projectPointOnLine(const Vec3& point, const Vec3& line0, const Vec3& line1);

	/************************************************************************************
		MATRIX
	*************************************************************************************/

	template <class MATRIX>
	void matrixIndexToRowColumn(uint index, uint& outRow, uint &outColumn);
	Mtx44 matrixPerspective(float aspectRatio, float fovDegs, float _near, float _far);
	Mtx44 matrixOrtho(float width, float height, float _near, float _far);
	Mtx44 matrixOrtho(float aspectRatio);
	Mtx44 matrixOrtho(const Vec2&);
	Mtx44 matrixTranslate(float x, float y, float z);
	Mtx44 matrixTranslate(const Vec3&);
	Mtx33 matrixTranslate(const Vec2&);
	Mtx33 matrixTranslate(float x, float y);
	Mtx33 matrixScale(float x, float y);
	Mtx33 matrixScale(const Vec2&);
	Mtx44 matrixScale(float x, float y, float z);
	Mtx44 matrixScale(const Vec3&);
	Mtx33 matrixTexTransform(const math::Vec2& translate, const math::Vec2& scale, float rotateRads, const math::Vec2& originTranslate = Vec2(-0.5f));
	Mtx33 matrixTexTransformDegs(const math::Vec2& translate, const math::Vec2& scale, float rotateDegs, const math::Vec2& originTranslate = Vec2(-0.5f));
	Mtx33 matrixTranspose(const Mtx33&);
	void matrixTranspose(const Mtx44&, Mtx44& outResult);
	Mtx44 matrixTranspose(const Mtx44&);
	Mtx44 matrixPitch(float rads);
	Mtx44 matrixPitchDegs(float degs);
	Mtx44 matrixYaw(float rads);
	Mtx44 matrixYawDegs(float degs);
	Mtx44 matrixRoll(float rads);
	Mtx44 matrixRollDegs(float degs);
	Mtx44 matrixYawPitchRoll(const Euler&);
	Mtx44 matrixYawPitchRoll(float yaw, float pitch, float roll);
	Mtx44 matrixYawPitchRollDegs(float yaw, float pitch, float roll);
	Mtx44 matrixRotateAxis(float x, float y, float z, float angle);
	Mtx44 matrixRotateAxis(const Vec3& axis, float angle);
	Mtx44 matrixRotateDegs(float x, float y, float z, float angle);
	Mtx44 matrixRotateDegs(const Vec3& axis, float angle);
	Mtx44 matrixRotateEuler(const Euler&);
	Mtx33 matrixRotate2d(float rads);
	Mtx33 matrixRotate2dDegs(float);
	Mtx44 matrixOrthogonalUp(const Vec3& look);
	Mtx44 matrixOrthogonalLook(const Vec3& look);
	Mtx44 matrixOrthogonalLookUp(const Vec3& look, const Vec3& up);
	float matrixDeterminant(const Mtx22&);
	float matrixDeterminant(const Mtx33&);
	float matrixDeterminant(const Mtx44&);
	Mtx33 matrixCofactor(const Mtx33&);
	void matrixCofactor(const Mtx44&, Mtx44& outResult);
	Mtx44 matrixCofactor(const Mtx44&);
	Mtx33 matrixInvert(const Mtx33&);
	void matrixInvert(const Mtx44&, Mtx44& outResult);
	Mtx44 matrixInvert(const Mtx44&);
	Mtx44 matrixNormalTransform(const Mtx44& world);
	Mtx44 matrixView(const Vec3& pos, const Vec3& right, const Vec3& up, const Vec3& look);
	Mtx44 matrixViewLook(const Vec3& pos, const Vec3& look);
	Mtx44 matrixViewLookUp(const Vec3& pos, const Vec3& look, const Vec3& up);
	Frustum extractViewFrustum(const Mtx44&);

	/************************************************************************************
		INTERSECTION
	*************************************************************************************/

	Vec3 pointClosestAxis(const Vec3&, const math::Vec3& boxScale);
	bool pointOnQuad(const Vec2& point, const Vec2& squareMin, const Vec2& squareMax);
	bool pointOnQuad(const Vec2& point, const Aabb2&);
	bool pointInsideAabb(const math::Vec3& point, const Aabb3&);
	IntersectRaySphereInfo intersectRaySphere(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& sphereOrigin, float sphereRadius);
	IntersectRayPlaneInfo intersectRayPlane(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& planePoint, const Vec3& planeNormal);
	IntersectRayCircleInfo intersectRayCircle(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& circlePoint, const Vec3& circleNormal, float radius);
	IntersectRayAabbInfo intersectRayAabb(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& boxMin, const Vec3& boxMax);
	IntersectRayAabbInfo intersectRayAabb(const Vec3& rayOrigin, const Vec3& rayDirection, const Aabb3&);
	IntersectRayTriangleInfo intersectRayTriangle(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& edge0, const Vec3& edge1, const Vec3& edge2);
	IntersectRayQuadInfo intersectRayQuad(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& quadOrigin, const Vec3& quadEdgeRight, const Vec3& quadEdgeUp);
	IntersectRayCylinderInfo intersectRayCylinder(const Vec3& rayOrigin, const Vec3& rayDirection, const Vec3& cylinderStart, const Vec3& cylinderEnd, float radius);
	bool intersectAabb3Frustum(const Aabb3&, const Frustum&);
	IntersectAabb3Info intersectAabb3(const Aabb3& aabb0, const Aabb3& aabb1);
	IntersectAabb3Info intersectAabb3(const Vec3& pos0, const Vec3& scale0, const Vec3& pos1, const Vec3& scale1);
	IntersectAabb3DynamicInfo intersectAabb3Dynamic(const Vec3& pos0, const Vec3& scale0, const Vec3& aabb0RelativeVelocity, const Vec3& pos1, const Vec3& scale1);
	IntersectSphereAabbInfo intersectSphereAabb(const math::Vec3& posSphere, float radius, const math::Vec3& posAabb, const math::Vec3& scaleAabb);
	IntersectSphereAabbInfo intersectSphereAabbDynamic(const math::Vec3& posSphere, float radius, const math::Vec3& sphereRelativeVelocity, const math::Vec3& posAabb, const math::Vec3& scaleAabb);
	ePlane classifyPointPlane(const Vec3& point, const Vec3& planePoint, const Vec3& planeNormal);
	ePlane classifyPointPlane(const Vec3& point, const Plane&);

	/************************************************************************************
		TEMPLATE FUNCTIONS
	*************************************************************************************/

	template <class TYPE>
	bool between(TYPE value, TYPE minVal, TYPE maxVal, bool isInclusive = true);
	template <class TYPE>
	void swap(TYPE&, TYPE&);
	template <class TYPE>
	TYPE min(TYPE, TYPE);
	template <class TYPE>
	TYPE max(TYPE, TYPE);
	template <class TYPE>
	TYPE minGreater(TYPE val0, TYPE val1, TYPE valMin);
	template <class TYPE>
	eGrowth growth(TYPE);
	template <class TYPE>
	eSign signEnum(TYPE);
	template <class TYPE>
	TYPE lerp(TYPE value1, TYPE value2, float percentage);
	template <class TYPE>
	TYPE smoothSquare(TYPE value1, TYPE value2, float percentage);
	template <class TYPE>
	TYPE smoothCube(TYPE value1, TYPE value2, float percentage);
	template <class TYPE>
	TYPE smoothQuart(TYPE value1, TYPE value2, float percentage);
	template <class TYPE>
	TYPE smoothHermite(TYPE value1, TYPE value2, float percentage);
	template <class TYPE>
	TYPE smoothHermite2(TYPE value1, TYPE value2, float percentage);
	// NOTE: is there a way of making the smoothstep into one template without iteration? smoothstep<N>(v1, v2, p)
	template <class TYPE>
	TYPE smoothstep2(TYPE val1, TYPE val2, float percentage);
	template <class TYPE>
	TYPE smoothstep3(TYPE val1, TYPE val2, float percentage);
	template <class TYPE>
	TYPE smoothstep4(TYPE val1, TYPE val2, float percentage);
	template <class TYPE>
	TYPE smoothstep5(TYPE val1, TYPE val2, float percentage);
	template <class TYPE>
	TYPE randomArray(const TYPE* array, uint count);
	template <class TYPE>
	TYPE saturate(const TYPE&, float saturation);
	template <class TYPE>
	float average(const TYPE&);
	
	//============================================================================
	//	Template operator
	//============================================================================

	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator+(const VEC_TYPE&, const Aabb<VEC_TYPE>&);
	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator+(const Aabb<VEC_TYPE>&, const VEC_TYPE&);
	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator-(const VEC_TYPE&, const Aabb<VEC_TYPE>&);
	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator-(const Aabb<VEC_TYPE>&, const VEC_TYPE&);

	/************************************************************************************
		TEMPLATE FUNCTIONS DEFINITION
	*************************************************************************************/
	
	//------------------------------------------------------------------
	//	matrixIndexToRowColumn
	//------------------------------------------------------------------
	template<class MATRIX>
	inline void matrixIndexToRowColumn(uint index, uint& outRow, uint& outColumn)
	{
		outRow = index / MATRIX::s_width;
		outColumn = index % MATRIX::s_width;
	}
	//------------------------------------------------------------------
	//	between
	//------------------------------------------------------------------
	template<class TYPE>
	inline bool between(TYPE value, TYPE minVal, TYPE maxVal, bool isInclusive)
	{
		if ( isInclusive )
			return value >= minVal && value <= maxVal;
		return value > minVal && value < maxVal;
	}
	//------------------------------------------------------------------
	//	swap
	//------------------------------------------------------------------
	template <class TYPE>
	inline void swap(TYPE& t0, TYPE& t1)
	{
		TYPE temp;
		temp = t0;
		t0 = t1;
		t1 = temp;
	}
	//------------------------------------------------------------------
	//	min
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE min(TYPE a, TYPE b)
	{
		return a < b ? a : b;
	}
	//------------------------------------------------------------------
	//	max
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE max(TYPE a, TYPE b)
	{
		return a > b ? a : b;
	}
	//------------------------------------------------------------------
	//	minGreater
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE minGreater(TYPE val0, TYPE val1, TYPE valMin)
	{
		if ( val0 > valMin && val1 > valMin )
			return min( val0, val1 );
		if ( val0 > valMin && val1 < valMin )
			return val0;
		if ( val0 < valMin && val1 > valMin )
			return val1;
			
		return valMin;
	}
	//------------------------------------------------------------------
	//	growth
	//------------------------------------------------------------------
	template <class TYPE>
	inline eGrowth growth(TYPE t)
	{
		return t > (TYPE)0 ? eGrowth::INCREASE : eGrowth::DECREASE;
	}
	//------------------------------------------------------------------
	//	signEnum
	//------------------------------------------------------------------
	template <class TYPE>
	inline eSign signEnum(TYPE t)
	{
		return t > (TYPE)0 ? math::eSign::POSITIVE : math::eSign::NEGATIVE;
	}
	//------------------------------------------------------------------
	//	lerp
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE lerp(TYPE val1, TYPE val2, float percentage)
	{
		return val1 + (val2 - val1) * percentage;
	}
	//------------------------------------------------------------------
	//	smooth
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothSquare(TYPE val1, TYPE val2, float percentage)
	{
		return lerp( val1, val2, percentage * percentage );
	}
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothCube(TYPE val1, TYPE val2, float percentage)
	{
		return lerp( val1, val2, percentage * percentage * percentage );
	}
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothQuart(TYPE val1, TYPE val2, float percentage)
	{
		return lerp( val1, val2, percentage * percentage * percentage * percentage );
	}
	//------------------------------------------------------------------
	//	smoothHermite
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothHermite(TYPE val1, TYPE val2, float percentage)
	{
		// https://en.wikipedia.org/wiki/Smoothstep
		float x = percentage;
		return lerp( val1, val2, 3.0f * x*x - 2.0f * x*x*x );
	}
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothHermite2(TYPE val1, TYPE val2, float percentage)
	{
		float x = percentage;
		return lerp( val1, val2, 6.0f * x*x*x*x*x - 15.0f * x*x*x*x + 10.0f * x*x*x );
	}
	//------------------------------------------------------------------
	//	smoothstep
	//	https://www.youtube.com/watch?v=mr5xkf6zSzk
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothstep2(TYPE val1, TYPE val2, float percentage)
	{
		float x = percentage;
		return lerp( val1, val2, 1.0f - (1.0f-x)*(1.0f-x) );
	}
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothstep3(TYPE val1, TYPE val2, float percentage)
	{
		float x = percentage;
		return lerp( val1, val2, 1.0f - (1.0f-x)*(1.0f-x)*(1.0f-x) );
	}
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothstep4(TYPE val1, TYPE val2, float percentage)
	{
		float x = percentage;
		return lerp( val1, val2, 1.0f - (1.0f-x)*(1.0f-x)*(1.0f-x)*(1.0f-x) );
	}
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE smoothstep5(TYPE val1, TYPE val2, float percentage)
	{
		float x = percentage;
		return lerp( val1, val2, 1.0f - (1.0f-x)*(1.0f-x)*(1.0f-x)*(1.0f-x)*(1.0f-x) );
	}
	//------------------------------------------------------------------
	//	randomArray
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE randomArray(const TYPE* array, uint count)
	{
		// NOTE: this is done because I don't trust the random number generator to make a 1.0f (verify)
		uint index = (uint)((float)count * 2.0f * random());
		return array[index % count];
	}
	//------------------------------------------------------------------
	//	saturate
	//------------------------------------------------------------------
	template <class TYPE>
	inline TYPE saturate(const TYPE& t, float saturation)
	{
		/*
			c1 = c0 + (m - c0) * s
			WHERE:
				c0 = initial color component
				c1 = final color component (result)
				m = average of all components
				s = saturation
		*/

		TYPE result;
		float m = math::average( t );
		float s = saturation;

		result = t + (TYPE(m) - t) * s;

		return result;
	}
	//------------------------------------------------------------------
	//	average
	//------------------------------------------------------------------
	template <class TYPE>
	inline float average(const TYPE& t)
	{
		float result = 0.0f;

		for ( uint i = 0; i < TYPE::GetComponentCount(); i++ )
			result += t[i];

		return result / TYPE::GetComponentCount();
	}

	//============================================================================
	//	Template operator definition
	//============================================================================

	//------------------------------------------------------------------
	//	 operator +
	//------------------------------------------------------------------
	template <class VEC_TYPE>
	inline Aabb<VEC_TYPE> operator+(const VEC_TYPE& v, const Aabb<VEC_TYPE>& aabb)
	{
		Aabb<VEC_TYPE> result(aabb);
		result.min += v;
		result.max += v;
		return result;
	}
	//------------------------------------------------------------------
	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator+(const Aabb<VEC_TYPE>& aabb, const VEC_TYPE& v)
	{
		return v + aabb;
	}
	//------------------------------------------------------------------
	//	 operator -
	//------------------------------------------------------------------
	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator-(const VEC_TYPE& v, const Aabb<VEC_TYPE>& aabb)
	{
		return (-v) + aabb;
	}
	//------------------------------------------------------------------
	template <class VEC_TYPE>
	Aabb<VEC_TYPE> operator-(const Aabb<VEC_TYPE>& aabb, const VEC_TYPE& v)
	{
		return v - aabb;
	}
}
