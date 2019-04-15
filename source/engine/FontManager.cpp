#include "FontManager.h"
#include "../engine/Exception.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
FontManager::FontManager() : m_tFonts(30, tableHashFunc_Uint), m_handleFonts(15)
{
	m_initialized = false;
}
//-------------------------------------------------------------------
FontManager::~FontManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void FontManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( auto it = m_tFonts.GetIterator(); it; it++ )
		(*it).Get().Shutdown();

	m_tFonts.Clear();
	m_handleFonts.Clear();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void FontManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_initialized = true;
}
//-------------------------------------------------------------------
//	LoadFont
//-------------------------------------------------------------------
Handle<Font> FontManager::LoadFont(const String& name)
{
	Handle<Font> result = GetFont( name );

	if ( result.IsValid() )
	{
		// Check for hash collision
		CHECK( result.Get().GetName().GetString() == HashString(name).GetString() );
		return result;
	}

	result = m_handleFonts.Create();
	result.Get().Initialize( name.AsChar() );

	m_tFonts.Add( result, result.Get().GetName().GetHash() );

	return result;
}
//-------------------------------------------------------------------
//	GetFont
//-------------------------------------------------------------------
Handle<Font> FontManager::GetFont(const HashString& name)
{
	Handle<Font> result = m_tFonts.Get( name.GetHash() );

	if ( result.IsValid() )
	{
		// Check for hash collision
		CHECK( result.Get().GetName().GetString() == HashString(name).GetString() );
	}

	return result;
}
