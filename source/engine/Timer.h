#pragma once

#include "Type.h"
#include "../engine/Chronometer.h"

class Timer
{
	public:
		Timer();
		~Timer() {}

		void Reset();
		double GetTimeSinceCreation()const;
		double GetTimeSinceReset()const;

	private:
		int64 GetCurrentCount()const;

		double m_period;
		int64 m_tickCreate;
		int64 m_tickLastCount;
};
