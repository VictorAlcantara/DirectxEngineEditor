#pragma once

#include "CoreModule.h"
#include "Vector.h"
#include "../engine/List.h"

enum class eKeyState
{
	Press = 1 << 0, // just pressed
	Down = 1 << 1,
	Release = 1 << 2, // just released
	Up = 1 << 3,
};

enum eKey
{
	KEY_NONE = 0,

	KEY_SPACE = (int)' ',
	KEY_QUOTE = (int)'\'',
	KEY_COMMA = (int)',',
	KEY_MINUS = (int)'-',
	KEY_PERIOD = (int)'.',
	KEY_SLASH = (int)'/',

	KEY_0 = (int)'0',
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_SEMICOLON = (int)';',
	KEY_EQUAL = (int)'=',

	KEY_A = (int)'A',
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_BRACKET_OPEN = (int)'[',
	KEY_BRACKET_CLOSE = (int)']',

	KEY_NUMERIC_ADD,
	KEY_NUMERIC_SUBTRACT,
	KEY_CTRL,
	KEY_ALT,
	KEY_SHIFT,
	KEY_ESCAPE,
	KEY_RETURN,
	KEY_DELETE,
	KEY_BACKSPACE,
	KEY_HOME,
	KEY_END,
	KEY_CAPITAL,

	KEY_UP,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_LEFT,

	KEY_MOUSE_LEFT,
	KEY_MOUSE_RIGHT,

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	
	KEY_COUNT // MUST ALWAYS BE THE LAST - used for indexing
};

class Input
{
	CORE_MODULE;

	public:
		~Input();

		void OnKeyPress(eKey);
		void OnKeyRelease(eKey);
		void ToggleCapsLock() { m_capsLock = !m_capsLock; }
		void SetCapsLock(bool b) { m_capsLock = b; }

		void OnMouseMove(int x, int y);

		bool IsKey(eKeyState keyState, eKey key)const;
		bool IsKeyPress(eKey)const;
		bool IsKeyDown(eKey)const;
		bool IsKeyDownOrPress(eKey)const;
		bool IsKeyRelease(eKey)const;
		bool IsKeyUp(eKey)const;
		bool IsKeyUpOrRelease(eKey)const;
		eKeyState GetKeyState(eKey)const;
		bool IsCapsLock()const { return m_capsLock; }
		bool IsMouseLocked()const;
		math::Vec2 GetMousePos()const;
		math::Vec2 GetMousePosNormalized()const;
		math::Vec2 GetMouseLockPos()const;
		math::Vec2 GetMouseLockPosNormalized()const;
		void GetAllKeys(eKeyState, List<eKey>& outResult)const;

		char MapToChar(eKey)const;

		void Reset();
		void ShowCursor(bool);
		void LockMouse(bool);

	private:
		Input();

		void Initialize();
		void Shutdown();
		void UpdateAfterCoreLoop();

		bool IsKeyValid(eKey)const;

		bool m_initialized;

		bool m_capsLock;
		eKeyState m_keyState[KEY_COUNT];

		math::Vec2 m_mousePos;
		math::Vec2 m_mouseLockPos;
		bool m_mouseLock;
};
