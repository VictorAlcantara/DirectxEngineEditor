#include "../engine/Exception.h"
#include "../engine/AppMessageBox.h"
#include "../engine/Pointer.h"
#include "../engine/String.h"

Exception::Exception(const char* message)
{
	uint len = string::length(message);
	m_message = new char[len];
	memoryCopy( message, m_message, len );
}

Exception::~Exception()
{
	safeArrayRelease( m_message );
}

const char * Exception::what() const throw()
{
	return m_message;
}
