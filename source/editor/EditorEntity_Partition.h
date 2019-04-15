#pragma once

#include "EditorEntity.h"
#include "../engine/Pointer.h"

class EditorEntity_Partition : public EditorEntity
{
	CLASS_TAG( EditorEntity_Partition );

	public:
		EditorEntity_Partition();
		virtual ~EditorEntity_Partition() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Partition; }

		void ProcessCommand(const ScriptLine*)override final;

		const HashString& DebugGetAssertOnEntityContact()const { return m_debugEntityContact; }
		bool DebugGetAssertOnAddEntity()const { return m_debugAddEntity; }
		bool DebugGetAssertOnCreatePartition()const { return m_debugCreatePartition; }

	protected:
		void OnInitialize_EditorEntity()override final;

	private:
		HashString m_debugEntityContact;
		bool m_debugAddEntity;
		bool m_debugCreatePartition;
};
