#pragma once

#include "CoreModule.h"
#include "../engine/Pointer.h"
#include "../engine/List.h"

struct Event;
class IEventListener;

class EventDispatcher
{
	CORE_MODULE;

	public:
		void AddListener(IEventListener*);
		void RemoveListener(uint);

		void Process(SmartPtr<Event>);
		template <class EVENT_TYPE>
		void Process(SmartPtr<EVENT_TYPE>);

	private:
		List<IEventListener*> m_listeners;
};

template<class EVENT_TYPE>
inline void EventDispatcher::Process(SmartPtr<EVENT_TYPE> pEvent)
{
	Process( pEvent.As<Event>() );
}
