#include "../engine/Font.h"
#include "../engine/Texture.h"
#include "../engine/TgaLoader.h"
#include "../engine/Path.h"
#include "../engine/File.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Font::Font()
{
	m_charSize = 0;
	m_initialized = false;
}
//-------------------------------------------------------------------
Font::~Font()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Font::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Font::Initialize(const char* fontName)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	SetName( string::extractFileName( fontName, false ) );

	FileReader fileReader;
	fileReader.Load( path::font(fontName), eFileMode::Text );
	ScriptReader script( fileReader );
	String textureName;

	if ( !fileReader.IsLoaded() )
	{
		ASSERT( "Failed loading file" );
		return;
	}

	for ( uint i = 0; i < script.GetLineCount(); i++ )
	{
		String strData;

		auto pLine = script.GetLine( i );

		if ( pLine->HasString("bitmap") )
			LoadTexture( pLine->GetQuotedString() );
		else if ( pLine->HasString("charSize") )
			m_charSize = pLine->GetInt();
		else if ( pLine->HasString("char") )
		{
			if ( !m_hTexture || m_charSize == 0 )
			{
				ASSERT( "Something wrong with the font data" );
				continue;
			}

			String c;

			pLine->SetQuotedString( "char", c );

			if ( c.Length() != 1 )
			{
				ASSERT( "Invalid char" );
				continue;
			}

			uint iChar = (uint)c[0];
			math::Vec2 textureResolution = m_hTexture->GetDimension();
			BitmapCharInfo* pInfo = &m_charInfos[iChar];

			CHECK( pInfo->sizeProportion.x == 0.0f && pInfo->sizeProportion.y == 0.0f || pInfo->c == '\0' );
			pInfo->c = c[0];

			bool coordCheck = true;
			coordCheck &= pLine->SetFloatArray( "coord_min", &pInfo->texCoordMin[0], 2, false );
			coordCheck &= pLine->SetFloatArray( "coord_max", &pInfo->texCoordMax[0], 2, false );
			pLine->SetFloat( "offset", pInfo->yOffset );

			CHECK( coordCheck );

			pInfo->sizeProportion = pInfo->texCoordMax - pInfo->texCoordMin;
			pInfo->yOffset = pInfo->yOffset / m_charSize;
			pInfo->sizeProportion = math::scaleInv(pInfo->sizeProportion, math::Vec2((float)m_charSize));
			pInfo->texCoordMin = math::scaleInv( pInfo->texCoordMin, textureResolution );
			pInfo->texCoordMax = math::scaleInv( pInfo->texCoordMax, textureResolution );
		}
	}

	m_initialized = true;
}
//-------------------------------------------------------------------
//	LoadTexture
//-------------------------------------------------------------------
void Font::LoadTexture(const String& fontName)
{
	CHECK( !m_hTexture );
	
	String textureName = string::extractFileName( fontName, false );
	TgaLoader textureLoader;

	textureLoader.Load( path::font(textureName, path::EXTENSION_TEXTURE) );

	if ( !textureLoader.IsLoaded() )
	{
		ASSERT( "Failed loading font bitmap" );
		m_hTexture = primitive::texture::createWhite();
		return;
	}

	const TgaInfo& textureData = textureLoader.GetInfo();
	m_hTexture = g_pResource->GetTextureMngr()->CreateFromStream( textureName, textureData.pData, textureData.width, textureData.height, textureData.bitsPerPixel, false );

	if ( !m_hTexture.IsValid() )
	{
		m_hTexture = primitive::texture::createWhite();
		ASSERT( "Failed loading font bitmap" );
	}
}
//-------------------------------------------------------------------
//	GetCharInfo
//-------------------------------------------------------------------
const BitmapCharInfo& Font::GetCharInfo(char c)const
{
	static BitmapCharInfo invalid( math::Vec2(0.0), math::Vec2(0.5), math::Vec2(0.8f, 0.5f), 0.0f, '\0' );
	const BitmapCharInfo* pResult = &m_charInfos[(uint)c];

	if ( pResult->c != '\0' )
		return *pResult;

	invalid.texCoordMin = math::Vec2( math::random(0.0f, 0.4f), math::random(0.0f, 0.4f) );
	invalid.texCoordMax = math::Vec2( math::random(0.6f, 1.0f), math::random(0.6f, 1.0f) );
	invalid.c = c;
	return invalid;
}
//-------------------------------------------------------------------
//	GetTexture
//-------------------------------------------------------------------
Handle<ITexture> Font::GetTexture()
{
	return m_hTexture;
}
//-------------------------------------------------------------------
//	GetLineDimension
//-------------------------------------------------------------------
math::Vec2 Font::GetLineDimension(const String& text, float charSize, float spaceSize)const
{
	math::Vec2 result;
	float spaceScale = spaceSize * charSize * 0.5f;

	for ( uint i = 0; i < text.Length(); i++ )
	{
		if ( text[i] == '\n' )
			continue;

		const BitmapCharInfo& bitmapInfo = GetCharInfo( text[i] );

		math::Vec2 bitmapScale = bitmapInfo.sizeProportion * 0.5f;
		bitmapScale *= charSize;

		result.x += bitmapScale.x + spaceScale;
		result.y = math::max( result.y, bitmapScale.y );
	}

	if ( text.Length() > 0 )
		result.x -= spaceScale;

	return result * 2.0f;
}
//-------------------------------------------------------------------
//	GetTextArea
//-------------------------------------------------------------------
math::Vec2 Font::GetTextArea(const String& text, float charSize, float spaceSize, float lineBreakProportion)const
{
	// My first lambda :)
	auto calculateLine = [](math::Vec2& outResult, const math::Vec2& currentLine, float lineSpace)
	{
		outResult.x = math::max( outResult.x, currentLine.x );
		outResult.y += lineSpace;
	};

	math::Vec2 result;
	math::Vec2 lineScale;
	float spaceScale = spaceSize * charSize * 0.5f;

	for ( uint i = 0; i < text.Length(); i++ )
	{
		if ( text[i] == '\n' )
		{
			calculateLine( result, lineScale, lineBreakProportion * charSize * 0.5f );
			lineScale = math::Vec2(0.0f);
			continue;
		}

		const BitmapCharInfo& bitmapInfo = GetCharInfo( text[i] );

		math::Vec2 bitmapScale = bitmapInfo.sizeProportion * 0.5f;
		bitmapScale *= charSize;

		lineScale.x += bitmapScale.x + spaceScale;
		lineScale.y = math::max( lineScale.y, bitmapScale.y );
	}

	if ( text.Length() > 0 )
		result.x -= spaceScale;

	calculateLine( result, lineScale, lineScale.y );

	return result * 2.0f;
}
