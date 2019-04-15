#include "GameCommand.h"
#include "../engine/CoreInterface.h"
#include "../engine/ScriptReader.h"
#include "GameGlobal.h"
#include "GameWorld.h"
#include "GameEvent.h"
#include "GameEnum.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
GameCommand::GameCommand()
{
	m_pWorld = nullptr;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void GameCommand::OnShutdown()
{
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void GameCommand::OnInitialize()
{
}
//-------------------------------------------------------------------
//	Execute
//-------------------------------------------------------------------
bool GameCommand::Execute(const String& cmd)
{
	String error;

	if ( cmd.Length() > 0 )
	{
		ScriptReader r( cmd );
		uint lines = r.GetLineCount();

		for ( uint line = 0; line < lines; line++ )
		{
			auto pLine = r.GetLine(line);

			if ( pLine->HasString( "LOAD" ) )
			{
				String mapName = pLine->Get( 1 );

				m_pWorld->ScheduleLoad( mapName );
			}
			else if ( pLine->HasString( "GET_CLEAR" ) )
			{
				ClearGetData();
			}
			else if ( pLine->HasString( "GET" ) )
			{
				ClearGetData();

				String entityName = pLine->Get( 1 );

				if ( g_pEntity->CountName(entityName) > 0 )
					m_entitySelectedName = entityName;
				else
					error = "Entity with name " + entityName + " not found";
			}
			else if ( pLine->HasString( "GET_TYPE" ) )
			{
				ClearGetData();

				String entityType = pLine->Get( 1 );

				if ( g_pEntity->CountClassTag(entityType) > 0 )
					m_entitySelectedTag = entityType;
				else
					error = "Entity with tag " + entityType + " not found";
			}
			else if ( pLine->HasString( "CMD" ) )
			{

				pLine->RemoveAt( 0 );
			
				if ( m_entitySelectedName.IsValid() )
				{
					for ( auto it = g_pEntity->GetByName(m_entitySelectedName); it; it++ )
						(*it)->ProcessCommand( pLine );
				}
				else if ( m_entitySelectedTag.IsValid() )
				{
					for ( auto it = g_pEntity->GetByClassTag(m_entitySelectedTag); it; it++ )
						(*it)->ProcessCommand( pLine );
				}
				else
					error = "Attempted to use CMD with no entity selected";
			}
			else if (pLine->HasString("GAME_START"))
			{
				String msg;
				pLine->RemoveAt(0);
				msg = pLine->GetQuotedString();

				m_pWorld->ShowStartMessage( msg );
			}
			else if ( pLine->HasString("GAME_END") )
			{
				String msg;
				pLine->RemoveAt( 0 );
				msg = pLine->GetQuotedString();

				m_pWorld->ShowEndMessage( msg );
			}
			else if ( pLine->HasString("CAMERA") )
			{
				SmartPtr<Event_ChangeCameraMode> pEvent = new Event_ChangeCameraMode;
				if ( pLine->HasString("NORMAL") )
					pEvent->mode = eCameraMode::Normal;
				else if ( pLine->HasString("BACK") )
					pEvent->mode = eCameraMode::Back;
				else
					error = "Invalid camera mode";

				if ( error.Length() == 0 )
					g_pEvent->Process( pEvent );
			}
			else if ( pLine->HasString("PROFILER") )
			{
				String cmd = pLine->Get( 1 );
				String cmd2 = pLine->Get( 2 );

				if ( cmd == "RESET" || cmd == "CLEAR" )
				{
					g_pCore->ProfileClear();
				}
				else if ( cmd == "PRINT" || cmd == "REPORT" )
				{
					bool ms = false;

					if ( cmd2 == "MS" )
						ms = true;

					String report = g_pCore->ProfileReport(ms);
					debugToOutput( report.AsChar() );
				}
				else
					error = "Unrecognized command for PROFILER. Expected PRINT or REPORT";
			}
			else if ( pLine->HasString("SCENE") )
			{
				String cmd = pLine->Get( 1 );

				if ( cmd == "PROFILE" || cmd == "REPORT" )
				{
					String report = g_pScene->Debug_Report();
					debugToOutput( report.AsChar() );
				}
			}
			else if ( pLine->HasString("FPS_MAX") )
			{
				String cmd = pLine->Get(1);

				if ( cmd.Length() > 0 )
				{
					if ( string::isNumber(cmd) )
					{
						int fps = string::strToInt( cmd );

						if ( fps < 0 )
							error = "FPS must be 1 or greater";
						else
							g_pCore->SetFps( fps );
					}
					else
						error = "Parameter must be number";
				}
				else
					error = "Invalid parameter";
			}
			else
			{
				error = "Unrecognized command";

				if ( lines > 1 )
				{
					error += " - executed lines " + string::intToStr( line + 1 ) + "\\" + string::intToStr( lines );
				}

				break;
			}
		}
	}
	else
		error = "Empty command";

	if ( error.Length() > 0 )
	{
		m_lastError = error;
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
//	ClearGetData
//-------------------------------------------------------------------
void GameCommand::ClearGetData()
{
	m_entitySelectedName = "";
	m_entitySelectedTag = "";
}
//-------------------------------------------------------------------
//	SetWorld
//-------------------------------------------------------------------
void GameCommand::SetWorld(GameWorld* pWorld)
{
	m_pWorld = pWorld;
}
