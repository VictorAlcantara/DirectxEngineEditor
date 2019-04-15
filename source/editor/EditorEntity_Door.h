#pragma once

#include "EditorEntity.h"

struct Debug_Line;

class EditorEntity_Door : public EditorEntity
{
	CLASS_TAG( EditorEntity_Door );

	public:
		EditorEntity_Door();
		virtual ~EditorEntity_Door() = default;

		void SetOpenDirection(const math::Vec3&);

		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::Door; }

		void ProcessCommand(const ScriptLine*)override final;

		float GetOpenAmount()const { return m_openAmount; }
		float GetDuration()const { return m_duration; }

		bool IsLocked()const { return m_locked; }

		void ClearLockTextLines() { return m_lockTextLines.Clear(); }
		void AddLockTextLine(const String& s) { return m_lockTextLines.AddLast(s); }
		const List<String>& GetLockTextLines()const { return m_lockTextLines; }

	protected:
		void OnInitialize_EditorEntity()override final;
		void OnShutdown_EditorEntity()override final;
		void Update()override final;

	private:
		bool m_locked;
		float m_openAmount;
		float m_duration;
		math::Vec3 m_openDirection;
		List<String> m_lockTextLines;
		Handle<Debug_Line> m_hLine;
};

