#include "EventDispatcher.h"
#include "../engine/Debug.h"
#include "../engine/EventListener.h"
#include "../engine/Exception.h"

//-------------------------------------------------------------------
//	AddListener
//-------------------------------------------------------------------
void EventDispatcher::AddListener(IEventListener* pListener)
{
	if ( !pListener )
		return;

	#ifdef DEBUG_EVENT_DISPATCHER_REDUNDANCY_CHECK
		for ( auto it = m_listeners.GetIterator(); it; it++ )
		{
			if ( (*it)->GetId() == pListener->GetId() )
			{
				ASSERT( "Event dispatcher redundancy checker FAILED" );
			}
		}
	#endif

	m_listeners.AddLast( pListener );
}
//-------------------------------------------------------------------
//	RemoveListener
//-------------------------------------------------------------------
void EventDispatcher::RemoveListener(uint idListener)
{
	for ( auto pNode = m_listeners.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue()->GetId() == idListener )
		{
			pNode->Remove();
			return;
		}
	}
}
//-------------------------------------------------------------------
//	Process
//-------------------------------------------------------------------
void EventDispatcher::Process(SmartPtr<Event> pEvent)
{
	for ( auto it = m_listeners.GetIterator(); it; it++ )
		(*it)->HandleEvent( pEvent.Get() );
}
