#include "../engine/ScriptLine.h"
#include "../engine/Exception.h"
#include "Array.h"
#include "../engine/CoreInterface.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ScriptLine::ScriptLine()
{
}
//-------------------------------------------------------------------
ScriptLine::ScriptLine(const String& line)
{
	LineToScriptObject( line );
}
//-------------------------------------------------------------------
ScriptLine::ScriptLine(const char* data, uint size) : ScriptLine( String(data, size) )
{
}
//-------------------------------------------------------------------
//	RemoveAt
//-------------------------------------------------------------------
void ScriptLine::RemoveAt(uint index)
{
	uint i = 0;
	for ( auto pNode = m_objects.GetFirst(); pNode; pNode = pNode->GetNext(), i++ )
	{
		if ( index == i )
		{
			pNode->Remove();
			break;
		}
	}
}
//-------------------------------------------------------------------
//	HasString
//-------------------------------------------------------------------
bool ScriptLine::HasString(const String& value, uint findsToSkip)const
{
	return GetBy( &value, nullptr, findsToSkip, nullptr );
}
//-------------------------------------------------------------------
//	HasType
//-------------------------------------------------------------------
bool ScriptLine::HasType(eScriptType type, uint findsToSkip)const
{
	return GetBy( nullptr, &type, findsToSkip, nullptr );
}
//-------------------------------------------------------------------
//	HasTypeSequence
//-------------------------------------------------------------------
bool ScriptLine::HasTypeSequence(eScriptType type, uint sequenceCount, uint findsToSkip)const
{
	bool hasTypeSequence = sequenceCount > 0;

	for ( uint i = 0; i < sequenceCount; i++ )
		hasTypeSequence &= GetBy( nullptr, &type, findsToSkip + i, nullptr );

	return hasTypeSequence;
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
String ScriptLine::Get(uint findsToSkip)const
{
	uint finds = 0;
	for ( auto it = m_objects.GetIterator(); it; it++ )
	{
		if ( finds == findsToSkip )
			return (*it).value;
		finds++;
	}

	return String();
}
//-------------------------------------------------------------------
//	GetByType
//-------------------------------------------------------------------
String ScriptLine::GetByType(eScriptType type, uint findsToSkip)const
{
	String result;
	
	GetBy( nullptr, &type, findsToSkip, &result );

	return result;
}
//-------------------------------------------------------------------
//	GetByTypeSequence
//-------------------------------------------------------------------
String ScriptLine::GetByTypeSequence(eScriptType type, uint sequenceCount, char separator, uint findsToSkip)const
{
	String result;

	for ( uint i = 0; i < sequenceCount; i++ )
	{
		String text;

		GetBy( nullptr, &type, findsToSkip + i, &text );

		if ( i > 0 && text.Length() > 0 )
			result += separator;

		result += text;
	}

	return result;
}
//-------------------------------------------------------------------
//	GetFloat
//-------------------------------------------------------------------
float ScriptLine::GetFloat(uint findsToSkip)const
{
	String numberStr = GetByType( eScriptType::Number, findsToSkip );
	float result = 0.0f;

	if ( numberStr.Length() > 0 )
		result = string::strToFloat( numberStr );

	return result;
}
//-------------------------------------------------------------------
//	GetInt
//-------------------------------------------------------------------
int ScriptLine::GetInt(uint findsToSkip)const
{
	String numberStr = GetByType( eScriptType::Number, findsToSkip );
	int result = 0;

	if ( numberStr.Length() > 0 )
		result = string::strToInt( numberStr );

	return result;
}
//-------------------------------------------------------------------
//	GetQuotedString
//-------------------------------------------------------------------
String ScriptLine::GetQuotedString(uint findsToSkip)const
{
	String result = GetByType( eScriptType::QuotedString, findsToSkip );
	uint length = result.Length();

	return result;
}
//-------------------------------------------------------------------
//	SetFloat
//-------------------------------------------------------------------
bool ScriptLine::SetFloat(const String& text, float& outResult)const
{
	auto it = GetAt(text);

	if ( it )
		it++;

	if ( it )
	{
		if ( (*it).type != eScriptType::Number )
			return false;

		outResult = string::strToFloat( (*it).value );
		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	SetFloatArray
//-------------------------------------------------------------------
bool ScriptLine::SetFloatArray(const String& text, float* pOutResult, uint arrayCount, bool repeatLast)const
{
	CHECK( arrayCount > 0 );

	float* pTemp = nullptr;
	bool success = false;
	auto it = GetAt(text);
	int lastValue = -1;

	if ( it )
		it++;

	if ( it )
	{
		pTemp = new float[arrayCount];

		success = true;
		for ( uint i = 0; i < arrayCount; i++ )
		{
			if ( it && (*it).type == eScriptType::Number )
			{
				pTemp[i] = string::strToFloat( (*it).value );
				it++;
				lastValue = (int)i;
			}
			else if ( repeatLast && lastValue > -1 )
			{
				pTemp[i] = pTemp[lastValue];
			}
			else
			{
				success = false;
				break;
			}

		}
	}

	if ( success )
		memoryCopy( pTemp, pOutResult, arrayCount );

	safeArrayRelease( pTemp );

	return success;
}
//-------------------------------------------------------------------
//	SetInt
//-------------------------------------------------------------------
bool ScriptLine::SetInt(const String& text, int& outResult)const
{
	auto it = GetAt(text);

	if ( it )
	{
		it++;

		if ( !it || (*it).type != eScriptType::Number )
			return false;

		outResult = string::strToInt( (*it).value );
		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	SetIntArray
//-------------------------------------------------------------------
bool ScriptLine::SetIntArray(const String& text, int* pOutResult, uint arrayCount)const
{
	CHECK( arrayCount > 0 );

	int* pTemp = nullptr;
	bool success = false;
	auto it = GetAt(text);

	if ( it )
		it++;

	if ( it )
	{
		pTemp = new int[arrayCount];

		success = true;
		for ( uint i = 0; i < arrayCount; i++ )
		{
			if ( it && (*it).type == eScriptType::Number )
			{
				pTemp[i] = string::strToInt( (*it).value );
				it++;
			}
			else
			{
				success = false;
				break;
			}
		}
	}

	if ( success )
		memoryCopy( pTemp, pOutResult, arrayCount );

	safeArrayRelease( pTemp );

	return success;

}
//-------------------------------------------------------------------
//	SetInt
//-------------------------------------------------------------------
bool ScriptLine::SetBool(const String& text, bool& outResult)const
{
	auto it = GetAt(text);

	if ( it )
	{
		it++;

		if ( !it || (*it).type != eScriptType::String )
			return false;

		if ( string::equal((*it).value, "true", false) || string::equal((*it).value, "on", false) )
			outResult = true;
		else if ( string::equal((*it).value, "false", false) || string::equal((*it).value, "off", false) )
			outResult = false;
		else
		{
			ASSERT( "Not recognized" );
			return false;
		}

		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	SetQuotedString
//-------------------------------------------------------------------
bool ScriptLine::SetQuotedString(const String& text, String& outQuotedString)const
{
	auto it = GetAt(text);

	if ( it )
	{
		it++;

		if ( !it || (*it).type != eScriptType::QuotedString )
			return false;

		outQuotedString = (*it).value;
		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	GetBy
//-------------------------------------------------------------------
bool ScriptLine::GetBy(const String* text, eScriptType* type, uint findsToSkip, String* result)const
{
	uint foundCount = 0;

	// Bad, bad design...
	if ( text && type )
	{
		THROW( "ScriptLine::GetBy() This method was not designed to support the parameters \'text\' and \'type\' at the same time" );
	}

	for ( auto it = m_objects.GetIterator(); it; it++ )
	{
		if ( text && string::equal((*it).value.AsChar(), *text, false) )
			foundCount++;
		else if ( type && *type == (*it).type )
			foundCount++;

		if ( foundCount > findsToSkip )
		{
			if ( result )
				*result = (*it).value;

			return true;
		}
	}

	return false;
}
//-------------------------------------------------------------------
//	GetAt
//-------------------------------------------------------------------
ListIteratorConst<ScriptObject> ScriptLine::GetAt(const String& text)const
{
	String textFormatted = string::upperCase( text );

	for ( auto it = m_objects.GetIterator(); it; it++ )
	{
		if ( (*it).type == eScriptType::String && textFormatted == (*it).value )
			return it;
	}

	return ListIteratorConst<ScriptObject>();
}
//-------------------------------------------------------------------
//	LineToScriptObject
//-------------------------------------------------------------------
void ScriptLine::LineToScriptObject(const String& s)
{
	if ( s.Length() == 0 )
		return;

	String formatted = string::remove( s, "\t\n" );
	Array<String> separated;
	String quote = "\"\'";

	string::split( formatted.AsChar(), ' ', separated );

	for ( uint i = 0; i < separated.Count(); i++ )
	{
		ScriptObject so;

		if ( string::charCount( separated[i][0], quote ) > 0 )
		{
			String quotedString;
			uint j = i;
			bool foundCloseQuote = false;

			for ( ; j < separated.Count(); j++ )
			{
				if ( j != i )
					quotedString += " ";
				quotedString += separated[j];

				uint len = separated[j].Length();
				const char* pStr = separated[j].AsChar();
				bool isPrevCharQuoteString = separated[j].Length() > 1 && separated[j][separated[j].Length() - 2] == '\\';
				bool isCharCloseQuote = string::charCount( separated[j].Last(), quote ) > 0 && (j != i || separated[j].Length() > 1);

				if ( isCharCloseQuote && !isPrevCharQuoteString )
				{
					i = j;
					foundCloseQuote = true;
					break;
				}
			}

			CHECK( foundCloseQuote );

			if ( quotedString.Length() > 2 )
			{
				String valueUnformatted = string::copy(quotedString, 1, quotedString.Length() - 2);
				so.type = eScriptType::QuotedString;

				for ( uint i = 0; i < valueUnformatted.Length(); i++ )
				{
					if ( i < (valueUnformatted.Length() - 1) && valueUnformatted[i] == '\\' && string::charCount(valueUnformatted[i+1], quote) > 0 )
						i++;
					
					so.value += valueUnformatted[i];
				}
			}
			else
			{
				CHECK( "Quoted string has length of two (possible problem)" );
			}
		}
		else
		{
			so.value = separated[i];
			
			if ( string::isNumber(so.value.AsChar()) )
			{
				so.type = eScriptType::Number;
			}
			else
			{
				so.type = eScriptType::String;
				so.value = string::upperCase( separated[i] );
			}
		}

		if ( so.value.Length() > 0 )
			m_objects.AddLast( so );
	}
}
