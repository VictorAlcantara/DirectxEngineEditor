#include "BlendState.h"
#include "../engine/Pointer.h"
#include "../engine/CoreInterface.h"
#include "../engine/Exception.h"
#include "Api_BlendState.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
BlendState::BlendState()
{
	m_initialized = false;
}
//-------------------------------------------------------------------
BlendState::~BlendState()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void BlendState::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	for ( uint i = 0; i < (uint)eBlendMode::Count; i++ )
	{
		platform::blendState::createBlendMode( (eBlendMode)i, m_api[i] );
	}

	SetMode( eBlendMode::Disabled );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void BlendState::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized to shutdown" );
		return;
	}

	for ( uint i = 0; i < (uint)eBlendMode::Count; i++ )
		platform::release( m_api[i] );

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Activate
//-------------------------------------------------------------------
void BlendState::SetMode(eBlendMode mode)
{
	CHECK( (uint)mode < (uint)eBlendMode::Count );
	platform::blendState::setBlendMode( m_api[(uint)mode] );
}
