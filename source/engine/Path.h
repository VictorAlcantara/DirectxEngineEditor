#pragma once

#include "../engine/String.h"

namespace path
{
	extern const StringW EXTENSION_SHADER;
	extern const String EXTENSION_MAP;
	extern const String EXTENSION_TEXTURE;
	extern const String EXTENSION_FONT;
	extern const String EXTENSION_MODEL;

	extern const StringW DIRECTORY_SHADER;
	extern const String DIRECTORY_MAP;
	extern const String DIRECTORY_TEXTURE;
	extern const String DIRECTORY_FONT;
	extern const String DIRECTORY_MODEL;

	StringW shader(const StringW& fileName, const StringW& extension = EXTENSION_SHADER);
	String map(const String& fileName, const String& extension = EXTENSION_MAP);
	String texture(const String& file, const String& extension = EXTENSION_TEXTURE);
	String font(const String& file, const String& = EXTENSION_FONT);
	String model(const String& file, const String& = EXTENSION_MODEL);
}
