#pragma once

#include "Type.h"

class ProcessManager;

enum class eProcessState
{
	Idle,
	Running,
	Finished,
};

class Process
{
	friend class ProcessManager;

	public:
		Process();
		virtual ~Process();

		uint GetId()const { return m_id; }
		eProcessState GetState()const { return m_state; }

		void Exit();

	protected:
		virtual void OnEnter() {}
		virtual void Update() = 0;
		virtual void OnExit() {}


	private:
		void Enter();
		void Exit(bool addToRemoveListOnOwner);

		ProcessManager* m_pOwner;
		eProcessState m_state;
		uint m_id;
		static uint s_idControl;
};
