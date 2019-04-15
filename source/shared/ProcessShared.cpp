#include "../shared/ProcessShared.h"
#include "../engine/CoreInterface.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Process_ChangeKeyMapContext::Process_ChangeKeyMapContext()
{
	m_enable = true;
}
//-------------------------------------------------------------------
//	SetContext
//-------------------------------------------------------------------
void Process_ChangeKeyMapContext::SetContext(const HashString& contextName, bool enable)
{
	m_contextName = contextName;
	m_enable = enable;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Process_ChangeKeyMapContext::Update()
{
	if ( m_enable )
		g_pKeyMap->UseContext( m_contextName );
	else
		g_pKeyMap->EndUseContext( m_contextName );

	Exit();
}
