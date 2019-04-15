#include "../engine/Process.h"
#include "ProcessManager.h"

uint Process::s_idControl = 1;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process::Process()
{
	m_state = eProcessState::Idle;
	m_id = s_idControl++;
	m_pOwner = nullptr;
}
//-------------------------------------------------------------------
Process::~Process()
{
	CHECK( m_state != eProcessState::Running );
}
//-------------------------------------------------------------------
//	Enter
//-------------------------------------------------------------------
void Process::Enter()
{
	CHECK( m_state != eProcessState::Running );
	m_state = eProcessState::Running;
	OnEnter();
}
//-------------------------------------------------------------------
//	Exit
//-------------------------------------------------------------------
void Process::Exit()
{
	Exit( true );
}
//-------------------------------------------------------------------
void Process::Exit(bool addToRemoveList)
{
	CHECK( m_pOwner && m_state == eProcessState::Running );

	OnExit();
	m_state = eProcessState::Finished;

	if ( addToRemoveList )
		m_pOwner->Remove( GetId() );
}
