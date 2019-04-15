#pragma once

enum eToolActionAllowed
{
	TOOL_ALLOW_NONE = 0,
	TOOL_ALLOW_TRANSLATE = 1 << 1,
	TOOL_ALLOW_RESIZE = 1 << 2,
	TOOL_ALLOW_ROTATE = 1 << 3,
	TOOL_ALLOW_PICK_AXIS = 1 << 4,
};

enum class eEditorEntityType
{
	Unknown = 0,
	None,
	Block,
	PlayerSpawn,
	Camera,
	CameraTrack,
	CameraTrackPoint,
	PointLight,
	SpotLight,
	AmbientLight,
	TriggerCamera,
	TriggerCommand,
	Door,
	Model,
	Talk,
	Monster,
	Partition,
	Refraction,

	Count // MUST ALWAYS BE LAST! Used for indexing
};
