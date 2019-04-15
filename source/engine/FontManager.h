#pragma once

#include "ResourceModule.h"
#include "../engine/Font.h"
#include "../engine/Handle.h"
#include "../engine/Table.h"

class FontManager
{
	RESOURCE_MODULE;

	public:
		~FontManager();

		Handle<Font> LoadFont(const String& name);
		Handle<Font> GetFont(const HashString& name);

	private:
		FontManager();

		void Initialize();
		void Shutdown();

		bool m_initialized;
		Table<Handle<Font>, uint> m_tFonts;
		HandleManager<Font> m_handleFonts;
};

