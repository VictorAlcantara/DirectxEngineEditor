#include "api_File.h"
#include "FileUtil.h"
#include "../engine/Exception.h"
#include "Debug.h"

#ifdef WINDOWS

#include <iostream>

//-------------------------------------------------------------------
//	readAsText
//-------------------------------------------------------------------
void readAsText(platform::FileReadApi& api, FixedArray<SmartPtr<FileLine>>& lines)
{
	#ifdef DEBUG_FILE_CHECK_OPEN
		CHECK( api.file.is_open() );
	#endif

	List<SmartPtr<FileLine>> listFileLines;

	api.file.seekg( std::ios::beg );
	std::string line;
	while ( std::getline(api.file, line) )
		listFileLines.AddLast( new FileLine(line.c_str(), line.length()) );

	CHECK( lines.Count() == 0 );
	lines.Clear( listFileLines.Count() );

	for ( auto it = listFileLines.GetIterator(); it; it++ )
		lines.Add( *it );
}
//-------------------------------------------------------------------
//	readAsBinary
//-------------------------------------------------------------------
void readAsBinary(platform::FileReadApi& api, FixedArray<SmartPtr<FileLine>>& lines)
{
	#ifdef DEBUG_FILE_CHECK_OPEN
		CHECK( api.pFile );
	#endif

	uint dataSize = 0;

	fseek( api.pFile, 0, SEEK_END );
	dataSize = ftell( api.pFile );
	fseek( api.pFile, 0, SEEK_SET );

	uchar* pData = new uchar[dataSize];

	fread_s( pData, dataSize, dataSize, 1, api.pFile );

	CHECK( lines.Count() == 0 );
	lines.Clear( 1 );

	lines.Add( new FileLine((const char*)pData, dataSize) );

	safeArrayRelease( pData );
}
//-------------------------------------------------------------------
//	exists
//-------------------------------------------------------------------
bool platform::file::exists(const String& filePath)
{
	FILE* pFile = nullptr;

	if ( fopen_s( &pFile, filePath.AsChar(), "r" ) == 0 ) 
	{
		fclose(pFile);
		return true;
	}

	return false;
}
//-------------------------------------------------------------------
//	openToWrite
//-------------------------------------------------------------------
bool platform::file::openToWrite(FileWriteApi& api, const char* path, eFileOperation fileOperation)
{
	uint mode = 0;

	switch (fileOperation)
	{
		case eFileOperation::Append: mode = std::ios::app; break;
		case eFileOperation::Truncate: mode = std::ios::trunc; break;
	}

	api.file.open( path, mode );

	return api.file.is_open();
}
//-------------------------------------------------------------------
//	write
//-------------------------------------------------------------------
void platform::file::write(FileWriteApi& api, const char* pData)
{
	api.file << pData;
}
//-------------------------------------------------------------------
//	close
//-------------------------------------------------------------------
void platform::file::close(FileWriteApi& api)
{
	if ( api.file.is_open() )
		api.file.close();
}
//-------------------------------------------------------------------
//	openToRead
//-------------------------------------------------------------------
bool platform::file::openToRead(FileReadApi& api, const char* path, eFileMode fileMode)
{
	if ( fileMode == eFileMode::Text )
	{
		api.file.open( path, std::ios::in );

		return api.file.is_open();
	}
	else
	{
		fopen_s( &api.pFile, path, "rb" );

		return api.pFile != nullptr;
	}
}
//-------------------------------------------------------------------
//	read
//-------------------------------------------------------------------
void platform::file::read(FileReadApi& api, FixedArray<SmartPtr<FileLine>>& lines, eFileMode fileMode)
{
	if ( fileMode == eFileMode::Text )
		readAsText( api, lines );
	else
		readAsBinary( api, lines );
}
//-------------------------------------------------------------------
//	close
//-------------------------------------------------------------------
void platform::file::close(FileReadApi& api)
{
	if ( api.file.is_open() )
		api.file.close();
	if ( api.pFile )
		fclose( api.pFile );
}
//-------------------------------------------------------------------
//	readAllFilesInDirectory
//-------------------------------------------------------------------
bool platform::file::readAllFilesInDirectory(const wchar* directory, List<StringW>& outResult)
{
	WIN32_FIND_DATAW findData;
	HANDLE handle;

	handle = FindFirstFileW( directory, &findData );

	if ( INVALID_HANDLE_VALUE )
		return false;

	outResult.AddLast( findData.cFileName );

	while ( FindNextFileW(handle, &findData) )
	{
		outResult.AddLast( findData.cFileName );
	}

	FindClose( handle );

	return true;
}
//-------------------------------------------------------------------
bool platform::file::readAllFilesInDirectory(const char* directory, List<String>& outResult)
{
	WIN32_FIND_DATA findData;
	HANDLE handle;
	String directoryFormatted(directory);

	if ( directoryFormatted.Length() > 0 && directoryFormatted[directoryFormatted.Length() - 1] != '*' )
		directoryFormatted += '*';

	handle = FindFirstFile( directoryFormatted.AsChar(), &findData );

	if ( handle == INVALID_HANDLE_VALUE )
		return false;

	do
	{
		if ( findData.cFileName[0] != '.' )
			outResult.AddLast( findData.cFileName );
	} 
	while ( FindNextFile(handle, &findData) );

	FindClose( handle );

	return true;
}

#endif