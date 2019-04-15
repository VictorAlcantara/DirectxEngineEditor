#include "../engine/String.h"
#include <comdef.h>
#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/Debug.h"
#include "Array.h"

static const String number = "0123456789";
static const String plusMinusDot = "+-.";
static const String numberSymbols = "0123456789";
static const String numericChars = "0123456789.-";

//-------------------------------------------------------------------
//	ExtractNumberStringArray
//-------------------------------------------------------------------
Array<String> ExtractNumberStringArray(const String& s, uint enforceArrayCount = 0)
{
	String strFormatted = string::substituteExclusive( s, numericChars, ' ' );
	Array<String> strNumbers;
	string::split( strFormatted, ' ', strNumbers );

	if ( enforceArrayCount != 0 && strNumbers.Count() != enforceArrayCount )
		THROW("string::strToFloatArray() string does not have the same number of elements as output array");

	for ( uint i = 0; i < strNumbers.Count(); i++ )
	{
		if ( !string::isNumber(strNumbers[i]) )
			THROW( "string::strToFloatArray() string elements is not a number" );
	}

	return strNumbers;
}
//-------------------------------------------------------------------
//	validateStringNumber
//-------------------------------------------------------------------
bool validateStringNumber(const String& str, uint maximumNumericCharsAllowed)
{
	if ( !string::isNumber(str) )
	{
		THROW(String("strToInt() string \'" + str + "\' is not a number :(").AsChar());
		return false;
	}

	uint dotIndex = string::findFirst( str, '.' );
	String integerPart = string::copy( str, 0, dotIndex );
	String decimalPart = string::copy( str, safeAddUint(dotIndex, 1), 0 );

	if ( integerPart.Length() > maximumNumericCharsAllowed || decimalPart.Length() > maximumNumericCharsAllowed )
	{
		THROW( String("strToInt(): string \'" +  String("\' has exceeded maximum of ") + string::intToStr(maximumNumericCharsAllowed) + String(" characters")).AsChar() );
		return false;
	}

	return true;
}

//=============================================================================
//	Helper function declared in header
//=============================================================================

//-------------------------------------------------------------------
//	operator+
//-------------------------------------------------------------------
String operator+(const String& s1, const String& s2)
{
	String result( s1 );
	result += s2;
	return result;
}
//-------------------------------------------------------------------
StringW operator+(const StringW& s1, const StringW& s2)
{
	StringW result( s1 );
	result += s2;
	return result;
}
//-------------------------------------------------------------------
//	operator==
//-------------------------------------------------------------------
bool operator==(uint hash, const HashString& hs)
{
	return hash == hs.GetHash();
}
//-------------------------------------------------------------------
bool operator==(const HashString& hs, uint hash)
{
	return hash == hs.GetHash();
}
//-------------------------------------------------------------------
bool operator==(const HashString& hs1, const HashString& hs2)
{
	return hs1.GetHash() == hs2.GetHash();
}
//-------------------------------------------------------------------
//	operator!=
//-------------------------------------------------------------------
bool operator!=(uint hash, const HashString& hs)
{
	return hash != hs.GetHash();
}
//-------------------------------------------------------------------
bool operator!=(const HashString& hs, uint hash)
{
	return hash != hs.GetHash();
}
//-------------------------------------------------------------------
bool operator!=(const HashString& hs1, const HashString& hs2)
{
	return hs1.GetHash() != hs2.GetHash();
}
//-------------------------------------------------------------------
//	quote
//-------------------------------------------------------------------
String string::quote(const String& s)
{
	return "\"" + s + "\"";
}
//-------------------------------------------------------------------
//	copy
//-------------------------------------------------------------------
String string::copy(const String& str, uint startAt, uint count)
{
	return String( str, startAt, count );
}
//-------------------------------------------------------------------
//	completeTrunc
//-------------------------------------------------------------------
String string::completeTrunc(const String& s, char completeWith, uint count)
{
	if ( s.Length() == count )
		return s;

	if ( count == 0 )
		return "";

	char* pValue = new char[count];

	for ( uint i = 0; i < count; i++ )
	{
		if ( i < s.Length() )
			pValue[i] = s[i];
		else
			pValue[i] = completeWith;
	}

	String result( pValue, count );

	safeArrayRelease( pValue );

	return result;
}
//-------------------------------------------------------------------
//	equal
//-------------------------------------------------------------------
bool string::equal(const String& value1, const String& value2, bool caseSensitive)
{
	if ( value1.Length() != value2.Length() )
		return false;

	String v1( caseSensitive ? value1 : upperCase(value1) );
	String v2( caseSensitive ? value2 : upperCase(value2) );

	return v1 == v2;
}
//-------------------------------------------------------------------
//	length
//-------------------------------------------------------------------
uint string::length(const char* c)
{
	uint tolerance = 5000;
	uint result = 0;

	if ( !c )
		return result;

	while ( c[result] != '\0' )
	{
		result++;

		if ( result > tolerance )
		{
			THROW( "string::length() tolerance reached. Probably input string is invalid." );
		}
	}

	return result;
}
//-------------------------------------------------------------------
//	remove
//-------------------------------------------------------------------
String string::remove(const String& str, const String& charsToLookFor)
{
	String result;
	
	if ( str.Length() > 0 )
	{
		char* newString = new char[str.Length()];
		uint newStringLength = 0;

		for ( uint i = 0; i < str.Length(); i++ )
		{
			if ( contains( charsToLookFor, str[i] ) )
				continue;

			newString[newStringLength] = str[i];
			newStringLength++;
		}

		if ( newStringLength )
			result = String( newString, newStringLength );

		safeArrayRelease( newString );
	}

	return result;
}
//-------------------------------------------------------------------
//	substituteExclusive
//-------------------------------------------------------------------
String string::substituteExclusive(const String& str, const String& charsToNotSubstitute, char substituteFor)
{
	if ( str.Length() == 0 )
		return String();

	char* newStr = new char[str.Length()];

	for ( uint i = 0; i < str.Length(); i++ )
	{
		if ( contains(charsToNotSubstitute, str[i]) )
			newStr[i] = str[i];
		else
			newStr[i] = substituteFor;
	}

	String result = String(newStr, str.Length());
	safeArrayRelease( newStr );

	return result;
}
//-------------------------------------------------------------------
//	split
//-------------------------------------------------------------------
void string::split(const String& str, char splitAt, Array<String>& outResult)
{
	uint strLength = str.Length();

	if ( strLength == 0 )
		return;

	int64 idxStart = 0;
	int64 idxEnd = -1;

	while ( idxStart < strLength && idxEnd < strLength )
	{
		idxStart = findFirstNot( str, splitAt, (uint)idxEnd + 1 );
		idxEnd = findFirst( str, splitAt, (uint)idxStart );

		if ( idxStart < strLength )
		{
			uint idxEndWord = (uint)idxEnd != UINT_INVALID ? (uint)idxEnd : strLength;
			outResult.Add( copy( str, (uint)idxStart, idxEndWord - (uint)idxStart ) );
		}
	}
}
//-------------------------------------------------------------------
//	join
//-------------------------------------------------------------------
String string::join(const List<String>& strList, const String& joinStr)
{
	String result;

	for ( auto pNode = strList.GetFirst(); pNode; pNode = pNode->GetNext() )
		result += pNode->GetValue() + (pNode->GetNext() ? joinStr : "");

	return result;
}
//-------------------------------------------------------------------
//	findFirst
//-------------------------------------------------------------------
uint string::findFirst(const String& str, char c, uint startAt)
{
	for ( uint i = startAt; i < str.Length(); i++ )
	{
		if ( str[i] == c )
			return i;
	}

	return UINT_INVALID;
}
//-------------------------------------------------------------------
//	findFirstNot
//-------------------------------------------------------------------
uint string::findFirstNot(const String& str, char c, uint startAt)
{
	for ( uint i = startAt; i < str.Length(); i++ )
	{
		if ( str[i] != c )
			return i;
	}

	return UINT_INVALID;
}
//-------------------------------------------------------------------
//	findLast
//-------------------------------------------------------------------
uint string::findLast(const String& str, char c)
{
	for ( int i = (int)str.Length() - 1; i >= 0; i-- )
	{
		if ( str[i] == c )
			return (uint)i;
	}

	return UINT_INVALID;
}
//-------------------------------------------------------------------
//	trimRight
//-------------------------------------------------------------------
String string::trimRight(const String& str, char lookFor)
{
	int beforeTrimLength = (int)str.Length();
	int afterTrimLength = beforeTrimLength;

	for ( int i = beforeTrimLength-1; i > 0; i-- )
	{
		if ( str[i] != lookFor )
			break;

		afterTrimLength--;
	}

	if ( afterTrimLength < 1 )
		return String();

	return String( str.AsChar(), afterTrimLength );
}
//-------------------------------------------------------------------
//	trimLeft
//-------------------------------------------------------------------
String string::trimLeft(const String& str, char lookFor)
{
	uint beginIndex = 0;

	if ( str.Length() == 0 )
		return String();

	for ( uint i = 0; i < str.Length(); i++ )
	{
		if ( str[i] != lookFor )
			break;

		beginIndex++;
	}

	if ( beginIndex >= str.Length() )
		return String();

	return String( str, beginIndex, str.Length() - beginIndex );
}
//-------------------------------------------------------------------
//	trim
//-------------------------------------------------------------------
String string::trim(const String& str, char lookFor)
{
	String result = trimRight( str, lookFor );
	result = trimLeft( result.AsChar(), lookFor );

	return result;
}
//-------------------------------------------------------------------
//	intToChar
//-------------------------------------------------------------------
char string::intToChar(int value)
{
	if ( !math::between( value, 0, 9 ) )
	{
		THROW( "intToChar() integer value must be between 0 and 9" );
	}

	return (char)( (int)('0') + value );
}
//-------------------------------------------------------------------
//	intToStr
//-------------------------------------------------------------------
String string::intToStr(int value)
{
	String result( value < 0 ? "-" : "" );

	const uint precision = 255;
	bool done = false;

	int numberDigits = 0;
	char charNumber[precision]; // sounds like Charmander :P

	//memoryWrite( charNumber, precision, '\0' );

	value = value > 0 ? value : -value;

	do
	{
		if ( numberDigits > precision )
		{
			ASSERT( "Number of digits greater than precision" );
			break;
		}

		int mod10 = value % 10;
		value /= 10;

		charNumber[numberDigits] = (char)( (int)('0') + mod10 );
		numberDigits++;
	} while ( value != 0 );

	memoryReverse( charNumber, numberDigits );

	result += String( charNumber, numberDigits );

	return result;
}
//-------------------------------------------------------------------
String string::uintToStr(uint value)
{
	String result;

	const uint precision = 255;
	bool done = false;

	int numberDigits = 0;
	char charNumber[precision]; // sounds like Charmander :P

	//memoryWrite( charNumber, precision, '\0' );

	do
	{
		if ( numberDigits > precision )
		{
			ASSERT( "Number of digits greater than precision" );
			break;
		}

		int mod10 = value % 10;
		value /= 10;

		charNumber[numberDigits] = (char)( (int)('0') + mod10 );
		numberDigits++;
	} while ( value != 0 );

	memoryReverse( charNumber, numberDigits );

	result += String( charNumber, numberDigits );

	return result;
}
//-------------------------------------------------------------------
//	floatToStr
//-------------------------------------------------------------------
String string::floatToStr(float value)
{
	// Decimal conversion
	String result;
	const uint precision = 6;
	char charNumber[precision]; // sounds like Charmander :P
	int intPart = (int)value;
	float decimalPart = fabs(value - intPart);

	memoryWrite( charNumber, precision, '\0' );

	for ( uint i = 0; i < precision; i++ )
	{
		decimalPart *= 10;
		intPart = (int)decimalPart;

		charNumber[i] = intToChar( intPart );

		decimalPart = decimalPart - intPart;
	}

	result = String(charNumber, precision);

	// Integer conversion
	// Using fabs, because negative values greater than -1.0 are round to 0
	// therefore negative number will be handled in this function
	result = intToStr( (int)fabs(value) ) + String('.') + result;

	if ( value < 0.0f )
		result = '-' + result;

	// Only to make the result look cute ^^
	result = trimRight(result.AsChar(), '0');

	if ( result.Last() == '.' )
		result = result + '0';

	return result;
}
//-------------------------------------------------------------------
//	vec2ToStr
//-------------------------------------------------------------------
String string::vec2ToStr(const math::Vec2& v)
{
	return String( floatToStr(v.x) + " " + floatToStr(v.y) );
}
//-------------------------------------------------------------------
//	vec3ToStr
//-------------------------------------------------------------------
String string::vec3ToStr(const math::Vec3& v)
{
	return String( floatToStr(v.x) + " " + floatToStr(v.y) + " " + floatToStr(v.z) );
}
//-------------------------------------------------------------------
//	vec4ToStr
//-------------------------------------------------------------------
String string::vec4ToStr(const math::Vec4& v)
{
	return String( floatToStr(v.x) + " " + floatToStr(v.y) + " " + floatToStr(v.z) + " " + floatToStr(v.w) );
}
//-------------------------------------------------------------------
//	boolToStr
//-------------------------------------------------------------------
String string::boolToStr(bool value)
{
	return value ? "True" : "False";
}
//-------------------------------------------------------------------
//	eulerToStr
//-------------------------------------------------------------------
String string::eulerToStr(const math::Euler& angle)
{
	math::Euler angleDegs = angle.AsDegs();
	String result = vec3ToStr( math::Vec3( angleDegs.pitch, angleDegs.yaw, angleDegs.roll ) );
	return result;
}
//-------------------------------------------------------------------
//	upperCase
//-------------------------------------------------------------------
String string::upperCase(const String& str)
{
	String result = str;

	for ( uint i = 0; i < result.Length(); i++ )
	{
		if (math::between( result[i], 'a', 'z' ) )
			result[i] =  upperCase(result[i]);
	}

	return result;
}
//-------------------------------------------------------------------
char string::upperCase(char c)
{
	char result = c;

	if ( math::between( result, 'a', 'z' ) )
		result = (char)((int)result - 32 );

	return result;
}
//-------------------------------------------------------------------
//	lowerCase
//-------------------------------------------------------------------
String string::lowerCase(const String& str)
{
	String result = str;

	for ( uint i = 0; i < result.Length(); i++ )
	{
		if ( math::between( result[i], 'A', 'Z' ) )
			result[i] = (char)((int)result[i] + 32);
	}

	return result;
}
//-------------------------------------------------------------------
char string::lowerCase(char c)
{
	char result = c;

	if ( math::between( result, 'A', 'Z' ) )
		result = (char)((int)result + 32 );
	else
	{
		ASSERT( "character is not upper case" );
	}

	return result;
}
//-------------------------------------------------------------------
//	contains
//-------------------------------------------------------------------
bool string::contains(const String& str, char c)
{
	for ( uint i = 0; i < str.Length(); i++ )
	{
		if ( c == str[i] )
			return true;
	}

	return false;
}
//-------------------------------------------------------------------
bool string::contains(const String& str, const String& subStr)
{
	uint strLength = str.Length();
	uint subStrLength = subStr.Length();
	uint subStrIndex = 0;

	if ( strLength == 0 || subStrLength == 0 )
		return false;

	if ( subStrLength > strLength )
		return false;

	for ( uint i = 0; i < strLength; i++ )
	{
		if ( subStr[subStrIndex] == str[i] )
		{
			subStrIndex++;
			if ( subStrIndex == subStrLength )
				return true;
		}
		else
			subStrIndex = 0;
	}

	return false;
}
//-------------------------------------------------------------------
bool string::containsNumber(char c)
{
	return c >= '0' && c <= '9';
}
//-------------------------------------------------------------------
//	charCount
//-------------------------------------------------------------------
uint string::charCount(const String& str, const String& lookFor)
{
	uint result = 0;

	for ( uint i = 0; i < str.Length(); i++ )
	{
		for ( uint j = 0; j < lookFor.Length(); j++ )
		{
			if ( str[i] == lookFor[j] )
			{
				result++;
				break;
			}
		}
	}

	return result;
}
//-------------------------------------------------------------------
//	isNumber
//-------------------------------------------------------------------
bool string::isNumber(char c)
{
	return containsNumber(c) || contains( plusMinusDot, c );
}
//-------------------------------------------------------------------
bool string::isNumber(const String& str)
{
	if ( str.Length() == 0 )
		return false;

	uint dotCharCount = 0;
	uint negativeCharIndex = 0;
	uint positiveCharIndex = 0;
	bool _containsNumber = false;

	for ( uint i = 0; i < str.Length(); i++ )
	{
		_containsNumber |= containsNumber(str[i]);
		bool containsNumberRelated = contains(plusMinusDot, str[i]);

		if ( !_containsNumber && !containsNumberRelated )
			return false;

		switch ( str[i] )
		{
			case '.': dotCharCount++; break;
			case '-': negativeCharIndex = i; break;
			case '+': positiveCharIndex = i; break;
		}
	}

	if ( !_containsNumber || dotCharCount > 1 || negativeCharIndex > 0 || positiveCharIndex > 0 )
		return false;

	return true;
}
//-------------------------------------------------------------------
bool string::isNumber(const String& s, int integerPartMax, int decimalPartMax)
{
	if ( s.Length() == 0 )
		return false;

	int decimalCount = 0;
	int integerPartCount = 0;
	int decimalPartCount = 0;

	for ( uint i = 0; i < s.Length(); i++ )
	{
		if ( s[i] == '-' || s[i] == '+' )
		{
			if ( i != 0 )
				return false;
		}
		else if ( s[i] == '.' )
		{
			decimalCount++;
			if ( decimalCount > 1 )
				return false;
		}
		else if ( string::containsNumber(s[i]) )
		{
			if ( decimalCount == 0 )
				integerPartCount++;
			else
				decimalPartCount++;
		}
		else
			return false;
	}

	if ( (integerPartMax >= 0 && integerPartCount > integerPartMax) || (decimalPartMax >= 0 && decimalPartCount > decimalPartMax) )
		return false;

	return true;
}
//-------------------------------------------------------------------
//	isAlpha
//-------------------------------------------------------------------
bool string::isAlpha(char c)
{
	return math::between( c, 'A', 'Z' ) || math::between( c, 'a', 'z' );
}
//-------------------------------------------------------------------
//	charToInt
//-------------------------------------------------------------------
int string::charToInt(char c)
{
	if ( !math::between(c, '0', '9') )
	{
		THROW( "charToInt() invalid char for conversion to integer" );
	}

	return (int)c - (int)('0');
}
//-------------------------------------------------------------------
//	strToInt
//-------------------------------------------------------------------
int string::strToInt(const String& str)
{
	int result = 0;
	uint power10 = 1;
	uint maximumNumericCharsAllowed = 6;

	if ( !validateStringNumber(str, maximumNumericCharsAllowed) )
		return 0;

	uint dotIndex = findFirst( str, '.' );

	if ( dotIndex == 0 )
		return 0;

	uint idxLastNumberInteger = dotIndex < str.Length() ? dotIndex - 1 : str.Length() - 1;

	for ( int i = (int)idxLastNumberInteger; i >= 0; i-- )
	{
		if ( str[i] == '.' )
			break;

		if ( contains( "+-", str[i] ) )
			continue;

		result += charToInt(str[i]) * power10;
		power10 *= 10;
	}

	if ( str[0] == '-' )
		result *= -1;

	return result;
}
//-------------------------------------------------------------------
//	strToFloat
//-------------------------------------------------------------------
float string::strToFloat(const String& str)
{
	float result;
	uint power10 = 1;

	// Convert integer part
	// fabs is used because negative numbers greater than -1.0 are round to zero
	// and zero has no negative. The result will be turned negative in another step of this function (only if necessary, obviously)
	result = fabs( (float)strToInt( str ) );

	// Convert decimal part
	uint index = findFirst(str, '.');

	index = index == UINT_INVALID ? 1 : index + 1;

	for ( ; index < str.Length(); index++ )
	{
		power10 *= 10;
		result += (float)charToInt(str[index]) / (float)power10;
	}

	if ( str[0] == '-' )
		result *= -1.0f;

	return result;
}
//-------------------------------------------------------------------
//	strToIntArray
//-------------------------------------------------------------------
void string::strToIntArray(const String& s, int** ppOutput, uint outputCount)
{
	Array<String> strVector = ExtractNumberStringArray(s, outputCount);

	for ( uint i = 0; i < outputCount; i++ )
		*ppOutput[i] = strToInt( strVector[i] );
}
//-------------------------------------------------------------------
//	strToFloatArray
//-------------------------------------------------------------------
void string::strToFloatArray(const String& s, float** ppOutput, uint outputCount)
{
	Array<String> strVector = ExtractNumberStringArray(s, outputCount);

	for ( uint i = 0; i < outputCount; i++ )
		*ppOutput[i] = strToFloat( strVector[i] );
}
//-------------------------------------------------------------------
//	strToVec2
//-------------------------------------------------------------------
math::Vec2 string::strToVec2(const String& s)
{
	math::Vec2 result(0.0f);
	float* vector[] = { &result.x, &result.y };
	uint count = ARRAY_COUNT(vector);
	strToFloatArray( s, vector, count );

	for ( uint i = 0; i < count; i++ )
		result[i] = *vector[i];

	return result;
}
//-------------------------------------------------------------------
//	strToVec3
//-------------------------------------------------------------------
math::Vec3 string::strToVec3(const String& s)
{
	// NOTE: repetition :(
	math::Vec3 result(0.0f);
	float* vector[] = { &result.x, &result.y, &result.z };
	uint count = ARRAY_COUNT(vector);
	strToFloatArray( s, vector, count );

	for ( uint i = 0; i < count; i++ )
		result[i] = *vector[i];

	return result;
}
//-------------------------------------------------------------------
//	addExtension
//-------------------------------------------------------------------
String string::addExtension(const String& file, const String& extension)
{
	String result;
	uint indexAtExtension = findLast( file, '.' );

	CHECK( file.Length() > 0 );

	if ( extension.Length() == 0 )
		return file;

	if ( indexAtExtension != UINT_INVALID )
	{
		result = copy( file, 0, indexAtExtension );
	}
	else
		result = file;

	result = result + (extension[0] == '.' ? extension : String(".") + extension);
	return result;
}
//-------------------------------------------------------------------
StringW string::addExtension(const StringW& file, const StringW& extension)
{
	uint indexAtExtension = UINT_INVALID;
	StringW result;

	CHECK( file.Length() > 0 );

	if ( extension.Length() == 0 )
		return file;

	for ( int i = (int)file.Length() - 1; i >= 0; i-- )
	{
		if ( file[i] == '.' )
		{
			ASSERT( "Untested" );
			indexAtExtension = (uint)i;
			break;
		}
	}

	if ( indexAtExtension != UINT_INVALID )
		result = StringW( file, 0, indexAtExtension );
	else
		result = file;

	result = result + (extension[0] == L'.' ? extension : StringW(L".") + extension);
	return result;
}
//-------------------------------------------------------------------
//	extractFileName
//-------------------------------------------------------------------
String string::extractFileName(const String& path, bool returnWithExtension)
{
	uint indexBackSlash = findLast( path.AsChar(), '\\' );
	uint indexFowardSlash = findLast( path.AsChar(), '/' );
	uint indexSlash = math::min( indexBackSlash, indexFowardSlash );
	String result;

	if ( indexSlash != UINT_INVALID )
		result = String( path, indexSlash + 1, path.Length() );
	else
		result = path;

	if ( !returnWithExtension )
	{
		uint indexExtension = findLast( result, '.' );

		if ( indexExtension != UINT_INVALID )
			result = String( result, indexExtension );
	}

	return result;
}
//-------------------------------------------------------------------
//	extractFileExtension
//-------------------------------------------------------------------
String string::extractFileExtension(const String& file, bool returnWithDot)
{
	uint dotIndex = findLast( file, '.' );

	if ( dotIndex == UINT_INVALID || file.Length() == 0 || dotIndex == file.Length() - 1 )
		return "";

	return copy( file, dotIndex + (returnWithDot ? 0 : 1), file.Length() );
}
//-------------------------------------------------------------------
//	upperFirst
//-------------------------------------------------------------------
String string::upperFirst(const String& str)
{
	bool makeUpperCase = true;
	String result = str;

	for ( uint i = 0; i < result.Length(); i++ )
	{
		if ( makeUpperCase )
		{
			result[i] = upperCase( result[i] );
			makeUpperCase = false;
		}
		else if ( result[i] == ' ' )
			makeUpperCase = true;
	}

	return result;
}
