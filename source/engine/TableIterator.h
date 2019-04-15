#pragma once

#include "NodeList.h"
#include "../engine/Exception.h"

template <class TYPE, class KEY>
class Table;

/****************************************************************************************
	TableIterator
*****************************************************************************************/

template <class TYPE>
class TableIterator
{
	template <class _TYPE, class KEY>
	friend class Table;

	public:
		TableIterator();

		void Invalidate();
		bool IsNull()const;

		operator bool()const;
		void operator++(int);
		void operator--(int);
		const TYPE& operator*()const;
		TYPE& operator*();

	private:
		TableIterator(NodeList<TYPE*>*);

		NodeList<TYPE*>* m_pCurrent;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline TableIterator<TYPE>::TableIterator()
{
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
template <class TYPE>
inline TableIterator<TYPE>::TableIterator(NodeList<TYPE*>* pNode)
{
	m_pCurrent = pNode;
}
//-------------------------------------------------------------------
//	Invalidate
//-------------------------------------------------------------------
template <class TYPE>
inline void TableIterator<TYPE>::Invalidate()
{
	return m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
//	IsNull
//-------------------------------------------------------------------
template <class TYPE>
inline bool TableIterator<TYPE>::IsNull()const
{
	return !m_pCurrent;
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template <class TYPE>
inline TableIterator<TYPE>::operator bool()const
{
	return !IsNull();
}
//-------------------------------------------------------------------
//	operator ++
//-------------------------------------------------------------------
template <class TYPE>
inline void TableIterator<TYPE>::operator++(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetNext();
}
//-------------------------------------------------------------------
//	operator --
//-------------------------------------------------------------------
template <class TYPE>
inline void TableIterator<TYPE>::operator--(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetPrev();
}
//-------------------------------------------------------------------
//	operator *
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE& TableIterator<TYPE>::operator*()const
{
	if ( !m_pCurrent )
	{
		ASSERT( "Node is null" );
	}

	return *m_pCurrent->GetValue();
}
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE& TableIterator<TYPE>::operator*()
{
	if ( !m_pCurrent )
	{
		ASSERT( "Node is null" );
	}

	return *m_pCurrent->GetValue();
}

/****************************************************************************************
	TableIteratorConst
*****************************************************************************************/

template <class TYPE>
class TableIteratorConst
{
	template <class _TYPE, class KEY>
	friend class Table;

	public:
		TableIteratorConst();

		void Invalidate();
		bool IsNull()const;

		operator bool()const;
		void operator++(int);
		void operator--(int);
		const TYPE& operator*()const;

	private:
		TableIteratorConst(const NodeList<TYPE*>*);

		const NodeList<TYPE*>* m_pCurrent;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline TableIteratorConst<TYPE>::TableIteratorConst()
{
	m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
template <class TYPE>
inline TableIteratorConst<TYPE>::TableIteratorConst(const NodeList<TYPE*>* pNode)
{
	m_pCurrent = pNode;
}
//-------------------------------------------------------------------
//	Invalidate
//-------------------------------------------------------------------
template <class TYPE>
inline void TableIteratorConst<TYPE>::Invalidate()
{
	return m_pCurrent = nullptr;
}
//-------------------------------------------------------------------
//	IsNull
//-------------------------------------------------------------------
template <class TYPE>
inline bool TableIteratorConst<TYPE>::IsNull()const
{
	return !m_pCurrent;
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template <class TYPE>
inline TableIteratorConst<TYPE>::operator bool()const
{
	return !IsNull();
}
//-------------------------------------------------------------------
//	operator ++
//-------------------------------------------------------------------
template <class TYPE>
inline void TableIteratorConst<TYPE>::operator++(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetNext();
}
//-------------------------------------------------------------------
//	operator --
//-------------------------------------------------------------------
template <class TYPE>
inline void TableIteratorConst<TYPE>::operator--(int)
{
	if ( m_pCurrent )
		m_pCurrent = m_pCurrent->GetPrev();
}
//-------------------------------------------------------------------
//	operator *
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE& TableIteratorConst<TYPE>::operator*()const
{
	if ( !m_pCurrent )
	{
		ASSERT( "Node is null" );
	}

	return *m_pCurrent->GetValue();
}
