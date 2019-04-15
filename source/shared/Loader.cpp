#include "../shared/Loader.h"
#include "../engine/String.h"
#include "../engine/File.h"
#include "../engine/ScriptReader.h"
#include "../editor/EditorGlobal.h"
#include "../engine/AppMessageBox.h"
#include "../shared/Command.h"
#include "../engine/Handle.h"
#include "../engine/Entity.h"
#include "../engine/CoreInterface.h"
#include "../game/Entity_Player.h"
#include "../game/Entity_Block.h"
#include "../game/Entity_AmbientLight.h"
#include "../game/Entity_PointLight.h"
#include "../game/Entity_SpotLight.h"
#include "../game/Entity_GameCamera.h"
#include "../game/Entity_CameraTrack.h"
#include "../game/Entity_Door.h"
#include "../game/Entity_TriggerCamera.h"
#include "../game/Entity_TriggerCommand.h"
#include "../game/Entity_Talk.h"
#include "../game/Entity_Model.h"
#include "../game/Entity_Monster.h"
#include "../game/Entity_Refraction.h"
#include "../engine/Component_Render.h"
#include "../engine/RenderInfo.h"

bool USE_PROFILE = false;

#define MAKE_DEBUG_PROFILE(name, useProfile) static const HashString name = ((useProfile) ? (#name) : "");

MAKE_DEBUG_PROFILE( PROFILE_MAP_FILE, USE_PROFILE );
MAKE_DEBUG_PROFILE( PROFILE_MAP_SCRIPT, USE_PROFILE );
MAKE_DEBUG_PROFILE( PROFILE_ENTITY_SCRIPT, USE_PROFILE );

//=============================================================================
//	Loader
//=============================================================================

//-------------------------------------------------------------------
//	Load
//-------------------------------------------------------------------
bool Loader::Load(const String& path)
{
	FileReader file;
	Handle<Entity> hCurrentEntity;

	g_pCore->ProfileStart( PROFILE_MAP_FILE );
	if ( file.Load( path, eFileMode::Text ) )
	{
		g_pCore->ProfileEnd( PROFILE_MAP_FILE );

		g_pCore->ProfileStart( PROFILE_MAP_SCRIPT );
		ScriptReader script( file );
		g_pCore->ProfileEnd( PROFILE_MAP_SCRIPT );

		g_pCore->ProfileStart( PROFILE_ENTITY_SCRIPT );
		for ( uint i = 0; i < script.GetLineCount(); i++ )
		{
			auto pLine = script.GetLine(i);

			String firstCmd = pLine->Get( 0 );
			
			bool hasLoaded = Load_Partition(&script, i);

			if ( !hasLoaded )
			{
				if ( command::isEntity(firstCmd) )
					hCurrentEntity = CreateEntity( firstCmd );

				if ( hCurrentEntity )
					hCurrentEntity->ProcessCommand( pLine );
			}
		}
		g_pCore->ProfileEnd( PROFILE_ENTITY_SCRIPT );
	}
	else
		return false;

	if ( USE_PROFILE )
		debugToOutput( g_pCore->ProfileReport(false).AsChar() );

	OnLoadEnd();

	return true;
}

//=============================================================================
//	EditorLoader
//=============================================================================

//-------------------------------------------------------------------
//	CreateEntity
//-------------------------------------------------------------------
Handle<Entity> EditorLoader::CreateEntity(const String& cmd)
{
	if ( cmd == command::ENTITY_BLOCK ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Block );
	if ( cmd == command::ENTITY_DOOR ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Door );
	if ( cmd == command::ENTITY_PLAYER_SPAWN ) 
		return g_pEditor->EntityCreate( eEditorEntityType::PlayerSpawn );
	if ( cmd == command::ENTITY_CAMERA ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Camera );
	if ( cmd == command::ENTITY_CAMERA_TRACK ) 
		return g_pEditor->EntityCreate( eEditorEntityType::CameraTrack );
	if ( cmd == command::ENTITY_AMBIENT_LIGHT ) 
		return g_pEditor->EntityCreate( eEditorEntityType::AmbientLight );
	if ( cmd == command::ENTITY_POINT_LIGHT ) 
		return g_pEditor->EntityCreate( eEditorEntityType::PointLight );
	if ( cmd == command::ENTITY_SPOT_LIGHT ) 
		return g_pEditor->EntityCreate( eEditorEntityType::SpotLight );
	if ( cmd == command::ENTITY_TRIGGER_CAMERA ) 
		return g_pEditor->EntityCreate( eEditorEntityType::TriggerCamera );
	if ( cmd == command::ENTITY_TRIGGER_COMMAND ) 
		return g_pEditor->EntityCreate( eEditorEntityType::TriggerCommand );
	if ( cmd == command::ENTITY_MODEL ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Model );
	if ( cmd == command::ENTITY_TALK ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Talk );
	if ( cmd == command::ENTITY_MONSTER ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Monster );
	if ( cmd == command::ENTITY_PARTITION ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Partition );
	if ( cmd == command::ENTITY_REFRACTION ) 
		return g_pEditor->EntityCreate( eEditorEntityType::Refraction );

	ASSERT( "Unknown entity" );
	
	return Handle<Entity>();
}
//-------------------------------------------------------------------
//	OnLoadEnd
//-------------------------------------------------------------------
void EditorLoader::OnLoadEnd()
{
	//for ( auto it = g_pEntity->GetIterator(); it; it++ )
	//{
	//	if ( (*it)->GetClassRelationship_Is<EditorEntity_Block>() )
	//	{
	//		math::Vec4 color4 = (*it)->GetComponentAs<Component_Render>()->GetData()->vec4Group.Get( (renderFlag)eRenderFlagVec4::Color );

	//		CHECK( !math::isZero(color4) );
	//	}
	//}
}

//=============================================================================
//	GameLoader
//=============================================================================

//-------------------------------------------------------------------
//	CreateEntity
//-------------------------------------------------------------------
Handle<Entity> GameLoader::CreateEntity(const String& cmd)
{
	if ( cmd == command::ENTITY_BLOCK )
		return g_pEntity->Create<Entity_Block>();
	if ( cmd == command::ENTITY_PLAYER_SPAWN )
		return g_pEntity->Create<Entity_Player>();
	if ( cmd == command::ENTITY_DOOR )
		return g_pEntity->Create<Entity_Door>();
	if ( cmd == command::ENTITY_CAMERA )
		return g_pEntity->Create<Entity_GameCamera>();
	if ( cmd == command::ENTITY_CAMERA_TRACK )
		return g_pEntity->Create<Entity_CameraTrack>();
	if ( cmd == command::ENTITY_TRIGGER_CAMERA )
		return g_pEntity->Create<Entity_TriggerCamera>();
	if ( cmd == command::ENTITY_TRIGGER_COMMAND )
		return g_pEntity->Create<Entity_TriggerCommand>();
	if ( cmd == command::ENTITY_AMBIENT_LIGHT )
		return g_pEntity->Create<Entity_AmbientLight>();
	if ( cmd == command::ENTITY_POINT_LIGHT )
		return g_pEntity->Create<Entity_PointLight>();
	if ( cmd == command::ENTITY_SPOT_LIGHT )
		return g_pEntity->Create<Entity_SpotLight>();
	if ( cmd == command::ENTITY_TALK )
		return g_pEntity->Create<Entity_Talk>();
	if ( cmd == command::ENTITY_MODEL )
		return g_pEntity->Create<Entity_Model>();
	if ( cmd == command::ENTITY_MONSTER )
		return g_pEntity->Create<Entity_Monster>();
	if ( cmd == command::ENTITY_REFRACTION )
		return g_pEntity->Create<Entity_Refraction>();

	ASSERT( "Unknown entity" );

	return Handle<Entity>();
}
//-------------------------------------------------------------------
//	CreatePartition
//-------------------------------------------------------------------
void GameLoader::CreatePartition(const VecGrid& gridPos, const VecGrid& gridScale, bool debugAddEntity, const String& debugEntityContact)
{
	math::Vec3 pos = Grid::ToVec3( gridPos );
	math::Vec3 scale = Grid::ToVec3( gridScale );

	g_pScene->CreatePartition( pos, scale );
	g_pCollision->CreatePartition( pos, scale, debugAddEntity, debugEntityContact );
}
//-------------------------------------------------------------------
//	OnLoadEnd
//-------------------------------------------------------------------
void GameLoader::OnLoadEnd()
{
}
//-------------------------------------------------------------------
//	Load_Partition
//-------------------------------------------------------------------
bool GameLoader::Load_Partition(ScriptReader* pScript, uint& line)
{
	auto pLine = pScript->GetLine( line );

	if ( pLine->Get(0) != command::ENTITY_PARTITION )
		return false;

	VecGrid partitionPos;
	VecGrid partitionScale;
	String debugEntityContact;
	bool debugAddEntity = false;
	bool debugCreatePartition = false;
	bool isPartitionDataValid = true;

	for ( uint j = 0; j < 5; j++ )
	{
		line++;
		if ( !pScript->HasLine(line) )
		{
			isPartitionDataValid = false;
			break;
		}

		bool isCommandValid = false;
		pLine = pScript->GetLine( line );
		isCommandValid |= pLine->SetIntArray( command::GRID_POS, &partitionPos[0], 3 );
		isCommandValid |= pLine->SetIntArray( command::GRID_SCALE, &partitionScale[0], 3 );
		isCommandValid |= pLine->SetBool( command::DEBUG_ADD_ENTITY, debugAddEntity );
		isCommandValid |= pLine->SetBool( command::DEBUG_CREATE_PARTITION, debugCreatePartition );
		isCommandValid |= pLine->SetQuotedString( command::DEBUG_ENTITY_CONTACT, debugEntityContact );
		isCommandValid |= pLine->HasString( command::DEBUG_ENTITY_CONTACT );

		CHECK( isCommandValid );
		isPartitionDataValid &= isCommandValid;
	}

	if ( isPartitionDataValid )
	{
		#ifdef DEBUG_MODE
			CHECK( !debugCreatePartition );
		#endif
		
		CreatePartition( partitionPos, partitionScale, debugAddEntity, debugEntityContact );
	}

	return true;
}

#undef MAKE_DEBUG_PROFILE