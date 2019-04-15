#pragma once

#include "EditorEntity.h"

class EditorEntity_Talk : public EditorEntity
{
	CLASS_TAG( EditorEntity_Talk );

	public:
		EditorEntity_Talk();
		virtual ~EditorEntity_Talk() = default;

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Talk; }

		void ProcessCommand(const ScriptLine*)override final;

		void AddTextLine(const String&);
		void ClearTextLines();
		const List<String>& GetTextLines()const;

		void AddCommandLine(const String&);
		void ClearCommandLines();
		const List<String>& GetCommandLines()const;

		bool IsFireEventOnce()const { return m_eventFireOnce; }

	protected:
		void OnInitialize_EditorEntity()override final;
		void OnShutdown_EditorEntity()override final;

	private:
		List<String> m_textLines;
		bool m_eventFireOnce;
		List<String> m_cmdLines;
};
