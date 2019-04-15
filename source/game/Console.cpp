#include "Console.h"
#include "../engine/Entity.h"
#include "../engine/Font.h"
#include "../engine/CoreInterface.h"
#include "../engine/Primitive.h"
#include "../engine/FontEffect.h"
#include "../engine/Effect.h"
#include "../engine/ScriptReader.h"
#include "GameGlobal.h"
#include "../shared/Command.h"
#include "../shared/ProcessShared.h"

/****************************************************************************************
	namespace
*****************************************************************************************/

namespace consoleGlobal
{
	namespace action
	{
		namespace context
		{
			const HashString CONSOLE_CONTEXT = "ActionContextConsole";
		};

		const HashString TOGGLE = "ActionContextToggle";
		const HashString HIDE = "ActionContextHide";
		const HashString ENTER = "ActionContextEnter";
		const HashString TEXT_DELETE = "ActionContextTextDelete";
		const HashString TEXT_BACKSPACE = "ActionContextTextBackspace";
		const HashString LINE_PREV = "ActionContextLinePrev";
		const HashString LINE_NEXT = "ActionContextLineNext";
	};
};

/****************************************************************************************
	Console definition
*****************************************************************************************/

const math::Vec4 Console::s_logColors[] = {
	math::normalizeRgb( 0, 180, 50 ),
	math::normalizeRgb( 240, 200, 50 ),
	math::normalizeRgb( 230, 50, 20 ),
};

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Console::Console()
{
	m_inUse = false;
	m_logCurrent = 0;
	m_logDuration = 5.0f;
	m_logTimer = 0.0f;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void Console::OnShutdown()
{
	m_pEffect->Shutdown();
	m_pEffect = nullptr;

	m_pFontEffect->Shutdown();
	m_pFontEffect = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void Console::OnInitialize()
{
	m_hModel = primitive::geometry::createSquare();

	m_pEffect = new Effect_Color;
	m_pEffect->Initialize();
	m_pEffect->GetConfig().renderer.blendMode = eBlendMode::Alpha;
	m_pEffect->GetConfig().renderer.depthOperation = eDepthOperation::Always;

	m_pFontEffect = new FontEffect_Color;
	m_pFontEffect->Initialize();
	m_pFontEffect->SetCharSpaceProportion( 0.05f );

	m_hFont = g_pResource->GetFontMngr()->LoadFont( "consolas" );

	m_inUse = false;

	KeyMapContext* pContext = g_pKeyMap->CreateContext( consoleGlobal::action::context::CONSOLE_CONTEXT );
	pContext->AddAction( consoleGlobal::action::ENTER, KEY_RETURN, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::HIDE, KEY_ESCAPE, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::LINE_NEXT, KEY_DOWN, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::LINE_PREV, KEY_UP, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::TEXT_BACKSPACE, KEY_BACKSPACE, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::TEXT_DELETE, KEY_DELETE, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::TOGGLE, KEY_CTRL, (uint)eKeyState::Down )
		->Link( KEY_QUOTE, (uint)eKeyState::Press );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void Console::Update()
{
	if ( g_pKeyMap->CanPerform(consoleGlobal::action::TOGGLE) )
	{
		Show( !m_inUse );
	}
	else if ( g_pKeyMap->CanPerform(consoleGlobal::action::HIDE) )
	{
		Show( false );
	}
	else if ( g_pKeyMap->CanPerform(consoleGlobal::action::ENTER) )
	{
		ExecuteCommand( m_command );
		m_command = "";
	}
	else if ( g_pKeyMap->CanPerform(consoleGlobal::action::TEXT_BACKSPACE) )
	{
		if ( m_command.Length() == 1 )
			m_command = "";
		else if ( m_command.Length() > 1 )
			m_command = string::copy( m_command, 0 , m_command.Length() - 1 );
	}
	else if ( g_pKeyMap->CanPerform(consoleGlobal::action::TEXT_DELETE) )
	{
		m_iterator.Invalidate();
		m_command = "";
	}
	else if ( m_inUse )
	{
		bool moveUpDown = true;

		if ( g_pKeyMap->CanPerform(consoleGlobal::action::LINE_PREV) )
		{
			if ( m_iterator )
				m_iterator--;
			if ( !m_iterator )
				m_iterator = m_queue.GetIteratorLast();
		}
		else if ( g_pKeyMap->CanPerform(consoleGlobal::action::LINE_NEXT) )
		{
			if ( m_iterator )
				m_iterator++;
			if ( !m_iterator )
				m_iterator = m_queue.GetIterator();
		}
		else
			moveUpDown = false;

		if ( moveUpDown )
			m_command = m_iterator ? *m_iterator : "";
		else
		{
			List<eKey> keysPressed;
			g_pInput->GetAllKeys(eKeyState::Press, keysPressed);

			for ( auto it = keysPressed.GetIterator(); it; it++ )
				m_command += g_pInput->MapToChar(*it);
		}
	}
}
//-------------------------------------------------------------------
//	Show
//-------------------------------------------------------------------
void Console::Show(bool b)
{
	m_command = "";
	m_iterator.Invalidate();
	m_inUse = b;

	if ( m_inUse )
	{
		g_pKeyMap->QueueActiveContexts();
		g_pKeyMap->DeactivateAllContexts();
		g_pKeyMap->UseContext( consoleGlobal::action::context::CONSOLE_CONTEXT );
	}
	else
	{
		List<const KeyMapContext*> contexts;
		g_pKeyMap->GetQueuedContexts( contexts );

		for ( auto it = contexts.GetIterator(); it; it++ )
		{
			if ( (*it)->GetName() != consoleGlobal::action::context::CONSOLE_CONTEXT )
			{
				SmartPtr<Process_ChangeKeyMapContext> pProcess = new Process_ChangeKeyMapContext;
				pProcess->SetContext( (*it)->GetName(), true );

				g_pProcess->Add( pProcess.As<Process>() );
			}
		}

		g_pKeyMap->EndUseContext( consoleGlobal::action::context::CONSOLE_CONTEXT );
	}
}
//-------------------------------------------------------------------
//	ExecuteCommand
//-------------------------------------------------------------------
void Console::ExecuteCommand(const String& cmd)
{
	bool addToQueue = m_pCommand->Execute( cmd );

	if ( addToQueue )
	{
		for ( auto it = m_queue.GetIterator(); it; it++ )
		{
			if (  string::equal( *it, cmd, false ) )
				addToQueue = false;
		}

		if ( addToQueue )
			m_queue.AddLast( cmd );
	}
	else
	{
		AddLog( m_pCommand->GetLastError(), eConsoleLog::Warning );
	}
	
	m_iterator.Invalidate();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void Console::Render()
{
	math::Vec2 viewportDimension = g_pRenderer->GetWindowViewport().GetDimension();
	math::Vec2 textPadding( 10.0f, -2.0f );
	uint consoleFontSize = 18;
	float fontAdjust = 1.15f;
	float consoleFontSizeAdjusted = (float)consoleFontSize * fontAdjust; 
	math::Vec3 consoleBoxScale = math::Vec3(viewportDimension.x * 0.5f, consoleFontSizeAdjusted* 0.5f, 1.0f);

	if ( m_inUse )
	{
		math::Vec3 pos = math::screenCoordToCenter( math::Vec2(consoleBoxScale.x, consoleBoxScale.y), viewportDimension ).Xyz(0.1f);

		math::Mtx44 world = math::matrixScale( consoleBoxScale ) * math::matrixTranslate( pos );
		math::Mtx44 view;
		math::Mtx44 proj = math::matrixOrtho( viewportDimension );

		// Box
		m_pEffect->GetConfig().color4 = math::normalizeRgb( 50, 200 );
		m_pEffect->Render( m_hModel, world, view, proj );

		// Text
		m_pFontEffect->SetCharSize( consoleFontSize );
		m_pFontEffect->SetColor( math::normalizeRgb(200) );
		m_pFontEffect->SetScreenPos( textPadding );
		m_pFontEffect->SetZ( pos.z * 0.9f );
		m_pFontEffect->Render( m_command, m_hFont.GetPtr() );
	}

	// Logs
	if ( m_logTimer > g_pCore->GetTime() )
	{
		uint logFontSize = 16;
		float logFontSizeAdjusted = (float)logFontSize * fontAdjust;
		float consoleBoxPadding = m_inUse ? consoleBoxScale.y * 2.0f : 0.0f;
		m_pFontEffect->SetCharSize( logFontSize );
		math::Mtx44 view;
		math::Mtx44 proj = math::matrixOrtho( viewportDimension );
		math::Vec3 logBoxScale( consoleBoxScale.x, logFontSizeAdjusted * 0.5f, 1.0f );

		m_pEffect->GetConfig().color4 = math::normalizeRgb( 80, 150 );

		for ( uint i = 0; i < math::min<uint>( m_logCurrent, ARRAY_COUNT(m_logs) ); i++ )
		{
			math::Vec2 textPos( 0, consoleBoxPadding + (float)logFontSizeAdjusted * i );

			// Box
			{
				math::Vec3 pos = math::screenCoordToCenter( logBoxScale.x, textPos.y + logBoxScale.y, viewportDimension ).Xyz(0.1f);

				math::Mtx44 world = math::matrixScale( logBoxScale ) * math::matrixTranslate( pos );

				m_pEffect->Render( m_hModel, world, view, proj );
			}

			// Text
			m_pFontEffect->SetColor( s_logColors[(uint)m_logs[i].type] );
			m_pFontEffect->SetScreenPos( textPos + textPadding );
			m_pFontEffect->Render( m_logs[i].msg, m_hFont.GetPtr() );
		}
	}
	else
	{
		m_logCurrent = 0;
		for ( uint i = 0; i < ARRAY_COUNT(m_logs); i++ )
			m_logs[i] = ConsoleLog();
	}
}
//-------------------------------------------------------------------
//	SetCommand
//-------------------------------------------------------------------
void Console::SetCommand(SmartPtr<ICommand> pCommand)
{
	m_pCommand = pCommand;
}
//-------------------------------------------------------------------
//	AddLog
//-------------------------------------------------------------------
void Console::AddLog(const String& s, eConsoleLog type)
{
	ConsoleLog* pLog = &m_logs[m_logCurrent % ARRAY_COUNT(m_logs)];
	pLog->msg = s;
	pLog->type = type;
	m_logCurrent++;
	m_logTimer = g_pCore->GetTime() + m_logDuration;
}
