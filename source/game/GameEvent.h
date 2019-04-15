#pragma once

#include "../engine/Event.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "GameGlobal.h"

class Entity;

struct Event_PlayerUse : public Event
{
	EVENT_DECL( Event_PlayerUse );

	Handle<Entity> hEntity;
	math::Vec3 pos;
	math::Vec3 dir;
	math::Vec3 aabbScale;
};

struct Event_TriggerCamera : public Event
{
	EVENT_DECL( Event_TriggerCamera );

	Handle<Entity> hTrigger;
	Handle<Entity> hEntity;
};

struct Event_ChangeCameraMode : public Event
{
	EVENT_DECL( Event_ChangeCameraMode );

	eCameraMode mode;

	Event_ChangeCameraMode() : mode(eCameraMode::Back) {}
};
