#include "../engine/EntityFinder.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EntityFinder::EntityFinder()
{
	m_initialized = false;
}
//-------------------------------------------------------------------
EntityFinder::~EntityFinder()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EntityFinder::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
	}
	else
	{
		OnShutdown();
		m_initialized = false;
	}
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EntityFinder::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
	}
	else
	{
		OnInitialize();
		m_initialized = true;
	}
}
