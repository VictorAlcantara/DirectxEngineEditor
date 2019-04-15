#pragma once

class AppMessageBox
{
	public:
		static void Error(const char* message, const char* title = nullptr);
		static void Alert(const char* message, const char* title = nullptr);
};
