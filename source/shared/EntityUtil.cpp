#include "../shared/EntityUtil.h"
#include "../engine/ScriptLine.h"

//-------------------------------------------------------------------
//	processCommand_MaterialColor
//-------------------------------------------------------------------
bool processCommand_MaterialColor(const String& cmdColor, float* floatArray, uint count, const ScriptLine* pLine)
{
	bool result = false;

	CHECK( count > 0 );

	result |= pLine->SetFloatArray(cmdColor, floatArray, count, true);
	result |= pLine->SetFloatArray(cmdColor + "_RGB", floatArray, count, true);
	result |= pLine->SetFloatArray(cmdColor + "_RGBA", floatArray, count, true);

	if ( !result )
	{
		if ( count >= 1 )
			result |= pLine->SetFloat( cmdColor + "_RED", floatArray[0] );
		if ( count >= 2 )
			result |= pLine->SetFloat( cmdColor + "_GREEN", floatArray[1] );
		if ( count >= 3 )
			result |= pLine->SetFloat( cmdColor + "_BLUE", floatArray[2] );
		if ( count >= 4 )
			result |= pLine->SetFloat( cmdColor + "_ALPHA", floatArray[3] );
	}

	return result;
}
