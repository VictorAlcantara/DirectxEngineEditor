#pragma once

#include "Platform.h"

class IWorld;

class Application
{
	public:
		Application();
		~Application();

		void Initialize(platform::InitializationApi&, IWorld*);
		void Run();
		void Shutdown();

		void ShowCursor(bool);
		void SetCursorPos(uint x, uint y);

		bool IsCursorVisible()const;
	private:
		bool m_initialized;
		platform::ApplicationApi m_applicationApi;
		bool m_cursorVisible;
};
