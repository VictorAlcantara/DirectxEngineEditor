#pragma once

#include "../engine/Class.h"
#include "../engine/Handle.h"
#include "../engine/String.h"

class AiStateMachine;
class Entity;
struct Debug_Line;

/****************************************************************************************
	AiState
*****************************************************************************************/

class AiState : public NonCopyable, public ClassStructure
{
	friend class AiStateMachine;

	CLASS_TAG( AiState );

	public:
		AiState();
		virtual ~AiState() = default;

	protected:
		virtual void OnEnter() {}
		virtual void OnExit() {}
		virtual void Update() = 0;
	
		AiStateMachine* m_pOwner;
};

/****************************************************************************************
	AiState_Rest
*****************************************************************************************/

class AiState_Rest : public AiState
{
	CLASS_TAG( AiState_Rest );

	public:
		AiState_Rest();
		virtual ~AiState_Rest() = default;

		void SetDuration(float f) { m_duration = f; }

	protected:
		void OnEnter()override final;
		void OnExit()override final;
		void Update()override final;

	private:
		float m_time;
		float m_duration;
};

/****************************************************************************************
	AiState_Chase
*****************************************************************************************/

class AiState_Chase : public AiState
{
	CLASS_TAG( AiState_Chase );

	public:
		AiState_Chase();
		virtual ~AiState_Chase() = default;

		void SetDuration(float f) { m_duration = f; }

	protected:
		void OnEnter()override final;
		void OnExit()override final;
		void Update()override final;

	private:
		float m_duration;
		float m_time;
};

/****************************************************************************************
	AiState_Slash
*****************************************************************************************/

class AiState_Slash : public AiState
{
	CLASS_TAG( AiState_Slash );

	public:
		AiState_Slash();
		virtual ~AiState_Slash() = default;

	protected:
		void OnEnter()override final;
		void OnExit()override final;
		void Update()override final;

	private:
		Handle<Debug_Line> m_hLine;
		float m_duration;
		float m_time;
};

/****************************************************************************************
	AiState_ProjectileThrow
*****************************************************************************************/

class AiState_ProjectileThrow : public AiState
{
	CLASS_TAG( AiState_ProjectileThrow );

	public:
		AiState_ProjectileThrow();
		virtual ~AiState_ProjectileThrow() = default;

	protected:
		void Update()override final;

	private:
		bool m_hasThrown;
};
