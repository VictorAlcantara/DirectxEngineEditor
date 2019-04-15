#include "../engine/Path.h"
#include "../engine/Debug.h"
#include "../engine/EngineGlobal.h"

namespace path
{
	const StringW EXTENSION_SHADER = L".hlsl";
	const String EXTENSION_MAP = ".map";
	const String EXTENSION_TEXTURE = ".tga";
	const String EXTENSION_FONT = ".fnt";
	const String EXTENSION_MODEL = ".obj";

	const StringW DIRECTORY_SHADER = L"../Resource/Shader/";
	const String DIRECTORY_MAP = "../Resource/Map/";
	const String DIRECTORY_TEXTURE = "../Resource/Texture/";
	const String DIRECTORY_FONT = "../Resource/Font/";
	const String DIRECTORY_MODEL = "../Resource/Model/";
}

//-------------------------------------------------------------------
//	shader
//-------------------------------------------------------------------
StringW path::shader(const StringW& fileName, const StringW& extension)
{
	return DIRECTORY_SHADER + string::addExtension( fileName, extension );
}
//-------------------------------------------------------------------
//	map
//-------------------------------------------------------------------
String path::map(const String& fileName, const String& extension)
{
	return DIRECTORY_MAP + string::addExtension( fileName, extension );
}
//-------------------------------------------------------------------
//	texture
//-------------------------------------------------------------------
String path::texture(const String& fileName, const String& extension)
{
	return DIRECTORY_TEXTURE + string::addExtension( fileName, extension );
}
//-------------------------------------------------------------------
//	font
//-------------------------------------------------------------------
String path::font(const String& fileName, const String& extension)
{
	return DIRECTORY_FONT + string::addExtension( fileName, extension );
}
//-------------------------------------------------------------------
//	model
//-------------------------------------------------------------------
String path::model(const String& fileName, const String& extension)
{
	return DIRECTORY_MODEL + string::addExtension( fileName, extension );
}
