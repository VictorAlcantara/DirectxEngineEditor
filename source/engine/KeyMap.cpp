#include "KeyMap.h"
#include "../engine/TableHashFunction.h"
#include "../engine/CoreInterface.h"

/****************************************************************************************
	class KeyMapObject
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
KeyMapObject::KeyMapObject()
{
	m_nodes = new List<KeyMapNode>();
}
//-------------------------------------------------------------------
KeyMapObject::KeyMapObject(eKey key, uint keyStateFlag) : KeyMapObject()
{
	Link( key, keyStateFlag );
}
//-------------------------------------------------------------------
//	Link
//-------------------------------------------------------------------
KeyMapObject* KeyMapObject::Link(eKey key, uint keyStateFlag)
{
	CHECK( !HasKey(key) );

	m_nodes->AddLast( KeyMapNode(key, keyStateFlag) );

	#ifdef DEBUG_MODE
		for ( auto pNode = m_nodes->GetLast()->GetPrev(); pNode; pNode = pNode->GetPrev() )
		{
			// Having a key press on a link is probably a very bad idea (except for the last key on the link)
			CHECK( pNode->GetValue().stateKeyFlag != (uint)eKeyState::Press );
		}
	#endif

	return this;
}
//-------------------------------------------------------------------
//	CanPerform
//-------------------------------------------------------------------
bool KeyMapObject::CanPerform()const
{
	for ( auto it = m_nodes->GetIterator(); it; it++ )
	{
		if ( !((uint)g_pInput->GetKeyState( (*it).key ) & (*it).stateKeyFlag ) )
			return false;
	}

	return true;
}
//-------------------------------------------------------------------
//	HasKey
//-------------------------------------------------------------------
bool KeyMapObject::HasKey(eKey key)const
{
	for ( auto it = m_nodes->GetIterator(); it; it++ )
	{
		if ( (*it).key == key )
			return true;
	}

	return false;
}

/****************************************************************************************
	class KeyMapGroup
*****************************************************************************************/

//-------------------------------------------------------------------
//	AddKeyMapObject
//-------------------------------------------------------------------
KeyMapObject* KeyMapGroup::AddKeyMapObject(eKey key, uint keyStateFlag)
{
	CHECK( keyStateFlag > 0 );

	auto pNode = m_objects.GetFirst();

	for ( ; pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue().HasKey(key) )
		{
			ASSERT( "Key already registered" );
			return &pNode->GetValue();
		}
	}

	m_objects.AddLast( KeyMapObject(key, keyStateFlag) );

	return &m_objects.GetLast()->GetValue();
}
//-------------------------------------------------------------------
//	CanPerform
//-------------------------------------------------------------------
bool KeyMapGroup::CanPerform()const
{
	for ( auto it = m_objects.GetIterator(); it; it++ )
	{
		if ( (*it).CanPerform() )
			return true;
	}

	return false;
}

/****************************************************************************************
	class KeyMapContext
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
KeyMapContext::KeyMapContext(const HashString& name) : m_tableGroup(50, tableHashFunc_HashString)
{
	m_enabled = false;
	m_name = name;
	m_useCounter = 0;
}
//-------------------------------------------------------------------
//	AddAction
//-------------------------------------------------------------------
KeyMapObject* KeyMapContext::AddAction(const HashString& action, eKey key, uint keyStateFlag)
{
	CHECK( action.IsValid() && keyStateFlag > 0 );

	KeyMapGroup* pGroup = GetGroup( action );

	if ( !pGroup )
	{
		SmartPtr<KeyMapGroup> pNewGroup = new KeyMapGroup;
		m_tableGroup.Add( pNewGroup, action );
		pGroup = pNewGroup.Get();
	}

	return pGroup->AddKeyMapObject( key, keyStateFlag );
}
//-------------------------------------------------------------------
//	RemoveAction
//-------------------------------------------------------------------
void KeyMapContext::RemoveAction(const HashString& action)
{
	m_tableGroup.Remove( action );
}
//-------------------------------------------------------------------
//	CanPerform
//-------------------------------------------------------------------
bool KeyMapContext::CanPerform(const HashString& action)const
{
	auto pGroup = GetGroup(action);

	if ( pGroup )
		return pGroup->CanPerform();

	return false;
}
//-------------------------------------------------------------------
//	GetGroup
//-------------------------------------------------------------------
KeyMapGroup* KeyMapContext::GetGroup(const HashString& action)
{
	return m_tableGroup.Get(action).Get();
}
//-------------------------------------------------------------------
const KeyMapGroup* KeyMapContext::GetGroup(const HashString& action)const
{
	return m_tableGroup.Get(action).Get();
}

/****************************************************************************************
	class KeyMap
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
KeyMap::KeyMap() : m_tableContext(20, tableHashFunc_HashString)
{
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void KeyMap::Clear()
{
	m_tableContext.Clear();
	m_queue.Clear();
}
//-------------------------------------------------------------------
//	CanPerform
//-------------------------------------------------------------------
bool KeyMap::CanPerform(const HashString& action)const
{
	for ( auto it = m_tableContext.GetIterator(); it; it++ )
	{
		const KeyMapContext* pContext = (*it).Get();
		
		if ( pContext->IsEnabled() && pContext->CanPerform(action) )
			return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	CreateContext
//-------------------------------------------------------------------
KeyMapContext* KeyMap::CreateContext(const HashString& name)
{
	KeyMapContext* pResult = GetContext( name );

	CHECK( !pResult );

	if ( !pResult )
	{
		CHECK( name.IsValid() );

		SmartPtr<KeyMapContext> pNewContext = new KeyMapContext( name );
		m_tableContext.Add( pNewContext, name );
		pResult = pNewContext.Get();
	}

	return pResult;
}
//-------------------------------------------------------------------
//	RemoveContext
//-------------------------------------------------------------------
void KeyMap::RemoveContext(const HashString& name)
{
	CHECK( GetContext(name) );
	m_tableContext.Remove( name );
}
//-------------------------------------------------------------------
//	UseContext
//-------------------------------------------------------------------
KeyMapContext* KeyMap::UseContext(const HashString& name)
{
	KeyMapContext* pContext = GetContext(name);

	CHECK( pContext );

	if ( pContext )
	{
		//CHECK( !pContext->IsEnabled() );
		pContext->Enable();
		pContext->m_useCounter++;
	}

	return pContext;
}
//-------------------------------------------------------------------
//	EndUseContext
//-------------------------------------------------------------------
void KeyMap::EndUseContext(const HashString& name)
{
	KeyMapContext* pContext = GetContext(name);

	CHECK( pContext );

	if ( pContext )
		pContext->Disable();
}
//-------------------------------------------------------------------
//	GetContext
//-------------------------------------------------------------------
KeyMapContext* KeyMap::GetContext(const HashString& name)
{
	return m_tableContext.Get(name).Get();
}
//-------------------------------------------------------------------
//	RemoveFromQueue
//-------------------------------------------------------------------
void KeyMap::RemoveFromQueue(const HashString& name)
{
	for ( auto pNode = m_queue.GetFirst(); pNode; )
	{
		auto pNext = pNode->GetNext();

		if ( pNode->GetValue()->GetName() == name )
			pNode->Remove();

		pNode = pNext;
	}
}
//-------------------------------------------------------------------
//	QueueActiveContexts
//-------------------------------------------------------------------
void KeyMap::QueueActiveContexts()
{
	for ( auto it = m_tableContext.GetIterator(); it; it++ )
	{
		if ( (*it)->IsEnabled() )
			m_queue.AddLast( (*it) );
	}
}
//-------------------------------------------------------------------
//	DeactivateAllContexts
//-------------------------------------------------------------------
void KeyMap::DeactivateAllContexts()
{
	for ( auto it = m_tableContext.GetIterator(); it; it++ )
		(*it)->Disable();
}
//-------------------------------------------------------------------
//	GetQueuedContexts
//-------------------------------------------------------------------
void KeyMap::GetQueuedContexts(List<const KeyMapContext*>& outResult)const
{
	for ( auto it = m_tableContext.GetIterator(); it; it++ )
		outResult.AddLast( (*it).Get() );
}
