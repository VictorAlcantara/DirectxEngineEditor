#include "EditorCommand.h"
#include "../engine/CoreInterface.h"
#include "../engine/ScriptReader.h"
#include "../editor/EditorWorld.h"
#include "../shared/FirstPersonCamera.h"
#include "../engine/Debug.h"
#include "EditorEntity_PointLight.h"
#include "EditorEntity_SpotLight.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorCommand::EditorCommand()
{
	m_pWorld = nullptr;
}
//-------------------------------------------------------------------
//	OnShutdown
//-------------------------------------------------------------------
void EditorCommand::OnShutdown()
{
	m_pWorld = nullptr;
}
//-------------------------------------------------------------------
//	OnInitialize
//-------------------------------------------------------------------
void EditorCommand::OnInitialize()
{
}
//-------------------------------------------------------------------
//	Execute
//-------------------------------------------------------------------
bool EditorCommand::Execute(const String& cmd)
{
	String error;
	float floatData;
	bool bData;

	if ( cmd.Length() > 0 )
	{
		ScriptReader r( cmd );

		auto pLine = r.GetLine(0);

		if ( pLine->HasString( "LOAD" ) )
		{
			String mapName = pLine->Get( 1 );

			if ( mapName.Length() > 0 )
				m_pWorld->ScheduleLoad( mapName );
			else
				error = "Load requires a map name";
		}
		else if ( pLine->HasString( "SAVE" ) )
		{
			String mapName = pLine->Get( 1 );

			if ( mapName.Length() > 0 )
				m_pWorld->Save( mapName );
			else
				error = "Save requires a map name";
		}
		else if ( pLine->HasString("NEW") )
		{
			m_pWorld->ScheduleNew();
		}
		else if ( pLine->HasString("DEBUG_CAMERA_POS") )
			m_pWorld->DebugCameraPos();
		else if ( pLine->HasString("PARTITION_TOGGLE") )
			m_pWorld->PartitionToggle();
		else if ( pLine->SetBool("SHADOW", bData) )
		{
			if ( bData )
				g_pScene->EnableShadow( true );
			else
				g_pScene->EnableShadow( false );
		}
		else if ( pLine->HasString("BREAK_ON_RENDER") )
		{
			String cmd = pLine->Get( 1 );

			if ( cmd == "SELECTED" )
			{
				auto hSelected = m_pWorld->GetEntitySelected();

				if ( hSelected )
					g_pScene->Debug_BreakOnRender( hSelected );
				else
					error = "No entity selected";
			}
			else
				error = "Unrecognized command";
		}
		else if ( pLine->SetFloat("CAMERA_FAR", floatData) )
		{
			floatData = math::clamp( floatData, 20.0f, 1000.0f );
			auto camera = m_pWorld->GetCamera()->GetCameraTransform();
			m_pWorld->GetCamera()->SetFov( camera.fovDegs, camera.aspectRatio, camera.fNear, floatData );
		}
		else if ( pLine->SetBool("LIGHTS", bData) || pLine->SetBool("LIGHT", bData) )
		{
			g_pScene->EnableLights( bData );
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

				if ( cmd == "MS" )
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
		else if ( pLine->HasString("ENTITY_ID") )
		{
			Handle<Entity> hEntity = m_pWorld->GetEntitySelected();

			if ( hEntity )
				debugToOutput( (int)hEntity->GetId(), "Entity id: " );
			else
				error = "Entity must be selected";
		}
		else if ( pLine->HasString("LIGHT_ID") )
		{
			Handle<Entity> hEntity = m_pWorld->GetEntitySelected();

			if ( hEntity )
			{
				if ( hEntity->GetClassRelationship_Is<EditorEntity_PointLight>() )
					debugToOutput( (int)hEntity->Convert<EditorEntity_PointLight>()->GetLight()->GetId(), "light id: " );
				if ( hEntity->GetClassRelationship_Is<EditorEntity_SpotLight>() )
					debugToOutput( (int)hEntity->Convert<EditorEntity_SpotLight>()->GetLight()->GetId(), "light id: " );
			}
			else
				error = "Entity must be selected";
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
			error = "Unrecognized command";
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
