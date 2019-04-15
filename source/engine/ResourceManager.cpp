#include "ResourceManager.h"
#include "../engine/Path.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "FontManager.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ResourceManager::ResourceManager()
{
	m_initialized = false;
}
//-------------------------------------------------------------------
ResourceManager::~ResourceManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void ResourceManager::Initialize()
{
	CHECK( !m_initialized );

	if ( m_initialized )
		return;

	m_pModelMngr = new ModelManager;
	m_pModelMngr->Initialize();

	m_pTextureMngr = new TextureManager;
	m_pTextureMngr->Initialize();

	m_pFontMngr = new FontManager;
	m_pFontMngr->Initialize();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void ResourceManager::Shutdown()
{
	CHECK( m_initialized );

	if ( !m_initialized )
		return;

	m_pModelMngr->Shutdown();
	m_pModelMngr = nullptr;
	
	m_pTextureMngr->Shutdown();
	m_pTextureMngr = nullptr;

	m_pFontMngr->Shutdown();
	m_pFontMngr = nullptr;

	m_initialized = false;
}
//-------------------------------------------------------------------
//	GetModelMngr
//-------------------------------------------------------------------
ModelManager* ResourceManager::GetModelMngr()
{
	return m_pModelMngr.Get();
}
//-------------------------------------------------------------------
//	GetTextureMngr
//-------------------------------------------------------------------
TextureManager* ResourceManager::GetTextureMngr()
{
	return m_pTextureMngr.Get();
}
//-------------------------------------------------------------------
//	GetFontMngr
//-------------------------------------------------------------------
FontManager* ResourceManager::GetFontMngr()
{
	return m_pFontMngr.Get();
}