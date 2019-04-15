#pragma once

#include "EditorState.h"
#include "EditorEntityUtil.h"
#include "../engine/EventListener.h"
#include "../engine/Math.h"

class EditorState_PickAxis : public EditorState_Base, public EventListener
{
	public:
		EditorState_PickAxis();
		virtual ~EditorState_PickAxis() = default;

		void Enter()override final;
		void Update()override final;
		void Exit()override final;

		void HandleEvent(Event*)override final;

		eEditorState GetType()const override final { return eEditorState::PickAxis; }

	private:
		math::Vec3 m_openDirection;
};


