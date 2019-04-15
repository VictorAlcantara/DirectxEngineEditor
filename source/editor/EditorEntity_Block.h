#pragma once

#include "EditorEntity.h"
#include "../engine/Pointer.h"

class EditorEntity_Block : public EditorEntity
{
	CLASS_TAG( EditorEntity_Block );

	public:
		EditorEntity_Block();
		virtual ~EditorEntity_Block() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Block; }

		void ProcessCommand(const ScriptLine*)override final;

	protected:
		void OnInitialize_EditorEntity()override final;
};
