#pragma once

#include "EditorEntity.h"
#include "../engine/Math.h"

class EditorEntity_AmbientLight : public EditorEntity
{
	CLASS_TAG( EditorEntity_AmbientLight );

	public:
		EditorEntity_AmbientLight();
		virtual ~EditorEntity_AmbientLight() = default;

		const math::Vec4& GetAmbientColor()const { return m_ambientColor4; }

		eEditorEntityType GetType()const override final { return eEditorEntityType::AmbientLight; }
		String GetSerialized()const override;
		void ProcessCommand(const ScriptLine*);

	protected:
		void OnInitialize_EditorEntity()override final;
		void OnShutdown_EditorEntity()override final;

	private:
		math::Vec4 m_ambientColor4;
};

