#pragma once

#include "Type.h"

// https://en.wikipedia.org/wiki/Linear_congruential_generator

class Random
{
	public:
		Random(uint seed);

		void SetSeed(uint);

		float Get();
		float GetRange(float, float);
		float GetSign();

		template <class TYPE>
		TYPE GetArray(const TYPE*, uint count);

	private:
		uint m_seed;
		uint m_a;
		uint m_c;
		uint m_m;
};

template <class TYPE>
inline TYPE Random::GetArray(const TYPE* array, uint count)
{
	uint index = (uint)( count * GetRange(0.0f, 2.0f) );
	return array[index % count];
}


