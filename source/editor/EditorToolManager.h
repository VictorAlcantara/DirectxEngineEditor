#pragma once

#include "../engine/Class.h"
#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "EditorTool.h"

class EditorEntity;
class Effect_Color;
class Effect_LightDir4;

class EditorToolManager : NonCopyable
{

	public:
		EditorToolManager();
		~EditorToolManager();

		void Initialize();
		void Shutdown();
		void Update(const math::CameraTransform&);
		void Render(const math::CameraTransform&);

		void Attach(Handle<Entity>);
		void Detach();

		bool IsUsing()const;

	private:
		bool HasSpecificToolAction(eEditorToolAction* pOutAction)const;
		void HandleClick(const math::CameraTransform& camera);
		void HandleUnclick();
		void HandleToolUse(const math::CameraTransform& camera);
		void HandleAdjustToGrid();

		void RenderPassOccluded(const math::CameraTransform&);
		void RenderPassNormal(const math::CameraTransform&);

		bool m_initialized;
		Handle<Entity> m_hEntitySelected;

		SimplePtr<EditorTool> m_editorTools[3];
		eEditorToolAction m_editorToolAction;
		EditorTool* m_pCurrent;

		SimplePtr<Effect_Color> m_pEffectOneColor;
		SimplePtr<Effect_LightDir4> m_pEffectLightDir4;
};
