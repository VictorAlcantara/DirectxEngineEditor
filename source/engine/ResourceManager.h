#pragma once

#include "CoreModule.h"
#include "../engine/Class.h"
#include "../engine/Pointer.h"

class ModelManager;
class TextureManager;
class FontManager;

class ResourceManager : public NonCopyable
{
	CORE_MODULE;

	public:
		~ResourceManager();
		void Initialize();
		void Shutdown();

		ModelManager* GetModelMngr();
		TextureManager* GetTextureMngr();
		FontManager* GetFontMngr();

	private:
		ResourceManager();

		bool m_initialized;

		SimplePtr<ModelManager> m_pModelMngr;
		SimplePtr<TextureManager> m_pTextureMngr;
		SimplePtr<FontManager> m_pFontMngr;
};
