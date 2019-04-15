#pragma once

#include "EditorEntity.h"
#include "../engine/Pointer.h"

struct Debug_Cube;

class EditorEntity_Model : public EditorEntity
{
	CLASS_TAG( EditorEntity_Model );

	public:
		EditorEntity_Model();
		virtual ~EditorEntity_Model() = default;

		void RotateYawPitchRoll(float yaw, float pitch, float roll)override final;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Model; }

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize_EditorEntity()override final;
		void OnShutdown_EditorEntity()override final;
		void Update()override final;

	private:
		void OnModelChange();

		Handle<Debug_Cube> m_hAabb;
		mutable math::Euler m_angle;
};
