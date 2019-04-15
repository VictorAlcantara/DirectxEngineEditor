#pragma once

#include "../engine/Pointer.h"

class IEditorState;

enum class eEditorState
{
	None,
	Idle,
	EntityCreate,
	PickEntity,
	PickAxis,
	EntityToolUse,
};

class EditorStateMachine
{
	public:
		void ChangeState(IEditorState*);
		void Update();
		const IEditorState* GetCurrentState()const;

	private:
		SmartPtr<IEditorState> m_pState;
		SmartPtr<IEditorState> m_pNewState;
};

class IEditorState
{
	public:
		virtual void SetOwner(EditorStateMachine*) = 0;
		virtual void Enter() = 0;
		virtual void Update() = 0;
		virtual void Exit() = 0;

		virtual bool CanChange() = 0;

		virtual eEditorState GetType()const = 0;
};

class EditorState_Base : public IEditorState
{
	public:
		EditorState_Base();
		virtual ~EditorState_Base() = default;

		void SetOwner(EditorStateMachine*)override;

		virtual void Enter()override {}
		virtual void Update()override {}
		virtual void Exit()override {}

		bool CanChange()override { return true; }

	protected:
		EditorStateMachine* m_pOwner;
};
