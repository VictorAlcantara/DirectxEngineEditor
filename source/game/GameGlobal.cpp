#include "GameGlobal.h"

namespace gameGlobal
{
	namespace ai
	{
		namespace monster
		{
			const HashString SLASH_RANGE = "AiMonsterSlashRange";
			const HashString SLASH_DURATION = "AiMonsterSlashDuration";
			const HashString FORCE_MAX = "AiMonsterForceMax";
			const HashString CHASE_TIME_MIN = "AiMonsterChaseTimeMin";
			const HashString CHASE_TIME_MAX = "AiMonsterChaseTimeMax";
			const HashString CHASE_TIME_TURN_MIN = "AiMonsterChaseTimeTurnMin";
			const HashString CHASE_TIME_TURN_MAX = "AiMonsterChaseTimeTurnMax";

			namespace damage
			{
				const HashString SLASH = "AiMonsterDamageSlash";
				const HashString PROJECTILE = "AiMonsterDamageProjectile";
				const HashString PROJECTILE_MINI = "AiMonsterDamageProjectileMini";
			};
		};
	};

	namespace action
	{
		namespace context
		{
			const HashString GAME_CONTEXT = "ContextActionGame";
		};

		const HashString QUICKLOAD = "ActionQuickLoad";
		const HashString PLAYER_FORWARD = "ActionPlayerForward";
		const HashString PLAYER_BACKWARD = "ActionPlayerBackward";
		const HashString PLAYER_TURN_LEFT = "ActionPlayerTurnLeft";
		const HashString PLAYER_TURN_RIGHT = "ActionPlayerTurnRight";
	};
};
