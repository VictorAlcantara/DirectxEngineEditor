#pragma once

#include "EditorEntity.h"
#include "../engine/Process.h"

struct Debug_Line;
class Process_EditorCameraTrackFind;

class EditorEntity_TriggerCamera : public EditorEntity
{
	CLASS_TAG( EditorEntity_TriggerCamera );

	public:
		EditorEntity_TriggerCamera();
		virtual ~EditorEntity_TriggerCamera() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::TriggerCamera; }

		void ProcessCommand(const ScriptLine*)override final;

		void OnPickEntity(Handle<Entity>)override final;

		void ConnectToTrack(Handle<Entity>);

	protected:
		void OnInitialize_EditorEntity()override final;
		void Update()override final;
		void OnShutdown_EditorEntity()override final;

	private:
		Handle<Entity> m_hCameraTrack;
		Handle<Debug_Line> m_hLine;
		SmartPtr<Process_EditorCameraTrackFind> m_pProcessTrackFind;
};

class Process_EditorCameraTrackFind : public Process
{
	public:
		Process_EditorCameraTrackFind();
		virtual ~Process_EditorCameraTrackFind() = default;

		void SetTrackIdToFind(uint trackId);
		void SetOwner(Handle<Entity>);

	protected:
		void OnEnter()override final;
		void Update()override final;
	
	private:
		Handle<Entity> m_hOwner;
		uint m_trackId;
};
