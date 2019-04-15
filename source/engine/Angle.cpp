#include "Angle.h"
#include "../engine/Math.h"

/****************************************************************************************
	Helper function
*****************************************************************************************/

void mapToOneRotation(float& inOutValue, float maxAngle)
{
	inOutValue = math::remainder( inOutValue, maxAngle );
	if ( inOutValue < 0.0f ) inOutValue += maxAngle;
}

/****************************************************************************************
	Euler struct definition
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
math::Euler::Euler() : yaw(0.0f), pitch(0.0f), roll(0.0f), type(eAngle::Degree)
{
}
//-------------------------------------------------------------------
math::Euler::Euler(float yaw, float pitch, float roll, eAngle type) : yaw(yaw), pitch(pitch), roll(roll), type(type)
{
}
//-------------------------------------------------------------------
math::Euler::Euler(float yawPitchRoll, eAngle type) : Euler(yawPitchRoll, yawPitchRoll, yawPitchRoll, type)
{
}
//-------------------------------------------------------------------
//	AsRads
//-------------------------------------------------------------------
math::Euler math::Euler::As(eAngle type)const
{
	return type == eAngle::Degree ? AsDegs() : AsRads();
}
//-------------------------------------------------------------------
//	AsRads
//-------------------------------------------------------------------
math::Euler math::Euler::AsRads()const
{
	if ( type == eAngle::Degree )
		return Euler( toRads(yaw), toRads(pitch), toRads(roll), eAngle::Radian );
	return *this;
}
//-------------------------------------------------------------------
//	AsDegs
//-------------------------------------------------------------------
math::Euler math::Euler::AsDegs()const
{
	if ( type == eAngle::Radian )
		return Euler( toDegs(yaw), toDegs(pitch), toDegs(roll), eAngle::Degree );
	return *this;
}
//-------------------------------------------------------------------
//	MapToOneRotation
//-------------------------------------------------------------------
void math::Euler::MapToOneRotation()
{
	if ( type == eAngle::Degree )
	{
		mapToOneRotation( yaw, 360.0f );
		mapToOneRotation( pitch, 360.0f );
		mapToOneRotation( roll, 360.0f );
	}
	else
	{
		mapToOneRotation( yaw, TWO_PI );
		mapToOneRotation( pitch, TWO_PI );
		mapToOneRotation( roll, TWO_PI );
	}
}
