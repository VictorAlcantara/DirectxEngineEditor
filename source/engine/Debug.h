#pragma once

#include "Platform.h"

void debugToOutput(const char*);
void debugToOutput(bool, const char* msg = nullptr);
void debugToOutput(float, const char* msg = nullptr);
void debugToOutput(int, const char* msg = nullptr);

#ifdef DEBUG_MODE
	#define BREAKPOINT DebugBreak();
	#define BREAKPOINT_IF(cond) { if ( (cond) ) BREAKPOINT; }
	#define DEBUG_MATRIX_INDEX_CHECK
	#define DEBUG_SIMPLEPTR_NULL_DEFERENCE
	#define DEBUG_MATH_CHECK_RGB_NORMALIZATION
	#define DEBUG_MATH_CHECK_MATRIX_ORTHOGONAL
	#define DEBUG_STRING_INDEX_CHECK
	#define DEBUG_EVENT_DISPATCHER_REDUNDANCY_CHECK
	#define DEBUG_SMART_POINTER_COUNTER
	#define DEBUG_GRID_SCALE
	#define DEBUG_GUI_CHECK_CONFIG_CONSISTENCY
	#define DEBUG_GUI_CHECK_PARENT_EXISTS
	#define DEBUG_FILE_CHECK_OPEN
	#define DEBUG_TABLE_SLOT_DEPTH
	#define DEBUG_GUI_PANEL_STATE
	#define DEBUG_COLLISION_MANAGER_COLLIDER_CONSISTENCY_CHECK
	#define DEBUG_SHADER
	//#define DEBUG_SHOW_CONTACT_NORMALS
#endif