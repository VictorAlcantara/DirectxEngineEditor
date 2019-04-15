#pragma once

#include "NodeList.h"
#include "../engine/Exception.h"

template <class T>
class List;

/****************************************************************************************
	ListIterator
*****************************************************************************************/

template <class TYPE>
class ListIterator
{
	template <class _TYPE>
	friend class List;

	public:
		ListIterator();

		void Invalidate();
		bool IsNull()const;

		operator bool()const;
		void operator++(int);
		void operator--(int);
		const TYPE& operator*()const;
		TYPE& operator*();

	private:
		ListIterator(NodeList<TYPE>*);

		NodeList<TYPE>* m_pCurrent;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline ListIterator<TYPE>::ListIterator()
{
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
template <class TYPE>
inline ListIterator<TYPE>::ListIterator(NodeList<TYPE>* pNode)
{
	m_pCurrent = pNode;
}
//-------------------------------------------------------------------
//	Invalidate
//-------------------------------------------------------------------
template <class TYPE>
inline void ListIterator<TYPE>::Invalidate()
{
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
//	IsNull
//-------------------------------------------------------------------
template <class TYPE>
inline bool ListIterator<TYPE>::IsNull()const
{
	return !m_pCurrent;
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template <class TYPE>
inline ListIterator<TYPE>::operator bool()const
{
	return !IsNull();
}
//-------------------------------------------------------------------
//	operator ++
//-------------------------------------------------------------------
template <class TYPE>
inline void ListIterator<TYPE>::operator++(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetNext();
}
//-------------------------------------------------------------------
//	operator --
//-------------------------------------------------------------------
template <class TYPE>
inline void ListIterator<TYPE>::operator--(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetPrev();
}
//-------------------------------------------------------------------
//	operator *
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE& ListIterator<TYPE>::operator*()const
{
	if ( !m_pCurrent )
	{
		ASSERT( "Node is null" );
	}

	return m_pCurrent->GetValue();
}
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE& ListIterator<TYPE>::operator*()
{
	if ( !m_pCurrent )
	{
		ASSERT( "Node is null" );
	}

	return m_pCurrent->GetValue();
}

/****************************************************************************************
	ListIteratorConst
*****************************************************************************************/

template <class TYPE>
class ListIteratorConst
{
	template <class _TYPE>
	friend class List;

	public:
		ListIteratorConst();

		void Invalidate();
		bool IsNull()const;

		operator bool()const;
		void operator++(int);
		void operator--(int);
		const TYPE& operator*()const;

	private:
		ListIteratorConst(const NodeList<TYPE>*);

		const NodeList<TYPE>* m_pCurrent;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline ListIteratorConst<TYPE>::ListIteratorConst()
{
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
template <class TYPE>
inline ListIteratorConst<TYPE>::ListIteratorConst(const NodeList<TYPE>* pNode)
{
	m_pCurrent = pNode;
}
//-------------------------------------------------------------------
//	Invalidate
//-------------------------------------------------------------------
template <class TYPE>
inline void ListIteratorConst<TYPE>::Invalidate()
{
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
//	IsNull
//-------------------------------------------------------------------
template <class TYPE>
inline bool ListIteratorConst<TYPE>::IsNull()const
{
	return !m_pCurrent;
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template <class TYPE>
inline ListIteratorConst<TYPE>::operator bool()const
{
	return !IsNull();
}
//-------------------------------------------------------------------
//	operator ++
//-------------------------------------------------------------------
template <class TYPE>
inline void ListIteratorConst<TYPE>::operator++(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetNext();
}
//-------------------------------------------------------------------
//	operator --
//-------------------------------------------------------------------
template <class TYPE>
inline void ListIteratorConst<TYPE>::operator--(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetPrev();
}
//-------------------------------------------------------------------
//	operator *
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE& ListIteratorConst<TYPE>::operator*()const
{
	if ( !m_pCurrent )
	{
		ASSERT( "Node is null" );
	}

	return m_pCurrent->GetValue();
}
