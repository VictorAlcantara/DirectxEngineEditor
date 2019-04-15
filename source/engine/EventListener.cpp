#include "../engine/EventListener.h"
#include "../engine/CoreInterface.h"
#include "../engine/Event.h"

uint EventListener::s_idControl = 1;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EventListener::EventListener()
{
	m_id = s_idControl++;
	m_initialized = false;
}
//-------------------------------------------------------------------
EventListener::~EventListener()
{
	ShutdownListener();
}
//-------------------------------------------------------------------
//	InitializeListener
//-------------------------------------------------------------------
void EventListener::InitializeListener()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	g_pEvent->AddListener( this );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	ShutdownListener
//-------------------------------------------------------------------
void EventListener::ShutdownListener()
{
	g_pEvent->RemoveListener( GetId() );

	m_initialized = false;
}

