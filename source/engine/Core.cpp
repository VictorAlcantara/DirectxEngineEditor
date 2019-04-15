#include "Core.h"
#include "../engine/Exception.h"
#include "../engine/Math.h"
#include "Renderer.h"
#include "Input.h"
#include "Timer.h"
#include "Application.h"
#include "ApplicationCoreInterface.h"
#include "EventDispatcher.h"
#include "ResourceManager.h"
#include "../engine/Debug.h"
#include "EntityManager.h"
#include "../engine/Scene.h"
#include "../engine/Path.h"
#include "TextureManager.h"
#include "CollisionManager.h"
#include "Physics.h"
#include "ProcessManager.h"
#include "KeyMap.h"
#include "ModelManager.h"
#include "../engine/TableHashFunction.h"
#include "EventEngine.h"
#include "../engine/CoreInterface.h"

#define POINTER_SHUTDOWN(pointer) \
	if ( pointer ) \
		pointer->Shutdown(); \
	pointer = nullptr;

//-------------------------------------------------------------------
//	Cdtor
//-------------------------------------------------------------------
Core::Core() : m_profiles(50, tableHashFunc_HashString)
{
	m_initialized = false;
	m_profileJustCleared = false;
	SetFps( 60 );
}
//-------------------------------------------------------------------
Core::~Core()
{
	Shutdown();
}
//-------------------------------------------------------------------
//	Initialize
//-------------------------------------------------------------------
void Core::Initialize(platform::ApplicationApi& applicationApi, Application* pApplication)
{
	if ( m_initialized )
	{
		ASSERT( "Core already initialized" );
		return;
	}

	m_pTimer = new Timer;
	m_timeSinceLastFrame = 0.0f;

	SetFps( applicationApi.fps );

	m_pEventDispatcher = new EventDispatcher;

	m_pApplicationInterface = new ApplicationCoreInterface( pApplication );

	m_pInput = new Input;
	m_pInput->Initialize();

	m_pResourceMngr = new ResourceManager;
	m_pResourceMngr->Initialize();

	m_pRenderer = new Renderer;
	m_pRenderer->Initialize( applicationApi );

	m_pEntityMngr = new EntityManager;
	m_pEntityMngr->Initialize();

	m_pScene = new Scene;
	m_pScene->Initialize();

	m_pResourceMngr->GetTextureMngr()->LoadDirectory( path::DIRECTORY_TEXTURE );
	m_pResourceMngr->GetModelMngr()->LoadDirectory( path::DIRECTORY_MODEL );

	m_pCollisionMngr = new CollisionManager;
	m_pCollisionMngr->Initialize();

	m_pPhysics = new Physics;
	m_pPhysics->Initialize();

	m_pProcessMngr = new ProcessManager;
	m_pProcessMngr->Initialize();

	m_pKeyMap = new KeyMap;
	m_pKeyMap->Initialize();

	m_coreState = eCoreState::Idle;

	m_initialized = true;

	auto pEvent = SmartPtr<Event_Core>::New();
	pEvent->action = eCore::InitializationEnd;
	m_pEventDispatcher->Process( pEvent );
}
//-------------------------------------------------------------------
//	Shutdown
//-------------------------------------------------------------------
void Core::Shutdown()
{
	if ( !m_initialized )
		return;

	ProfileClear();

	POINTER_SHUTDOWN( m_pWorld );

	POINTER_SHUTDOWN( m_pKeyMap );
	POINTER_SHUTDOWN( m_pProcessMngr );
	POINTER_SHUTDOWN( m_pPhysics );
	POINTER_SHUTDOWN( m_pCollisionMngr );
	POINTER_SHUTDOWN( m_pEntityMngr );
	POINTER_SHUTDOWN( m_pScene );
	POINTER_SHUTDOWN( m_pRenderer );
	POINTER_SHUTDOWN( m_pResourceMngr );
	POINTER_SHUTDOWN( m_pInput );

	m_pEventDispatcher = nullptr;

	m_initialized = false;
}
//-------------------------------------------------------------------
//	Loop
//-------------------------------------------------------------------
void Core::Loop()
{
	double timeSinceReset = m_pTimer->GetTimeSinceReset();

	if ( m_pWorld && timeSinceReset > (double)m_frameTime )
	{
		m_pTimer->Reset();
		m_timeSinceLastFrame = (float)timeSinceReset;

		#ifdef DEBUG_MODE
			m_timeSinceLastFrame = math::clamp( m_timeSinceLastFrame, 0.0f, 1.0f );
		#endif

		m_coreState = eCoreState::Update;
		m_pProcessMngr->Update();
		m_pWorld->Update();
		m_coreState = eCoreState::Render;

		m_pWorld->Render();

		m_pInput->UpdateAfterCoreLoop();
	
		ProfilerUpdate();

		m_coreState = eCoreState::Idle;
	}
}
//-------------------------------------------------------------------
//	ChangeWorld
//-------------------------------------------------------------------
void Core::ChangeWorld(IWorld* pWorld)
{
	if ( m_pWorld )
		m_pWorld->Shutdown( &m_prevWorldInfo );

	m_pWorld = pWorld;

	if ( m_pWorld )
		m_pWorld->Initialize( &m_prevWorldInfo );
}
//-------------------------------------------------------------------
//	GetTime
//-------------------------------------------------------------------
float Core::GetTime()const
{
	return (float)m_pTimer->GetTimeSinceCreation();
}
//-------------------------------------------------------------------
//	GetTimeSinceLastFrame
//-------------------------------------------------------------------
float Core::GetTimeSinceLastFrame()const
{
	return m_timeSinceLastFrame;
}
//-------------------------------------------------------------------
//	SetFps
//-------------------------------------------------------------------
void Core::SetFps(uint fps)
{
	if ( !math::between<uint>( fps, 1, engineDefault::FPS_MAX ) )
	{
		ASSERT( "Invalid FPS. Changing to 60" );
		fps = 60;
	}

	m_fps = fps;
	m_frameTime = 1.0f / (float)m_fps;
}
//-------------------------------------------------------------------
//	ProfilerUpdate
//-------------------------------------------------------------------
void Core::ProfilerUpdate()
{
	for ( auto it = m_profiles.GetIterator(); it; it++ )
	{
		if ( (*it).state == PROFILE_ENDED )
		{
			(*it).sampleCount++;
			(*it).state = PROFILE_IDLE;
		}
	}

	m_profileJustCleared = false;
}
//-------------------------------------------------------------------
//	ProfileClear
//-------------------------------------------------------------------
void Core::ProfileClear()
{
	m_profiles.Clear();
	m_profileJustCleared = true;
}
//-------------------------------------------------------------------
//	ProfileStart
//-------------------------------------------------------------------
void Core::ProfileStart(const HashString& profileName)
{
	if ( !profileName.IsValid() )
		return;

	auto pProfile = m_profiles.Find( profileName );

	if ( !pProfile )
	{
		pProfile = m_profiles.AddGetPtr( Profile(), profileName );
		pProfile->name = profileName.GetString();
	}

	CHECK( pProfile->state != eProfileState::PROFILE_STARTED );
	
	pProfile->startTime = (float)m_pTimer->GetTimeSinceCreation();
	pProfile->state = eProfileState::PROFILE_STARTED;
}
//-------------------------------------------------------------------
//	ProfileEnd
//-------------------------------------------------------------------
void Core::ProfileEnd(const HashString& profileName)
{
	if ( !profileName.IsValid() )
		return;

	auto pProfile = m_profiles.Find( profileName );

	CHECK( m_profileJustCleared || pProfile );

	if ( pProfile )
	{
		CHECK( m_profileJustCleared || pProfile->state == PROFILE_STARTED );

		pProfile->sampleTime += (float)m_pTimer->GetTimeSinceCreation() - pProfile->startTime;
		pProfile->startTime = -1.0f;
		pProfile->state = eProfileState::PROFILE_ENDED;
	}
}
//-------------------------------------------------------------------
//	ProfileReport
//-------------------------------------------------------------------
String Core::ProfileReport(bool displayMilliseconds)const
{
	uint completeCountMax = 30;
	String result;
	float profileTime = 0.0f;
	float sampleCount = 1.0f;

	uint completeCount = 0;
	for ( auto it = m_profiles.GetIterator(); it; it++ )
	{
		uint l = (*it).name.Length();
		if ( l < completeCountMax )
			completeCount = math::max( completeCount, l );
	}
	

	for ( auto it = m_profiles.GetIterator(); it; it++ )
	{
		String value;

		if ( (*it).sampleCount > 0 )
			sampleCount = (float)(*it).sampleCount;

		float sampleAverage = (*it).sampleTime / sampleCount;
		value = string::floatToStr( sampleAverage * ( displayMilliseconds ? 1000.0f : 1.0f ) );
		profileTime += sampleAverage;

		result += 
			string::completeTrunc((*it).name, ' ', completeCount) + ": " +
			value +
			"\n";
	}

	if ( profileTime != 0.0f )
		result += string::completeTrunc("MAX FPS", ' ', completeCount ) + ": " + string::intToStr((int)(1.0f / profileTime)) + "\n";

	return result;
}
//-------------------------------------------------------------------
//	GetApplication
//-------------------------------------------------------------------
ApplicationCoreInterface* Core::GetApplication()
{
	return m_pApplicationInterface.Get();
}
//-------------------------------------------------------------------
//	GetRenderer
//-------------------------------------------------------------------
Renderer* Core::GetRenderer()
{
	return m_pRenderer.Get();
}
//-------------------------------------------------------------------
//	GetInput
//-------------------------------------------------------------------
Input* Core::GetInput()
{
	return m_pInput.Get();
}
//-------------------------------------------------------------------
//	GetEventDispatcher
//-------------------------------------------------------------------
EventDispatcher* Core::GetEventDispatcher()
{
	return m_pEventDispatcher.Get();
}
//-------------------------------------------------------------------
//	GetResourceMngr
//-------------------------------------------------------------------
ResourceManager* Core::GetResourceMngr()
{
	return m_pResourceMngr.Get();
}
//-------------------------------------------------------------------
//	IsInitialized
//-------------------------------------------------------------------
bool Core::IsInitialized()const
{
	return m_initialized;
}
//-------------------------------------------------------------------
//	GetEntityManager
//-------------------------------------------------------------------
EntityManager* Core::GetEntityMngr()
{
	return m_pEntityMngr.Get();
}
//-------------------------------------------------------------------
//	GetScene
//-------------------------------------------------------------------
Scene* Core::GetScene()
{
	return m_pScene.Get();
}
//-------------------------------------------------------------------
//	GetCollisionMngr
//-------------------------------------------------------------------
CollisionManager* Core::GetCollisionMngr()
{
	return m_pCollisionMngr.Get();
}
//-------------------------------------------------------------------
//	GetPhysics
//-------------------------------------------------------------------
Physics* Core::GetPhysics()
{
	return m_pPhysics.Get();
}
//-------------------------------------------------------------------
//	GetProcessMngr
//-------------------------------------------------------------------
ProcessManager* Core::GetProcessMngr()
{
	return m_pProcessMngr.Get();
}
//-------------------------------------------------------------------
//	GetKeyMap
//-------------------------------------------------------------------
KeyMap* Core::GetKeyMap()
{
	return m_pKeyMap.Get();
}
