#pragma once

#include "EditorState.h"

class EditorState_EntityToolUse final : public EditorState_Base
{
	public:
		virtual ~EditorState_EntityToolUse() = default;

		void Enter()override;
		void Update()override;

		eEditorState GetType()const override final { return eEditorState::EntityToolUse; }
};


