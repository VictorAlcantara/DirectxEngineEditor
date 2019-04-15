#include "../engine/Debug.h"
#include "Platform.h"
#include "../engine/String.h"

#ifdef WINDOWS

void debugToOutput(const String& s)
{
	String result = s + String("\n");
	OutputDebugStringA( result.AsChar() );
}

void debugToOutput(const char* msg)
{
	debugToOutput( String(msg) );
}

void debugToOutput(float value, const char* msg)
{
	debugToOutput( String(msg) + string::floatToStr(value) );
}

void debugToOutput(bool value, const char* msg)
{
	debugToOutput( String(msg) + string::boolToStr(value) );
}

void debugToOutput(int value, const char* msg)
{
	debugToOutput( String(msg) + string::intToStr(value) );
}


#endif