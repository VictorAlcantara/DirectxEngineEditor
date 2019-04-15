#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"
#include "../engine/Event.h"

class Entity;
class Component;

enum class eComponentAction
{
	None,
	Add,
	Remove,
};

struct Event_Component : public Event
{
	EVENT_DECL( Event_Component );
	
	eComponentAction action;
	Handle<Component> hComponent;

	Event_Component() : action(eComponentAction::None) {}
};

class Component : public NonCopyable, public ClassStructure
{
	friend class Entity;
	CLASS_TAG( Component );

	public:
		virtual ~Component();

		uint GetId()const { return m_id; }
		Handle<Entity> GetOwner();
		const Handle<Entity> GetOwner()const;
		Handle<Component> GetHandle();

	protected:
		Component();

		virtual void OnInitialize() {}
		virtual void OnShutdown() {}
		virtual void Update() {}

		void UpdateEveryFrame(bool);

	private:
		void Initialize(Handle<Entity>& hOwner, Handle<Component>& hSelf);
		void Shutdown();

		static uint s_idControl;

		bool m_initialized;
		uint m_id;
		Handle<Entity> m_hOwner;
		Handle<Component> m_hSelf;
};
