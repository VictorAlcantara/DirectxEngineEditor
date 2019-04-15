#include "../engine/AppMessageBox.h"
#include "Platform.h"


#ifdef WINDOWS

void AppMessageBox::Error(const char* message, const char* title)
{
	MessageBoxA( NULL, message,  title ? title : "Error", MB_OK | MB_ICONERROR );
}

void AppMessageBox::Alert(const char* message, const char* title)
{
	MessageBoxA( NULL, message, title ? title : "Alert", MB_OK | MB_ICONWARNING );
}

#endif
