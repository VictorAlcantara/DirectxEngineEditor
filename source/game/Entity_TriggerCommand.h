#pragma once

#include "../engine/Entity.h"
#include "../engine/Process.h"
#include "../engine/CollisionInfo.h"
#include "GameTrigger.h"
#include "../engine/ScriptReader.h"
#include "../engine/Component_Trigger.h"
#include "../engine/Component_Physics.h"

class Process_CameraTrackFind;
class Entity_CameraTrack;
struct Debug_Cube;

/****************************************************************************************
	Component_Trigger_Command
*****************************************************************************************/

class Component_Trigger_Command : public Component_Trigger
{
	CLASS_TAG( Component_Trigger_Command );

	public:
		Component_Trigger_Command();
		virtual ~Component_Trigger_Command() = default;

		bool Ignore(const Handle<Entity>&)const override final;

		void OnTrigger_Update(Handle<Entity>&)override final;
};

/****************************************************************************************
	Entity_TriggerCommand
*****************************************************************************************/

class Entity_TriggerCommand : public Entity
{
	CLASS_TAG( Entity_TriggerCommand );

	public:
		Entity_TriggerCommand();
		virtual ~Entity_TriggerCommand() = default;

		void ProcessCommand(const ScriptLine*)override final;

		const List<String>& GetCommands()const;

		bool IsFireOnce()const { return m_fireOnce; }

	protected:
		void OnInitialize()override final;

	private:
		math::Vec3 m_pos;
		math::Vec3 m_scale;
		HandleAs<Component, Component_Trigger_Command> m_hCollision;
		HandleAs<Component, Component_Physics> m_hPhysics;
		bool m_fireOnce;
		bool m_active;
	
		List<String> m_cmds;
};
