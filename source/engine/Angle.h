#pragma once

namespace math
{
	enum class eAngle
	{
		Degree,
		Radian,
	};

	struct Euler
	{
		float yaw;
		float pitch;
		float roll;

		eAngle type;

		Euler();
		Euler(float yaw, float pitch, float roll, eAngle type = eAngle::Degree);
		Euler(float yawPitchRoll, eAngle type = eAngle::Degree);

		Euler As(eAngle)const;
		Euler AsRads()const;
		Euler AsDegs()const;

		// Probably a terrible name for a very simple function: make angle outside [0, 360] fall into range
		void MapToOneRotation();
	};
}
