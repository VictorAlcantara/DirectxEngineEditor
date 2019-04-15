#include "Timer.h"
#include "Platform.h"
#include "../engine/Exception.h"

#ifdef WIN32

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Timer::Timer()
{
	int64 frequency;

	int result = QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency );

	if ( !result )
	{
		THROW( "Timer::Timer() unable to get cpu frequency" );
	}

	m_period = 1.0 / (double)frequency;
	m_tickCreate = GetCurrentCount();

	Reset();
}
//-------------------------------------------------------------------
//	Reset
//-------------------------------------------------------------------
void Timer::Reset()
{
	m_tickLastCount = GetCurrentCount();
}
//-------------------------------------------------------------------
//	GetTimeSinceCreation
//-------------------------------------------------------------------
double Timer::GetTimeSinceCreation()const
{
	return (double)(GetCurrentCount() - m_tickCreate) * m_period;
}
//-------------------------------------------------------------------
//	GetTimeSinceReset
//-------------------------------------------------------------------
double Timer::GetTimeSinceReset()const
{
	int64 tickSinceLastReset = GetCurrentCount() - m_tickLastCount;
	double timeSinceLastReset = (double)tickSinceLastReset * m_period;

	return timeSinceLastReset;
}
//-------------------------------------------------------------------
//	GetCurrentCount
//-------------------------------------------------------------------
int64 Timer::GetCurrentCount()const
{
	int64 currentCount;

	int result = QueryPerformanceCounter( (LARGE_INTEGER*)&currentCount );

	if ( !result )
	{
		THROW( "Timer::Timer() unable to get cpu tick count" );
	}

	return currentCount;
}

#endif