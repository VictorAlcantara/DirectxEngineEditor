#include "../engine/FontEffect.h"
#include "../engine/Font.h"
#include "../engine/Effect.h"
#include "../engine/Texture.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../engine/Model.h"

/****************************************************************************************
	FontEffect class definition
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
FontEffect::FontEffect()
{
	m_size = 12;
	m_pos.x = 0.25f;
	m_pos.y = 0.5f;
	m_pos.z = 0.01f;
	m_charSpaceProportion = 0.05f;
	m_lineBreakProportion = 1.0f;
	m_initialized = false;
}
//-------------------------------------------------------------------
FontEffect::~FontEffect()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void FontEffect::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	OnShutdown();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void FontEffect::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_hModel = primitive::geometry::createSquare();

	OnInitialize();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	GetModel
//-------------------------------------------------------------------
Handle<Model> FontEffect::GetModel()
{
	return m_hModel;
}

/****************************************************************************************
	FontEffect_Color class definition
*****************************************************************************************/

//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void FontEffect_Color::OnInitialize()
{
	m_pEffect = new Effect_Text2d;
	m_pEffect->Initialize();
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void FontEffect_Color::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;
}
//-------------------------------------------------------------------
//	SetColor
//-------------------------------------------------------------------
void FontEffect_Color::SetColor(float r, float g, float b, float a)
{
	m_pEffect->GetConfig().color4 = math::Vec4( r, g, b, a );
}
//-------------------------------------------------------------------
void FontEffect_Color::SetColor(const math::Vec4& color)
{
	m_pEffect->GetConfig().color4 = color;
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void FontEffect_Color::Render(const String& text, Font* pFont)
{
	math::Vec3 pos = GetScreenPos();
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();

	m_pEffect->GetConfig().sampler.u = eTextureAddress::Clamp;
	m_pEffect->GetConfig().sampler.v = eTextureAddress::Clamp;
	m_pEffect->GetConfig().sampler.filter = eTextureFilter::Linear;
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;
	uint line = 1;

	// TODO: refactor code to a method in base class
	float xPosScreen = 0.0f;
	for ( uint i = 0; i < text.Length(); i++ )
	{
		if ( text[i] == '\n' )
		{
			line++;
			xPosScreen = 0.0f;
			continue;
		}

		const BitmapCharInfo& bitmapInfo = pFont->GetCharInfo( text[i] );

		m_pEffect->GetConfig().hTexture = pFont->GetTexture();
		m_pEffect->GetConfig().textureTransform = GetTextureTransform(&bitmapInfo);

		math::Vec2 bitmapScale = bitmapInfo.sizeProportion * 0.5f;
		bitmapScale *= (float)GetCharSize();

		math::Vec2 posScreen;
		posScreen.x += pos.x + bitmapScale.x + xPosScreen;
		posScreen.y += pos.y + (float)(GetCharSize() * line * GetLineBreakProportion()) - bitmapScale.y + bitmapInfo.yOffset * (float)GetCharSize();
		math::Vec2 posWorld = math::screenCoordToCenter( posScreen, viewportDimension );

		math::Mtx44 transform = math::matrixTranslate( posWorld.Xyz( pos.z ) );
		math::Mtx44 scale = math::matrixScale( bitmapScale );
		math::Mtx44 proj = math::matrixOrtho( viewportDimension );

		m_pEffect->Render( GetModel(), scale * transform, math::Mtx44(), proj );

		xPosScreen += (bitmapInfo.sizeProportion.x + GetCharSpaceProportion()) * GetCharSize();
	}
}
//-------------------------------------------------------------------
//	GetTextureTransform
//-------------------------------------------------------------------
math::Mtx33 FontEffect_Color::GetTextureTransform(const BitmapCharInfo* pBitmapInfo)const
{
	math::Vec2 scale = pBitmapInfo->texCoordMax - pBitmapInfo->texCoordMin;
	
	return math::matrixScale( scale ) * math::matrixTranslate( pBitmapInfo->texCoordMin );
}
//-------------------------------------------------------------------
//	GetColor
//-------------------------------------------------------------------
const math::Vec4& FontEffect_Color::GetColor()const
{
	return m_pEffect->GetConfig().color4;
}
