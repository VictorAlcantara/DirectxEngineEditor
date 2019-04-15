#pragma once

#include "EditorEntity.h"
#include "../engine/Handle.h"

struct Debug_Line;

class EditorEntity_CameraTrack : public EditorEntity
{
	CLASS_TAG( EditorEntity_CameraTrack );

	public:
		EditorEntity_CameraTrack();
		virtual ~EditorEntity_CameraTrack() = default;

		void OnToolUse()override final;

		Handle<Entity> GetTrackPoint0();
		Handle<Entity> GetTrackPoint1();

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::CameraTrack; }

		void ProcessCommand(const ScriptLine*)override final;

		uint GetTrackId()const { return m_trackId; }

		void OnClone()override final;

	protected:
		void OnShutdown_EditorEntity()override final;
		void OnInitialize_EditorEntity()override final;
		void Update()override final;

	private:
		void AdjustPointPosition();
		void AssignNewTrackId(uint);

		static uint s_trackIdControl;

		bool m_isClone;
		uint m_trackId;

		Handle<Entity> m_hTrackPoint0;
		Handle<Entity> m_hTrackPoint1;

		Handle<Debug_Line> m_hLine0;
		Handle<Debug_Line> m_hLine1;
};

