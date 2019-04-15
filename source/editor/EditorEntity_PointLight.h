#pragma once

#include "EditorEntity.h"
#include "../engine/Light.h"
#include "../engine/Handle.h"

class EditorEntity_PointLight : public EditorEntity
{
	CLASS_TAG( EditorEntity_PointLight );
	
	public:
		EditorEntity_PointLight();
		virtual ~EditorEntity_PointLight() = default;

		Handle<PointLight> GetLight() { return m_hLight; }

		eEditorEntityType GetType()const override final { return eEditorEntityType::PointLight; }
		String GetSerialized()const override;
		void ProcessCommand(const ScriptLine*);

	protected:
		void Update()override final;
		void OnInitialize_EditorEntity()override final;
		void OnShutdown_EditorEntity()override final;

	private:
		Handle<PointLight> m_hLight;
};
