#pragma once

#include "ListBridge.h"
#include "../engine/Class.h"

//=============================================================================
//	NodeList declaration
//=============================================================================

template <class T>
class NodeList : public NonCopyable
{
	public:
		NodeList(IListBridge*);
		~NodeList();

		void SetValue(const T&);

		T& GetValue();
		const T& GetValue()const;

		NodeList<T>* GetNext();
		const NodeList<T>* GetNext()const;
		NodeList<T>* GetPrev();
		const NodeList<T>* GetPrev()const;

		void AddNext(const T&);
		void AddPrev(const T&);

		void Remove();

		uint GetId()const { return m_id; }

	private:
		uint m_id;
		static uint s_idControl;

		T m_value;
		NodeList<T>* m_pPrev;
		NodeList<T>* m_pNext;

		IListBridge* m_pBridge;
};

//=============================================================================
//	NodeList definition
//=============================================================================

template <class T>
uint NodeList<T>::s_idControl = 0;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class T>
inline NodeList<T>::NodeList(IListBridge* pBridge)
{
	m_pBridge = pBridge;

	m_pPrev = nullptr;
	m_pNext = nullptr;

	m_id = ++s_idControl;
}
//-------------------------------------------------------------------
template <class T>
inline NodeList<T>::~NodeList()
{
	m_pBridge->OnRemoveNode( m_id );

	if ( m_pPrev )
		m_pPrev->m_pNext = m_pNext;

	if ( m_pNext )
		m_pNext->m_pPrev = m_pPrev;
}
//-------------------------------------------------------------------
//	SetValue
//-------------------------------------------------------------------
template <class T>
inline void NodeList<T>::SetValue(const T& t)
{
	m_value = t;
}
//-------------------------------------------------------------------
//	GetValue
//-------------------------------------------------------------------
template <class T>
inline T& NodeList<T>::GetValue()
{
	return m_value;
}
//-------------------------------------------------------------------
template <class T>
inline const T& NodeList<T>::GetValue()const
{
	return m_value;
}
//-------------------------------------------------------------------
//	GetNext
//-------------------------------------------------------------------
template <class T>
inline NodeList<T>* NodeList<T>::GetNext()
{
	return m_pNext;
}
//-------------------------------------------------------------------
template <class T>
inline const NodeList<T>* NodeList<T>::GetNext()const
{
	return m_pNext;
}
//-------------------------------------------------------------------
//	GetPrev
//-------------------------------------------------------------------
template <class T>
inline NodeList<T>* NodeList<T>::GetPrev()
{
	return m_pPrev;
}
//-------------------------------------------------------------------
template <class T>
inline const NodeList<T>* NodeList<T>::GetPrev()const
{
	return m_pPrev;
}
//-------------------------------------------------------------------
//	AddNext
//-------------------------------------------------------------------
template <class T>
inline void NodeList<T>::AddNext(const T& t)
{
	NodeList<T>* pNewNextNode = new NodeList<T>(m_pBridge);
	pNewNextNode->SetValue( t );
	
	pNewNextNode->m_pPrev = this;
	pNewNextNode->m_pNext = this->m_pNext;

	this->m_pNext = pNewNextNode;
	if ( pNewNextNode->m_pNext )
		pNewNextNode->m_pNext->m_pPrev = pNewNextNode;

	m_pBridge->OnAddNode();
}
//-------------------------------------------------------------------
//	AddPrev
//-------------------------------------------------------------------
template <class T>
inline void NodeList<T>::AddPrev(const T& t)
{
	NodeList<T>* pNewPrevNode = new NodeList<T>(m_pBridge);
	pNewPrevNode->SetValue( t );

	pNewPrevNode->m_pPrev = this->m_pPrev;
	pNewPrevNode->m_pNext = this;

	if ( pNewPrevNode->m_pPrev )
		pNewPrevNode->m_pPrev->m_pNext = pNewPrevNode;
	this->m_pPrev = pNewPrevNode;

	m_pBridge->OnAddNode();
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
template <class T>
inline void NodeList<T>::Remove()
{
	delete this;
}
