#include "EditorGlobal.h"

namespace editorGlobal
{
	namespace grid
	{
		const math::Vec4 COLOR_MAIN(0.4f, 0.4f, 0.4f, 1.0f);
		const math::Vec4 COLOR_GUIDE(0.4f, 0.4f, 0.3f, 1.0f);
		const uint LEVEL_SIZE_ONE = 5;
		const uint UNIT_SIZE_ONE = (uint)math::pow(2, LEVEL_SIZE_ONE);
		const float SIZE_UNIT_ONE = 1.0f / UNIT_SIZE_ONE;
		const uint LEVEL_MIN = 1;
		const uint LEVEL_MAX = LEVEL_SIZE_ONE * 2;
		const uint LINE_COUNT = 100;
		const float RENDER_DEPTH_BIAS = 0.00001f;
	}

	namespace entity
	{
		const float SCALE_MIN = grid::SIZE_UNIT_ONE;
		const math::Vec4 HIGHLIGHT_COLOR(1.0f, 1.0f, 0.0f, 1.0f);
		const float HIGHLIGHT_DEPTH_BIAS = grid::RENDER_DEPTH_BIAS;
		const int GRID_SCALE_MIN = 1;

		const float DIFFUSE_INTENSITY = 0.2f;
	}

	namespace tool
	{
		const float AXIS_MOVEMENT_AMOUNT_THRESHOLD = 1.5f;
		const int GRID_TRANSLATE_MIN = 2;
		const float AXIS_RADIUS = 0.1f;
		const float CAMERA_SCALE = tool::AXIS_RADIUS * 100.0f;
		const float AXIS_SCALE_OUTWARD = 0.5f;
		const float DIFFUSE_INTENSITY = 0.7f;
		const math::Vec4 UNSELECTED_NORMAL_AMBIENT[3] = {
			(math::normalizeRgb(  26, 132, 229 ).Xyz() * (1.0f - DIFFUSE_INTENSITY)).Xyzw(1.0f),
			(math::normalizeRgb( 232,  30,  30 ).Xyz() * (1.0f - DIFFUSE_INTENSITY)).Xyzw(1.0f),
			(math::normalizeRgb(   0, 177,  37 ).Xyz() * (1.0f - DIFFUSE_INTENSITY)).Xyzw(1.0f)
		};
		const math::Vec4 UNSELECTED_NORMAL_DIFFUSE[3] = {
			(math::normalizeRgb(  26, 132, 229 ).Xyz() * DIFFUSE_INTENSITY).Xyzw(1.0f),
			(math::normalizeRgb( 232,  30,  30 ).Xyz() * DIFFUSE_INTENSITY).Xyzw(1.0f),
			(math::normalizeRgb(   0, 177,  37 ).Xyz() * DIFFUSE_INTENSITY).Xyzw(1.0f)
		};
		const math::Vec4 SELECTED_NORMAL_AMBIENT = (math::normalizeRgb(255, 255, 0).Xyz() * (1.0f - DIFFUSE_INTENSITY)).Xyzw(1.0f);
		const math::Vec4 SELECTED_NORMAL_DIFFUSE = (math::normalizeRgb(255, 255, 0).Xyz() * DIFFUSE_INTENSITY).Xyzw(1.0f);
		const math::Vec4 SELECTED_OCCLUDED_COLOR = math::saturate( SELECTED_NORMAL_DIFFUSE.Xyz(), 0.5f ).Xyzw(1.0f);
		const math::Vec4 UNSELECTED_OCCLUDED_COLOR = math::Vec3(0.65f).Xyzw(1.0f);

		const math::Vec3 LIGHT0_DIR = math::normalize( math::Vec3(
			-1.0f * math::random(-0.5f, 0.5f),
			-1.0f * math::random(-0.5f, 0.5f),
			 1.0f * math::random(-0.5f, 0.5f)
		) );
		const math::Vec3 LIGHT1_DIR = math::normalize( math::Vec3(
			 1.0f * math::random(-0.5f, 0.5f),
			 1.0f * math::random(-0.5f, 0.5f),
			-1.0f * math::random(-0.5f, 0.5f)
		) );
		const math::Vec4 LIGHT0_COLOR = math::Vec4(1.0f);
		const math::Vec4 LIGHT1_COLOR = math::Vec4(math::Vec3(0.3f), 1.0f);
	}

	namespace gui
	{
		namespace flag
		{
			const HashString FLAG_ENTITY_TALK_COMMAND = "FlagEntityTalkCommand";
			const HashString FLAG_ENTITY_TALK_TEXT = "FlagEntityTalkText";
		}
	}

	namespace guiPage
	{
		const float TRANSITION_ANIMATION_DURATION = 0.3f;
		const HashString PAGE_MAIN_MENU = HashString( "mainMenu" );
		const HashString PAGE_PROPERTY_BLOCK = HashString( "propertyBlock" );
		const HashString PAGE_PROPERTY_DOOR = HashString( "propertyDoor" );
		const HashString PAGE_PROPERTY_CAMERA_TRACK = HashString( "propertyCameraTrack" );
		const HashString PAGE_PROPERTY_PLAYER_SPAWN = HashString( "propertyPlayerSpawn" );
		const HashString PAGE_PROPERTY_AMBIENT_LIGHT = HashString( "propertyAmbientLight" );
		const HashString PAGE_PROPERTY_POINT_LIGHT = HashString( "propertyPointLight" );
		const HashString PAGE_PROPERTY_SPOT_LIGHT = HashString( "propertySpotLight" );
		const HashString PAGE_PROPERTY_TRIGGER_CAMERA = HashString( "propertyTriggerCamera" );
		const HashString PAGE_PROPERTY_TRIGGER_COMMAND = HashString( "propertyTriggerCommand" );
		const HashString PAGE_PROPERTY_TALK = HashString( "propertyTalk" );
		const HashString PAGE_PROPERTY_PARTITION = HashString( "propertyPartition" );
		const HashString PAGE_PROPERTY_MODEL = HashString( "propertyModel" );
		const HashString PAGE_PROPERTY_REFRACTION = HashString( "propertyRefraction" );
		const HashString PAGE_PROPERTY_MONSTER = HashString( "propertyMonster" );
		const HashString PAGE_TEXTURE_SELECT = HashString( "textureSelect" );
		const HashString PAGE_MODEL_SELECT = HashString( "modelSelect" );
		const HashString PAGE_ENTITY_SELECT = HashString( "entitySelect" );
		const HashString PAGE_TRIGGER_SELECT = HashString( "triggerSelect" );
		const HashString PAGE_TEXT_AREA = HashString( "textArea" );
	}

	namespace action
	{
		namespace context
		{
			const HashString EDITOR_DEFAULT = "ActionContextEditorDefault";
			const HashString GUI_COMPONENT_TEXTBOX = "ActionContextGuiComponentTextbox";
			const HashString GUI_COMPONENT_NUMBERBOX = "ActionContextGuiComponentNumberbox";
			const HashString GUI_COMPONENT_TEXTAREA = "ActionContextGuiComponentTextArea";
		}

		const HashString LEFT_CLICK = "ActionLeftClick";
		const HashString ESCAPE = "ActionEscape";
		const HashString CONFIRM = "ActionConfirm";
		const HashString TEXT_BACKSPACE = "ActionTextBackspace";
		const HashString TEXT_DELETE = "ActionTextDelete";
		const HashString TEXT_HOME = "ActionTextHome";
		const HashString TEXT_END = "ActionTextEnd";
		const HashString TEXT_UP = "ActionTextUp";
		const HashString TEXT_DOWN = "ActionTextDown";
		const HashString ENTITY_SELECT = "ActionEntitySelect";
		const HashString ENTITY_DELETE = "ActionEntityDelete";
		const HashString ENTITY_CLONE = "ActionEntityClone";
		const HashString ENTITY_RESET = "EntityReset";
		const HashString TEST_CREATE_BLOCK = "ActionTestCreateBlock";
		const HashString CAMERA_FORWARD = "ActionCameraForward";
		const HashString CAMERA_BACKWARD = "ActionCameraBackward";
		const HashString CAMERA_STRAFE_RIGHT = "ActionCameraStrafeRight";
		const HashString CAMERA_STRAFE_LEFT = "ActionCameraStrafeLeft";
		const HashString CAMERA_UP = "ActionCameraUp";
		const HashString CAMERA_DOWN = "ActionCameraDown";
		const HashString CAMERA_LOOK_RIGHT = "ActionCameraLookRight";
		const HashString CAMERA_LOOK_LEFT = "ActionCameraLookLeft";
		const HashString CAMERA_LOOK_UP = "ActionCameraLookUp";
		const HashString CAMERA_LOOK_DOWN = "ActionCameraLookDown";
		const HashString CAMERA_ACCELERATE = "ActionCameraAccelerate";
		const HashString GRID_INCREASE = "ActionGridIncrease";
		const HashString GRID_DECREASE = "ActionGridDecrease";
		const HashString GRID_TOGGLE = "ActionGridToggle";
		const HashString PARTITION_TOGGLE = "ActionPartitionToggle";
		const HashString MODE_CHANGE = "ActionModeChange";
		const HashString QUICKSAVE = "ActionQuickSave";
		const HashString QUICKLOAD = "ActionQuickLoad";
	}
}
