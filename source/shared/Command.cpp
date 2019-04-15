#include "Command.h"

//-------------------------------------------------------------------
//	formatFloat
//-------------------------------------------------------------------
String command::formatFloat(const String& cmd, float value)
{
	return cmd + String(" ") + string::floatToStr( value );
}
//-------------------------------------------------------------------
//	isEntity
//-------------------------------------------------------------------
bool command::isEntity(const String& cmd)
{
	String cmdFormatted = string::upperCase( string::copy( cmd, 0, 7 ) );

	if ( cmdFormatted == "ENTITY_" )
		return true;

	return false;
}
//-------------------------------------------------------------------
//	serializeCommandList
//-------------------------------------------------------------------
String command::serializeCommandList(const String& cmd, const List<String>& lines)
{
	String result;

	for ( auto it = lines.GetIterator(); it; it++ )
	{
		result += serializeLine( cmd, formatCommandQuote(*it) );
	}

	return result;
}
//-------------------------------------------------------------------
//	serializeQuoted
//-------------------------------------------------------------------
String command::serializeQuoted(const String& cmd, const HashString& name, bool emptyIfNull)
{
	if ( name.GetString().Length() > 0 )
		return serializeLine(cmd, string::quote(name.GetString()) );
	return emptyIfNull ? "" : serializeLine(cmd);
}
//-------------------------------------------------------------------
//	serializeLine
//-------------------------------------------------------------------
String command::serializeLine()
{
	return "\n";
}
//-------------------------------------------------------------------
String command::serializeLine(const String& cmd)
{
	return cmd + serializeLine();
}
//-------------------------------------------------------------------
String command::serializeLine(const String& cmd, const String& value)
{
	if ( value.Length() < 1 )
		return "";
	return cmd + " " + value + serializeLine();
}
//-------------------------------------------------------------------
//	formatCommandQuote
//-------------------------------------------------------------------
String command::formatCommandQuote(const String& s)
{
	if ( s.Length() == 0 )
		return "\'\'";

	uint length = 0;
	char* formattedString = new char[s.Length() * 2];

	for ( uint i = 0; i < s.Length(); i++ )
	{
		if ( string::contains("\'\"", s[i]) )
		{
			if ( i == 0 || s[i-1] != '\\' )
				formattedString[length++] = '\\';
		}
		
		formattedString[length++] = s[i];
	}

	String result = String(formattedString, length);
	safeArrayRelease( formattedString );

	return string::quote( result );
}