#pragma once

#include "Platform.h"

#if defined(WINDOWS)

#include <exception>
#include <cassert>


class Exception : public std::exception
{
	public:
		Exception(const char*);
		~Exception();
		virtual const char* what()const throw();

	private:
		char* m_message;
};

#ifdef DEBUG_MODE
	#define CHECK(condition) { if (!(condition)) assert((condition)); }
	#define ASSERT(msg) assert(false && (msg))
#else
	#define CHECK(condition)
	#define ASSERT(msg)
#endif

#define THROW(message) { ASSERT("Exception thrown"); throw Exception(message); }

#endif