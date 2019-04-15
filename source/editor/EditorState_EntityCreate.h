#pragma once

#include "EditorState.h"
#include "EditorEntity.h"
#include "../shared/VecGrid.h"
#include "../engine/Gradient.h"
#include "../engine/Exception.h"

struct RenderInfo_EditorEntity;

class EditorState_EntityCreate : public EditorState_Base
{
	public:
		EditorState_EntityCreate();
		virtual ~EditorState_EntityCreate() = default;

		void SetEntityTypeToCreate(eEditorEntityType type) { CHECK(type != eEditorEntityType::None); m_editorEntityType = type; }

		void Enter()override;
		void Update()override;
		void Exit()override;

		eEditorState GetType()const override { return eEditorState::EntityCreate; }

	private:
		void CreateEntityGhost();
		VecGrid GetEntityGridPos(bool attachToOtherBlocks);
		void SetAlpha(float);
		void RestoreRenderData();

		RenderInfo_EditorEntity* GetRenderInfo(Entity&, bool updateTransform);

		EditorEntity* m_pEntityGhost;
		eEditorEntityType m_editorEntityType;
		Gradient<float, GradientTransition_Smooth, GradientPeriod_Invert> m_gradient;
		float m_entityAlphaDefault;
		eMaterial m_entityMaterialDefault;
		bool m_castShadowDefault;
};
