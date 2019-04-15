#pragma once

#include "../engine/String.h"
#include "Array.h"
#include "FileUtil.h"
#include "../engine/Pointer.h"

/****************************************************************************************
	FileWriter
*****************************************************************************************/

class FileWriter
{
	public:
		FileWriter();
		~FileWriter();

		bool Open(const String& pathName, eFileOperation);
		void Write(const char*);
		void Close();

		bool IsOpen()const { return m_open; }

	private:
		platform::FileWriteApi m_api;
		bool m_open;
};

/****************************************************************************************
	FileReader
*****************************************************************************************/

class FileReader
{
	public:
		FileReader();
		~FileReader();

		bool Load(const String& pathName, eFileMode);
		const FileLine& GetLine(uint)const;
		uint LineCount()const;

		bool IsLoaded()const { return m_loaded; }

	private:
		FixedArray<SmartPtr<FileLine>> m_lines;
		bool m_loaded;
};
