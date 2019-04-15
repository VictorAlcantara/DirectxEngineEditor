#pragma once

#include "../engine/String.h"

#define EVENT_DECL(className) \
	static const HashString& GetHashStatic() { static HashString hash(#className); return hash; } \
	const HashString& GetHash()const override { return className::GetHashStatic(); } \
	virtual ~className() = default;

struct Event
{
	virtual const HashString& GetHash()const = 0;
};

template <class EVENT_TYPE>
bool isEvent(Event* pEvent, EVENT_TYPE** pEventCast = nullptr)
{
	if ( pEvent->GetHash() == EVENT_TYPE::GetHashStatic() )
	{
		if ( pEventCast )
			*pEventCast = (EVENT_TYPE*)pEvent;
		return true;
	}

	return false;
}
