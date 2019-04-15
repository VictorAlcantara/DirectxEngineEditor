#pragma once

#include "../engine/File.h"
#include "Array.h"
#include "../engine/ScriptLine.h"
#include "../engine/Pointer.h"

class ScriptReader
{
	public:
		ScriptReader() = default;
		ScriptReader(const FileReader&);
		ScriptReader(const String&);

		void AddLine(const String&);

		uint GetLineCount()const;

		ScriptLine* GetLine(uint);
		const ScriptLine* GetLine(uint)const;

		bool HasLine(uint)const;

	private:
		void CreateIndex();

		List<SmartPtr<ScriptLine>> m_lines;
		SimplePtr<FixedArray<ScriptLine*>> m_index;
};
