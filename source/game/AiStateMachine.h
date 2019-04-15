#pragma once

#include "../engine/Class.h"
#include "../engine/Pointer.h"

class AiView;
class AiState;

/****************************************************************************************
	AiStateMachine
*****************************************************************************************/

class AiStateMachine : public NonCopyable
{
	public:
		AiStateMachine() = delete;
		AiStateMachine(SmartPtr<AiView>);

		void ScheduleChangeState(SmartPtr<AiState>);

		void Update();

		AiView* GetView();
		const AiView* GetView()const;

		AiState* GetState();
		const AiState* GetState()const;

		template <class TYPE>
		TYPE* GetStateAs();
		template <class TYPE>
		const TYPE* GetStateAs()const;

	private:
		void ChangeState();

		SmartPtr<AiView> m_pView;
		SmartPtr<AiState> m_pState;	
		bool m_changeState;
		SmartPtr<AiState> m_pNewState;	
};

//=============================================================================
//	Template method implementation
//=============================================================================

//-------------------------------------------------------------------
//	GetStateAs
//-------------------------------------------------------------------
template <class TYPE>
TYPE* AiStateMachine::GetStateAs()
{
	if ( !m_pState )
		return nullptr;

	if ( TYPE::GetClassTagStatic() != m_pState->GetClassTag() )
		return nullptr;

	return (TYPE*)m_pState.Get();
}
//-------------------------------------------------------------------
template <class TYPE>
const TYPE* AiStateMachine::GetStateAs()const
{
	if ( !m_pState )
		return nullptr;

	if ( TYPE::GetClassTagStatic() != m_pState->GetClassTag() )
		return nullptr;

	return (TYPE*)m_pState.Get();
}
