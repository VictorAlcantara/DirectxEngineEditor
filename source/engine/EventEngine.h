#pragma once

#include "../engine/Event.h"
#include "../engine/Handle.h"
#include "../engine/CollisionInfo.h"
#include "../engine/EngineGlobal.h"

class Entity;
class Component;

enum class eCore
{
	InitializationEnd,
};

struct Event_TransformChange : public Event
{
	EVENT_DECL( Event_TransformChange );
	
	eTransformation flag;
	Handle<Entity> hEntity;

	Event_TransformChange();
};

struct Event_Core : public Event
{
	EVENT_DECL( Event_Core );

	eCore action;

	Event_Core() : action(eCore::InitializationEnd) {}
};

struct Event_ComponentPropertyChange : public Event
{
	EVENT_DECL( Event_ComponentPropertyChange );

	Handle<Component> hComponent;
};

struct Event_LightTransform : public Event
{
	EVENT_DECL( Event_LightTransform );
	
	uint lightId = 0;
};
