#pragma once

#include "../engine/Pointer.h"
#include "../engine/ScriptReader.h"
#include "../engine/String.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"

class IFontEffect;
class ITexture;
class Texture2d;
class FontManager;

struct BitmapCharInfo
{
	math::Vec2 texCoordMin;
	math::Vec2 texCoordMax;
	math::Vec2 sizeProportion;
	float yOffset;
	char c;

	BitmapCharInfo(const math::Vec2& _textCoordMin, const math::Vec2& _textCoordMax, const math::Vec2& _sizeProportion, float _yOffset, char _c) : 
		texCoordMin(_textCoordMin), texCoordMax(_textCoordMax), sizeProportion(_sizeProportion), yOffset(_yOffset), c(_c) {}
	BitmapCharInfo() : texCoordMin(0.0f), texCoordMax(0.0f), yOffset(0.0f), c('\0') {}
};

class Font
{
	friend class FontManager;

	public:
		Font();
		~Font();

		const BitmapCharInfo& GetCharInfo(char)const;
		Handle<ITexture> GetTexture();
		math::Vec2 GetLineDimension(const String& text, float charSize, float spaceSize)const;
		math::Vec2 GetTextArea(const String& text, float charSize, float spaceSize, float lineBreakProportion)const;
		const HashString& GetName()const { return m_name; }

	private:
		void Initialize(const char* fontName);
		void Shutdown();

		void LoadTexture(const String& fontName);
		void SetName(const HashString& name) { m_name = name; }

		bool m_initialized;
		HashString m_name;
		Handle<ITexture> m_hTexture;
		uint m_charSize;
		BitmapCharInfo m_charInfos[255];
};
