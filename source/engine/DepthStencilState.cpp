#include "DepthStencilState.h"
#include "../engine/Pointer.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"

//=============================================================================
//	DepthStencilState class
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
DepthStencilState::DepthStencilState()
{
	m_allowWrite = true;
	m_currentDepthOperation = eDepthOperation::Disabled;
	m_depthStencilApiArray = nullptr;
	m_apiCount = 0;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void DepthStencilState::OnInitialize()
{
	m_apiCount = platform::depthStencilState::createDepthStencilState( &m_depthStencilApiArray );
	SetDepthOperation( eDepthOperation::Less );
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void DepthStencilState::OnShutdown()
{
	for ( uint i = 0; i < m_apiCount; i++ )
		platform::release( m_depthStencilApiArray[i] );
}
//-------------------------------------------------------------------
//	EnableDepthWrite
//-------------------------------------------------------------------
void DepthStencilState::EnableDepthWrite(bool b)
{
	m_allowWrite = b;
	SetDepthOperation( m_currentDepthOperation );
}
//-------------------------------------------------------------------
//	SetDepthOperation
//-------------------------------------------------------------------
void DepthStencilState::SetDepthOperation(eDepthOperation depthOp)
{
	m_currentDepthOperation = depthOp;
	platform::depthStencilState::setOperation( depthOp, m_allowWrite, m_depthStencilApiArray, m_apiCount );
}
