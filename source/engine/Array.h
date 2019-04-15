#pragma once

#include "Type.h"
#include "../engine/Pointer.h"
#include "../engine/Exception.h"

/****************************************************************************************
	Array
*****************************************************************************************/

template <class T>
class Array : public NonCopyable
{
	public:
		Array();
		Array(const Array<T>&);
		~Array();

		void Add(const T&);
		void Clear();

		uint Count()const { return m_count; }
		
		const T& Last()const;

		T* GetPointer();
		const T* GetPointer()const;

		Array<T>& operator=(const Array&);
		Array<T>& operator+=(const T&);
		const T& operator[](uint)const;
		T& operator[](uint);

	private:
		uint m_count;
		T* m_data;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template<class T>
inline Array<T>::Array()
{
	m_count = 0;
	m_data = nullptr;
}
//-------------------------------------------------------------------
template<class T>
inline Array<T>::Array(const Array<T>& array) : Array()
{
	*this = array;
}
//-------------------------------------------------------------------
template<class T>
inline Array<T>::~Array()
{
	safeArrayRelease( m_data );
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
template <class T>
inline void Array<T>::Add(const T& t)
{
	*this += t;
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class T>
inline void Array<T>::Clear()
{
	safeArrayRelease( m_data );
	m_count = 0;
}
//-------------------------------------------------------------------
//	Last
//-------------------------------------------------------------------
template<class T>
inline const T& Array<T>::Last()const
{
	CHECK( m_count > 0 );
	return m_data[m_count - 1];
}
//-------------------------------------------------------------------
//	GetPointer
//-------------------------------------------------------------------
template<class T>
inline T* Array<T>::GetPointer()
{
	return m_data;
}
//-------------------------------------------------------------------
template<class T>
inline const T * Array<T>::GetPointer() const
{
	return m_data;
}
//-------------------------------------------------------------------
//	operator=
//-------------------------------------------------------------------
template<class T>
inline Array<T>& Array<T>::operator=(const Array<T>& array)
{
	if ( &array != this )
	{
		m_count = array.m_count;
		safeArrayRelease( m_data );
		m_data = new T[m_count];
		memoryCopy( array.GetPointer(), m_data, m_count );
	}

	return *this;
}
//-------------------------------------------------------------------
//	operator+=
//-------------------------------------------------------------------
template<class T>
inline Array<T>& Array<T>::operator+=(const T& object)
{
	m_count++;

	T* tempData = new T[m_count];

	if ( m_data )
	{
		memoryCopy( m_data, tempData, m_count-1 );
		safeArrayRelease( m_data );
	}

	tempData[m_count-1] = object;

	m_data = tempData;

	return *this;
}
//-------------------------------------------------------------------
//	operator[]
//-------------------------------------------------------------------
template<class T>
inline const T& Array<T>::operator[](uint index)const
{
	if ( index >= m_count )
		THROW( "Item index out of bounds" );

	return m_data[index];
}
//-------------------------------------------------------------------
template<class T>
inline T& Array<T>::operator[](uint index)
{
	if ( index >= m_count )
		THROW( "Item index out of bounds" );

	return m_data[index];
}

/****************************************************************************************
	FixedArray
*****************************************************************************************/

template <class TYPE>
class FixedArray : public NonCopyable
{
	public:
		FixedArray();
		FixedArray(uint size);
		~FixedArray();

		void Clear(uint newSize = 0);

		void Add(const TYPE&);

		uint Count()const { return m_current; }
		uint GetSize()const { return m_size; }

		TYPE& Get(uint index);
		const TYPE& Get(uint index)const;
		TYPE* GetPointer();
		const TYPE* GetPointer()const;

		FixedArray& operator+=(const TYPE&);

		const TYPE& operator[](uint index)const;
		TYPE& operator[](uint index);

	private:
		TYPE* m_data;
		uint m_current;
		uint m_size;
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
template <class T>
inline FixedArray<T>::FixedArray()
{
	Clear();
}
//-------------------------------------------------------------------
template <class T>
inline FixedArray<T>::FixedArray(uint size)
{
	Clear( size );
}
//-------------------------------------------------------------------
template <class T>
inline FixedArray<T>::~FixedArray()
{
	Clear();
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
template <class T>
inline void FixedArray<T>::Clear(uint newSize)
{
	m_size = newSize;
	m_current = 0;

	safeArrayRelease( m_data );

	if ( m_size > 0 )
		m_data = new T[m_size];
}
//-------------------------------------------------------------------
//	Add
//-------------------------------------------------------------------
template <class T>
inline void FixedArray<T>::Add(const T& t)
{
	CHECK( m_size != 0 );
	CHECK( m_current < m_size );

	if ( m_current < m_size )
		m_data[m_current++] = t;
}
//-------------------------------------------------------------------
//	operator+=
//-------------------------------------------------------------------
template <class T>
inline FixedArray<T>& FixedArray<T>::operator+=(const T& t)
{
	Add( t );

	return *this;
}
//-------------------------------------------------------------------
//	operator[]
//-------------------------------------------------------------------
template <class T>
inline const T& FixedArray<T>::operator[](uint index)const
{
	CHECK( index < m_current );
	return m_data[index];
}
//-------------------------------------------------------------------
template <class T>
inline T& FixedArray<T>::operator[](uint index)
{
	CHECK( index < m_current );
	return m_data[index];
}
//-------------------------------------------------------------------
//	Get
//-------------------------------------------------------------------
template <class T>
inline T& FixedArray<T>::Get(uint index)
{
	return (*this)[index];
}
//-------------------------------------------------------------------
template <class T>
inline const T& FixedArray<T>::Get(uint index)const
{
	return (*this)[index];
}
//-------------------------------------------------------------------
//	GetPointer
//-------------------------------------------------------------------
template<class T>
inline T* FixedArray<T>::GetPointer()
{
	CHECK( m_size != 0 );
	return m_data;
}
//-------------------------------------------------------------------
template<class T>
inline const T* FixedArray<T>::GetPointer()const
{
	CHECK( m_size != 0 );
	return m_data;
}