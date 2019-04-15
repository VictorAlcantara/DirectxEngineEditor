#pragma once

#include "../engine/CoreInterface.h"
#include "../editor/EditorWorld.h"
#include "../editor/Grid.h"
#include "../engine/Math.h"
#include "EditorGui.h"
#include "EditorTool.h"
#include "EditorEntity_Block.h"
#include "EditorEntity_PlayerSpawn.h"

#define g_pEditor g_pCore->GetWorld<EditorWorld>()

namespace editorGlobal
{
	namespace grid
	{
		extern const math::Vec4 COLOR_MAIN;
		extern const math::Vec4 COLOR_GUIDE;
		extern const uint LEVEL_SIZE_ONE; // The level that the grid reaches one in world space
		extern const uint UNIT_SIZE_ONE; // Units in grid space that equates to one in world space
		extern const float SIZE_UNIT_ONE; // The size in world space of one unit in grid space
		extern const uint LEVEL_MIN;
		extern const uint LEVEL_MAX;
		extern const uint LINE_COUNT;
		extern const float RENDER_DEPTH_BIAS;
	}

	namespace entity
	{
		extern const float SCALE_MIN;
		extern const math::Vec4 HIGHLIGHT_COLOR;
		extern const float HIGHLIGHT_DEPTH_BIAS;
		extern const int GRID_SCALE_MIN;

		extern const float DIFFUSE_INTENSITY;
	}

	namespace tool
	{
		extern const float AXIS_MOVEMENT_AMOUNT_THRESHOLD;
		extern const int GRID_TRANSLATE_MIN;
		extern const float AXIS_RADIUS;
		extern const float CAMERA_SCALE;
		extern const float AXIS_SCALE_OUTWARD;
		extern const float DIFFUSE_INTENSITY;
		extern const math::Vec4 UNSELECTED_NORMAL_AMBIENT[3];
		extern const math::Vec4 UNSELECTED_NORMAL_DIFFUSE[3];
		extern const math::Vec4 SELECTED_NORMAL_AMBIENT;
		extern const math::Vec4 SELECTED_NORMAL_DIFFUSE;
		extern const math::Vec4 SELECTED_OCCLUDED_COLOR;
		extern const math::Vec4 UNSELECTED_OCCLUDED_COLOR;
		extern const math::Vec3 LIGHT0_DIR;
		extern const math::Vec3 LIGHT1_DIR;
		extern const math::Vec4 LIGHT0_COLOR;
		extern const math::Vec4 LIGHT1_COLOR;
	}

	namespace gui
	{
		namespace flag
		{
			extern const HashString FLAG_ENTITY_TALK_COMMAND;
			extern const HashString FLAG_ENTITY_TALK_TEXT;
		}
	}

	namespace guiPage
	{
		extern const float TRANSITION_ANIMATION_DURATION;
		extern const HashString PAGE_MAIN_MENU;
		extern const HashString PAGE_PROPERTY_BLOCK;
		extern const HashString PAGE_PROPERTY_DOOR;
		extern const HashString PAGE_PROPERTY_CAMERA_TRACK;
		extern const HashString PAGE_PROPERTY_PLAYER_SPAWN;
		extern const HashString PAGE_PROPERTY_AMBIENT_LIGHT;
		extern const HashString PAGE_PROPERTY_POINT_LIGHT;
		extern const HashString PAGE_PROPERTY_SPOT_LIGHT;
		extern const HashString PAGE_PROPERTY_TRIGGER_CAMERA;
		extern const HashString PAGE_PROPERTY_TRIGGER_COMMAND;
		extern const HashString PAGE_PROPERTY_TALK;
		extern const HashString PAGE_PROPERTY_PARTITION;
		extern const HashString PAGE_PROPERTY_MODEL;
		extern const HashString PAGE_PROPERTY_REFRACTION;
		extern const HashString PAGE_PROPERTY_MONSTER;
		extern const HashString PAGE_TEXTURE_SELECT;
		extern const HashString PAGE_MODEL_SELECT;
		extern const HashString PAGE_ENTITY_SELECT;
		extern const HashString PAGE_TRIGGER_SELECT;
		extern const HashString PAGE_TEXT_AREA;
	}

	namespace action
	{
		namespace context
		{
			extern const HashString EDITOR_DEFAULT;
			extern const HashString GUI_COMPONENT_TEXTBOX;
			extern const HashString GUI_COMPONENT_NUMBERBOX;
			extern const HashString GUI_COMPONENT_TEXTAREA;
		}

		extern const HashString LEFT_CLICK;
		extern const HashString ESCAPE;
		extern const HashString CONFIRM;
		extern const HashString TEXT_BACKSPACE;
		extern const HashString TEXT_DELETE;
		extern const HashString TEXT_HOME;
		extern const HashString TEXT_END;
		extern const HashString TEXT_UP;
		extern const HashString TEXT_DOWN;
		extern const HashString ENTITY_SELECT;
		extern const HashString ENTITY_DELETE;
		extern const HashString ENTITY_CLONE;
		extern const HashString ENTITY_RESET;
		extern const HashString TEST_CREATE_BLOCK;
		extern const HashString CAMERA_FORWARD;
		extern const HashString CAMERA_BACKWARD;
		extern const HashString CAMERA_STRAFE_RIGHT;
		extern const HashString CAMERA_STRAFE_LEFT;
		extern const HashString CAMERA_UP;
		extern const HashString CAMERA_DOWN;
		extern const HashString CAMERA_LOOK_RIGHT;
		extern const HashString CAMERA_LOOK_LEFT;
		extern const HashString CAMERA_LOOK_UP;
		extern const HashString CAMERA_LOOK_DOWN;
		extern const HashString CAMERA_ACCELERATE;
		extern const HashString GRID_INCREASE;
		extern const HashString GRID_DECREASE;
		extern const HashString GRID_TOGGLE;
		extern const HashString PARTITION_TOGGLE;
		extern const HashString MODE_CHANGE;
		extern const HashString QUICKSAVE;
		extern const HashString QUICKLOAD;
	}
}
