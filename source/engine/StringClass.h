#pragma once

#include "Platform.h"

#ifdef WINDOWS

class String
{
	public:
		String();
		String(char);
		String(const char*, uint size);
		String(const String&, uint size);
		String(const char*);
		String(const wchar*);
		String(const String&);
		String(const String&, uint startAtIndex, uint charsToCopyCount);

		String& operator=(const String&);
		String& operator+=(const String&);
		bool operator==(const String&)const;
		const char& operator[](uint)const;
		char& operator[](uint);

		const char* AsChar()const;

		uint Length()const;
		char Last()const;
		void SetAt(char, uint index);

	private:
		void ValidateIndex(uint)const;

		std::string m_sData;
};

class StringW
{
	public:
		StringW();
		StringW(const wchar*);
		StringW(const StringW&, uint startAtIndex, uint charsToCopyCount);

		StringW& operator=(const StringW&);
		StringW& operator+=(const StringW&);
		bool operator==(const StringW&)const;
		wchar operator[](uint)const;

		const wchar* AsWchar()const;

		uint Length()const;

	private:
		void ValidateIndex(uint)const;

		std::wstring m_sData;
};

#endif

class HashString
{
	public:
		HashString();
		HashString(const char*);
		HashString(const String&);
		HashString(const HashString&);

		const String& GetString()const { return m_data; }
		uint GetHash()const { return m_hash; }
		bool IsValid()const { return m_hash != 0 || m_data.Length() > 0; }


		HashString& operator=(const HashString&);

	private:
		uint GenerateHash(const String&);

		String m_data;
		uint m_hash;
};

String operator+(const String& s1, const String& s2);
StringW operator+(const StringW& s1, const StringW& s2);

bool operator==(uint, const HashString&);
bool operator==(const HashString&, uint);
bool operator==(const HashString&, const HashString&);
bool operator!=(uint, const HashString&);
bool operator!=(const HashString&, uint);
bool operator!=(const HashString&, const HashString&);
