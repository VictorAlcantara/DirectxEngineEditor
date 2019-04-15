#include "../engine/Gradient.h"
#include "../engine/CoreInterface.h"

/****************************************************************************************
	GradientTransition_Smooth
*****************************************************************************************/

//-------------------------------------------------------------------
//	Calculate
//-------------------------------------------------------------------
float GradientTransition_Smooth::Calculate(float progress)const
{
	return math::smoothHermite( 0.0f, 1.0f, progress );
}

/****************************************************************************************
	GradientTransition_Linear
*****************************************************************************************/

//-------------------------------------------------------------------
//	Calculate
//-------------------------------------------------------------------
float GradientTransition_Linear::Calculate(float progress)const
{
	return progress;
}

/****************************************************************************************
	GradientPeriod_Repeat
*****************************************************************************************/

//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
float GradientPeriod_Repeat::Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod)
{
	return Update( timeProgress, timeDuration, hasCompletedPeriod, g_pCore->GetTimeSinceLastFrame() );
}
//-------------------------------------------------------------------
float GradientPeriod_Repeat::Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod, float deltaTime)
{
	float progress = timeProgress / timeDuration;
	timeProgress += deltaTime;

	if ( progress > 1.0f )
	{
		timeProgress = 0.0f;
		progress = 1.0f;
		hasCompletedPeriod = true;
	}
	else
		hasCompletedPeriod = false;

	return progress;
}
//-------------------------------------------------------------------
//	Break
//-------------------------------------------------------------------
float GradientPeriod_Repeat::Break(float& timeProgress, float timeDuration, bool& hasCompletedPeriod)
{
	timeProgress = timeDuration - timeProgress;

	return Update( timeProgress, timeDuration, hasCompletedPeriod, 0.0f );
}

/****************************************************************************************
	GradientPeriod_Invert
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GradientPeriod_Invert::GradientPeriod_Invert()
{
	m_invert = false;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
float GradientPeriod_Invert::Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod)
{
	return Update( timeProgress, timeDuration, hasCompletedPeriod, g_pCore->GetTimeSinceLastFrame() );
}
//-------------------------------------------------------------------
float GradientPeriod_Invert::Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod, float deltaTime)
{
	float progress = timeProgress / timeDuration;
	bool invert = m_invert;
	timeProgress += deltaTime;

	if ( progress > 1.0f )
	{
		timeProgress = 0.0f;
		progress = 1.0f;
		m_invert = !m_invert;
		hasCompletedPeriod = true;
	}
	else
		hasCompletedPeriod = false;

	if ( invert )
		return 1.0f - progress;

	return progress;
}
//-------------------------------------------------------------------
//	Break
//-------------------------------------------------------------------
float GradientPeriod_Invert::Break(float& timeProgress, float timeDuration, bool& hasCompletedPeriod)
{
	m_invert = !m_invert;
	timeProgress = timeDuration - timeProgress;

	return Update( timeProgress, timeDuration, hasCompletedPeriod, 0.0f );
}
