#pragma once

#include "../engine/Entity.h"
#include "AiStateMachine.h"
#include "AiView.h"
#include "../engine/Process.h"
#include "../engine/Component_Collision.h"
#include "../engine/Component_Physics.h"

class Process_TargetSelect;
struct Debug_Line;

/****************************************************************************************
	Entity_Monster
*****************************************************************************************/

class Entity_Monster : public Entity
{
	CLASS_TAG( Entity_Monster );

	public:
		Entity_Monster();
		virtual ~Entity_Monster();

		void ProcessCommand(const ScriptLine*)override final;

		void HeadTo(const math::Vec3&, float force);

		math::Vec3 GetHeading()const;

	protected:
		void Update()override final;
		void OnInitialize()override final;
		void OnShutdown()override final;

	private:
		void UpdateCollisionInfoFromRenderInfo();
		void StartAi();
		void StopAi();
		void Spawn();

		HandleAs<Component, Component_Collision> m_hCollision;
		HandleAs<Component, Component_Physics> m_hPhysics;

		SmartPtr<AiStateMachine> m_pStateMachine;
		SmartPtr<AiView_Monster> m_pView;
		SmartPtr<Process_TargetSelect> m_pTargetSelect;

		math::Vec3 m_desiredHeading;
		float m_headingVelocity;
		Handle<Debug_Line> m_hDebugHeading;

		bool m_spawned;
		bool m_active;
};

/****************************************************************************************
	Process_TargetSelect
*****************************************************************************************/

class Process_TargetSelect : public Process
{
	public:
		void SetView(SmartPtr<AiView>);
		void SetTargetClassTag(const HashString&);

	private:
		void Update()override final;

		SmartPtr<AiView> m_pView;
		HashString m_classTag;
};
