#pragma once

#include "../engine/Math.h"
#include "../engine/Pointer.h"
#include "../engine/String.h"
#include "GuiComponent.h"
#include "../engine/Table.h"
#include "../engine/Handle.h"

class ITexture;
class GuiPage;

class EditorGui
{
	public:
		EditorGui();
		~EditorGui();

		void Initialize();
		void Shutdown();
		void Update();
		void Render(const math::CameraTransform&);

		GuiPage* CreatePage(const HashString&);
		void HideAll();
		void RemovePage(const HashString&);

		GuiPage* FindPage(const HashString&);
		const GuiPage* FindPage(const HashString&)const;

		void RemoveFromAll(const HashString& componentName);

		void AddToStack(GuiPage*);
		void RemoveStackTop();
		void RemoveStackBottom();
		void RemoveFromStack(const HashString&);
		void ClearStack();
		uint GetStackCount()const;
		GuiPage* GetStackTop();
		const GuiPage* GetStackTop()const;
		GuiPage* GetStackBottom();
		const GuiPage* GetStackBottom()const;
		const List<GuiPage*>& GetStack()const;

		bool IsCursorOnTopAnyPage()const;

	private:
		bool m_initialized;
		Table<SmartPtr<GuiPage>, uint> m_pages;
		List<GuiPage*> m_pageStack;
};
