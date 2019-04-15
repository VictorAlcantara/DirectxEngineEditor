#pragma once

#include "Type.h"
#include "../engine/TableHashFunction.h"
#include "../engine/List.h"
#include "Class.h"
#include "Debug.h"
#include "Pointer.h"
#include "TableIterator.h"

template <class TYPE, class KEY>
class Table : public NonCopyable
{
	typedef uint (*tableHashFunc)(const KEY&, uint);

	private:
		struct NodeTable
		{
			TYPE object;
			NodeList<TYPE*>* pNodeOnList;
			KEY key;

			NodeTable()
			{
				pNodeOnList = nullptr;
			};
		};

		struct SlotTable
		{
			List<NodeTable> slotNodes;
		};

	public:
		Table() = delete;
		Table(uint slots, tableHashFunc);
		~Table();

		void Add(TYPE&, const KEY&);
		void AddIfNotExists(TYPE&, const KEY&);
		TYPE* AddGetPtr(TYPE&, const KEY&);
		void AddOrChange(TYPE&, const KEY&);
		bool Remove(const KEY&);
		void Clear();

		TYPE* Find(const KEY&);
		const TYPE* Find(const KEY&)const;

		TYPE Get(const KEY&);
		const TYPE Get(const KEY&)const;

		TableIterator<TYPE> GetIterator();
		const TableIteratorConst<TYPE> GetIterator()const;

		uint Count()const { return m_objects.Count(); }

		void Debug_SlotDepthCheck(uint value) { m_debugSlotDepthMax = value; }

	private:
		uint GetIndex(const KEY&)const;
		NodeTable* FindNode(const KEY&);
		const NodeTable* FindNode(const KEY&)const;

		tableHashFunc m_pHashFunc;

		SlotTable* m_slots;
		uint m_slotCount;
		List<TYPE*> m_objects;
		uint m_debugSlotDepthMax;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline Table<TYPE, KEY>::Table(uint slots, tableHashFunc pHashFunc)
{
	CHECK( pHashFunc && slots > 0 );
	m_pHashFunc = pHashFunc;
	m_slots = new SlotTable[slots];
	m_slotCount = slots;
	m_debugSlotDepthMax = 5;
}
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline Table<TYPE, KEY>::~Table()
{
	safeArrayRelease( m_slots );
}
//-------------------------------------------------------------------
//	AddIfNotExists
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline void Table<TYPE, KEY>::AddIfNotExists(TYPE& object, const KEY& key)
{
	if ( !Find(key) )
		Add( object, key );
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline void Table<TYPE, KEY>::Add(TYPE& object, const KEY& key)
{
	AddGetPtr( object, key );
}
//-------------------------------------------------------------------
//	AddGetPtr
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline TYPE* Table<TYPE, KEY>::AddGetPtr(TYPE& object, const KEY& key)
{
	if ( Find(key) )
	{
		ASSERT( "Key already exists in table" );
		return nullptr;
	}

	uint index = GetIndex(key);

	NodeTable node;
	node.object = object;
	node.key = key;

	if ( index >= m_slotCount )
	{
		index = m_slotCount - 1;
		ASSERT( "Index out of bounds" );
	}

	#ifdef DEBUG_TABLE_SLOT_DEPTH
		CHECK( (m_debugSlotDepthMax == 0) || (m_slots[index].slotNodes.Count() <= m_debugSlotDepthMax) );
	#endif

	m_slots[index].slotNodes.AddLast( node );

	m_objects.AddLast( &m_slots[index].slotNodes.GetLast()->GetValue().object );
	m_slots[index].slotNodes.GetLast()->GetValue().pNodeOnList = m_objects.GetLast();

	return m_objects.GetLast()->GetValue();
}
//-------------------------------------------------------------------
//	AddOrChange
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline void Table<TYPE, KEY>::AddOrChange(TYPE& object, const KEY& key)
{
	auto pFind = Find( key );

	if ( pFind )
		*pFind = object;
	else
		Add( object, key );
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline bool Table<TYPE, KEY>::Remove(const KEY& key)
{
	uint index = GetIndex(key);

	for ( auto pNode = m_slots[index].slotNodes.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue().key == key )
		{
			pNode->GetValue().pNodeOnList->Remove();
			pNode->Remove();
			return true;
		}
	}

	return false;
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline void Table<TYPE, KEY>::Clear()
{
	if ( Count() == 0 )
		return;

	m_objects.Clear();
	safeArrayRelease( m_slots );
	m_slots = new SlotTable[m_slotCount];
}
//-------------------------------------------------------------------
//	Find
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline TYPE* Table<TYPE, KEY>::Find(const KEY& key)
{
	NodeTable* pNode = FindNode(key);

	if ( pNode )
		return &pNode->object;

	return nullptr;
}
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline const TYPE* Table<TYPE, KEY>::Find(const KEY& key)const
{
	const NodeTable* pNode = FindNode(key);

	if ( pNode )
		return &pNode->object;

	return nullptr;
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline TYPE Table<TYPE, KEY>::Get(const KEY& key)
{
	NodeTable* pNode = FindNode(key);

	if ( pNode )
		return pNode->object;

	return TYPE();
}
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline const TYPE Table<TYPE, KEY>::Get(const KEY& key)const
{
	const NodeTable* pNode = FindNode(key);

	if ( pNode )
		return pNode->object;

	return TYPE();
}
//-------------------------------------------------------------------
//	GetIterator
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline TableIterator<TYPE> Table<TYPE, KEY>::GetIterator()
{
	return TableIterator<TYPE>(m_objects.GetFirst());
}
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline const TableIteratorConst<TYPE> Table<TYPE, KEY>::GetIterator()const
{
	return TableIteratorConst<TYPE>(m_objects.GetFirst());
}
//-------------------------------------------------------------------
//	GetIndex
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline uint Table<TYPE, KEY>::GetIndex(const KEY& key)const
{
	return (*m_pHashFunc)( key, m_slotCount );
}
//-------------------------------------------------------------------
//	FindNode
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline typename Table<TYPE, KEY>::NodeTable* Table<TYPE, KEY>::FindNode(const KEY& key)
{
	NodeTable* pResult = nullptr;

	uint index = GetIndex(key);

	for ( auto pNode = m_slots[index].slotNodes.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue().key == key )
		{
			pResult = &pNode->GetValue();
			break;
		}
	}

	return pResult;
}
//-------------------------------------------------------------------
template <class TYPE, class KEY>
inline typename const Table<TYPE, KEY>::NodeTable* Table<TYPE, KEY>::FindNode(const KEY& key)const
{
	const NodeTable* pResult = nullptr;

	uint index = GetIndex(key);

	for ( auto pNode = m_slots[index].slotNodes.GetFirst(); pNode; pNode = pNode->GetNext() )
	{
		if ( pNode->GetValue().key == key )
		{
			pResult = &pNode->GetValue();
			break;
		}
	}

	return pResult;
}