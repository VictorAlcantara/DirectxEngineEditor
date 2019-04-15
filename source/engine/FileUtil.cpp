#include "FileUtil.h"
#include "../engine/Pointer.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
FileLine::FileLine() : FileLine(nullptr, 0)
{
}
//-------------------------------------------------------------------
FileLine::FileLine(const char* pData, uint size)
{
	m_pData = new char[size];
	memoryCopy( pData, m_pData, size );
	m_size = size;
}
//-------------------------------------------------------------------
FileLine::FileLine(const FileLine& other)
{
	*this = other;
}
//-------------------------------------------------------------------
FileLine::~FileLine()
{
	Clear();
}
//-------------------------------------------------------------------
//	Clear
//-------------------------------------------------------------------
void FileLine::Clear()
{
	safeArrayRelease( m_pData );
	m_size = 0;
}
//-------------------------------------------------------------------
//	operator=
//-------------------------------------------------------------------
FileLine& FileLine::operator=(const FileLine& other)
{
	if ( this != &other )
	{
		Clear();

		if ( other.GetSize() > 0 )
		{
			m_size = other.GetSize();
			m_pData = new char[m_size];

			memoryCopy( other.GetData(), m_pData, m_size );
		}
	}

	return *this;
}
