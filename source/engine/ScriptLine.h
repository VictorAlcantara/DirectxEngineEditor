#pragma once

#include "../engine/String.h"
#include "../engine/List.h"

class ScriptLine;

enum class eScriptType
{
	None,
	String,
	QuotedString,
	Number,
};

struct ScriptObject
{
	friend class ScriptLine;

	ScriptObject() : type(eScriptType::None) {}

	private:
		eScriptType type;
		String value;

};

class ScriptLine
{
	public:
		ScriptLine();
		ScriptLine(const String&);
		ScriptLine(const char*, uint size);

		void RemoveAt(uint);

		bool HasString(const String&, uint findsToSkip = 0)const;
		bool HasType(eScriptType, uint findsToSkip = 0)const;
		bool HasTypeSequence(eScriptType, uint sequenceCount, uint findsToSkip = 0)const;

		String Get(uint findsToSkip)const;
		String GetByType(eScriptType, uint findsToSkip = 0)const;
		String GetByTypeSequence(eScriptType, uint sequenceCount, char separator = ' ', uint findsToSkip = 0)const;

		float GetFloat(uint findsToSkip = 0)const;
		int GetInt(uint findsToSkip = 0)const;
		String GetQuotedString(uint findsToSkip = 0)const;

		bool SetFloat(const String& text, float& outResult)const;
		bool SetFloatArray(const String& text, float* pOutResult, uint arrayCount, bool repeatLast)const;
		bool SetInt(const String& text, int& outResult)const;
		bool SetIntArray(const String& text, int* pOutResult, uint arrayCount)const;
		bool SetBool(const String& text, bool& outResult)const;
		bool SetQuotedString(const String& text, String& outQuotedString)const;

	private:
		bool GetBy(const String* text, eScriptType*, uint findsToSkip, String* result)const;
		ListIteratorConst<ScriptObject> GetAt(const String& text)const;
		void LineToScriptObject(const String&);

		List<ScriptObject> m_objects;
};
