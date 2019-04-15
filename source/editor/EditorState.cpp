#include "EditorState.h"
#include "EditorEvent.h"
#include "../engine/CoreInterface.h"
#include "../engine/Exception.h"

/****************************************************************************************
	EditorStateMachine
*****************************************************************************************/

//-------------------------------------------------------------------
//	ChangeState
//-------------------------------------------------------------------
void EditorStateMachine::ChangeState(IEditorState* pNewState)
{
	CHECK( !m_pNewState );

	if ( pNewState )
		pNewState->SetOwner( this );

	if ( m_pState )
	{
		m_pState->Exit();
		m_pNewState = pNewState;
	}
	else
	{
		m_pState = pNewState;
	}

	SmartPtr<Event_StateChange> pEvent = new Event_StateChange;
	pEvent->currentState = m_pState ? m_pState->GetType() : eEditorState::None;
	pEvent->nextState = m_pNewState ? m_pNewState->GetType() : eEditorState::None;

	g_pEvent->Process( pEvent );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorStateMachine::Update()
{
	if ( m_pState )
	{
		m_pState->Update();

		if ( m_pNewState && m_pState->CanChange() )
		{
			m_pState = m_pNewState;
			m_pNewState = nullptr;

			m_pState->Enter();
		}
	}
}
//-------------------------------------------------------------------
//	GetCurrentState
//-------------------------------------------------------------------
const IEditorState* EditorStateMachine::GetCurrentState()const
{
	return m_pState.Get();
}

/****************************************************************************************
	EditorState_Base
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorState_Base::EditorState_Base()
{
	m_pOwner = nullptr;
}
//-------------------------------------------------------------------
//	SetOwner
//-------------------------------------------------------------------
void EditorState_Base::SetOwner(EditorStateMachine* pOwner)
{
	m_pOwner = pOwner;
}
