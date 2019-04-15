#pragma once

#include "GuiUtil.h"
#include "../engine/Pointer.h"
#include "../engine/Math.h"
#include "../engine/String.h"
#include "../engine/Handle.h"

class GuiAnimation;
class ITexture;
class Model;

/****************************************************************************************
	GuiConfig_Dimension
*****************************************************************************************/

struct GuiConfig_Dimension
{
	math::Vec2 dimension;
	bool allowTransparency;

	GuiConfig_Dimension() : dimension(10.0f), allowTransparency(true) {}
};

/****************************************************************************************
	GuiConfig_Base
*****************************************************************************************/

struct GuiConfig_Base
{
	GuiAnchor anchor;
	uint layer;
	String command;

	GuiConfig_Base() : layer(5) {}
};

/****************************************************************************************
	GuiConfig_Panel
*****************************************************************************************/

struct GuiConfig_Panel : public GuiConfig_Base, public GuiConfig_Dimension
{
	SmartPtr<GuiAnimation> pAnimation;
	math::Vec4 color4;

	GuiConfig_Panel() : color4(1.0f) 
	{
	}
};

/****************************************************************************************
	GuiConfig_Button
*****************************************************************************************/

struct GuiConfig_Button : public GuiConfig_Base, public GuiConfig_Dimension
{
	SmartPtr<GuiAnimation> pAnimation;
	buttonFunction pClickFunction;
	Handle<ITexture> hTexture;
	Handle<Model> hModel;
	math::Vec4 idleColor4;
	math::Vec4 mouseOverColor4;
	math::Euler modelAngularVelocity;
	uint flag;

	GuiConfig_Button();
};

/****************************************************************************************
	GuiConfig_Text
*****************************************************************************************/

struct GuiConfig_Text : public GuiConfig_Base
{
	String text;
	math::Vec4 color4;
	String fontName;
	uint charSize;
	float charSpaceProportion;

	GuiConfig_Text() : color4(1.0f), charSize(20), charSpaceProportion(0.05f), fontName("consolas") {}
};

/****************************************************************************************
	GuiConfig_Slider
*****************************************************************************************/

struct GuiConfig_Slider : public GuiConfig_Base, public GuiConfig_Dimension
{
	math::Vec3 sliderColor3;
	math::Vec3 circleColor3;
	float circleScale;
	struct
	{
		bool use = false;
		float value = 0.0f;
	} defaultValue;
	float minValue;
	float maxValue;
	sliderFunction pSliderFunction;

	GuiConfig_Slider();
};

/****************************************************************************************
	GuiConfig_Textbox
*****************************************************************************************/

struct GuiConfig_Textbox : public GuiConfig_Base, public GuiConfig_Dimension
{
	textboxFunction pTextboxFunction;
	uint charSize;
	float charSpaceProportion;
	uint charsAllowed;
	uint maxChars;

	GuiConfig_Textbox();
};

/****************************************************************************************
	GuiConfig_Numberbox
*****************************************************************************************/

struct GuiConfig_Numberbox : public GuiConfig_Base, public GuiConfig_Dimension
{
	numberboxFunction pNumberboxFunction;
	uint charSize;
	float charSpaceProportion;
	float valueMin;
	float valueMax;

	GuiConfig_Numberbox();
};


/****************************************************************************************
	GuiConfig_TextArea
*****************************************************************************************/

struct GuiConfig_TextArea : public GuiConfig_Base
{
	textAreaFunction pTextAreaFunction;
	uint charSize;
	float charSpaceProportion;
	uint linesPerArea;
	uint charsAllowed;
	uint flag;

	GuiConfig_TextArea();
};

/****************************************************************************************
	GuiConfig_Checkbox
*****************************************************************************************/

struct GuiConfig_Checkbox : public GuiConfig_Base
{
	math::Vec3 backgroundColor3;
	math::Vec3 selectedColor3;
	float circleScale;
	checkboxFunction pCheckboxFunction;
	float dimension;

	GuiConfig_Checkbox();
};
