#pragma once

#include "../engine/Class.h"
#include "Platform.h"
#include "../engine/Pointer.h"
#include "../engine/WorldInterface.h"
#include "../engine/Exception.h"
#include "../engine/Table.h"

class Timer;
class Renderer;
class Input;
class Application;
class ApplicationCoreInterface;
class EventDispatcher;
class ResourceManager;
class EntityManager;
class Scene;
class CollisionManager;
class Physics;
class ProcessManager;
class KeyMap;

enum class eCoreState
{
	Idle,
	Update,
	Render,
};

enum eProfileState
{
	PROFILE_IDLE = 0,
	PROFILE_STARTED,
	PROFILE_ENDED,
};

struct Profile
{
	String name;
	float startTime = -1.0f;
	eProfileState state = eProfileState::PROFILE_IDLE;
	float sampleTime = 0.0f;
	uint sampleCount = 0;
};

class Core : public Singleton<Core>
{
	friend class Singleton<Core>;

	public:
		virtual ~Core();

		void SetFps(uint);

		void Loop();
		void Initialize(platform::ApplicationApi&, Application*);
		void Shutdown();
		void ChangeWorld(IWorld*);

		float GetTime()const;
		float GetTimeSinceLastFrame()const;

		ApplicationCoreInterface* GetApplication();
		Renderer* GetRenderer();
		Input* GetInput();
		EventDispatcher* GetEventDispatcher();
		ResourceManager* GetResourceMngr();
		EntityManager* GetEntityMngr();
		Scene* GetScene();
		CollisionManager* GetCollisionMngr();
		Physics* GetPhysics();
		ProcessManager* GetProcessMngr();
		KeyMap* GetKeyMap();

		bool IsInitialized()const;

		template <class TYPE>
		TYPE* GetWorld();

		void ProfileClear();
		void ProfileStart(const HashString& profileName);
		void ProfileEnd(const HashString& profileName);
		String ProfileReport(bool displayMiliseconds)const;

		eCoreState GetState()const { return m_coreState; }

	private:
		Core();

		void ProfilerUpdate();

		bool m_initialized;

		SimplePtr<Timer> m_pTimer;
		uint m_fps;
		float m_frameTime;
		float m_timeSinceLastFrame;
		eCoreState m_coreState;

		bool m_profileJustCleared;

		SimplePtr<Renderer> m_pRenderer;
		WorldInfo m_prevWorldInfo;
		SimplePtr<IWorld> m_pWorld;
		SimplePtr<Input> m_pInput;
		SimplePtr<ApplicationCoreInterface> m_pApplicationInterface;
		SimplePtr<EventDispatcher> m_pEventDispatcher;
		SimplePtr<ResourceManager> m_pResourceMngr;
		SimplePtr<EntityManager> m_pEntityMngr;
		SimplePtr<Scene> m_pScene;
		SimplePtr<CollisionManager> m_pCollisionMngr;
		SimplePtr<Physics> m_pPhysics;
		SimplePtr<ProcessManager> m_pProcessMngr;
		SimplePtr<KeyMap> m_pKeyMap;
		Table<Profile, HashString> m_profiles;
};

//=============================================================================
//	Template method definition
//=============================================================================

template <class TYPE>
inline TYPE* Core::GetWorld()
{
	if ( !m_pWorld )
		return nullptr;

	if ( TYPE::GetClassTagStatic() == m_pWorld->GetClassTag() )
		return (TYPE*)m_pWorld.Get();

	THROW( "Core::GetWorld() wrong world type casting" );

	return nullptr;
}
