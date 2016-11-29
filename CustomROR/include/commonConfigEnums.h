
#pragma once

namespace CUSTOMROR {

	// Internal representation of game types (*NOT* AOE/ROR values)
	enum ConfigGameType {
		CFG_GAME_UNKNOWN = 0,
		CFG_GAME_RANDOM_GAME,
		CFG_GAME_DEATHMATCH,
		CFG_GAME_SCENARIO,
		CFG_GAME_TYPES_COUNT
	};

	static const char *ConfigGameTypeNames[] { "?", "RM", "DM", "SCN" };

}
