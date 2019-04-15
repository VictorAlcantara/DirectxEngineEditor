#pragma once

#include "EditorState.h"
#include "../engine/Handle.h"
#include "../engine/Entity.h"

class EditorState_Idle final : public EditorState_Base
{
	public:
		virtual ~EditorState_Idle() = default;

		void Enter()override;
		void Update()override;

		eEditorState GetType()const override { return eEditorState::Idle; }

	private:
		void SelectEntity(Handle<Entity>);
		void UnselectEntity();
};
