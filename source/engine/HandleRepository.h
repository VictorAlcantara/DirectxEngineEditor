#pragma once

#include "../engine/Handle.h"
#include "../engine/Table.h"
#include "../engine/Class.h"
#include "Type.h"
#include "../engine/TableHashFunction.h"

template <class TYPE>
class HandleRepository_Id : public NonCopyable
{
	public:
		HandleRepository_Id(uint handleSlots, uint tableSlots);
		~HandleRepository_Id();
		
		Handle<TYPE> Create();
		void Remove(const Handle<TYPE>&);
		void Remove(uint id);
		void Clear();

		Handle<TYPE> Get(uint id);
		TableIterator<Handle<TYPE>> GetIterator();
		TableIteratorConst<Handle<TYPE>> GetIterator()const;

	private:
		HandleManager<TYPE> m_handles;
		Table<Handle<TYPE>, uint> m_table;
};

//=============================================================================
//	Template class definition
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline HandleRepository_Id<TYPE>::HandleRepository_Id(uint handleSlots, uint tableSlots) : m_handles(handleSlots), m_table(tableSlots, tableHashFunc_Uint)
{
}
//-------------------------------------------------------------------
template <class TYPE>
inline HandleRepository_Id<TYPE>::~HandleRepository_Id()
{
	Clear();
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class TYPE>
inline void HandleRepository_Id<TYPE>::Clear()
{
	m_table.Clear();
	m_handles.Clear();
}
//-------------------------------------------------------------------
//	Create
//-------------------------------------------------------------------
template <class TYPE>
inline Handle<TYPE> HandleRepository_Id<TYPE>::Create()
{
	Handle<TYPE> result = m_handles.Create();
	m_table.Add( result, result->GetId() );
	return result;
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
template <class TYPE>
inline void HandleRepository_Id<TYPE>::Remove(const Handle<TYPE>& handle)
{
	m_table.Remove( handle->GetId() );
	m_handles.Remove( handle );
}
//-------------------------------------------------------------------
template <class TYPE>
inline void HandleRepository_Id<TYPE>::Remove(uint id)
{
	Handle<TYPE> handle = Get( id );
	Remove( handle );
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class TYPE>
inline Handle<TYPE> HandleRepository_Id<TYPE>::Get(uint id)
{
	return m_table.Get( id );
}
//-------------------------------------------------------------------
//	GetIterator
//-------------------------------------------------------------------
template <class TYPE>
inline TableIterator<Handle<TYPE>> HandleRepository_Id<TYPE>::GetIterator()
{
	return m_table.GetIterator();
}
//-------------------------------------------------------------------
template <class TYPE>
inline TableIteratorConst<Handle<TYPE>> HandleRepository_Id<TYPE>::GetIterator()const
{
	return m_table.GetIterator();
}
