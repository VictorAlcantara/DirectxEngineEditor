#pragma once

#include "../engine/Event.h"
#include "../engine/Math.h"
#include "EditorTool.h"
#include "GuiUtil.h"
#include "EditorState.h"
#include "EditorEntityUtil.h"
#include "../engine/EngineGlobal.h"

class EditorEntity;

/****************************************************************************************
	Event struct
*****************************************************************************************/

struct Event_EntitySelect : public Event
{
	EVENT_DECL( Event_EntitySelect );

	Handle<Entity> hEntity;

	Event_EntitySelect() {}
};

struct Event_EntityToolUse : public Event
{
	EVENT_DECL( Event_EntityToolUse );

	Handle<Entity> hEntity;
	math::eAxis axisEnum;
	math::eSign axisSign;
	eEditorToolAction entityToolAction;

	Event_EntityToolUse() : axisEnum(math::eAxis::X), axisSign(math::eSign::POSITIVE), entityToolAction(eEditorToolAction::None) {}
};

struct Event_EntityToolEndUse : public Event
{
	EVENT_DECL( Event_EntityToolEndUse );

	Handle<Entity> hEntity;

	Event_EntityToolEndUse() = default;
};

struct Event_EntityUnselect : public Event
{
	EVENT_DECL( Event_EntityUnselect );
};

struct Event_GuiUse : public Event
{
	EVENT_DECL( Event_GuiUse );

	eEditorEntityType entityType;
	eGuiAction action;
	String command;
	uint flag;
	uint chainCurrent;
	uint chainCount;
	
	Event_GuiUse() : entityType(eEditorEntityType::Unknown), flag(0), action(eGuiAction::None),chainCurrent(0), chainCount(0) {}
};

struct Event_ButtonUse : public Event
{
	EVENT_DECL( Event_ButtonUse );
	eEditorEntityType entityType;
	eGuiAction action;
	uint flag;

	Event_ButtonUse() : entityType(eEditorEntityType::None), action(eGuiAction::None), flag(0) {}
};

struct Event_StateChange : public Event
{
	EVENT_DECL( Event_StateChange );

	eEditorState currentState;
	eEditorState nextState;

	Event_StateChange() : currentState(eEditorState::None), nextState(eEditorState::None) {}
};

struct Event_ChangeTexture : public Event
{
	EVENT_DECL( Event_ChangeTexture );

	eGuiTexture textureType;
	uint textureId;

	Event_ChangeTexture() : textureId(0), textureType(eGuiTexture::DiffuseMap) {}
};

struct Event_ChangeModel : public Event
{
	EVENT_DECL( Event_ChangeModel );

	uint modelId;

	Event_ChangeModel() : modelId(0) {}
};

struct Event_EditorToolUse : public Event
{
	EVENT_DECL( Event_EditorToolUse );

	eEditorToolAction action;
	math::eAxis axisEnum;
	math::eSign axisSign;

	Event_EditorToolUse() : action(eEditorToolAction::None), axisEnum(math::eAxis::X), axisSign(math::POSITIVE) {}
};

/****************************************************************************************
	Send event
*****************************************************************************************/

void sendEvent_EntitySelect(Handle<Entity>);
void sendEvent_EntityUnselect();