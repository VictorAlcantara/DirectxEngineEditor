#pragma once

#include "../engine/Math.h"
#include "../engine/String.h"
#include "../engine/List.h"

class IGuiComponent;

typedef void (*buttonFunction)(IGuiComponent*, uint flag);
typedef void (*sliderFunction)(IGuiComponent*, float value);
typedef void (*checkboxFunction)(IGuiComponent*, bool value);
typedef void (*textboxFunction)(IGuiComponent*, const String& value);
typedef void (*numberboxFunction)(IGuiComponent*, float value);
typedef void (*textAreaFunction)(IGuiComponent*, const List<String>& cmds, uint flag);

enum eGuiTextAllow
{
	GUI_TEXT_ALLOW_LETTER = 1 << 1,
	GUI_TEXT_ALLOW_NUMBER = 1 << 2,
	GUI_TEXT_ALLOW_UNDERSCORE = 1 << 3,
	GUI_TEXT_ALLOW_QUOTE = 1 << 4,
	GUI_TEXT_ALLOW_COMMA = 1 << 5,
	GUI_TEXT_ALLOW_PERIOD = 1 << 6,
	GUI_TEXT_ALLOW_MARK = 1 << 7,
	GUI_TEXT_ALLOW_BRACKET = 1 << 8,
	GUI_TEXT_ALLOW_SPACE = 1 << 9,
	GUI_TEXT_ALLOW_COLON = 1 << 10,
	GUI_TEXT_ALLOW_NEGATIVE = 1 << 11,
	GUI_TEXT_ALLOW_MATH_SYMBOL = 1 << 12,
	GUI_TEXT_ALLOW_ALL = 
		GUI_TEXT_ALLOW_LETTER | GUI_TEXT_ALLOW_NUMBER | GUI_TEXT_ALLOW_UNDERSCORE |
		GUI_TEXT_ALLOW_QUOTE | GUI_TEXT_ALLOW_COMMA | GUI_TEXT_ALLOW_PERIOD | GUI_TEXT_ALLOW_MARK |
		GUI_TEXT_ALLOW_BRACKET | GUI_TEXT_ALLOW_SPACE | GUI_TEXT_ALLOW_COLON | GUI_TEXT_ALLOW_NEGATIVE |
		GUI_TEXT_ALLOW_MATH_SYMBOL,
};

enum class eGuiPageState
{
	Visible,
	Invisible,
	Showing,
	Hiding,
};

enum class eGuiComponent
{
	Panel,
	Button,
	Text,
	Track,
	Slider,
	Textbox,
	Numberbox,
	TextArea,
	Checkbox,
};

enum class eGuiAction
{
	None,
	CreateEntity,
	PickEntity,
	OpenTextureSelectMenu,
	OpenModelSelectMenu,
	OpenEntitySelectMenu,
	OpenTriggerSelectMenu,
	OpenTextArea,
	SelectTexture,
	EntityProcessCommand,
	EntityProcessCommandChain,
	CameraTrackDisconnect,
	PickAxis,
};

enum class eGuiUnit
{
	Percentage,
};

enum class eGuiTrackAlign
{
	Center,
};

enum class eGuiAnchorHorizontal
{
	Left,
	Center,
	Right,
};

enum class eGuiAnchorVertical
{
	Top,
	Center,
	Bottom,
};

enum class eGuiTexture
{
	DiffuseMap,
	SpecularMap,
	NormalMap,
};

struct GuiAnchor
{
	eGuiAnchorHorizontal horizontal;
	eGuiAnchorVertical vertical;

	int paddingHorizontal;
	int paddingVertical;

	GuiAnchor() : horizontal(eGuiAnchorHorizontal::Center), vertical(eGuiAnchorVertical::Center), paddingHorizontal(0), paddingVertical(0) {}
	GuiAnchor(eGuiAnchorHorizontal horizontal, eGuiAnchorVertical vertical, int paddingHorizontal, int paddingVertical) :
		horizontal(horizontal),
		vertical(vertical),
		paddingHorizontal(paddingHorizontal),
		paddingVertical(paddingVertical) 
	{}
};

struct GuiPageInfo
{
	float transitionProgress;
	eGuiPageState state;

	GuiPageInfo() : transitionProgress(0.0f), state(eGuiPageState::Invisible) {}
};

struct GuiAnimationData
{
	math::Vec2 scale;
	float alpha;
	float saturate;

	GuiAnimationData() : scale(1.0f), alpha(1.0f), saturate(0.0f) {}
};


