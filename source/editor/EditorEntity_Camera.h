#pragma once

#include "EditorEntity.h"

class EditorEntity_Camera : public EditorEntity
{
	CLASS_TAG( EditorEntity_Camera );

	public:
		EditorEntity_Camera();
		virtual ~EditorEntity_Camera();
	
		eEditorEntityType GetType()const override final { return eEditorEntityType::Camera; }
		String GetSerialized()const override;
		void ProcessCommand(const ScriptLine*);

	protected:
		void OnInitialize_EditorEntity()override final;

	private:
		math::Vec3 m_direction;
};

