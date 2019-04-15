#pragma once

#include "../engine/Math.h"
#include "GuiUtil.h"
#include "../engine/String.h"
#include "GuiConfig.h"
#include "../engine/Table.h"
#include "../engine/TableHashFunction.h"
#include "../engine/Handle.h"

// TODO: This works, but it is highly inflexible
// change to MVC(?)

class EditorEntity;
class EditorEntity_PointLight;
class EditorGui;
class GuiPage;
class GuiAnimation;
class GuiComponent_Panel;
class Entity;

class GuiPageControl
{
	private:
		struct TextboxInfo
		{
			String nameSufix;
			String labelText;
			uint fontSize;
			math::Vec2 padding;
			textboxFunction pFunction;
			String value;
			ushort maxChars;
			uint charsAllowed;
			uint layer;
			String command;

			TextboxInfo() : pFunction(nullptr), fontSize(16), layer(1), maxChars(10), charsAllowed(GUI_TEXT_ALLOW_ALL) {}
		};

		struct NumberboxInfo
		{
			String nameSufix;
			String labelText;
			uint fontSize;
			math::Vec2 padding;
			numberboxFunction pFunction;
			float value;
			float valueMin;
			float valueMax;
			uint layer;
			String command;

			NumberboxInfo() : pFunction(nullptr), fontSize(16), layer(1), valueMin(0.0f), valueMax(1.0f), value(0.0f) {}
		};

		struct SliderInfo
		{
			String nameSufix;
			String labelText;
			uint fontSize;
			math::Vec2 padding;
			sliderFunction pFunction;
			float value;
			struct
			{
				bool use = false;
				float value = 0.0f;
			} defaultValue;
			float valueMin;
			float valueMax;
			uint layer;
			String command;

			SliderInfo() : value(0.0f), valueMin(0.0f) , valueMax(1.0f), pFunction(nullptr), fontSize(12), layer(1) {}
		};

		struct CheckboxInfo
		{
			String nameSufix;
			String labelText;
			math::Vec2 padding;
			checkboxFunction pFunction;
			bool value;
			String command;
			uint layer;
			uint fontSize;

			CheckboxInfo() : value(false), pFunction(nullptr), layer(1), fontSize(12) {}
		};

		struct ButtonTextInfo
		{
			String nameSufix;
			GuiConfig_Button configButton;
			struct TextInfo
			{
				String label;
				float charSize;
				math::Vec4 color4;

				TextInfo() : charSize(12.0f), color4(1.0f) {}

			} text;
		};

		struct PropertyNameValue
		{
			eGuiComponent componentType;
			HashString sufix;
			float valueFloat;
			String valueStr;
			bool valueBool;

			PropertyNameValue() : componentType(eGuiComponent::Slider), valueFloat(0.0f) {}
			PropertyNameValue(const String& s, float v, eGuiComponent type) : componentType(type), sufix(s), valueFloat(v) {}
			PropertyNameValue(const String& s, const String& v, eGuiComponent type) : componentType(type), sufix(s), valueStr(v) {}
			PropertyNameValue(const String& s, bool v, eGuiComponent type) : componentType(type), sufix(s), valueBool(v) {}
		};

		struct PropertyNameValueTable
		{
			private:
				Table<PropertyNameValue, HashString> table;

			public:
				PropertyNameValueTable() : table(30, tableHashFunc_HashString) {}

				void Add(const String& s, float value, eGuiComponent type)
				{
					table.Add( PropertyNameValue(s, value, type), HashString(s) );
				}

				void Add(const String& s, const String& value, eGuiComponent type)
				{
					table.Add( PropertyNameValue(s, value, type), HashString(s) );
				}

				void Add(const String& s, bool value, eGuiComponent type)
				{
					table.Add( PropertyNameValue(s, value, type), HashString(s) );
				}

				PropertyNameValue Get(const String& sufix)const
				{
					CHECK( table.Find(sufix) );
					return table.Get( HashString(sufix) );
				}

				TableIteratorConst<PropertyNameValue> GetIterator()const
				{
					return table.GetIterator();
				}
		};

	public:
		void ShowPageMain();
		bool ShowPageProperty(EditorEntity*);
		void OpenTextureSelectMenu(eGuiTexture);
		void OpenModelSelectMenu();
		void OpenEntitySelectMenu();
		void OpenTriggerSelectMenu();
		void OpenTextArea(const List<String>&, uint flag);

	private:
		EditorGui* GetGui();
		void ShowPage(GuiPage*, bool clearStack);

		GuiPage* CreatePageProperty_Block(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_Door(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_CameraTrack();
		GuiPage* CreatePageProperty_AmbientLight(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_PointLight(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_SpotLight(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_TriggerCamera();
		GuiPage* CreatePageProperty_TriggerCommand(PropertyNameValueTable& valueTable);
		GuiPage* CreatePageProperty_Model(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_Talk(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_Partition(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_Refraction(PropertyNameValueTable&);
		GuiPage* CreatePageProperty_Monster(PropertyNameValueTable&);

		GuiComponent_Panel* CreatePropertyPanel(GuiPage*);
		
		GuiAnimation* CreateAnimation_Button();
		GuiAnimation* CreateAnimation_StrechAlphaWeak();
		GuiAnimation* CreateAnimation_StrechInsideOut();
		GuiAnimation* CreateAnimation_StrechElastic();

		float MakeSlider(GuiPage*, IGuiComponent*, SliderInfo&);
		math::Vec2 MakeButtonText(GuiPage*, IGuiComponent*, ButtonTextInfo&);
		void MakeTextbox(GuiPage* pPage, IGuiComponent* pParent, TextboxInfo&);
		void MakeNumberbox(GuiPage* pPage, IGuiComponent* pParent, NumberboxInfo&);
		void MakeCheckbox(GuiPage* pPage, IGuiComponent* pParent, CheckboxInfo&);

		void SetValues(GuiPage*, const PropertyNameValueTable&);
		void SetValue_Slider(GuiPage*, const String& componentName, float value);
		void SetValue_Textbox(GuiPage*, const String& componentName, const String& value);
		void SetValue_Numberbox(GuiPage*, const String& componentName, float value);
		void SetValue_Checkbox(GuiPage*, const String& componentName, bool value);
};
