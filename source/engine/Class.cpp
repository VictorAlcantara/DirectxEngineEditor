#include "Class.h"
#include "../engine/Exception.h"

#define CLASS_MODULE(className) \
	className::~className() \
	{ \
		CHECK( !m_initialized ); \
	} \
	void className::Initialize() \
	{ \
		if ( !IsInitialized() ) \
		{ \
			OnInitialize(); \
			m_initialized = true; \
		} \
		else \
		{ \
			ASSERT( "Already initialized" ); \
		} \
	} \
	void className::Shutdown() \
	{ \
		if ( IsInitialized() ) \
		{ \
			OnShutdown(); \
			m_initialized = false; \
		} \
		else \
		{ \
			ASSERT( "Not initialized" ); \
		} \
	}

CLASS_MODULE( PublicModule );
CLASS_MODULE( ProtectedModule );
