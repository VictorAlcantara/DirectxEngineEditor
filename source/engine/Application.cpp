#include "Application.h"

#ifdef WIN32_DX11

#include "../engine/AppMessageBox.h"
#include "Pointer.h"
#include "../engine/Exception.h"
#include "../engine/CoreInterface.h"
#include "../engine/WorldInterface.h"
#include "Math.h"
#include "../engine/Debug.h"
#include "String.h"

eKey winKeyToEngineKey(WPARAM wParam);

//-------------------------------------------------------------------
//	windowsMessageHandle
//-------------------------------------------------------------------
LRESULT CALLBACK windowsMessageHandle(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool isCoreInitialized = g_pCore && g_pCore->IsInitialized();

	if ( isCoreInitialized )
	{
		switch ( msg )
		{
			case WM_CLOSE:
				PostQuitMessage( 0 );
			break;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				g_pInput->OnKeyPress( winKeyToEngineKey(wParam) );
			break;

			case WM_KEYUP:
			case WM_SYSKEYUP:
				g_pInput->OnKeyRelease( winKeyToEngineKey(wParam) );
				//debugToOutput( (String("Char ") + (char)wParam + String(" Ascii ") + string::intToStr((int)wParam)).AsChar() );
				
				// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getkeystate#return-value
				// Shouldn't this be in a better place?
				g_pInput->SetCapsLock( GetKeyState(VK_CAPITAL) & 0x0001 );
			break;

			case WM_SETFOCUS:
			case WM_KILLFOCUS:
				g_pInput->Reset();
			break;

			case WM_MOUSEMOVE:
				POINTS pos = MAKEPOINTS(lParam);
				g_pInput->OnMouseMove( pos.x, pos.y );
			break;

			case WM_LBUTTONDOWN:
				g_pInput->OnKeyPress( eKey::KEY_MOUSE_LEFT );
			break;

			case WM_RBUTTONDOWN:
				g_pInput->OnKeyPress( eKey::KEY_MOUSE_RIGHT );
			break;

			case WM_LBUTTONUP:
				g_pInput->OnKeyRelease( eKey::KEY_MOUSE_LEFT );
			break;

			case WM_RBUTTONUP:
				g_pInput->OnKeyRelease( eKey::KEY_MOUSE_RIGHT );
			break;

			default:
				return ::DefWindowProc( hWnd, msg, wParam, lParam );
		}
	}

	if ( msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || !isCoreInitialized )
		return ::DefWindowProc( hWnd, msg, wParam, lParam );

	return 0;
}
//-------------------------------------------------------------------
//	winKeyToEngineKey
//-------------------------------------------------------------------
eKey winKeyToEngineKey(WPARAM wParam)
{
	if ( math::between( (char)wParam, 'A', 'Z' ) )
		return (eKey)wParam;

	if ( math::between( (char)wParam, '0', '9' ) )
		return (eKey)wParam;

	switch ( wParam )
	{
		case VK_CONTROL: return KEY_CTRL;
		case VK_MENU: return KEY_ALT;
		case VK_SHIFT: return KEY_SHIFT;
		case VK_ESCAPE: return KEY_ESCAPE;
		case VK_RETURN: return KEY_RETURN;
		case VK_SPACE: return KEY_SPACE;
		case VK_BACK: return KEY_BACKSPACE;

		case VK_UP: return KEY_UP;
		case VK_DOWN: return KEY_DOWN;
		case VK_RIGHT: return KEY_RIGHT;
		case VK_LEFT: return KEY_LEFT;

		case VK_OEM_2: return KEY_SEMICOLON;
		case 193: return KEY_SLASH;

		case VK_OEM_PLUS: return KEY_EQUAL;

		case VK_ADD: return KEY_NUMERIC_ADD;

		case VK_SUBTRACT: return KEY_NUMERIC_SUBTRACT;

		case VK_OEM_MINUS: return KEY_MINUS;

		case VK_OEM_6:
		case '{':
		case '[': return KEY_BRACKET_OPEN;

		case VK_OEM_5:
		case '}':
		case ']': return KEY_BRACKET_CLOSE;

		case '\"':
		case VK_OEM_3:
		case VK_OEM_7: return KEY_QUOTE;

		case VK_OEM_COMMA: return KEY_COMMA;
		case VK_OEM_PERIOD: return KEY_PERIOD;

		case VK_DELETE: return KEY_DELETE;

		case VK_NUMPAD0: return KEY_0;
		case VK_NUMPAD1: return KEY_1;
		case VK_NUMPAD2: return KEY_2;
		case VK_NUMPAD3: return KEY_3;
		case VK_NUMPAD4: return KEY_4;
		case VK_NUMPAD5: return KEY_5;
		case VK_NUMPAD6: return KEY_6;
		case VK_NUMPAD7: return KEY_7;
		case VK_NUMPAD8: return KEY_8;
		case VK_NUMPAD9: return KEY_9;

		case VK_F1: return KEY_F1;
		case VK_F2: return KEY_F2;
		case VK_F3: return KEY_F3;
		case VK_F4: return KEY_F4;
		case VK_F5: return KEY_F5;
		case VK_F6: return KEY_F6;
		case VK_F7: return KEY_F7;
		case VK_F8: return KEY_F8;
		case VK_F9: return KEY_F9;
		case VK_F10: return KEY_F10;
		case VK_F11: return KEY_F11;

		case VK_HOME: return KEY_HOME;
		case VK_END: return KEY_END;
		case VK_CAPITAL: return KEY_CAPITAL;
	}

	return KEY_NONE;
}

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Application::Application()
{
	m_applicationApi = {};
	m_initialized = false;
}
//-------------------------------------------------------------------
Application::~Application()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Application::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	g_pCore->Shutdown();

	if ( m_applicationApi.hWnd )
	{
		DestroyWindow( m_applicationApi.hWnd );
		UnregisterClass( m_applicationApi.className, m_applicationApi.hInstance );
	}

	m_applicationApi = {};
	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Application::Initialize(platform::InitializationApi& initializationApi, IWorld* pWorld)
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	m_applicationApi.hInstance = initializationApi.hInstance;
	m_applicationApi.windowName = "Demo";
	m_applicationApi.className = "Demo";
	m_applicationApi.fps = 60;
	m_applicationApi.width = 1024;
	m_applicationApi.height = 768;

	WNDCLASSEX wndClass;

	ZeroMemory( &wndClass, sizeof( WNDCLASSEX ) );
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = windowsMessageHandle;
	wndClass.hInstance = m_applicationApi.hInstance;
	wndClass.lpszClassName = m_applicationApi.className;
	wndClass.cbSize = sizeof( WNDCLASSEX );
	wndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if ( !RegisterClassEx( &wndClass ) )
		THROW( "Failed registering class" );

	m_applicationApi.hWnd = CreateWindowEx( WS_EX_APPWINDOW,
		m_applicationApi.className, m_applicationApi.windowName,
		WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_applicationApi.width, m_applicationApi.height,
		NULL, NULL, m_applicationApi.hInstance, nullptr );

	if ( FAILED( m_applicationApi.hWnd ) )
		THROW( "Failed creating window" );

	ShowWindow( m_applicationApi.hWnd, SW_SHOW );
	UpdateWindow( m_applicationApi.hWnd );
	SetForegroundWindow( m_applicationApi.hWnd );
	SetFocus( m_applicationApi.hWnd );

	g_pCore->Initialize( m_applicationApi, this );
	g_pCore->ChangeWorld( pWorld );
	ShowCursor( true );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Run
//-------------------------------------------------------------------
void Application::Run()
{
	MSG msg = {};
	bool done = false;

	while ( !done )
	{
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if ( msg.message == WM_QUIT )
		{
			done = true;
		}

		g_pCore->Loop();
	}
}
//-------------------------------------------------------------------
//	ShowCursor
//-------------------------------------------------------------------
void Application::ShowCursor(bool show)
{
	int result = 0;

	do
	{
		result = ::ShowCursor( show ? TRUE : FALSE );
	} 
	while ( show ? result <= 0 : result >= 0 ); // I've never seen a while like this one... hope I'm not making any programming sin :/ But looks cute ^^

	m_cursorVisible = show;
}
//-------------------------------------------------------------------
//	SetMousePos
//-------------------------------------------------------------------
void Application::SetCursorPos(uint x, uint y)
{
	POINT cursor;
	cursor.x = (LONG)x;
	cursor.y = (LONG)y;

	::ClientToScreen( m_applicationApi.hWnd, &cursor );
	::SetCursorPos( (int)cursor.x, (int)cursor.y );
}
//-------------------------------------------------------------------
//	IsCursorVisible
//-------------------------------------------------------------------
bool Application::IsCursorVisible()const
{
	return m_cursorVisible;
}

#endif
