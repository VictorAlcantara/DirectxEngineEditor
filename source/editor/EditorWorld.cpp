#include "../editor/EditorWorld.h"
#include "../engine/CoreInterface.h"
#include "../engine/Model.h"
#include "../engine/Shader.h"
#include "../engine/Vertex.h"
#include "../engine/Texture.h"
#include "../engine/Pointer.h"
#include "../shared/FirstPersonCamera.h"
#include "../engine/AppMessageBox.h"
#include "../engine/File.h"
#include "../engine/ScriptReader.h"
#include "../engine/Debug.h"
#include "EditorToolManager.h"
#include "../engine/RenderTarget.h"
#include "../editor/Grid.h"
#include "EditorGlobal.h"
#include "EditorGui.h"
#include "../engine/Effect.h"
#include "EditorState.h"
#include "EditorState_Idle.h"
#include "EditorEvent.h"
#include "EditorState_EntityCreate.h"
#include "EditorState_PickEntity.h"
#include "EditorState_PickAxis.h"
#include "RenderInfoEditor.h"
#include "../engine/Path.h"
#include "../shared/Loader.h"
#include "../engine/Font.h"
#include "../engine/FontEffect.h"
#include "GuiPage.h"
#include "GuiPageControl.h"
#include "../engine/Scene.h"
#include "../engine/Primitive.h"
#include "../engine/TableHashFunction.h"
#include "../game/GameWorld.h"
#include "../game/Console.h"
#include "EditorCommand.h"
#include "../engine/api_File.h"
#include "EditorEntity_Block.h"
#include "EditorEntity_PlayerSpawn.h"
#include "EditorEntity_AmbientLight.h"
#include "EditorEntity_SpotLight.h"
#include "EditorEntity_PointLight.h"
#include "EditorEntity_Camera.h"
#include "EditorEntity_CameraTrack.h"
#include "EditorEntity_CameraTrackPoint.h"
#include "EditorEntity_Door.h"
#include "EditorEntity_TriggerCamera.h"
#include "EditorEntity_TriggerCommand.h"
#include "EditorEntity_Model.h"
#include "EditorEntity_Talk.h"
#include "EditorEntity_Monster.h"
#include "EditorEntity_Partition.h"
#include "EditorEntity_Refraction.h"
#include "../engine/Component_Render.h"

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
EditorWorld::EditorWorld() : m_entities(1000, tableHashFunc_Uint)
{
	m_initialized = false;
	m_isLoadScheduled = false;
	m_isNewScheduled = false;
	m_partitionState = PARTITION_SHOW_PARCIAL;
	m_loadingMap = false;
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void EditorWorld::Shutdown(WorldInfo* pInfo)
{
	if ( !m_initialized )
	{
		ASSERT( "Not initialized" );
		return;
	}

	if ( pInfo )
		pInfo->mapName = m_mapName;

	m_pConsole->Shutdown();
	m_pConsole = nullptr;

	m_pCommand->Shutdown();
	m_pCommand = nullptr;

	g_pProcess->Clear();

	m_pFinder->Shutdown();
	m_pFinder = nullptr;

	g_pEntity->Clear();
	g_pScene->Clear();

	m_pShaderRenderTarget->Shutdown();
	m_pShaderRenderTarget = nullptr;

	m_pModelRenderTarget->Shutdown();
	m_pModelRenderTarget = nullptr;

	m_pEffectOneColor->Shutdown();
	m_pEffectOneColor = nullptr;

	m_pGrid->Shutdown();
	m_pGrid = nullptr;

	m_pEffectObj->Shutdown();
	m_pEffectObj = nullptr;

	m_pState = nullptr;

	m_pGuiPageControl = nullptr;

	m_pGui->Shutdown();
	m_pGui = nullptr;

	m_pEditorTool->Shutdown();
	m_pEditorTool = nullptr;

	ShutdownListener();

	g_pKeyMap->Clear();

	m_partitionState = PARTITION_SHOW_PARCIAL;

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void EditorWorld::Initialize(const WorldInfo* pInfo)
{
	m_pFinder = new EditorEntityFinder;
	m_pFinder->Initialize();

	if ( m_initialized )
	{
		ASSERT( "Already initialized" );
		return;
	}

	float fFar = 100.0f;

	#ifdef DEBUG_MODE
		fFar = 35.0f;
	#endif

	m_pCamera = new FirstPersonCamera;
	m_pCamera->SetFov( 60.0f, g_pRenderer->GetWindowViewport().GetAspectRatio(), 0.001f, fFar );

	m_pEditorTool = new EditorToolManager;
	m_pEditorTool->Initialize();

	m_pGui = new EditorGui;
	m_pGui->Initialize();

	m_pGuiPageControl = new GuiPageControl;
	m_pGuiPageControl->ShowPageMain();

	m_pState = new EditorStateMachine;
	m_pState->ChangeState( new EditorState_Idle );

	InitializeListener();

	const float val1 = -0.5f;
	const float val2 =  0.5f;
	const math::Vec3 light0(
		-1.0f + math::random( val1, val2 ),
		-1.0f + math::random( val1, val2 ),
		 1.0f + math::random( val1, val2 )
	);

	const math::Vec3 light1(
		 1.0f + math::random( val1, val2 ),
		 1.0f + math::random( val1, val2 ),
		-1.0f + math::random( val1, val2 )
	);

	m_pGrid = new Grid;
	m_pGrid->Initialize();

	{
		Vertex_P3T2 vertices[4];

		float x = 1.0f;
		float y = 1.0f;
		float z = 0.0f;

		vertices[0].pos = math::Vec3( -x, -y, z );
		vertices[0].texCoord = math::Vec2( 0.0f, 1.0f );

		vertices[1].pos = math::Vec3( -x, y, z );
		vertices[1].texCoord = math::Vec2( 0.0f, 0.0f );

		vertices[2].pos = math::Vec3( x, -y, z );
		vertices[2].texCoord = math::Vec2( 1.0f, 1.0f );

		vertices[3].pos = math::Vec3( x, y, z );
		vertices[3].texCoord = math::Vec2( 1.0f, 0.0f );

		m_pModelRenderTarget = new Model;
		m_pModelRenderTarget->Initialize( eTopology::TriangleStrip, vertices, ARRAY_COUNT(vertices) );

		m_pShaderRenderTarget = new Shader_Texture;
		m_pShaderRenderTarget->Initialize();
	}

	m_pEffectOneColor = new Effect_Color;
	m_pEffectOneColor->Initialize();

	m_hSphereLine = primitive::geometry::createSphereLine(8, 5);
	m_hLine = primitive::geometry::createLine();
	m_hCubeLine = primitive::geometry::createCubeLine();
	m_hPyramidLine = primitive::geometry::createPyramidLine( 8 );

	KeyMapContext* pContext = g_pKeyMap->CreateContext( editorGlobal::action::context::EDITOR_DEFAULT );
	pContext->AddAction( editorGlobal::action::ESCAPE, KEY_ESCAPE, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::ENTITY_SELECT, KEY_MOUSE_LEFT, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::ENTITY_DELETE, KEY_DELETE, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::ENTITY_CLONE, KEY_NUMERIC_ADD, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::TEST_CREATE_BLOCK, KEY_CTRL, (uint)eKeyState::Down )
		->Link( KEY_B, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::LEFT_CLICK, KEY_MOUSE_LEFT, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_FORWARD, KEY_W, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_BACKWARD, KEY_S, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_STRAFE_RIGHT, KEY_D, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_STRAFE_LEFT, KEY_A, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_UP, KEY_Y, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_DOWN, KEY_H, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_LOOK_RIGHT, KEY_RIGHT, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_LOOK_LEFT, KEY_LEFT, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_LOOK_UP, KEY_UP, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_LOOK_DOWN, KEY_DOWN, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::CAMERA_ACCELERATE, KEY_SHIFT, (uint)eKeyState::Down | (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::GRID_DECREASE, KEY_BRACKET_OPEN, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::GRID_INCREASE, KEY_BRACKET_CLOSE, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::GRID_TOGGLE, KEY_G, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::QUICKSAVE, KEY_F5, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::QUICKLOAD, KEY_F9, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::PARTITION_TOGGLE, KEY_F2, (uint)eKeyState::Press );
	pContext->AddAction( consoleGlobal::action::TOGGLE, KEY_CTRL, (uint)eKeyState::Down )
		->Link( KEY_QUOTE, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::MODE_CHANGE, KEY_CTRL, (uint)eKeyState::Down )
		->Link( KEY_F1, (uint)eKeyState::Press );
	pContext->AddAction( editorGlobal::action::ENTITY_RESET, KEY_CTRL, (uint)eKeyState::Down )
		->Link( KEY_R, (uint)eKeyState::Press );
	g_pKeyMap->UseContext( pContext->GetName() );

	m_pEffectObj = new Effect_LightDir4;
	m_pEffectObj->Initialize();
	m_pEffectObj->GetConfig().renderer.blendMode = eBlendMode::Disabled;
	m_pEffectObj->GetConfig().SetLightDir( 0, math::normalize(1.0f, 1.0f, 1.0f) );
	m_pEffectObj->GetConfig().SetLightColor( 0, math::normalizeRgb(50, 150, 255) );
	m_pEffectObj->GetConfig().SetLightDir( 1, math::normalize(1.0f, -1.0f, -1.0f) );
	m_pEffectObj->GetConfig().SetLightColor( 1, math::normalizeRgb(50, 150, 255) );
	m_pEffectObj->GetConfig().ambientColor4 = math::normalizeRgb( 150 );
	m_pEffectObj->GetConfig().diffuseColor4 = math::normalizeRgb( 255 );

	m_pCommand = new EditorCommand;
	m_pCommand->Initialize();
	m_pCommand->SetWorld( this );

	m_pConsole = new Console;
	m_pConsole->Initialize();
	m_pConsole->SetCommand( m_pCommand.As<ICommand>() );

	m_initialized = true;

	if ( !m_isNewScheduled && pInfo && pInfo->mapName.Length() > 0 )
		Load( pInfo->mapName );
}
//-------------------------------------------------------------------
//	Update
//-------------------------------------------------------------------
void EditorWorld::Update()
{
	if ( m_isLoadScheduled )
	{
		Load( m_mapName );
	}
	else if ( m_isNewScheduled )
	{
		New();
	}

	m_pFinder->Update();

	if ( m_pConsole )
		m_pConsole->Update();

	g_pEntity->Update();

	m_pGui->Update();
	m_pEditorTool->Update( m_pCamera->GetCameraTransform() );
	m_pState->Update();

	m_pCamera->Update();
	m_pGrid->Update( m_pCamera->GetCameraTransform() );

	ProcessInput();
}
//-------------------------------------------------------------------
//	Render
//-------------------------------------------------------------------
void EditorWorld::Render()
{
	RenderScene();

	g_pRenderer->UseDefaultRenderTargetAndViewport();
	g_pRenderer->BeginScene( 0.0f, 0.0f, 1.0f );
	math::ViewportInfo v = g_pRenderer->GetWindowViewport();
	math::Mtx44 world = math::matrixTranslate( 0.0f, 0.0f, 99.0f );
	math::Mtx44 scale = math::matrixScale( (float)v.width * 0.5f, (float)v.height * 0.5f, 1.0f );
	math::Mtx44 view;
	math::Mtx44 proj = math::matrixOrtho( (float)v.width, (float)v.height, 0.01f, 100.0f );

	g_pRenderer->SetBlendMode( eBlendMode::Disabled );
	m_pShaderRenderTarget->SetWvp( scale * world, view, proj );
	m_pShaderRenderTarget->SetTexture( &g_pScene->GetSceneTexture().Get() );
	g_pRenderer->SetCullMode( eCullMode::None );
	g_pRenderer->SetDepthOperation( eDepthOperation::Always );
	g_pRenderer->SetFillMode( eFillMode::Solid );
	g_pRenderer->SetSamplerState( eTextureAddress::Wrap, eTextureAddress::Wrap, eTextureFilter::Linear, 0 );
	g_pRenderer->Render( m_pModelRenderTarget.Get(), m_pShaderRenderTarget.Get() );

	m_pGui->Render( m_pCamera->GetCameraTransform() );

	if ( m_pConsole )
		m_pConsole->Render();

	g_pRenderer->EndScene();
}
//-------------------------------------------------------------------
//	RenderScene
//-------------------------------------------------------------------
void EditorWorld::RenderScene()
{
	math::CameraTransform camera = GetCameraTransform();

	g_pScene->RenderToTexture( camera );

	if ( m_hModelObj )
	{
		math::Mtx44 world = math::matrixTranslate( 0.0f, 0.0f, 5.0f );
		math::Mtx44 view = camera.view;
		math::Mtx44 proj = camera.proj;

		m_pEffectObj->Render( m_hModelObj, world, view, proj );
	}

	m_pEditorTool->Render( camera );

	if ( m_hEntitySelected )
	{
		m_pGrid->Render( camera );

		auto pRenderInfo = m_hEntitySelected->GetComponentPtrAs<Component_Render>()->GetDataAs<RenderInfo_EditorEntity>();

		m_pEffectOneColor->GetConfig().renderer.wireframe = true;
		m_pEffectOneColor->GetConfig().color4 = pRenderInfo->highlightColor4;
		m_pEffectOneColor->GetConfig().depthBias = editorGlobal::entity::HIGHLIGHT_DEPTH_BIAS;
		m_pEffectOneColor->Render( pRenderInfo->GetModel(), pRenderInfo->GetTransform().GetWorld(), camera.view, camera.proj );

		if ( pRenderInfo->renderBoundingSphere )
		{
			math::Mtx44 scale = math::matrixScale( pRenderInfo->boundingSphereScale );

			m_pEffectOneColor->GetConfig().color4 = math::normalizeRgb( 33, 120, 153 );
			m_pEffectOneColor->Render( m_hSphereLine, scale * pRenderInfo->GetTransform().translate, camera.view, camera.proj );
		}
		if ( pRenderInfo->renderDirection )
		{
			math::Vec3 scale = math::Vec3(2.0f);
			math::Mtx44 transform = math::matrixScale(scale) * math::matrixOrthogonalLook(pRenderInfo->direction) * pRenderInfo->GetTransform().translate;
			m_pEffectOneColor->GetConfig().color4 = math::normalizeRgb( 255, 249, 17 );
			m_pEffectOneColor->Render( m_hLine, transform, camera.view, camera.proj );
		}
		if ( pRenderInfo->renderAabb )
		{
			math::Aabb3 aabb;

			if ( m_hEntitySelected->GetClassRelationship_Is<EditorEntity_SpotLight>() )
				aabb = m_hEntitySelected->Convert<EditorEntity_SpotLight>()->GetLight()->GetAabbGlobal();
			else if ( m_hEntitySelected->GetClassRelationship_Is<EditorEntity_PointLight>() )
				aabb = m_hEntitySelected->Convert<EditorEntity_PointLight>()->GetLight()->GetAabbGlobal();
			else
				aabb = pRenderInfo->GetAabbGlobal();

			//math::Vec3 scale = math::Vec3(1.0f);;
			math::Vec3 pos = aabb.GetPos();
			math::Vec3 scale = aabb.GetScale();
			//math::Vec3 pos;
			m_pEffectOneColor->GetConfig().color4 = math::normalizeRgb( 36, 222, 10 );
			m_pEffectOneColor->Render( m_hCubeLine, math::matrixScale(scale) * math::matrixTranslate(pos), camera.view, camera.proj );
		}
		if ( m_hEntitySelected->GetClassRelationship_Is<EditorEntity_SpotLight>() )
		{
			auto pLight = m_hEntitySelected->Convert<EditorEntity_SpotLight>()->GetLight();
			math::Vec3 pos = pLight->GetPos();
			math::Vec3 scale;
			scale.z = pLight->GetRange();
			scale.x = scale.z * math::tanDegs( (pLight->GetInnerConeDegs() + pLight->GetOuterConeDegs()) * 0.5f );
			scale.y = scale.x;
			math::Mtx44 mTranslate = math::matrixTranslate( pos );
			math::Mtx44 mScale = math::matrixScale( scale );
			math::Mtx44 mRotation;
			pLight->GetRotation( mRotation );
			math::Mtx44 mWorld = mScale * mRotation * mTranslate;

			m_pEffectOneColor->GetConfig().color4 = math::normalizeRgb( 200, 200, 200 );
			m_pEffectOneColor->Render( m_hPyramidLine, mWorld, camera.view, camera.proj );
		}
	}
}
//-------------------------------------------------------------------
//	OpenTextureSelectMenu
//-------------------------------------------------------------------
void EditorWorld::OpenTextureSelectMenu(eGuiTexture textureType)
{
	m_pGuiPageControl->OpenTextureSelectMenu( textureType );
}
//-------------------------------------------------------------------
//	OpenModelSelectMenu
//-------------------------------------------------------------------
void EditorWorld::OpenModelSelectMenu()
{
	m_pGuiPageControl->OpenModelSelectMenu();
}
//-------------------------------------------------------------------
//	OpenEntitySelectMenu
//-------------------------------------------------------------------
void EditorWorld::OpenEntitySelectMenu()
{
	UnselectEntity();
	m_pGuiPageControl->OpenEntitySelectMenu();
}
//-------------------------------------------------------------------
//	OpenTriggerSelectMenu
//-------------------------------------------------------------------
void EditorWorld::OpenTriggerSelectMenu()
{
	UnselectEntity();
	m_pGuiPageControl->OpenTriggerSelectMenu();
}
//-------------------------------------------------------------------
//	OpenTextArea
//-------------------------------------------------------------------
void EditorWorld::OpenTextArea(uint flag)
{
	const List<String>* pTextAreaValue = nullptr;
	Handle<Entity> hEntity = m_hEntitySelected;

	if ( hEntity->GetClassRelationship_Is<EditorEntity_TriggerCommand>() )
		pTextAreaValue = &hEntity.GetAs<EditorEntity_TriggerCommand>().GetCommands();
	else if ( hEntity->GetClassRelationship_Is<EditorEntity_Talk>() )
	{
		if ( flag == editorGlobal::gui::flag::FLAG_ENTITY_TALK_TEXT )
			pTextAreaValue = &hEntity.GetAs<EditorEntity_Talk>().GetTextLines();
		else if ( flag == editorGlobal::gui::flag::FLAG_ENTITY_TALK_COMMAND )
			pTextAreaValue = &hEntity.GetAs<EditorEntity_Talk>().GetCommandLines();
		else
			ASSERT( "Invalid flag" );
	}
	else if ( hEntity->GetClassRelationship_Is<EditorEntity_Door>() )
		pTextAreaValue = &hEntity.GetAs<EditorEntity_Door>().GetLockTextLines();

	CHECK( pTextAreaValue );

	if ( pTextAreaValue )
	{
		m_pGui->GetStackTop()->Show( false );
		m_pGuiPageControl->OpenTextArea( *pTextAreaValue, flag );
	}
}
//-------------------------------------------------------------------
//	Save
//-------------------------------------------------------------------
void EditorWorld::Save(const String& mapName)
{
	if ( mapName.Length() == 0 )
	{
		m_pConsole->AddLog( "Map name is empty", eConsoleLog::Error );
		return;
	}

	String mapFile = path::map(mapName); 
	FileWriter file;
	if ( file.Open( mapFile, eFileOperation::Truncate ) )
	{
		m_mapName = mapName;

		for ( auto it = m_entities.GetIterator(); it; it++ )
			file.Write( (*it)->GetSerialized().AsChar() );

		file.Close();

		m_pConsole->AddLog( "Saved " + m_mapName, eConsoleLog::Success );
	}
	else
	{
		m_pConsole->AddLog( "Error opening file to save", eConsoleLog::Error );
	}
}
//-------------------------------------------------------------------
//	ScheduleNew
//-------------------------------------------------------------------
void EditorWorld::ScheduleNew()
{
	m_isNewScheduled = true;
	m_isLoadScheduled = false;
}
//-------------------------------------------------------------------
//	New
//-------------------------------------------------------------------
void EditorWorld::New()
{
	Shutdown( nullptr );
	Initialize( nullptr );

	m_mapName = "";
	m_isNewScheduled = false;
}
//-------------------------------------------------------------------
//	ScheduleLoad
//-------------------------------------------------------------------
void EditorWorld::ScheduleLoad(const String& mapName)
{
	m_isLoadScheduled = true;
	m_isNewScheduled = false;
	m_mapName = mapName;
}
//-------------------------------------------------------------------
//	Load
//-------------------------------------------------------------------
void EditorWorld::Load(const String& mapName)
{
	m_loadingMap = true;

	if ( mapName.Length() > 0 )
	{
		String mapPath = path::map(mapName);

		if ( platform::file::exists(mapPath) )
		{
			m_mapName = string::upperCase( mapName );

			Shutdown( nullptr );
			Initialize( nullptr );

			EditorLoader loader;
			if ( loader.Load(mapPath) )
			{
				m_pConsole->Show( false );
				m_pConsole->AddLog( "Loaded map " + m_mapName, eConsoleLog::Success );
				PartitionToggle();
			}
			else
				m_pConsole->AddLog( "Error opening file to load", eConsoleLog::Error );
		}
		else
			m_pConsole->AddLog( "File \'" + mapName + "\' does not exist", eConsoleLog::Error );
	}

	m_isLoadScheduled = false;
	m_loadingMap = false;
}
//-------------------------------------------------------------------
//	ProcessInput
//-------------------------------------------------------------------
void EditorWorld::ProcessInput()
{
	if ( g_pKeyMap->CanPerform(editorGlobal::action::QUICKSAVE) )
		Save( m_mapName );
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::QUICKLOAD) )
		Load( m_mapName );
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::GRID_TOGGLE) )
		m_pGrid->ToggleGrid();
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::MODE_CHANGE) )
		g_pCore->ChangeWorld( new GameWorld() );
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::TEST_CREATE_BLOCK) )
	{
		SmartPtr<Event_ButtonUse> pEvent = new Event_ButtonUse;
		pEvent->entityType = eEditorEntityType::Block;
		pEvent->action = eGuiAction::CreateEntity;

		g_pEvent->Process( pEvent );
	}
	else if ( g_pKeyMap->CanPerform(editorGlobal::action::PARTITION_TOGGLE) )
	{
		PartitionToggle();
	}
}
//-------------------------------------------------------------------
//	GetGrid
//-------------------------------------------------------------------
Grid* EditorWorld::GetGrid()
{
	return m_pGrid.Get();
}
//-------------------------------------------------------------------
//	EntityGet
//-------------------------------------------------------------------
Handle<Entity> EditorWorld::EntityGet(uint id)
{
	return m_entities.Get( id );
}
//-------------------------------------------------------------------
const Handle<Entity> EditorWorld::EntityGet(uint id)const
{
	for ( auto it = m_entities.GetIterator(); it; it++ )
	{
		if ( (*it)->GetId() == id )
			return (*it);
	}

	return Handle<Entity>();
}
//-------------------------------------------------------------------
//	EntityCreate
//-------------------------------------------------------------------
Handle<Entity> EditorWorld::EntityCreate(eEditorEntityType type)
{
	Handle<Entity> hResult;

	switch ( type )
	{
		case eEditorEntityType::Block:
			hResult = g_pEntity->Create<EditorEntity_Block>();
			break;
		case eEditorEntityType::Door:
			hResult = g_pEntity->Create<EditorEntity_Door>();
			break;
		case eEditorEntityType::PlayerSpawn:
			hResult = g_pEntity->Create<EditorEntity_PlayerSpawn>();
			break;
		case eEditorEntityType::Camera:
			hResult = g_pEntity->Create<EditorEntity_Camera>();
			break;
		case eEditorEntityType::CameraTrack:
			hResult = g_pEntity->Create<EditorEntity_CameraTrack>();
			break;
		case eEditorEntityType::CameraTrackPoint:
			hResult = g_pEntity->Create<EditorEntity_CameraTrackPoint>();
			break;
		case eEditorEntityType::AmbientLight:
			if ( !m_hEntityAmbientLight )
				m_hEntityAmbientLight = g_pEntity->Create<EditorEntity_AmbientLight>();
			hResult = m_hEntityAmbientLight;
			break;
		case eEditorEntityType::PointLight:
			hResult = g_pEntity->Create<EditorEntity_PointLight>();
			break;
		case eEditorEntityType::SpotLight:
			hResult = g_pEntity->Create<EditorEntity_SpotLight>();
			break;
		case eEditorEntityType::TriggerCamera:
			hResult = g_pEntity->Create<EditorEntity_TriggerCamera>();
			break;
		case eEditorEntityType::TriggerCommand:
			hResult = g_pEntity->Create<EditorEntity_TriggerCommand>();
			break;
		case eEditorEntityType::Model:
			hResult = g_pEntity->Create<EditorEntity_Model>();
			break;
		case eEditorEntityType::Talk:
			hResult = g_pEntity->Create<EditorEntity_Talk>();
			break;
		case eEditorEntityType::Monster:
			hResult = g_pEntity->Create<EditorEntity_Monster>();
			break;
		case eEditorEntityType::Partition:
			hResult = g_pEntity->Create<EditorEntity_Partition>();
			break;
		case eEditorEntityType::Refraction:
			hResult = g_pEntity->Create<EditorEntity_Refraction>();
			break;
		default:
			ASSERT( "Unrecognized entity type" );
	}

	return hResult;
}
//-------------------------------------------------------------------
//	EntityClone
//-------------------------------------------------------------------
Handle<Entity> EditorWorld::EntityClone(EditorEntity* pEntity)
{
	Handle<Entity> hResult = EntityCreate( pEntity->GetType() );
	hResult.GetAs<EditorEntity>().OnClone();
	hResult->ProcessCommand( pEntity->GetSerialized() );
	return hResult;
}
//-------------------------------------------------------------------
//	GetEditorEntityList
//-------------------------------------------------------------------
TableIterator<Handle<Entity>> EditorWorld::GetEditorEntityList()
{
	return m_entities.GetIterator();
}
//-------------------------------------------------------------------
//	GetEntityAtRayIntersection
//-------------------------------------------------------------------
Handle<Entity> EditorWorld::GetEntityAtRayIntersection(const math::Vec3& pos, const math::Vec3& dir, uint ignoreId, float* closestT)
{
	Handle<Entity> hEntity;
	float smallestIntersect = math::FLOAT_MAX;

	for ( auto it = m_entities.GetIterator(); it; it++ )
	{
		if ( (*it)->GetId() == ignoreId )
			continue;

		RenderInfo* pRenderInfo = (*it)->GetComponentPtrAs<Component_Render>()->GetData();

		if ( (*it)->GetClassRelationship_Is<EditorEntity_Partition>() && m_partitionState != PARTITION_SHOW )
			continue;

		auto intersect = math::intersectRayAabb( pos, dir, pRenderInfo->GetAabbGlobal() );

		if ( intersect.collisionFlag == math::COLLISION_FRONT && (intersect.t < smallestIntersect) )
		{
			smallestIntersect = intersect.t;
			hEntity = (*it);
		}
	}

	if ( hEntity.IsValid() )
	{
		if ( closestT )
			*closestT = smallestIntersect;
	}

	return hEntity;
}
//-------------------------------------------------------------------
//	GetEntitySelected
//-------------------------------------------------------------------
Handle<Entity> EditorWorld::GetEntitySelected()
{
	return m_hEntitySelected;
}
//-------------------------------------------------------------------
//	GetGui
//-------------------------------------------------------------------
EditorGui* EditorWorld::GetGui()
{
	return m_pGui.Get();
}
//-------------------------------------------------------------------
//	UnselectEntity
//-------------------------------------------------------------------
void EditorWorld::UnselectEntity()
{
	m_hEntitySelected = Handle<Entity>();
	m_pEditorTool->Attach( m_hEntitySelected );
}
//-------------------------------------------------------------------
//	IsToolInUse
//-------------------------------------------------------------------
bool EditorWorld::IsToolInUse()const
{
	return m_pEditorTool && m_pEditorTool->IsUsing();
}
//-------------------------------------------------------------------
//	GetCamera
//-------------------------------------------------------------------
FirstPersonCamera* EditorWorld::GetCamera()
{
	return m_pCamera.Get();
}
//-------------------------------------------------------------------
//	GetCameraTransform
//-------------------------------------------------------------------
math::CameraTransform EditorWorld::GetCameraTransform()const
{
	return m_pCamera->GetCameraTransform();
}
//-------------------------------------------------------------------
//	GetEditorToolManager
//-------------------------------------------------------------------
EditorToolManager* EditorWorld::GetEditorToolManager()
{
	return m_pEditorTool.Get();
}
//-------------------------------------------------------------------
//	GetGuiPageControl
//-------------------------------------------------------------------
GuiPageControl* EditorWorld::GetGuiPageControl()
{
	return m_pGuiPageControl.Get();
}
//-------------------------------------------------------------------
//	GetEntityFinder
//-------------------------------------------------------------------
EditorEntityFinder* EditorWorld::GetEntityFinder()
{
	return m_pFinder.Get();
}
//-------------------------------------------------------------------
//	GetCurrentState
//-------------------------------------------------------------------
const IEditorState* EditorWorld::GetCurrentState()const
{
	return m_pState->GetCurrentState();
}
//-------------------------------------------------------------------
//	DebugCameraPos
//-------------------------------------------------------------------
void EditorWorld::DebugCameraPos()const
{
	math::Vec3 camPos = GetCameraTransform().pos;

	debugToOutput( String("Cam: " + string::vec3ToStr(camPos)).AsChar() );
}
//-------------------------------------------------------------------
//	PartitionToggle
//-------------------------------------------------------------------
void EditorWorld::PartitionToggle()
{
	m_partitionState = (m_partitionState + 1) % PARTITION_COUNT;

	if ( m_hEntitySelected && m_hEntitySelected->GetClassRelationship_Is<EditorEntity_Partition>() )
		UnselectEntity();

	for ( auto it = m_entities.GetIterator(); it; it++ )
	{
		if ( (*it)->GetClassRelationship_Is<EditorEntity_Partition>() )
		{
			auto pCmpRender = (*it)->GetComponentPtrAs<Component_Render>();
			if ( pCmpRender && pCmpRender->GetData() )
			{
				if ( m_partitionState == PARTITION_HIDE )
					pCmpRender->GetData()->visible = false;
				else
				{
					pCmpRender->GetData()->visible = true;
					pCmpRender->GetData()->fillMode = m_partitionState == PARTITION_SHOW ? eFillMode::Solid : eFillMode::Wireframe;
				}
			}
		}
	}
}

//=============================================================================
//	Event handling
//=============================================================================

//-------------------------------------------------------------------
//	HandleEvent
//-------------------------------------------------------------------
void EditorWorld::HandleEvent(Event* pEvent)
{
	Event_EntitySelect* pEventEntitySelect = nullptr;
	Event_EntityUnselect* pEventEntityUnselect = nullptr;
	Event_ButtonUse* pEventButtonUse = nullptr;
	Event_GuiUse* pEventGuiUse = nullptr;
	Event_ChangeTexture* pChangeTexture = nullptr;
	Event_ChangeModel* pChangeModel = nullptr;
	Event_EntityManager* pEventEntity = nullptr;

	// Event_EntitySelect
	if ( isEvent(pEvent, &pEventEntitySelect) )
	{
		m_hEntitySelected = EntityGet( pEventEntitySelect->hEntity->GetId() );
	}
	// Event_EntityUnselect
	else if ( isEvent(pEvent, &pEventEntityUnselect) )
	{
		UnselectEntity();
	}
	// Event_ButtonUse
	else if ( isEvent(pEvent, &pEventButtonUse) )
	{
		if ( pEventButtonUse->action == eGuiAction::CreateEntity )
		{
			EditorState_EntityCreate* pStateEntityCreate = new EditorState_EntityCreate;
			pStateEntityCreate->SetEntityTypeToCreate( pEventButtonUse->entityType );
			m_pState->ChangeState( pStateEntityCreate );
		}
		else if ( pEventButtonUse->action == eGuiAction::OpenTextureSelectMenu )
		{
			OpenTextureSelectMenu( eGuiTexture(pEventButtonUse->flag) );
		}
		else if ( pEventButtonUse->action == eGuiAction::OpenModelSelectMenu )
		{
			OpenModelSelectMenu();
		}
		else if ( pEventButtonUse->action == eGuiAction::OpenEntitySelectMenu )
		{
			OpenEntitySelectMenu();
		}
		else if ( pEventButtonUse->action == eGuiAction::OpenTriggerSelectMenu )
		{
			OpenTriggerSelectMenu();
		}
		else if ( pEventButtonUse->action == eGuiAction::OpenTextArea )
		{
			OpenTextArea( pEventButtonUse->flag );
		}
		else if ( pEventButtonUse->action == eGuiAction::PickEntity )
		{
			EditorState_PickEntity* pStateEntityPick = new EditorState_PickEntity;
			if ( pEventButtonUse->flag != 0 )
				pStateEntityPick->SetEntityFilter( (eEditorEntityType)pEventButtonUse->flag );
			m_pState->ChangeState( pStateEntityPick );
		}
		else if ( pEventButtonUse->action == eGuiAction::PickAxis )
		{
			EditorState_PickAxis* pStateEntityPickAxis = new EditorState_PickAxis;
			m_pState->ChangeState( pStateEntityPickAxis );
		}
		else
		{
			ASSERT( "Unknown action" );
		}
	}
	// Event_ChangeTexture
	else if ( isEvent(pEvent, &pChangeTexture) )
	{
		if ( m_hEntitySelected )
		{
			String textureCommand;
			Handle<ITexture> hTexture = g_pResource->GetTextureMngr()->Get( pChangeTexture->textureId );

			if ( pChangeTexture->textureType == eGuiTexture::DiffuseMap ) textureCommand = command::TEXTURE_DIFFUSE;
			else if ( pChangeTexture->textureType == eGuiTexture::SpecularMap )textureCommand = command::TEXTURE_SPECULAR;
			else if ( pChangeTexture->textureType == eGuiTexture::NormalMap )textureCommand = command::TEXTURE_NORMAL_MAP;
			else
			{
				ASSERT( "Unrecognized texture type" );
			}

			m_hEntitySelected->ProcessCommand( textureCommand + " " + (hTexture ? hTexture->GetName().GetString() : "") );
		}
	}
	else if ( isEvent(pEvent, &pChangeModel) )
	{
		if ( m_hEntitySelected )
		{
			String modelName = g_pResource->GetModelMngr()->GetModelFileName( pChangeModel->modelId );
			String cmd = command::MODEL + " " + string::quote( modelName );
			m_hEntitySelected->ProcessCommand( cmd );
		}
	}
	// Event_GuiUse
	else if ( isEvent(pEvent, &pEventGuiUse) )
	{
		if ( pEventGuiUse->action == eGuiAction::EntityProcessCommand )
		{
			m_hEntitySelected->ProcessCommand( pEventGuiUse->command );
		}
		else if ( pEventGuiUse->action == eGuiAction::EntityProcessCommandChain )
		{
			HandleEvent_ProcessEntityCommandChain(pEventGuiUse);
		}
	}
	// Event_EntityManager
	else if ( isEvent(pEvent, &pEventEntity) )
	{
		if ( pEventEntity->action == eEventEntity::Create )
		{
			CHECK( !m_entities.Find( pEventEntity->hEntity->GetId() ) );

			if ( pEventEntity->hEntity->GetClassRelationship<EditorEntity>() == eClassRelationship::Inherits )
				m_entities.Add( pEventEntity->hEntity, pEventEntity->hEntity->GetId() );
			else
				ASSERT( "Probably wrong..." );
		}
		else if ( pEventEntity->action == eEventEntity::Remove )
		{
			if ( m_hEntitySelected.IsValid() && pEventEntity->hEntity->GetId() == m_hEntitySelected->GetId() )
				UnselectEntity();
			m_entities.Remove( pEventEntity->hEntity->GetId() );
		}
	}

	if ( pEventEntitySelect || pEventEntityUnselect )
		m_pEditorTool->Attach( m_hEntitySelected );
}
//-------------------------------------------------------------------
//	HandleEvent_ProcessEntityCommandChain
//-------------------------------------------------------------------
void EditorWorld::HandleEvent_ProcessEntityCommandChain(Event_GuiUse* pEventGuiUse)
{
	// Trigger command
	if ( m_hEntitySelected->GetClassRelationship_Is<EditorEntity_TriggerCommand>() )
	{
		EditorEntity_TriggerCommand* pEntity = (EditorEntity_TriggerCommand*)m_hEntitySelected.GetPtr();

		if ( pEventGuiUse->chainCurrent == 1 )
			pEntity->ClearCommands();

		if ( pEventGuiUse->command.Length() > 0 )
			pEntity->AddCommand( pEventGuiUse->command );
	}
	// Talk
	else if ( m_hEntitySelected->GetClassRelationship_Is<EditorEntity_Talk>() )
	{
		EditorEntity_Talk* pEntity = (EditorEntity_Talk*)m_hEntitySelected.GetPtr();

		// Text
		if ( pEventGuiUse->flag == editorGlobal::gui::flag::FLAG_ENTITY_TALK_TEXT )
		{
			if ( pEventGuiUse->chainCurrent == 1 )
				pEntity->ClearTextLines();

			if ( pEventGuiUse->command.Length() > 0 )
				pEntity->AddTextLine( pEventGuiUse->command );
		}
		else if ( pEventGuiUse->flag == editorGlobal::gui::flag::FLAG_ENTITY_TALK_COMMAND )
		{
			if ( pEventGuiUse->chainCurrent == 1 )
				pEntity->ClearCommandLines();

			if ( pEventGuiUse->command.Length() > 0 )
				pEntity->AddCommandLine( pEventGuiUse->command );
		}
		else
			ASSERT( "Invalid flag" );
	}
	// Door
	else if ( m_hEntitySelected->GetClassRelationship_Is<EditorEntity_Door>() )
	{
		EditorEntity_Door* pEntity = (EditorEntity_Door*)m_hEntitySelected.GetPtr();

		if ( pEventGuiUse->chainCurrent == 1 )
			pEntity->ClearLockTextLines();

		if ( pEventGuiUse->command.Length() > 0 )
			pEntity->AddLockTextLine( pEventGuiUse->command );
	}
	else
		ASSERT( "Invalid entity" );

	if ( pEventGuiUse->chainCurrent >= pEventGuiUse->chainCount )
	{
		m_pGui->GetStackTop()->Show( false );
		m_pGui->RemoveStackTop();
		m_pGui->GetStackTop()->Show( true );
	}
}