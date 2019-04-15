#pragma once

#include "../engine/Math.h"

template <class, class, class>
class Gradient;

/****************************************************************************************
	GradientTransition_Smooth
*****************************************************************************************/

// TODO: change name to reflect smooth function being used
class GradientTransition_Smooth
{
	template<class, class, class>
	friend class Gradient;

	private:
		float Calculate(float progress)const;
};

/****************************************************************************************
	GradientTransition_Linear
*****************************************************************************************/

class GradientTransition_Linear
{
	template<class, class, class>
	friend class Gradient;

	private:
	float Calculate(float progress)const;
};

/****************************************************************************************
	GradientPeriod_Repeat
*****************************************************************************************/

class GradientPeriod_Repeat
{
	template<class, class, class>
	friend class Gradient;

	private:
		float Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod);
		float Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod, float deltaTime);
		float Break(float& timeProgress, float timeDuration, bool& hasCompletedPeriod);
};


/****************************************************************************************
	GradientPeriod_Invert
*****************************************************************************************/

class GradientPeriod_Invert
{
	template<class, class, class>
	friend class Gradient;

	private:
		GradientPeriod_Invert();
		~GradientPeriod_Invert() = default;

		float Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod);
		float Update(float& timeProgress, float timeDuration, bool& hasCompletedPeriod, float deltaTime);
		float Break(float& timeProgress, float timeDuration, bool& hasCompletedPeriod);

		bool m_invert;
};

/****************************************************************************************
	Gradient
*****************************************************************************************/

template <class TYPE, class TRANSITION, class PERIOD>
class Gradient
{
	public:
		Gradient();

		void SetDuration(float t) { m_timeDuration = t; }
		void SetInterval(const TYPE& start, const TYPE& end) { SetIntervalStart(start); SetIntervalEnd(end); }
		void SetIntervalStart(const TYPE& t) { m_intervalStart = t; }
		void SetIntervalEnd(const TYPE& t) { m_intervalEnd = t; }

		void Reset() { m_progress = 0.0f; }
		void Update();
		void Break();
		bool HasCompletedCicleDuringLastUpdate()const;

		TYPE Get()const;
		float GetProgress()const { return m_progress; }


	private:
		float m_timeProgress;
		float m_timeDuration;
		float m_progress;
		bool m_hasCompletedPeriod;

		TYPE m_intervalStart;
		TYPE m_intervalEnd;

		TRANSITION m_transition;
		PERIOD m_period;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template<class TYPE, class TRANSITION, class PERIOD>
inline Gradient<TYPE, TRANSITION, PERIOD>::Gradient()
{
	SetInterval( TYPE(0), TYPE(0) );
	m_timeProgress = 0.0f;
	m_timeDuration = 1.0f;
	m_progress = 0.0f;
	m_hasCompletedPeriod = false;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
template<class TYPE, class TRANSITION, class PERIOD>
inline void Gradient<TYPE, TRANSITION, PERIOD>::Update()
{
	m_progress = m_period.Update( m_timeProgress, m_timeDuration, m_hasCompletedPeriod );
}
//-------------------------------------------------------------------
//	Break
//-------------------------------------------------------------------
template<class TYPE, class TRANSITION, class PERIOD>
inline void Gradient<TYPE, TRANSITION, PERIOD>::Break()
{
	m_progress = m_period.Break( m_timeProgress, m_timeDuration, m_hasCompletedPeriod );
}
//-------------------------------------------------------------------
//	HasCompletedCicleDuringLastUpdate
//-------------------------------------------------------------------
template <class TYPE, class TRANSITION, class PERIOD>
inline bool Gradient<TYPE, TRANSITION, PERIOD>::HasCompletedCicleDuringLastUpdate()const
{
	return m_hasCompletedPeriod;
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template<class TYPE, class TRANSITION, class PERIOD>
inline TYPE Gradient<TYPE, TRANSITION, PERIOD>::Get()const
{
	return  math::lerp( m_intervalStart, m_intervalEnd, m_transition.Calculate( m_progress ) );
}
