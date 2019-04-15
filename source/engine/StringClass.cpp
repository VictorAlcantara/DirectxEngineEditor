#include "StringClass.h"
#include <comdef.h>
#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/Debug.h"
#include "Array.h"
#include "../engine/String.h"

#ifdef DEBUG_STRING_INDEX_CHECK
	#define CHECK_INDEX(idx) ValidateIndex((idx))
#else
	#define CHECK_INDEX 
#endif

#ifdef DEBUG_MODE
	#define CHECK_HASH_COLLISION
#endif

#ifdef CHECK_HASH_COLLISION
	#include "../engine/Table.h"
	#include "../engine/TableHashFunction.h"
#endif

//=============================================================================
//	String
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
String::String()
{
	m_sData = "";
}
//-------------------------------------------------------------------
String::String(char c)
{
	const char data[] = { c, '\0' };
	m_sData = std::string(data);
}
//-------------------------------------------------------------------
String::String(const char* data, uint size)
{
	m_sData = std::string(data, size);
}
//-------------------------------------------------------------------
String::String(const String& str, uint size)
{
	m_sData = std::string(str.AsChar(), size);
}
//-------------------------------------------------------------------
String::String(const char* data)
{
	if ( !data )
		m_sData = "";
	else
		m_sData = std::string(data);
}
//-------------------------------------------------------------------
String::String(const wchar* data)
{
	_bstr_t formatted( data );
	m_sData = std::string( (const char*)formatted );
}
//-------------------------------------------------------------------
String::String(const String& other)
{
	m_sData = other.m_sData;
}
//-------------------------------------------------------------------
String::String(const String& other, uint startAtIndex, uint charsToCopyCount)
{
	uint length = other.Length();

	m_sData = "";

	if ( startAtIndex < length && charsToCopyCount != 0 && length > 0 )
	{
		if ( (startAtIndex + charsToCopyCount) > length )
			charsToCopyCount = length - startAtIndex;

		char* pStr = new char[charsToCopyCount];

		memoryCopy( (const char*)&other.AsChar()[startAtIndex], pStr, charsToCopyCount );

		m_sData = std::string( pStr, charsToCopyCount );

		safeArrayRelease( pStr );
	}
}
//-------------------------------------------------------------------
//	Operator=
//-------------------------------------------------------------------
String& String::operator=(const String& other)
{
	if ( this != &other )
		m_sData = other.m_sData;

	return *this;
}
//-------------------------------------------------------------------
//	Operator+=
//-------------------------------------------------------------------
String& String::operator+=(const String& other)
{
	m_sData += other.m_sData;

	return *this;
}
//-------------------------------------------------------------------
//	operator==
//-------------------------------------------------------------------
bool String::operator==(const String& s)const
{
	return s.m_sData == m_sData;
}
//-------------------------------------------------------------------
//	operator[]
//-------------------------------------------------------------------
const char& String::operator[](uint index)const
{
	CHECK_INDEX( index );
	return m_sData[index];
}
//-------------------------------------------------------------------
char& String::operator[](uint index)
{
	CHECK_INDEX( index );
	return m_sData[index];
}
//-------------------------------------------------------------------
//	AsChar
//-------------------------------------------------------------------
const char* String::AsChar()const
{
	return m_sData.c_str();
}
//-------------------------------------------------------------------
//	Length
//-------------------------------------------------------------------
uint String::Length()const
{
	return (uint)m_sData.length();
}
//-------------------------------------------------------------------
//	Last
//-------------------------------------------------------------------
char String::Last()const
{
	return (*this)[Length() - 1];
}
//-------------------------------------------------------------------
//	SetAt
//-------------------------------------------------------------------
void String::SetAt(char c, uint index)
{
	CHECK_INDEX( index );

	m_sData[index] = c;
}
//-------------------------------------------------------------------
//	ValidateIndex
//-------------------------------------------------------------------
void String::ValidateIndex(uint index)const
{
	if ( index >= Length() )
	{
		THROW( "index out of bounds" );
	}
}

//=============================================================================
//	StringW
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
StringW::StringW()
{
	m_sData = L"";
}
//-------------------------------------------------------------------
StringW::StringW(const wchar* pData)
{
	m_sData = std::wstring( pData );
}
//-------------------------------------------------------------------
StringW::StringW(const StringW& other, uint startAtIndex, uint charsToCopyCount)
{
	uint length = other.Length();

	m_sData = L"";

	if ( startAtIndex < length && charsToCopyCount != 0 && length > 0 )
	{
		if ( (startAtIndex + charsToCopyCount) > length )
			charsToCopyCount = length - startAtIndex;

		wchar* pWstr = new wchar[charsToCopyCount];

		memoryCopy( (const wchar*)&other.AsWchar()[startAtIndex], pWstr, charsToCopyCount );

		m_sData = std::wstring( pWstr, charsToCopyCount );

		safeArrayRelease( pWstr );
	}
}

//-------------------------------------------------------------------
//	Operator=
//-------------------------------------------------------------------
StringW& StringW::operator=(const StringW& other)
{
	m_sData = other.m_sData;
	return *this;
}
//-------------------------------------------------------------------
//	Operator+=
//-------------------------------------------------------------------
StringW& StringW::operator+=(const StringW& other)
{
	m_sData += other.m_sData;
	return *this;
}
//-------------------------------------------------------------------
//	operator==
//-------------------------------------------------------------------
bool StringW::operator==(const StringW& s)const
{
	return s.m_sData == m_sData;
}
//-------------------------------------------------------------------
//	operator[]
//-------------------------------------------------------------------
wchar StringW::operator[](uint index)const
{
	CHECK_INDEX( index );
	return m_sData[index];
}
//-------------------------------------------------------------------
//	AsWchar
//-------------------------------------------------------------------
const wchar* StringW::AsWchar()const
{
	return m_sData.c_str();
}
//-------------------------------------------------------------------
//	Length
//-------------------------------------------------------------------
uint StringW::Length()const
{
	return (uint)m_sData.length();
}
//-------------------------------------------------------------------
//	ValidateIndex
//-------------------------------------------------------------------
void StringW::ValidateIndex(uint index)const
{
	if ( index >= Length() )
	{
		THROW( "index out of bounds" );
	}
}


//=============================================================================
//	HashString
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
HashString::HashString()
{
	m_hash = 0;
}
//-------------------------------------------------------------------
HashString::HashString(const char* str) : HashString(String(str))
{
}
//-------------------------------------------------------------------
HashString::HashString(const String& str)
{
	m_data = string::remove(string::upperCase( str ), " ~�`[]{},.\'\"\\-+=*|/");
	m_hash = GenerateHash( m_data );

	#ifdef CHECK_HASH_COLLISION
		static Table<String, uint> tableCollision(1000, tableHashFunc_Uint);

		if ( m_hash > 0 )
		{
			const String* pData = tableCollision.Find( m_hash );

			if ( pData && pData->Length() > 0 )
			{
				CHECK( (*pData) == m_data );
			}
			
			if ( !pData )
				tableCollision.Add( m_data, m_hash );
		}
	#endif
}
//-------------------------------------------------------------------
HashString::HashString(const HashString& hstr)
{
	*this = hstr;
}
//-------------------------------------------------------------------
//	GenerateHash
//-------------------------------------------------------------------
uint HashString::GenerateHash(const String& str)
{
	// http://www.eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
	// Using Shift-Add-Xor hash
	uint result = 0;
	for ( uint i = 0; i < str.Length(); i++ )
	{
		result ^= (result << 5) + (result >> 2) + str[i];
	}

	return result;
}
//-------------------------------------------------------------------
//	operator=
//-------------------------------------------------------------------
HashString& HashString::operator=(const HashString& hstr)
{
	m_data = hstr.m_data;
	m_hash = hstr.m_hash;

	return *this;
}
