#pragma once

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef wchar_t wchar;
typedef __int64 int64;
typedef uint bool4;

const uint UINT_INVALID = (uint)~0;
const bool4 bool4True = 1;
const bool4 bool4False = 0;

inline uint safeAddUint(uint value0, uint value1)
{
	uint result = value0 + value1;

	if ( result < value0 || result < value1 )
		result = UINT_INVALID;

	return result;
}
