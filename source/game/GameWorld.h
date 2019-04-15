#pragma once

#include "../engine/WorldInterface.h"
#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/Handle.h"
#include "../engine/Effect.h"
#include "CameraManager.h"
#include "../engine/EventListener.h"

class Entity;
class Entity_Player;
class GameGui;
class GameCommand;
class Console;
class Process_GuiDisplayRadiusMonitor;

class GameWorld : public IWorld, public EventListener
{
	CLASS_TAG( GameWorld );

	public:
		GameWorld();
		virtual ~GameWorld();

		virtual void Initialize(const WorldInfo*)override final;
		virtual void Shutdown(WorldInfo*)override final;
		virtual void Update()override final;
		virtual void Render()override final;

		void ScheduleLoad(const String&);

		virtual void HandleEvent(Event*)override final;

		void ShowStartMessage(const String&);
		void ShowEndMessage(const String&);

		GameCommand* GetCommand();

	private:
		void Load(const String&);
		void ProcessInput();

		HandleAs<Entity, Entity_Player> m_hPlayer;
		bool m_isLoadScheduled;
		String m_mapName;
		Handle<Model> m_hSceneQuad;
		SimplePtr<Effect_Texture> m_pEffectScene;
		SimplePtr<CameraManager> m_pCameraManager;
		SmartPtr<GameCommand> m_pCommand;
		SmartPtr<Console> m_pConsole;
		SmartPtr<GameGui> m_pGui;
		SmartPtr<Process_GuiDisplayRadiusMonitor> m_pGuiDisplayRadiusMonitor;
};
