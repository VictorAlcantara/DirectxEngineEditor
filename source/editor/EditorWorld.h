#pragma once

#include "../engine/WorldInterface.h"
#include "../engine/Pointer.h"
#include "../engine/List.h"
#include "../engine/Math.h"
#include "../shared/VecGrid.h"
#include "../engine/EventListener.h"
#include "EditorEntity.h"
#include "../engine/Handle.h"
#include "GuiUtil.h"
#include "../engine/Table.h"
#include "EditorEntityFinder.h"
#include "../engine/Process.h"

class FirstPersonCamera;
class EditorToolManager;
class Grid;
class EditorGui;
class Effect_Color;
class Model;
class RenderTarget;
class Shader_Texture;
class EditorStateMachine;
class PlayerSpawn;
class EditorEntity_Block;
class EditorEntity_PlayerSpawn;
class Effect_LightDir4;
class GuiPageControl;
class ITexture;
class IEditorState;
class EditorCommand;
class Console;
class Process_Load;
struct Event_GuiUse;

class EditorWorld : public IWorld, public EventListener
{
	friend class Process_Load;
	CLASS_TAG( EditorWorld );

	private:
		enum ePartitonState
		{
			PARTITION_SHOW_PARCIAL = 0,
			PARTITION_HIDE,
			PARTITION_SHOW,

			PARTITION_COUNT
		};

	public:
		EditorWorld();

		void Initialize(const WorldInfo*)override final;
		void Shutdown(WorldInfo*)override final;
		void Update()override final;
		void Render()override final;

		void HandleEvent(Event*)override final;

		void ScheduleNew();
		void ScheduleLoad(const String& mapName);
		void Save(const String& mapName);

		Grid* GetGrid();
		EditorGui* GetGui();

		Handle<Entity> EntityGet(uint);
		const Handle<Entity> EntityGet(uint)const;
		Handle<Entity> EntityCreate(eEditorEntityType);
		Handle<Entity> EntityClone(EditorEntity*);
		TableIterator<Handle<Entity>> GetEditorEntityList();
		Handle<Entity> GetEntityAtRayIntersection(const math::Vec3& pos, const math::Vec3& dir, uint ignoreId = 0, float* closestT = nullptr);
		Handle<Entity> GetEntitySelected();

		bool IsToolInUse()const;
		bool IsLoadingMap()const { return m_loadingMap; }
		FirstPersonCamera* GetCamera();
		math::CameraTransform GetCameraTransform()const;
		EditorToolManager* GetEditorToolManager();
		GuiPageControl* GetGuiPageControl();

		EditorEntityFinder* GetEntityFinder();

		const IEditorState* GetCurrentState()const;

		void DebugCameraPos()const;
		void PartitionToggle();

	private:
		void ProcessInput();
		void UnselectEntity();
		void RenderScene();
		void OpenTextureSelectMenu(eGuiTexture);
		void OpenModelSelectMenu();
		void OpenEntitySelectMenu();
		void OpenTriggerSelectMenu();
		void OpenTextArea(uint flag);
		void New();
		void Load(const String& mapName);

		void HandleEvent_ProcessEntityCommandChain(Event_GuiUse*);

		bool m_initialized;
		bool m_loadingMap;
		bool m_isNewScheduled;
		bool m_isLoadScheduled;
		String m_mapName;
		SimplePtr<FirstPersonCamera> m_pCamera;

		int m_partitionState;

		Table<Handle<Entity>, uint> m_entities;
		Handle<Entity> m_hEntitySelected;
		Handle<Entity> m_hEntityAmbientLight;

		SimplePtr<EditorToolManager> m_pEditorTool;
		SimplePtr<Grid> m_pGrid;

		SimplePtr<EditorGui> m_pGui;
		SimplePtr<GuiPageControl> m_pGuiPageControl;

		SimplePtr<Effect_Color> m_pEffectOneColor;

		SimplePtr<Model> m_pModelRenderTarget;
		SimplePtr<Shader_Texture> m_pShaderRenderTarget;

		SimplePtr<EditorStateMachine> m_pState;

		Handle<Model> m_hSphereLine;
		Handle<Model> m_hLine;
		Handle<Model> m_hCubeLine;
		Handle<Model> m_hPyramidLine;

		SimplePtr<EditorEntityFinder> m_pFinder;

		// Obj
		SimplePtr<Effect_LightDir4> m_pEffectObj;
		Handle<Model> m_hModelObj;

		SmartPtr<Console> m_pConsole;
		SmartPtr<EditorCommand> m_pCommand;
};
