#pragma once

#include "EditorEntity.h"

class EditorEntity_CameraTrack;

class EditorEntity_CameraTrackPoint : public EditorEntity
{
	CLASS_TAG( EditorEntity_CameraTrackPoint );
	friend class EditorEntity_CameraTrack;

	public:
		EditorEntity_CameraTrackPoint();
		virtual ~EditorEntity_CameraTrackPoint() = default;

		void OnToolUse()override final;

		String GetSerialized()const override { return String(); }
		eEditorEntityType GetType()const override final { return eEditorEntityType::CameraTrackPoint; }

		void ProcessCommand(const ScriptLine*)override final {}

		bool CanDelete()const override final { return false; }
		bool CanClone()const override final { return false; }

		Handle<Entity> GetOwner();
		EditorEntity_CameraTrack* GetOwnerPtr();
		const EditorEntity_CameraTrack* GetOwnerPtr()const;

	protected:
		void OnInitialize_EditorEntity()override final;
		void SetOwner(Handle<Entity>);

	private:
		Handle<Entity> m_hOwner;
};

