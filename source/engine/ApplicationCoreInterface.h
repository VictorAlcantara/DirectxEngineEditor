#pragma once

#include "../engine/Class.h"
#include "../engine/Math.h"

class Application;

class ApplicationCoreInterface : public NonCopyable
{
	public:
		ApplicationCoreInterface() = delete;
		ApplicationCoreInterface(Application*);

		void ShowCursor(bool);
		void SetMousePos(const math::Vec2&);

		bool IsCursorVisible();

	private:
		Application* m_pApplication;
};
