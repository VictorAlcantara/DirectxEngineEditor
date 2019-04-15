#pragma once

#include "Type.h"

struct Event;

class IEventListener
{
	public:
		virtual uint GetId()const = 0;
		virtual void HandleEvent(Event*) = 0;

	protected:
		virtual void InitializeListener() = 0;
		virtual void ShutdownListener() = 0;
};

class EventListener : public IEventListener
{
	public:
		EventListener();
		virtual ~EventListener();

		uint GetId()const override final { return m_id; }

	protected:
		void InitializeListener()override final;
		void ShutdownListener()override final;

		static uint s_idControl;
		uint m_id;
		bool m_initialized;
};


