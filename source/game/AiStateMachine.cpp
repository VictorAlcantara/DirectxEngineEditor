#include "AiStateMachine.h"
#include "AiState.h"
#include "AiView.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
AiStateMachine::AiStateMachine(SmartPtr<AiView> pView)
{
	m_pView = pView;
	m_changeState = false;
}
//-------------------------------------------------------------------
//	ScheduleChangeState
//-------------------------------------------------------------------
void AiStateMachine::ScheduleChangeState(SmartPtr<AiState> pState)
{
	m_pNewState = pState;
	m_changeState = true;
}
//-------------------------------------------------------------------
//	ChangeState
//-------------------------------------------------------------------
void AiStateMachine::ChangeState()
{
	if ( m_pState )
		m_pState->OnExit();

	m_pState = m_pNewState;
	m_pNewState = nullptr;

	if ( m_pState )
	{
		m_pState->m_pOwner = this;
		m_pView->OnStateChange( m_pState.Get() );
		m_pState->OnEnter();
	}

	m_changeState = false;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void AiStateMachine::Update()
{
	if ( m_changeState )
		ChangeState();

	if ( m_pState )
		m_pState->Update();
}
//-------------------------------------------------------------------
//	GetView
//-------------------------------------------------------------------
AiView* AiStateMachine::GetView()
{
	return m_pView.Get();
}
//-------------------------------------------------------------------
const AiView* AiStateMachine::GetView()const
{
	return m_pView.Get();
}
//-------------------------------------------------------------------
//	GetState
//-------------------------------------------------------------------
AiState* AiStateMachine::GetState()
{
	return m_pState.Get();
}
//-------------------------------------------------------------------
const AiState* AiStateMachine::GetState()const
{
	return m_pState.Get();
}
