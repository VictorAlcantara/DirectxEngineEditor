#pragma once

#pragma warning (disable: 4150)

#include "../engine/Exception.h"
#include "Class.h"
#include "Type.h"
#include "Debug.h"

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof(*array) )

//-------------------------------------------------------------------
//	safePtrRelease
//-------------------------------------------------------------------
template <class PTR>
inline void safePtrRelease(PTR& pointer)
{
	delete pointer;
	pointer = nullptr;
}
//-------------------------------------------------------------------
//	safeArrayRelease
//-------------------------------------------------------------------
template <class ARRAY>
inline void safeArrayRelease(ARRAY& pArray)
{
	delete[] pArray;
	pArray = nullptr;
}
//-------------------------------------------------------------------
//	memoryCopy
//-------------------------------------------------------------------
template <class T>
inline void memoryCopy(const T* source, T* dest, uint count)
{
	for ( uint i = 0; i < count; i++ )
		dest[i] = source[i];
}
//-------------------------------------------------------------------
//	memoryInvert
//-------------------------------------------------------------------
template <class T>
inline void memoryInvert(const T* source, T* dest, uint count)
{
	for ( uint i = 0; i < count; i++ )
		dest[i] = source[count - 1 - i];
}
//-------------------------------------------------------------------
//	memoryWrite
//-------------------------------------------------------------------
inline void memoryWrite(char* source, uint sourceSize, const char* data, uint dataSize)
{
	if ( sourceSize % dataSize != 0 )
	{
		THROW( "memoryWrite() data write cannot be safely performed" );
	}

	for ( uint i = 0; i < sourceSize; i++ )
		source[i] = data[i % dataSize];
}
//-------------------------------------------------------------------
//	memoryWrite
//-------------------------------------------------------------------
inline void memoryWrite(char* source, uint sourceSize, char data)
{
	for ( uint i = 0; i < sourceSize; i++ )
		source[i] = data;
}
//-------------------------------------------------------------------
template <class T>
inline void memoryWrite(T* source, uint count, T data)
{
	for ( uint i = 0; i < count; i++ )
		source[i] = data;
}
//-------------------------------------------------------------------
//	memoryReverse
//-------------------------------------------------------------------
template <class T>
inline void memoryReverse(T* source, uint size)
{
	for ( uint i = 0; i < size/2; i++ )
	{
		uint mirrorIndex = size - 1 - i;
		T temp = source[i];
		
		source[i] = source[mirrorIndex];
		source[mirrorIndex] = temp;
	}
}

/****************************************************************************************
	SimplePtr
*****************************************************************************************/

template <class TYPE>
class SimplePtr : NonCopyable
{
	public:
		SimplePtr();
		SimplePtr(TYPE& data);
		SimplePtr(TYPE* pData);
		~SimplePtr();

		TYPE* Get();
		const TYPE* Get()const;

		void Clear();

		SimplePtr& operator=(TYPE& data);
		SimplePtr& operator=(TYPE* pData);

		TYPE* operator->();
		const TYPE* operator->()const;

		TYPE* operator*();
		const TYPE* operator*()const;

		operator bool()const;

	private:
		bool CanAssign(TYPE* pPointer);

		TYPE* m_pPointer;
};

//=============================================================================
//	SimplePtr definition
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class TYPE>
inline SimplePtr<TYPE>::SimplePtr()
{
	m_pPointer = nullptr;
}
//-------------------------------------------------------------------
template <class TYPE>
inline SimplePtr<TYPE>::SimplePtr(TYPE& data) : SimplePtr()
{
	*this = data;
}
//-------------------------------------------------------------------
template <class TYPE>
inline SimplePtr<TYPE>::SimplePtr(TYPE* data) : SimplePtr()
{
	*this = data;
}
//-------------------------------------------------------------------
template <class T>
inline SimplePtr<T>::~SimplePtr()
{
	Clear();
}
//-------------------------------------------------------------------
//	operator=
//-------------------------------------------------------------------
template <class TYPE>
SimplePtr<TYPE>& SimplePtr<TYPE>::operator=(TYPE& data)
{
	if ( CanAssign(&data) )
	{
		Clear();
		m_pPointer = &data;
	}

	return *this;
}
//-------------------------------------------------------------------
template <class TYPE>
SimplePtr<TYPE>& SimplePtr<TYPE>::operator=(TYPE* pData)
{
	if ( CanAssign(pData) )
	{
		Clear();
		m_pPointer = pData;
	}

	return *this;
}
//-------------------------------------------------------------------
//	operator->
//-------------------------------------------------------------------
template <class TYPE>
inline TYPE* SimplePtr<TYPE>::operator->()
{
	#ifdef DEBUG_SIMPLEPTR_NULL_DEFERENCE
		CHECK( m_pPointer );
	#endif
	return Get();
}
//-------------------------------------------------------------------
template <class TYPE>
inline const TYPE* SimplePtr<TYPE>::operator->()const
{
	#ifdef DEBUG_SIMPLEPTR_NULL_DEFERENCE
		CHECK( m_pPointer );
	#endif
	return Get();
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template <class TYPE>
inline SimplePtr<TYPE>::operator bool()const
{
	return m_pPointer != nullptr;
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class TYPE>
TYPE* SimplePtr<TYPE>::Get()
{
	return m_pPointer;
}
//-------------------------------------------------------------------
template <class TYPE>
const TYPE* SimplePtr<TYPE>::Get()const
{
	return m_pPointer;
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class TYPE>
void SimplePtr<TYPE>::Clear()
{
	safePtrRelease( m_pPointer );
}
//-------------------------------------------------------------------
//	CanAssign
//-------------------------------------------------------------------
template <class TYPE>
bool SimplePtr<TYPE>::CanAssign(TYPE* pPointer)
{
	if ( m_pPointer && pPointer == m_pPointer )
	{
		ASSERT( "Self assignment attempted" );
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
//	operator*
//-------------------------------------------------------------------
template<class TYPE>
inline TYPE* SimplePtr<TYPE>::operator*()
{
	return Get();
}
//-------------------------------------------------------------------
template<class TYPE>
inline const TYPE* SimplePtr<TYPE>::operator*()const
{
	return Get();
}


/****************************************************************************************
	SmartPtrData
*****************************************************************************************/

template <class TYPE>
struct SmartPtrData
{
	uint* pCount;
	TYPE* pPointer;

	SmartPtrData() : pCount(nullptr), pPointer(nullptr) {}

	template <class NEW_TYPE>
	SmartPtrData<NEW_TYPE> As()
	{
		SmartPtrData<NEW_TYPE> result;
		result.pCount = pCount;
		result.pPointer = (NEW_TYPE*)pPointer;
		return result;
	}

	void Increase()
	{
		if ( IsNull() )
			return;

		#ifdef DEBUG_SMART_POINTER_COUNTER
			CHECK( pCount && *pCount != UINT_INVALID );
		#endif
		*pCount = *pCount + 1;
	}

	void Decrease()
	{
		if ( IsNull() )
			return;

		#ifdef DEBUG_SMART_POINTER_COUNTER
			CHECK( pCount && *pCount != 0 );
		#endif
		*pCount = *pCount - 1;
	}

	bool IsNull()const
	{
		return !pPointer && !pCount;
	}
};

/****************************************************************************************
	SmartPtr
*****************************************************************************************/

template <class TYPE>
class SmartPtr
{
	template <class NEW_TYPE>
	friend class SmartPtr;

	public:
		SmartPtr();
		SmartPtr(TYPE*);
		SmartPtr(const SmartPtr&);
		~SmartPtr();

		static SmartPtr<TYPE> New();

		TYPE* Get();
		const TYPE* Get()const;

		uint GetReferenceCount()const { return  m_data.pCount ? *m_data.pCount : 0; }

		template <class NEW_TYPE>
		SmartPtr<NEW_TYPE> As();

		TYPE* operator->();
		const TYPE* operator->()const;

		TYPE* operator*();
		const TYPE* operator*()const;

		operator bool()const;

		SmartPtr<TYPE>& operator=(TYPE*);
		SmartPtr<TYPE>& operator=(const SmartPtr&);
		
	private:
		void Release();

		SmartPtrData<TYPE> m_data;
};

//=============================================================================
//	Definition
//=============================================================================

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>::SmartPtr()
{
}
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>::SmartPtr(TYPE* pObject)
{
	*this = pObject;
}
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>::SmartPtr(const SmartPtr& pObject)
{
	*this = pObject;
}
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>::~SmartPtr()
{
	Release();
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template<class TYPE>
inline TYPE* SmartPtr<TYPE>::Get()
{
	return m_data.pPointer;
}
//-------------------------------------------------------------------
template<class TYPE>
inline const TYPE* SmartPtr<TYPE>::Get()const
{
	return m_data.pPointer;
}
//-------------------------------------------------------------------
//	As
//-------------------------------------------------------------------
template<class TYPE>
template<class NEW_TYPE>
inline SmartPtr<NEW_TYPE> SmartPtr<TYPE>::As()
{
	if ( !m_data.pPointer )
		return SmartPtr<NEW_TYPE>();

	SmartPtr<NEW_TYPE> result;
	result.m_data = m_data.As<NEW_TYPE>();
	result.m_data.Increase();
	
	return result;
}
//-------------------------------------------------------------------
//	New
//-------------------------------------------------------------------
template <class TYPE>
inline SmartPtr<TYPE> SmartPtr<TYPE>::New()
{
	SmartPtr<TYPE> pResult = new TYPE;
	return pResult;
}
//-------------------------------------------------------------------
//	operator->
//-------------------------------------------------------------------
template<class TYPE>
inline TYPE* SmartPtr<TYPE>::operator->()
{
	return Get();
}
//-------------------------------------------------------------------
template<class TYPE>
inline const TYPE* SmartPtr<TYPE>::operator->()const
{
	return Get();
}
//-------------------------------------------------------------------
//	operator*
//-------------------------------------------------------------------
template<class TYPE>
inline TYPE* SmartPtr<TYPE>::operator*()
{
	return Get();
}
//-------------------------------------------------------------------
template<class TYPE>
inline const TYPE* SmartPtr<TYPE>::operator*()const
{
	return Get();
}
//-------------------------------------------------------------------
//	operator bool
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>::operator bool()const
{
	return m_data.pPointer;
}
//-------------------------------------------------------------------
//	operator =
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>& SmartPtr<TYPE>::operator=(TYPE* pObject)
{
	if ( m_data.pPointer == pObject )
		return *this;

	Release();
	m_data.pCount = new uint;
	*m_data.pCount = 0;
	m_data.pPointer = pObject;
	m_data.Increase();

	return *this;
}
//-------------------------------------------------------------------
template<class TYPE>
inline SmartPtr<TYPE>& SmartPtr<TYPE>::operator=(const SmartPtr<TYPE>& pObject)
{
	if ( this == &pObject )
		return *this;

	Release();
	m_data = pObject.m_data;
	m_data.Increase();

	return *this;
}
//-------------------------------------------------------------------
//	Release
//-------------------------------------------------------------------
template<class TYPE>
inline void SmartPtr<TYPE>::Release()
{
	#ifdef DEBUG_MODE
		if ( !m_data.pCount && m_data.pPointer )
			ASSERT( "Count is null, but pointer is not" );
	#endif

	if ( m_data.pCount )
	{
		if ( *m_data.pCount > 0 )
			m_data.Decrease();

		if ( *m_data.pCount == 0 )
		{
			safePtrRelease( m_data.pPointer );
			safePtrRelease( m_data.pCount );
		}
	}

	m_data.pPointer = nullptr;
}
