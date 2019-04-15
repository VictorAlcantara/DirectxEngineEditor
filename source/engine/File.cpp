#include "../engine/File.h"
#include "../engine/Exception.h"
#include "../engine/Debug.h"
#include "../engine/api_File.h"

/****************************************************************************************
	FileWriter class
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
FileWriter::FileWriter()
{
	m_open = false;
	m_api = {};
}
//-------------------------------------------------------------------
FileWriter::~FileWriter()
{
	Close();
}
//-------------------------------------------------------------------
//	Open
//-------------------------------------------------------------------
bool FileWriter::Open(const String& pathName, eFileOperation fileOperation)
{
	m_open = platform::file::openToWrite( m_api, pathName.AsChar(), fileOperation );

	CHECK( m_open );

	return m_open;
}
//-------------------------------------------------------------------
//	Write
//-------------------------------------------------------------------
void FileWriter::Write(const char* pData)
{
	if ( m_open )
	{
		platform::file::write( m_api, pData );
	}
	else
	{
		ASSERT( "Cannot write to unopened file" );
	}
}
//-------------------------------------------------------------------
//	Close
//-------------------------------------------------------------------
void FileWriter::Close()
{
	platform::file::close( m_api );
}

/****************************************************************************************
	FileReader class
*****************************************************************************************/

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
FileReader::FileReader()
{
	m_loaded = false;
}
//-------------------------------------------------------------------
FileReader::~FileReader()
{
}
//-------------------------------------------------------------------
//	Load
//-------------------------------------------------------------------
bool FileReader::Load(const String& pathName, eFileMode fileMode )
{
	platform::FileReadApi api;

	m_loaded = false;
	m_lines.Clear();

	if ( platform::file::openToRead( api, pathName.AsChar(), fileMode ) )
	{

		platform::file::read( api, m_lines, fileMode );
		platform::file::close( api );

		m_loaded = true;
		return true;
	}
	else
		return false;
}
//-------------------------------------------------------------------
//	GetLine
//-------------------------------------------------------------------
const FileLine& FileReader::GetLine(uint index)const
{
	if ( index > LineCount() )
	{
		ASSERT( "Index out of bounds" );
	}

	return *m_lines[index].Get();
}
//-------------------------------------------------------------------
//	LineCount
//-------------------------------------------------------------------
uint FileReader::LineCount()const
{
	if ( !m_loaded )
	{
		ASSERT( "File not loaded" );
		return 0;
	}

	return m_lines.Count();
}

