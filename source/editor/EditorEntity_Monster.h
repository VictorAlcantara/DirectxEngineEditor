#pragma once

#include "EditorEntity.h"

class EditorEntity_Monster : public EditorEntity
{
	CLASS_TAG( EditorEntity_Monster );

	public:
		EditorEntity_Monster();
		virtual ~EditorEntity_Monster() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Monster; }

		virtual void ProcessCommand(const ScriptLine*);

		bool SpawnOnCreate()const { return m_spawnOnCreate; }
		bool Active()const { return m_active; }

	protected:
		void OnInitialize_EditorEntity()override final;

	private:
		bool m_spawnOnCreate;
		bool m_active;
};


