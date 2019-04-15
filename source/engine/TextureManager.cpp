#include "TextureManager.h"
#include "../engine/TableHashFunction.h"
#include "../engine/api_File.h"
#include "../engine/TgaLoader.h"
#include "../engine/Path.h"
#include "../engine/Primitive.h"
#include "EventEngine.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
TextureManager::TextureManager() : m_handles(1000), m_table(2000, tableHashFunc_Uint)
{
	m_directoryPath = path::DIRECTORY_TEXTURE;
	m_initialized = false;
}
//-------------------------------------------------------------------
TextureManager::~TextureManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void TextureManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	for ( auto it = m_table.GetIterator(); it; it++ )
		(*it).Get().Shutdown();

	m_table.Clear();
	m_handles.Clear();

	ShutdownListener();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void TextureManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	InitializeListener();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	LoadDirectory
//-------------------------------------------------------------------
void TextureManager::LoadDirectory(const String& directoryPath)
{
	List<String> files;

	if ( !platform::file::readAllFilesInDirectory( directoryPath.AsChar(), files ) )
	{
		ASSERT( "Error opening texture directory" );
		return;
	}

	m_directoryPath = directoryPath;

	if ( m_directoryPath.Last() != '/' && m_directoryPath.Last() != '\\' )
		m_directoryPath += '\\';

	for ( auto it = files.GetIterator(); it; it++ )
	{
		if ( string::extractFileExtension(*it, true) == path::EXTENSION_TEXTURE )
			CreateFromFile( m_directoryPath + (*it) );
	}
}
//-------------------------------------------------------------------
//	CreateFromFile
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::CreateFromFile(const String& filePath, bool logDuplicate)
{
	HashString textureHash = string::extractFileName(filePath, false);
	Handle<ITexture> hTexture = Get( textureHash ); 

	if ( hTexture.IsValid() )
	{
		// Check for hash collision
		const HashString& hTextureName = hTexture->GetName();
		CHECK( hTextureName.GetString() == textureHash.GetString() );
		#ifdef DEBUG_MODE
			if ( logDuplicate )
				ASSERT( "File already loaded" );
		#endif
		return hTexture;
	}

	TgaLoader tgaLoader;
	if ( !tgaLoader.Load( filePath ) )
	{
		ASSERT( "Failed loading texture file" );
		return hTexture;
	}

	hTexture = m_handles.CreateAs<Texture2d>();

	hTexture.GetAs<Texture2d>().Initialize( tgaLoader.GetInfo() );
	hTexture.GetAs<Texture2d>().SetName( textureHash );

	m_table.Add( hTexture, textureHash.GetHash() );

	return hTexture;
}
//-------------------------------------------------------------------
//	CreateEmpty
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::CreateEmpty(const String& name, uint width, uint height, eColorFormat format, bool logDuplicate)
{
	HashString textureHash = name;
	Handle<ITexture> hTexture = Get( textureHash ); 

	if ( hTexture.IsValid() )
	{
		// Check for hash collision
		CHECK( hTexture.Get().GetName().GetString() == textureHash.GetString() );
		if ( logDuplicate )
		{
			ASSERT( "Name already exists" );
		}
		return hTexture;
	}

	hTexture = m_handles.CreateAs<Texture2d>();

	hTexture.GetAs<Texture2d>().InitializeEmpty( width, height, format );
	hTexture.GetAs<Texture2d>().SetName( textureHash );

	m_table.Add( hTexture, textureHash.GetHash() );

	return hTexture;
}
//-------------------------------------------------------------------
//	CreateFromStream
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::CreateFromStream(const String& name, const uchar* pData, uint width, uint height, uint bitsPerPixel, bool logDuplicate)
{
	HashString textureHash = name;
	Handle<ITexture> hTexture = Get( textureHash ); 

	if ( hTexture.IsValid() )
	{
		// Check for hash collision
		CHECK( hTexture.Get().GetName().GetString() == textureHash.GetString() );

		#ifdef DEBUG_MODE
			if ( logDuplicate )
				ASSERT( "Name already exists" );
		#endif

		return hTexture;
	}

	hTexture = m_handles.CreateAs<Texture2d>();

	hTexture.GetAs<Texture2d>().Initialize( pData, width, height, bitsPerPixel );
	hTexture.GetAs<Texture2d>().SetName( textureHash );

	m_table.Add( hTexture, textureHash.GetHash() );

	return hTexture;
}
//-------------------------------------------------------------------
//	CreateRenderTargetTexture
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::CreateRenderTargetTexture(const String& name, uint width, uint height, eColorFormat format)
{
	HashString textureHash = name;
	Handle<ITexture> hTexture = Get( textureHash ); 

	CHECK( width > 0 && height > 0 );

	if ( hTexture.IsValid() )
	{
		// Check for hash collision
		CHECK( hTexture.Get().GetName().GetString() == textureHash.GetString() );
		ASSERT( "Name already exists" );
		return hTexture;
	}

	hTexture = m_handles.CreateAs<TextureRenderTarget>();

	hTexture.GetAs<TextureRenderTarget>().Initialize( width, height, format );
	hTexture.GetAs<TextureRenderTarget>().SetName( textureHash );

	m_table.Add( hTexture, textureHash.GetHash() );

	return hTexture;
}
//-------------------------------------------------------------------
//	CreateDepthStencilTexture
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::CreateDepthStencilTexture(const String& name, uint width, uint height)
{
	HashString textureHash = name;
	Handle<ITexture> hTexture = Get( textureHash ); 

	if ( hTexture.IsValid() )
	{
		// Check for hash collision
		CHECK( hTexture.Get().GetName().GetString() == textureHash.GetString() );
		ASSERT( "Name already exists" );
		return hTexture;
	}

	hTexture = m_handles.CreateAs<TextureDepthStencil>();

	hTexture.GetAs<TextureDepthStencil>().Initialize( width, height );
	hTexture.GetAs<TextureDepthStencil>().SetName( textureHash );

	m_table.Add( hTexture, textureHash.GetHash() );

	return hTexture;
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void TextureManager::Remove(Handle<ITexture> hTexture)
{
	if ( hTexture.IsValid() )
	{
		hTexture.Get().Shutdown();
		m_table.Remove( hTexture.Get().GetName().GetHash() );
		m_handles.Remove( hTexture );
	}
}
//-------------------------------------------------------------------
void TextureManager::Remove(const HashString& name)
{
	Handle<ITexture> hTexture = Get( name );

	Remove( hTexture );
}
//-------------------------------------------------------------------
//	GetAll
//-------------------------------------------------------------------
TableIteratorConst<Handle<ITexture>> TextureManager::GetAll()const
{
	return m_table.GetIterator();
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::Get(const HashString& name)
{
	CHECK( name.IsValid() );
	return Get( name.GetHash() );
}
//-------------------------------------------------------------------
Handle<ITexture> TextureManager::Get(uint textureHash)
{
	Handle<ITexture>* pResult = m_table.Find( textureHash );

	if ( pResult )
		return *pResult;

	return Handle<ITexture>();
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void TextureManager::HandleEvent(Event* pEvent)
{
	Event_Core* pEventCore = nullptr;

	if ( isEvent<Event_Core>(pEvent, &pEventCore) )
	{
		if ( pEventCore->action == eCore::InitializationEnd )
		{
			for ( uint i = 0; i < ARRAY_COUNT(primitive::texture::createFunctionPtrs); i++ )
				primitive::texture::createFunctionPtrs[i]();
		}
	}
}
