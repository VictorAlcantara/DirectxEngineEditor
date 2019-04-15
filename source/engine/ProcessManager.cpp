#include "ProcessManager.h"
#include "../engine/TableHashFunction.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
ProcessManager::ProcessManager() : m_tableProcess(500, tableHashFunc_Uint)
{
	m_initialized = false;
}
//-------------------------------------------------------------------
ProcessManager::~ProcessManager()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void ProcessManager::Clear()
{
	for ( auto it = m_tableProcess.GetIterator(); it; it++ )
	{
		if ( (*it)->GetState() == eProcessState::Running )
			(*it)->Exit(false);
	}

	m_tableProcess.Clear();
	m_listRemove.Clear();
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void ProcessManager::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
	}
	else
	{
		Clear();
		m_initialized = false;
	}
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void ProcessManager::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
	}
	else
	{
		m_initialized = true;
	}
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void ProcessManager::Update()
{
	for ( auto it = m_listRemove.GetIterator(); it; it++ )
	{
		if ( (*it)->GetState() == eProcessState::Finished )
			m_tableProcess.Remove( (*it)->GetId() );
	}

	m_listRemove.Clear();

	for ( auto it = m_tableProcess.GetIterator(); it; it++ )
	{
		CHECK( (*it)->GetState() == eProcessState::Running );
		(*it)->Update();
	}

	// Nothing necessarily wrong with > 1000 processes, but weird for what I aim
	// therefore the check
	CHECK( m_tableProcess.Count() < 1000 );
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
void ProcessManager::Add(SmartPtr<Process> pProcess)
{
	SmartPtr<Process> pFind = m_tableProcess.Get( pProcess->GetId() );

	if ( pFind && pFind->GetState() != eProcessState::Running )
		pProcess->Enter();
	else if ( !pFind )
	{
		m_tableProcess.Add( pProcess, pProcess->GetId() );
		pProcess->Enter();
		pProcess->m_pOwner = this;
	}
	else
		ASSERT( "Process already added and running" );
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
void ProcessManager::Remove(uint processId)
{
	SmartPtr<Process> pFind = m_tableProcess.Get( processId );
	if ( pFind )
	{
		if ( pFind->GetState() == eProcessState::Running )
			pFind->Exit( false );
		m_listRemove.AddLast( pFind );
	}
}
