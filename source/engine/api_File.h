#pragma once

#include "Platform.h"
#include "Array.h"
#include "FileUtil.h"
#include "../engine/String.h"
#include "../engine/List.h"

namespace platform
{
	namespace file
	{
		bool exists(const String& filePath);

		bool openToWrite(FileWriteApi&, const char* path, eFileOperation);
		void write(FileWriteApi&, const char* pData);
		void close(FileWriteApi&);

		bool openToRead(FileReadApi&, const char* path, eFileMode);
		void read(FileReadApi&, FixedArray<SmartPtr<FileLine>>&, eFileMode);
		void close(FileReadApi&);

		bool readAllFilesInDirectory(const wchar*, List<StringW>&);
		bool readAllFilesInDirectory(const char*, List<String>&);
	};
};
