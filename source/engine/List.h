#pragma once

#include "Type.h"
#include "Pointer.h"
#include "ListBridge.h"
#include "NodeList.h"
#include "../engine/ListIterator.h"

//=============================================================================
//	Declarations
//=============================================================================

template <class T>
class List : private IListBridge, public NonCopyable
{
	public:
		List();
		~List();

		void Clear();

		uint Count()const { return m_count; }

		void AddFirst(const T&);
		void AddLast(const T&);

		NodeList<T>* GetFirst();
		const NodeList<T>* GetFirst()const;

		NodeList<T>* GetLast();
		const NodeList<T>* GetLast()const;

		template <class KEY>
		bool Remove(const KEY&, bool (*listFindFunc)(const T&, const KEY&));

		template <class KEY>
		T Get(const KEY&, bool (*listFindFunc)(const T&, const KEY&));

		ListIterator<T> GetIterator();
		ListIteratorConst<T> GetIterator()const;

		ListIterator<T> GetIteratorLast();
		ListIteratorConst<T> GetIteratorLast()const;

	private:
		NodeList<T>* m_pFirst;
		NodeList<T>* m_pLast;

		uint m_count;

		void OnAddNode()override;
		void OnRemoveNode(uint id)override;
};

//=============================================================================
//	Definition class List
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class T>
inline List<T>::List()
{
	m_pFirst = nullptr;
	m_pLast = nullptr;
	m_count = 0;
}
//-------------------------------------------------------------------
template <class T>
inline List<T>::~List()
{
	Clear();
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class T>
inline void List<T>::Clear()
{
	if ( m_pFirst )
	{
		for ( auto pNode = m_pFirst; pNode; )
		{
			auto pNodeNext = pNode->GetNext();
			safePtrRelease( pNode );
			pNode = pNodeNext;
		}
	}

	m_pFirst = nullptr;
	m_pLast = nullptr;

	CHECK( m_count == 0 );
	m_count = 0;
}
//-------------------------------------------------------------------
//	AddFirst
//-------------------------------------------------------------------
template <class T>
inline void List<T>::AddFirst(const T& t)
{
	if ( !m_pFirst )
	{
		m_pFirst = new NodeList<T>( this );
		m_pFirst->SetValue( t );
		m_count++;
	}
	else
		m_pFirst->AddPrev( t );

	if ( !m_pLast )
		m_pLast = m_pFirst;
}
//-------------------------------------------------------------------
//	AddLast
//-------------------------------------------------------------------
template <class T>
inline void List<T>::AddLast(const T& t)
{
	if ( !m_pLast )
	{
		m_pLast = new NodeList<T>( this );
		m_pLast->SetValue( t );
		m_count++;
	}
	else
		m_pLast->AddNext( t );

	if ( !m_pFirst )
		m_pFirst = m_pLast;
}
//-------------------------------------------------------------------
//	GetFirst
//-------------------------------------------------------------------
template <class T>
inline NodeList<T>* List<T>::GetFirst()
{
	return m_pFirst;
}
//-------------------------------------------------------------------
template <class T>
inline const NodeList<T>* List<T>::GetFirst()const
{
	return m_pFirst;
}
//-------------------------------------------------------------------
//	GetLast
//-------------------------------------------------------------------
template <class T>
inline NodeList<T>* List<T>::GetLast()
{
	return m_pLast;
}
//-------------------------------------------------------------------
template <class T>
inline const NodeList<T>* List<T>::GetLast()const
{
	return m_pLast;
}
//-------------------------------------------------------------------
//	OnAddNode
//-------------------------------------------------------------------
template <class T>
inline void List<T>::OnAddNode()
{
	m_count++;

	if ( m_pFirst && m_pFirst->GetPrev() )
		m_pFirst = m_pFirst->GetPrev();

	if ( m_pLast && m_pLast->GetNext() )
		m_pLast = m_pLast->GetNext();
}
//-------------------------------------------------------------------
//	OnRemoveNode
//-------------------------------------------------------------------
template <class T>
inline void List<T>::OnRemoveNode(uint id)
{
	m_count--;

	if ( m_count == UINT_INVALID )
	{
		THROW("List::OnRemoveNode() m_count is invalid, something went terribly wrong :|");
	}

	if ( m_pFirst && m_pFirst->GetId() == id )
		m_pFirst = m_pFirst->GetNext();

	if ( m_pLast && m_pLast->GetId() == id )
		m_pLast = m_pLast->GetPrev();
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template<class T> template<class KEY>
inline T List<T>::Get(const KEY& key, bool(*listFindFunc)(const T&, const KEY&))
{
	CHECK( listFindFunc );

	for ( auto pNode = GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( listFindFunc(pNode->GetValue(), key) )
		{
			return pNode->GetValue();
		}
	}

	return T(0);
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
template<class T> template<class KEY>
inline bool List<T>::Remove(const KEY& key, bool(*listFindFunc)(const T&, const KEY&))
{
	CHECK( listFindFunc );

	for ( auto pNode = GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( listFindFunc(pNode->GetValue(), key) )
		{
			pNode->Remove();
			return true;
		}
	}

	return false;
}
//-------------------------------------------------------------------
//	GetIterator
//-------------------------------------------------------------------
template <class T>
inline ListIterator<T> List<T>::GetIterator()
{
	return ListIterator<T>( GetFirst() );
}
//-------------------------------------------------------------------
template <class T>
inline ListIteratorConst<T> List<T>::GetIterator()const
{
	return ListIteratorConst<T>( GetFirst() );
}
//-------------------------------------------------------------------
//	GetIteratorLast
//-------------------------------------------------------------------
template <class T>
inline ListIterator<T> List<T>::GetIteratorLast()
{
	return ListIterator<T>( GetLast() );
}
//-------------------------------------------------------------------
template <class T>
inline ListIteratorConst<T> List<T>::GetIteratorLast()const
{
	return ListIteratorConst<T>( GetLast() );
}
