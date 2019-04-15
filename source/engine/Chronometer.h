#pragma once

class Chronometer
{
	public:
		Chronometer();

		void Reset();
		void Update(float deltaTime);
		float GetTime()const;

	private:
		float m_time;
};

