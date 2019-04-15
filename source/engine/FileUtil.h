#pragma once

#include "Type.h"

enum class eFileOperation
{
	Append,
	Truncate,
};

enum class eFileMode
{
	Text,
	Binary,
};

class FileLine
{
	public:
		FileLine();
		FileLine(const char* pData, uint size);
		FileLine(const FileLine&);
		~FileLine();

		const char* GetData()const { return m_pData; }
		uint GetSize()const { return m_size; }

		FileLine& operator=(const FileLine&);

	private:
		void Clear();

		char* m_pData;
		uint m_size;
};
