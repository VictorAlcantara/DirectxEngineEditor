#include "GameWorld.h"
#include "../engine/CoreInterface.h"
#include "../editor/EditorWorld.h"
#include "../shared/Loader.h"
#include "../engine/Path.h"
#include "../engine/Primitive.h"
#include "GameEntityFinder.h"
#include "GameEvent.h"
#include "GameGui.h"
#include "GameCommand.h"
#include "Console.h"
#include "Entity_Player.h"
#include "Entity_Door.h"
#include "Entity_Talk.h"
#include "GameGlobal.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GameWorld::GameWorld()
{
	m_isLoadScheduled = false;
	m_initialized = false;
}
//-------------------------------------------------------------------
GameWorld::~GameWorld()
{
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void GameWorld::Shutdown(WorldInfo* pInfo)
{
	if ( pInfo )
		pInfo->mapName = m_mapName;

	ShutdownListener();

	g_pKeyMap->Clear();

	g_pProcess->Clear();

	m_pConsole->Shutdown();
	m_pConsole = nullptr;

	m_pEffectScene->Shutdown();
	m_pEffectScene = nullptr;

	m_pCameraManager->Shutdown();
	m_pCameraManager = nullptr;

	g_pEntity->Clear();
	g_pScene->Clear();

	m_pCommand->Shutdown();
	m_pCommand = nullptr;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void GameWorld::Initialize(const WorldInfo* pInfo)
{
	InitializeListener();

	m_pCommand = new GameCommand;
	m_pCommand->Initialize();
	m_pCommand->SetWorld( this );

	g_pEntity->SetEntityFinder( new GameEntityFinder );

	m_pCameraManager = new CameraManager;
	m_pCameraManager->Initialize();

	m_pConsole = new Console;
	m_pConsole->Initialize();
	m_pConsole->SetCommand( m_pCommand.As<ICommand>() );

	m_pGui = new GameGui;
	g_pProcess->Add( m_pGui.As<Process>() );

	m_hSceneQuad = primitive::geometry::createSquare();

	m_pEffectScene = new Effect_Texture;
	m_pEffectScene->Initialize();

	KeyMapContext* pContext = g_pKeyMap->CreateContext( gameGlobal::action::context::GAME_CONTEXT );
	pContext->AddAction( consoleGlobal::action::TOGGLE, KEY_CTRL, (uint)eKeyState::Down )
		->Link( KEY_QUOTE, (uint)eKeyState::Press );
	pContext->AddAction( gameGlobal::action::PLAYER_FORWARD, KEY_W, (uint)eKeyState::Down );
	pContext->AddAction( gameGlobal::action::PLAYER_BACKWARD, KEY_S, (uint)eKeyState::Down );
	pContext->AddAction( gameGlobal::action::PLAYER_TURN_LEFT, KEY_A, (uint)eKeyState::Down );
	pContext->AddAction( gameGlobal::action::PLAYER_TURN_RIGHT, KEY_D, (uint)eKeyState::Down );
	pContext->AddAction( gameGlobal::action::QUICKLOAD, KEY_F9, (uint)eKeyState::Press );
	g_pKeyMap->UseContext( pContext->GetName() );

	if ( pInfo )
		Load( pInfo->mapName );

	m_pGuiDisplayRadiusMonitor = new Process_GuiDisplayRadiusMonitor;
	m_pGuiDisplayRadiusMonitor->SetGui( m_pGui );
	m_pGuiDisplayRadiusMonitor->SetRadius( 3.5f );
	g_pProcess->Add( m_pGuiDisplayRadiusMonitor.As<Process>() );

	g_pScene->SetClearColor( 0.05f, 0.05f, 0.05f );

	m_initialized = true;
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void GameWorld::Update()
{
	if ( m_isLoadScheduled || g_pKeyMap->CanPerform(gameGlobal::action::QUICKLOAD) )
	{
		Load( m_mapName );
		m_isLoadScheduled = false;
	}

	m_pCameraManager->Update();
	g_pEntity->Update();
	g_pCollision->Update();
	g_pPhysics->Update();
	m_pConsole->Update();

	ProcessInput();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void GameWorld::Render()
{
	const Entity_GameCamera* pCamera = m_pCameraManager->GetActiveCamera();

	g_pRenderer->BeginScene( 0.01f, 0.01f, 0.01f );

	if ( pCamera )
	{
		//PROFILE_START( "renderScene" );
		math::CameraTransform  cameraTransform = pCamera->GetCameraTransform();
	
		g_pScene->RenderToTexture( cameraTransform );
		//PROFILE_END( "renderScene" );
	
		g_pRenderer->UseDefaultRenderTargetAndViewport();
		m_pEffectScene->GetConfig().hTexture = g_pScene->GetSceneTexture();
		m_pEffectScene->Render( m_hSceneQuad, math::matrixTranslate(0.0f, 0.0f, 0.99f), math::Mtx44(), math::Mtx44() );
	}
	else
		m_pConsole->AddLog( "No camera", eConsoleLog::Warning );

	m_pConsole->Render();

	if ( m_pGui )
	{
		m_pGui->Render();
	}

	g_pRenderer->EndScene();
}
//-------------------------------------------------------------------
//	ScheduleLoad
//-------------------------------------------------------------------
void GameWorld::ScheduleLoad(const String& mapName)
{
	m_isLoadScheduled = true;
	if ( mapName.Length() > 0 )
		m_mapName = mapName;
}
//-------------------------------------------------------------------
//	Load
//-------------------------------------------------------------------
void GameWorld::Load(const String& mapName)
{
	if ( mapName.Length() == 0 )
		return;

	m_mapName = mapName;

	Shutdown( nullptr );
	Initialize( nullptr );

	GameLoader loader;
	if ( loader.Load( path::map(mapName) ) )
	{
		for ( auto it = g_pEntity->GetIterator(); it; it++ )
		{
			if ( (*it)->GetClassRelationship_Is<Entity_Player>() )
			{
				m_hPlayer = (*it);
			}
		}

		m_pCameraManager->SetTarget( m_hPlayer.GetBase() );
		m_pConsole->AddLog( "Loaded map " + mapName, eConsoleLog::Success );

		//ShowStartMessage();
	}
	else
		m_pConsole->AddLog( "Error opening file to load", eConsoleLog::Error );
}
//-------------------------------------------------------------------
//	ShowStartMessage
//-------------------------------------------------------------------
void GameWorld::ShowStartMessage(const String& msg)
{
	m_hPlayer->Active(false);
	auto pFullScreenMessage = (GameGuiMode_FullScreenMessage*)m_pGui->ChangeMode( eGuiMode::FullScreenMessage );
	pFullScreenMessage->SetTextSpeed( 4 );
	pFullScreenMessage->SetText( msg );
	pFullScreenMessage->OnEnd( "get_type entity_player\ncmd active true" );
}
//-------------------------------------------------------------------
//	ShowEndMessage
//-------------------------------------------------------------------
void GameWorld::ShowEndMessage(const String& msg)
{
	m_hPlayer->Active(false);
	auto pFullScreenMessage = (GameGuiMode_FullScreenMessage*)m_pGui->ChangeMode( eGuiMode::FullScreenMessage );
	pFullScreenMessage->SetTextSpeed( 2 );
	pFullScreenMessage->SetText( msg );
	pFullScreenMessage->CanInterrupt( false );
}
//-------------------------------------------------------------------
//	ProcessInput
//-------------------------------------------------------------------
void GameWorld::ProcessInput()
{
	if ( g_pInput->IsKeyDown(KEY_CTRL) && g_pInput->IsKeyPress(KEY_F1) )
		g_pCore->ChangeWorld( new EditorWorld() );
	else if ( g_pInput->IsKeyPress(KEY_F2) )
	{
		debugToOutput( 1.0f / 60.0f * 1000.0f, "60 FPS: " );
		debugToOutput( g_pCore->ProfileReport(true).AsChar() );
	}
}
//-------------------------------------------------------------------
//	GetCommand
//-------------------------------------------------------------------
GameCommand* GameWorld::GetCommand()
{
	return m_pCommand.Get();
}
//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void GameWorld::HandleEvent(Event* pEvent)
{
	Event_PlayerUse* pEventUse = nullptr;

	if ( isEvent<Event_PlayerUse>(pEvent, &pEventUse) )
	{
		RaycastInfo raycasts[10];
		uint contactCount = g_pCollision->Raycast( pEventUse->pos, pEventUse->dir, raycasts, ARRAY_COUNT(raycasts), 1.0f );

		for ( uint i = 0; i < contactCount; i++ )
		{
			String msg;
			uint flag = 0;

			if ( raycasts[i].hEntity->GetClassRelationship_Is<Entity_Door>() )
			{
				Entity_Door* pDoor = (Entity_Door*)raycasts[i].hEntity.GetPtr();

				flag = pDoor->GetId();
				if ( pDoor->IsLocked() )
					msg = pDoor->GetLockText();
			}
			else if ( raycasts[i].hEntity->GetClassRelationship_Is<Entity_Talk>() )
			{
				Entity_Talk* pTalk = (Entity_Talk*)raycasts[i].hEntity.GetPtr();

				flag = pTalk->GetId();
				msg = string::join( pTalk->GetTalkLines(), '\n' );

				if ( pTalk->CanFireEvent() )
				{
					for ( auto itCmd = pTalk->GetCommandLines(true).GetIterator(); itCmd; itCmd++ )
					{
						m_pConsole->ExecuteCommand( *itCmd );
					}
				}
			}

			if ( msg.Length() > 0 )
			{
				GameGuiMode_Message* pGuiMessage = nullptr;

				if ( m_pGui->GetMode() && m_pGui->GetMode()->GetType() == eGuiMode::Message )
					pGuiMessage = (GameGuiMode_Message*)m_pGui->GetMode();
				else
					pGuiMessage = (GameGuiMode_Message*)m_pGui->ChangeMode(eGuiMode::Message);

				m_pGuiDisplayRadiusMonitor->SetLockPos( pEventUse->pos );
				m_pGuiDisplayRadiusMonitor->SetTarget( pEventUse->hEntity, pGuiMessage );

				if ( flag != pGuiMessage->GetFlag() )
				{
					pGuiMessage->SetFlag( flag );
					pGuiMessage->SetText( msg );
				}

				break;
			}
		}
	}
}
