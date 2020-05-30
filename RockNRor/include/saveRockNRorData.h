
#pragma once

#include <string>
#include <gameVersion.h>
#include "traceMessage.h"


namespace ROCKNROR {
;
namespace SYSTEM {

	// RockNRor savegame file extension, including the dot.
	const std::string CONST_RNR_SAVEGAME_EXTENSION = ".rnr";
	

	// Serialize all RockNRor data for current game
	// rnrDataFilename is relative path with extension
	bool serializeGameRockNRorData(const char *rnrDataFilename);

	// Deserialize all RockNRor data for a game
	// rnrDataFilename is relative path with extension
	bool deserializeGameRockNRorData(const char *rnrDataFilename, bool writeTrace = false);

	// Save RockNRor-specific data in a dedicated file, related to a "saved game" file.
	// saveGameFileName is filename withOUT path, with extension.
	bool saveRockNRorGameData(const char *saveGameFileName);

	// Load RockNRor-specific data from dedicated file, related to a "saved game" file.
	// saveGameFileName is the (standard) savegame filename withOUT path, with extension.
	bool loadRockNRorGameData(const char *saveGameFileName);

	// Load RockNRor-specific data from dedicated file, related to a "saved game" file.
	// saveGameFileName is the (standard) savegame filename withOUT path, withOUT extension.
	bool loadRockNRorGameDataNoExt(const char *saveGameFileNameNoExt);

}
}
