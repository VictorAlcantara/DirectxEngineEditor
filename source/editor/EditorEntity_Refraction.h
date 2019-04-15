#pragma once

#include "EditorEntity.h"

class EditorEntity_Refraction : public EditorEntity
{
	CLASS_TAG( EditorEntity_Refraction );

	public:
		EditorEntity_Refraction();
		virtual ~EditorEntity_Refraction() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Refraction; }

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize_EditorEntity()override final;
};
