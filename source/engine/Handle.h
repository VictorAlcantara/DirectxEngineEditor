#pragma once

#include "../engine/Class.h"
#include "Type.h"
#include "../engine/List.h"
#include "../engine/Pointer.h"

template <class>
class HandleManager;
template <class>
class Handle;
template <class, class>
class HandleAs;

/****************************************************************************************
	Functions declaration
*****************************************************************************************/

//-------------------------------------------------------------------
//	operator==
//-------------------------------------------------------------------
template <class T, class U>
bool operator==(const Handle<T>&, const Handle<U>&);
//-------------------------------------------------------------------
//	operator!=
//-------------------------------------------------------------------
template <class T, class U>
bool operator!=(const Handle<T>&, const Handle<U>&);

/****************************************************************************************
	Handle declaration
*****************************************************************************************/

template <class TYPE>
class Handle
{
	template <class> friend class HandleManager;
	template <class> friend class Handle;

	public:
		Handle();

		TYPE* GetPtr();
		const TYPE* GetPtr()const;
		TYPE* GetPtrOrNull();
		const TYPE* GetPtrOrNull()const;
		TYPE& Get();
		const TYPE&Get()const;
		template <class RETURN_TYPE>
		RETURN_TYPE& GetAs();
		template <class RETURN_TYPE>
		const RETURN_TYPE& GetAs()const;

		void Invalidate();
		bool IsValid()const;

		uint GetIndex()const { return m_index; }
		uint GetValidation()const { return m_validation; };

		const HandleManager<TYPE>* GetOwner()const { return m_pOwner; }

		TYPE* operator->();
		const TYPE* operator->()const;

		operator bool()const;

	private:
		HandleManager<TYPE>* m_pOwner;

		uint m_index;
		int m_validation;
};

//=============================================================================
//	Handle definition
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline Handle<TYPE>::Handle()
{
	Invalidate();
}
//-------------------------------------------------------------------
//	GetPtr
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE* Handle<TYPE>::GetPtr()
{
	return &Get();
}
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE* Handle<TYPE>::GetPtr()const
{
	return &Get();
}
//-------------------------------------------------------------------
//	GetPtrOrNull
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE* Handle<TYPE>::GetPtrOrNull()
{
	if ( IsValid() )
		return GetPtr();
	return nullptr;
}
//-------------------------------------------------------------------
//	GetPtrOrNull
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE* Handle<TYPE>::GetPtrOrNull()const
{
	if ( IsValid() )
		return GetPtr();
	return nullptr;
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE& Handle<TYPE>::Get()
{
	return GetAs<TYPE>();
}
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE& Handle<TYPE>::Get()const
{
	return GetAs<TYPE>();
}
//-------------------------------------------------------------------
//	GetAs
//-------------------------------------------------------------------
template <class TYPE> template <class RETURN_TYPE>
inline RETURN_TYPE& Handle<TYPE>::GetAs()
{
	return (RETURN_TYPE&)m_pOwner->Get( m_index, m_validation );
}
//-------------------------------------------------------------------
template <class TYPE> template <class RETURN_TYPE>
inline const RETURN_TYPE& Handle<TYPE>::GetAs()const
{
	return (const RETURN_TYPE&)m_pOwner->Get( m_index, m_validation );
}
//-------------------------------------------------------------------
//	Invalidate
//-------------------------------------------------------------------
template <class TYPE>
inline void Handle<TYPE>::Invalidate()
{
	m_pOwner = nullptr;
	m_index = 0;
	m_validation = 0;
}
//-------------------------------------------------------------------
//	IsValid
//-------------------------------------------------------------------
template <class TYPE>
inline bool Handle<TYPE>::IsValid()const
{
	if ( m_pOwner && m_validation > 0 && m_pOwner->GetValidation(m_index) == m_validation )
		return true;

	return false;
}
//-------------------------------------------------------------------
//	operator->
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE* Handle<TYPE>::operator->()
{
	return &Get();
}
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE* Handle<TYPE>::operator->()const
{
	return &Get();
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template <class TYPE>
inline Handle<TYPE>::operator bool()const
{
	return IsValid();
}

/****************************************************************************************
	HandleAs
*****************************************************************************************/

template <class TYPE_BASE, class TYPE>
class HandleAs
{
	public:
		HandleAs() = default;
		HandleAs(Handle<TYPE_BASE>& handle) { m_handle = handle; }

		void SetHandle(Handle<TYPE_BASE>& handle) { m_handle = handle; }
		Handle<TYPE_BASE>& GetBase() { return m_handle; }
		const Handle<TYPE_BASE>& GetBase()const { return m_handle; }

		void Invalidate() { m_handle.Invalidate(); }
		bool IsValid()const { return m_handle.IsValid(); }

		TYPE* operator->() { return (TYPE*)m_handle.GetPtr(); }
		const TYPE* operator->()const { return (TYPE*)m_handle.GetPtr(); }

		operator bool()const { return IsValid(); }

		void operator=(Handle<TYPE_BASE>& handle) { m_handle = handle; }
		
	private:
		Handle<TYPE_BASE> m_handle;
};

/****************************************************************************************
	HandleManager declaration
*****************************************************************************************/

template <class TYPE>
class HandleManager : public NonCopyable
{
	private:
		struct ObjectSlot
		{
			SmartPtr<TYPE> pObject;
			int validation;

			ObjectSlot()
			{
				validation = 0;
			}
		};

	public:
		HandleManager(uint handleSlots);
		~HandleManager();

		void Clear();
		Handle<TYPE> Create();
		template <class CREATE_TYPE>
		Handle<TYPE> CreateAs();
		void Remove(const Handle<TYPE>&);
		void Remove(uint index, int validation);

		TYPE& Get(uint index, int validation);
		const TYPE& Get(uint index, int validation)const;

		uint GetSlotCount()const { return m_slotCount; }
		int GetValidation(uint)const;

	private:
		ObjectSlot* m_slots;
		uint m_slotCount;
		List<uint> m_available;
};

//=============================================================================
//	HandleManager definition
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline HandleManager<TYPE>::HandleManager(uint slotsCount)
{
	m_slotCount = slotsCount;
	m_slots = nullptr;
	Clear();
}
//-------------------------------------------------------------------
template <class TYPE>
inline HandleManager<TYPE>::~HandleManager()
{
	safeArrayRelease( m_slots );
	m_available.Clear();
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class TYPE>
inline void HandleManager<TYPE>::Clear()
{
	safeArrayRelease( m_slots );
	m_slots = new ObjectSlot[m_slotCount];

	m_available.Clear();
	for ( uint i = 0; i < m_slotCount; i++ )
	{
		m_available.AddLast( i );
	}
}
//-------------------------------------------------------------------
//	Create
//-------------------------------------------------------------------
template <class TYPE>
inline Handle<TYPE> HandleManager<TYPE>::Create()
{
	return CreateAs<TYPE>();
}
//-------------------------------------------------------------------
//	CreateAs
//-------------------------------------------------------------------
template<class TYPE> template<class CREATE_TYPE>
inline Handle<TYPE> HandleManager<TYPE>::CreateAs()
{
	Handle<TYPE> result;

	auto pNode = m_available.GetFirst();

	if ( pNode )
	{
		uint index = pNode->GetValue();

		CHECK( !m_slots[index].pObject );

		m_slots[index].pObject = new CREATE_TYPE;

		m_slots[index].validation = (-m_slots[index].validation) + 1;
		result.m_index = index;
		result.m_validation = m_slots[index].validation;
		result.m_pOwner = this;

		CHECK( m_slots[index].validation > 0 );

		pNode->Remove();
	}
	else
	{
		THROW( "HandleManager::Create() No slot available for creating new handle" );
	}

	return result;
}
//-------------------------------------------------------------------
//	Remove
//-------------------------------------------------------------------
template <class TYPE>
inline void HandleManager<TYPE>::Remove(const Handle<TYPE>& handle)
{
	CHECK( handle.IsValid() );

	bool isOwner = handle.m_pOwner == this;

	CHECK( isOwner );

	if ( isOwner )
	{
		if ( handle.IsValid() )
			Remove( handle.GetIndex(), handle.GetValidation() );
	}
}
//-------------------------------------------------------------------
template <class TYPE>
inline void HandleManager<TYPE>::Remove(uint index, int validation)
{
	if ( index < m_slotCount )
	{
		CHECK( m_slots[index].validation > 0 );
		CHECK( m_slots[index].validation == validation );
		CHECK( m_slots[index].pObject.GetReferenceCount() <= 1 );

		m_slots[index].validation = -m_slots[index].validation;
		m_slots[index].pObject = nullptr;

		m_available.AddLast( index );
	}
	else
	{
		ASSERT( "Index out of bounds" );
	}
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE& HandleManager<TYPE>::Get(uint index, int validation)
{
	TYPE* pResult = nullptr;

	CHECK( m_slots[index].validation > 0 );
	CHECK( m_slots[index].validation == validation );

	if ( m_slots[index].validation > 0 && m_slots[index].validation == validation )
		pResult = m_slots[index].pObject.Get();

	CHECK( pResult );

	return *pResult;
}
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE& HandleManager<TYPE>::Get(uint index, int validation)const
{
	TYPE* pResult = nullptr;

	CHECK( m_slots[index].validation > 0 );
	CHECK( m_slots[index].validation == validation );

	if ( m_slots[index].validation > 0 && m_slots[index].validation == validation )
		pResult = m_slots[index].pObject.Get();

	return *pResult;
}
//-------------------------------------------------------------------
//	GetValidation
//-------------------------------------------------------------------
template <class TYPE>
inline int HandleManager<TYPE>::GetValidation(uint index)const
{
	return m_slots[index].validation;
}

/****************************************************************************************
	Functions definition
*****************************************************************************************/

//-------------------------------------------------------------------
//	operator==
//-------------------------------------------------------------------
template <class T, class U>
bool operator==(const Handle<T>& handle1, const Handle<U>& handle2)
{
	bool validOwner = handle1.GetOwner() && handle2.GetOwner() && (handle1.GetOwner() == handle2.GetOwner());
	auto validation1 = handle1.GetValidation();
	auto validation2 = handle2.GetValidation();

	CHECK( validOwner );

	return validOwner && handle1.GetIndex() == handle2.GetIndex() && (validation1 > 0 && validation2 > 0) && validation1 == validation2;
}
//-------------------------------------------------------------------
//	operator!=
//-------------------------------------------------------------------
template <class T, class U>
bool operator!=(const Handle<T>& handle1, const Handle<U>& handle2)
{
	return !( handle1 == handle2 );
}
