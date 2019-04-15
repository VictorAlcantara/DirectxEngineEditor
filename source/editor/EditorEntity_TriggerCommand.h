#pragma once

#include "EditorEntity.h"

class EditorEntity_TriggerCommand : public EditorEntity
{
	CLASS_TAG( EditorEntity_TriggerCommand );

	public:
		EditorEntity_TriggerCommand();
		virtual ~EditorEntity_TriggerCommand() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::TriggerCommand; }

		void ProcessCommand(const ScriptLine*)override final;

		void AddCommand(const String&);
		void ClearCommands();
		const List<String>& GetCommands()const;

		bool IsFireEventOnce()const { return m_eventFireOnce; }
		bool IsActive()const { return m_active; }

	protected:
		void OnInitialize_EditorEntity()override final;

	private:
		List<String> m_cmds;
		bool m_eventFireOnce;
		bool m_active;
};

