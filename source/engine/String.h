#pragma once

#include "Type.h"
#include "Vector.h"
#include "Array.h"
#include "StringClass.h"
#include "../engine/List.h"
#include "Angle.h"

namespace string
{
	String quote(const String&);
	String copy(const String&, uint startAt, uint count); // count == 0 means all the string from startAt to end is copied
	String completeTrunc(const String&, char charToComplete, uint count);

	bool equal(const String&, const String&, bool caseSensitive);
	uint length(const char*);

	String remove(const String&, const String& charsToRemove);
	String substituteExclusive(const String&, const String& charsToNotSubstitute, char substituteFor);
	void split(const String&, char lookFor, Array<String>& outResult);
	String join(const List<String>&, const String& joinStr);
	
	uint findFirst(const String&, char, uint startAt = 0); // Returns UINT_INVALID when char is not found
	uint findFirstNot(const String&, char, uint startAt = 0); // Returns UINT_INVALID when char is not found
	uint findLast(const String&, char); // Returns UINT_INVALID when char is not found
	String trimRight(const String&, char lookFor = ' ');
	String trimLeft(const String&, char lookFor = ' ');
	String trim(const String&, char lookFor = ' ');

	char intToChar(int);
	String intToStr(int);
	String uintToStr(uint);
	String floatToStr(float);
	String vec2ToStr(const math::Vec2&);
	String vec3ToStr(const math::Vec3&);
	String vec4ToStr(const math::Vec4&);
	String boolToStr(bool);
	String eulerToStr(const math::Euler&);

	String upperCase(const String&);
	char upperCase(char);
	String lowerCase(const String&);
	char lowerCase(char);

	bool contains(const String&, char lookFor);
	bool contains(const String& str, const String& subStr);
	bool containsNumber(char);
	uint charCount(const String& str, const String& charsToLookFor);
	// TODO: use only the one with part limitation
	bool isNumber(char);
	bool isNumber(const String&);
	bool isNumber(const String&, int integerPartMax, int decimalPartMax);
	bool isAlpha(char c);

	int charToInt(char);
	int strToInt(const String&);
	float strToFloat(const String&);
	void strToFloatArray(const String&, float** pOutput, uint outputCount);
	void strToIntArray(const String&, int** ppOutput, uint outputCount);
	math::Vec2 strToVec2(const String&);
	math::Vec3 strToVec3(const String&);

	String addExtension(const String& file, const String& extension);
	StringW addExtension(const StringW& file, const StringW& extension);

	String extractFileName(const String&, bool returnWithExtension);
	String extractFileExtension(const String&, bool returnWithExtension);

	String upperFirst(const String&);
}


