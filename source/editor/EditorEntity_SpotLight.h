#pragma once

#include "EditorEntity.h"
#include "../engine/Light.h"
#include "../engine/Handle.h"
#include "../engine/String.h"

class EditorEntity_SpotLight : public EditorEntity
{
	CLASS_TAG( EditorEntity_SpotLight );

	public:
		EditorEntity_SpotLight();
		virtual ~EditorEntity_SpotLight() = default;

		void RotateYawPitchRoll(float yaw, float pitch, float roll)override final;

		void ProcessCommand(const ScriptLine*);

		Handle<SpotLight> GetLight() { return m_hLight; }
		String GetSerialized()const override;
		eEditorEntityType GetType()const override final { return eEditorEntityType::SpotLight; }

		float GetEditorShadowBias()const { return m_editorShadowBias; }

	protected:
		void Update()override final;
		void OnInitialize_EditorEntity()override final;
		void OnShutdown_EditorEntity()override final;

	private:
		Handle<SpotLight> m_hLight;
		float m_editorShadowBias;
};
