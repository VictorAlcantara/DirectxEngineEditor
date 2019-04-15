#pragma once

#include "EditorState.h"
#include "EditorEntityUtil.h"

class EditorState_PickEntity : public EditorState_Base
{
	public:
		EditorState_PickEntity();
		virtual ~EditorState_PickEntity() = default;

		void Enter()override;
		void Update()override;

		void SetEntityFilter(eEditorEntityType filter) { m_filter = filter; }
		bool UsesFilter()const { return m_filter != eEditorEntityType::Unknown && m_filter != eEditorEntityType::None; }

		eEditorState GetType()const override final { return eEditorState::PickEntity; }

	private:
		eEditorEntityType m_filter;
		bool m_isShowingGrid;
};
