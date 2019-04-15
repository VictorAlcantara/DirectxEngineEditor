#pragma once

#include "EditorEntity.h"

class EditorEntity_PlayerSpawn : public EditorEntity
{
	CLASS_TAG( EditorEntity_PlayerSpawn );

	public:
		EditorEntity_PlayerSpawn();
		virtual ~EditorEntity_PlayerSpawn() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::PlayerSpawn; }
		
		void RotateYawPitchRoll(float yaw, float pitch, float roll)override final;

		virtual void ProcessCommand(const ScriptLine*);

	protected:
		void OnInitialize_EditorEntity()override final;
};
