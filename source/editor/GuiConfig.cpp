#include "GuiConfig.h"

GuiConfig_Button::GuiConfig_Button()
{
	idleColor4 = math::saturateRgb( math::Vec3(0.66f, 0.75f, 0.95f), 0.95f );
	mouseOverColor4 = math::clampUnit( idleColor4.Xyz() * 1.3f ).Xyzw(1.0f);
	pClickFunction = nullptr;
	flag = 0;
}

GuiConfig_Slider::GuiConfig_Slider()
{
	sliderColor3 = math::saturateRgb( math::Vec3(0.70f, 0.80f, 0.95f) * 0.9f, 0.95f ).Xyz(); 
	circleColor3 = math::clampUnit( sliderColor3 * 0.9f );
	minValue = 0.0f;
	maxValue = 1.0f;
	pSliderFunction = nullptr;

	dimension = math::Vec2( 100, 12 );
	circleScale = dimension.y * 0.6f;
}

GuiConfig_Textbox::GuiConfig_Textbox()
{
	pTextboxFunction = nullptr;
	charSize = 12;
	charSpaceProportion = 0.08f;
	charsAllowed = GUI_TEXT_ALLOW_ALL;
	maxChars = 255;

	dimension = math::Vec2( 100, 20 );
}

GuiConfig_Numberbox::GuiConfig_Numberbox()
{
	pNumberboxFunction = nullptr;
	charSize = 12;
	charSpaceProportion = 0.08f;
	valueMin = 0.0f;
	valueMax = 1.0f;

	dimension = math::Vec2( 100, 20 );
}

GuiConfig_TextArea::GuiConfig_TextArea()
{
	pTextAreaFunction = nullptr;
	charSize = 20;
	charSpaceProportion = 0.05f;
	linesPerArea = 20;
	charsAllowed = GUI_TEXT_ALLOW_ALL;
	flag = 0;
}

GuiConfig_Checkbox::GuiConfig_Checkbox()
{
	backgroundColor3 = math::saturateRgb( math::Vec3(0.70f, 0.80f, 0.95f) * 1.0f, 0.95f ).Xyz(); 
	selectedColor3 = math::clampUnit( backgroundColor3 * 0.6f );
	pCheckboxFunction = nullptr;
	dimension = 50.0f;
	circleScale = 1.0f;
}
