#include "Input.h"
#include "../engine/Pointer.h"
#include "../engine/Exception.h"
#include "Type.h"
#include "../engine/CoreInterface.h"
#include "../engine/Debug.h"


//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Input::Input()
{
	Reset();
	m_initialized = false;
	m_capsLock = false;
}
//-------------------------------------------------------------------
Input::~Input()
{
	CHECK( !m_initialized );
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Input::Initialize()
{
	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	Reset();

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Input::Shutdown()
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	Reset();

	m_initialized = false;
}
//-------------------------------------------------------------------
//	UpdateAfterCoreLoop
//-------------------------------------------------------------------
void Input::UpdateAfterCoreLoop()
{
	for ( uint i = 0; i < KEY_COUNT; i++ )
	{
		switch ( m_keyState[i] )
		{
			case eKeyState::Press: 
				m_keyState[i] = eKeyState::Down;
			break;
			case eKeyState::Release: 
				m_keyState[i] = eKeyState::Up;
			break;
		}
	}

	if ( m_mouseLock )
	{
		g_pApplication->SetMousePos( m_mouseLockPos );
	}
}
//-------------------------------------------------------------------
//	Reset
//-------------------------------------------------------------------
void Input::Reset()
{
	for ( uint i = 0; i < KEY_COUNT; i++ )
		m_keyState[i] = eKeyState::Up;

	m_mousePos = math::Vec2();
	m_mouseLock = false;

	if ( g_pCore->IsInitialized() )
		ShowCursor( true );
}
//-------------------------------------------------------------------
//	LockMouse
//-------------------------------------------------------------------
void Input::LockMouse(bool lock)
{
	m_mouseLock = lock;

	if ( m_mouseLock )
		m_mouseLockPos = GetMousePos();
}
//-------------------------------------------------------------------
//	ShowCursor
//-------------------------------------------------------------------
void Input::ShowCursor(bool show)
{
	g_pApplication->ShowCursor( show );
}
//-------------------------------------------------------------------
//	OnKeyPress
//-------------------------------------------------------------------
void Input::OnKeyPress(eKey key)
{
	if ( IsKeyValid(key) && !IsKeyDown(key) )
	{
		m_keyState[(uint)key] = eKeyState::Press;
	}
}
//-------------------------------------------------------------------
//	OnKeyRelease
//-------------------------------------------------------------------
void Input::OnKeyRelease(eKey key)
{
	if ( IsKeyValid(key) )
	{
		m_keyState[(uint)key] = eKeyState::Release;
	}
}
//-------------------------------------------------------------------
//	OnMouseMove
//-------------------------------------------------------------------
void Input::OnMouseMove(int x, int y)
{
	m_mousePos.x = (float)x;
	m_mousePos.y = (float)y;
}
//-------------------------------------------------------------------
//	IsKeyValid
//-------------------------------------------------------------------
bool Input::IsKeyValid(eKey key)const
{
	if ( (uint)key > KEY_COUNT )
	{
		ASSERT( "Invalid key" );
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
//	IsKey
//-------------------------------------------------------------------
bool Input::IsKey(eKeyState keyState, eKey key)const
{
	if ( IsKeyValid(key) )
		return m_keyState[key] == keyState;

	return false;
}
//-------------------------------------------------------------------
//	IsKeyPress
//-------------------------------------------------------------------
bool Input::IsKeyPress(eKey key)const
{
	return IsKey( eKeyState::Press, key );
}
//-------------------------------------------------------------------
//	IsKeyDown
//-------------------------------------------------------------------
bool Input::IsKeyDown(eKey key)const
{
	return IsKey( eKeyState::Down, key );
}
//-------------------------------------------------------------------
//	IsKeyDownOrPress
//-------------------------------------------------------------------
bool Input::IsKeyDownOrPress(eKey key)const
{
	return IsKeyDown( key ) || IsKeyPress( key );
}
//-------------------------------------------------------------------
//	IsKeyRelease
//-------------------------------------------------------------------
bool Input::IsKeyRelease(eKey key)const
{
	return IsKey( eKeyState::Release, key );
}
//-------------------------------------------------------------------
//	IsKeyUp
//-------------------------------------------------------------------
bool Input::IsKeyUp(eKey key)const
{
	return IsKey( eKeyState::Up, key );
}
//-------------------------------------------------------------------
//	IsKeyUpOrRelease
//-------------------------------------------------------------------
bool Input::IsKeyUpOrRelease(eKey key)const
{
	return IsKeyUp( key ) || IsKeyRelease( key );
}
//-------------------------------------------------------------------
//	GetKeyState
//-------------------------------------------------------------------
eKeyState Input::GetKeyState(eKey key)const
{
	if ( IsKeyValid(key) )
		return m_keyState[key];

	return eKeyState::Up;
}
//-------------------------------------------------------------------
//	IsMouseLocked
//-------------------------------------------------------------------
bool Input::IsMouseLocked()const
{
	return m_mouseLock;
}
//-------------------------------------------------------------------
//	GetMousePos
//-------------------------------------------------------------------
math::Vec2 Input::GetMousePos()const
{
	return m_mousePos;
}
//-------------------------------------------------------------------
//	GetMousePosNormalized
//-------------------------------------------------------------------
math::Vec2 Input::GetMousePosNormalized()const
{
	return math::scaleInv( GetMousePos(), g_pRenderer->GetWindowViewport().GetDimension() );
}
//-------------------------------------------------------------------
//	GetMouseLockPos
//-------------------------------------------------------------------
math::Vec2 Input::GetMouseLockPos()const
{
	return m_mouseLockPos;
}
//-------------------------------------------------------------------
//	GetMouseLockPosNormalized
//-------------------------------------------------------------------
math::Vec2 Input::GetMouseLockPosNormalized()const
{
	return math::scaleInv( GetMouseLockPos(), g_pRenderer->GetWindowViewport().GetDimension() );
}
//-------------------------------------------------------------------
//	GetAllKeys
//-------------------------------------------------------------------
void Input::GetAllKeys(eKeyState state, List<eKey>& outResult)const
{
	for ( uint i = 0; i < KEY_COUNT; i++ )
	{
		if ( IsKey(state, (eKey)i) )
			outResult.AddLast( (eKey)i );
	}
}
//-------------------------------------------------------------------
//	MapToChar
//-------------------------------------------------------------------
char Input::MapToChar(eKey key)const
{
	bool isCapital = IsCapsLock();
	bool isShift = IsKeyDownOrPress( KEY_SHIFT );

	if ( math::between<uint>(key, '0', '9') )
	{
		static const char numberShift[10] = { ')', '!', '@', '#', '$', '%', '\0', '&', '*', '(' };
		
		return isShift ? numberShift[ string::charToInt((char)key) ] : (char)key;
	}
	else if ( math::between<uint>(key, 'A', 'Z') )
		return !(isCapital ^ isShift) ? string::lowerCase((char)key) : (char)key;
	else if ( key == KEY_SLASH )
		return isShift ? '?' : '/';
	else if ( key == KEY_MINUS )
		return isShift ? '_' : '-';
	else if ( key == KEY_EQUAL )
		return isShift ? '+' : '=';
	else if ( key == KEY_BRACKET_OPEN )
		return isShift ? '{' : '[';
	else if ( key == KEY_BRACKET_CLOSE )
		return isShift ? '}' : ']';
	else if ( key == KEY_SEMICOLON )
		return isShift ? ':' : ';';
	else if ( key == KEY_PERIOD )
		return isShift ? '>' : '.';
	else if ( key == KEY_COMMA )
		return isShift ? '<' : ',';
	else if ( key == KEY_SPACE )
		return ' ';
	else if ( key == KEY_QUOTE )
		return '\'';
	else if ( key == KEY_NUMERIC_SUBTRACT )
		return '-';
	else if ( key == KEY_NUMERIC_ADD )
		return '+';

	return '\0';
}
