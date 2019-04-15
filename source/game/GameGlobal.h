#pragma once

#include "../engine/CoreInterface.h"
#include "GameWorld.h"
#include "GameCommand.h"
#include "../engine/String.h"

#define g_pCommand g_pCore->GetWorld<GameWorld>()->GetCommand()

namespace gameGlobal
{
	namespace ai
	{
		namespace monster
		{
			extern const HashString SLASH_RANGE;
			extern const HashString SLASH_DURATION;
			extern const HashString FORCE_MAX;
			extern const HashString CHASE_TIME_MIN;
			extern const HashString CHASE_TIME_MAX;
			extern const HashString CHASE_TIME_TURN_MIN;
			extern const HashString CHASE_TIME_TURN_MAX;
			
			namespace damage
			{
				extern const HashString SLASH;
				extern const HashString PROJECTILE;
				extern const HashString PROJECTILE_MINI;
			};
		};
	};

	namespace action
	{
		namespace context
		{
			extern const HashString GAME_CONTEXT;
		};

		extern const HashString QUICKLOAD;
		extern const HashString PLAYER_FORWARD;
		extern const HashString PLAYER_BACKWARD;
		extern const HashString PLAYER_TURN_LEFT;
		extern const HashString PLAYER_TURN_RIGHT;
	};
};
