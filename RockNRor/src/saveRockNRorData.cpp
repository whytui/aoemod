#include "../include/saveRockNRorData.h"
#include "../include/AOEPrimitives_global.h"
#include "../include/mainCustomAI.h"


using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;
namespace SYSTEM {

	// Serialize all RockNRor data for current game
	// rnrDataFilename is relative path with extension
	bool serializeGameRockNRorData(const char *rnrDataFilename) {
		// For now we only save customAI stuff
		return CUSTOM_AI::customAIHandler.SerializeToFile(rnrDataFilename) > 0;
	}


	// Deserialize all RockNRor data for a game
	// rnrDataFilename is relative path with extension
	bool deserializeGameRockNRorData(const char *rnrDataFilename, bool writeTrace) {
		if (!CheckFileExistence(rnrDataFilename)) {
			if (writeTrace) {
				traceMessageHandler.WriteMessageNoNotification("There is no RockNRor savegame information to load");
			}
			return false;
		}
		// For now we only save customAI stuff
		bool res = CUSTOM_AI::customAIHandler.DeserializeFromFile(rnrDataFilename);
		if (writeTrace) {
			if (res) {
				traceMessageHandler.WriteMessageNoNotification("Successfully loaded RockNRor savegame information");
			}
			else {
				// The RNR savegame file exists, but deserializing failed, this is a real issue.
				traceMessageHandler.WriteMessage("ERROR : failed to deserialize RockNRor savegame information");
			}
		}
		return res;
	}


	// Save RockNRor-specific data in a dedicated file, related to a "saved game" file.
	// saveGameFileName is (standard) savegame filename withOUT path, with extension.
	bool saveRockNRorGameData(const char *saveGameFileName) {
		STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		assert(settings && settings->IsCheckSumValid());
		if (!settings || !settings->IsCheckSumValid()) {
			return false;
		}
		if (!saveGameFileName || (saveGameFileName[0] == '\0')) {
			return false;
		}

		std::string stringSaveFilenameRnr = std::string(settings->commandLineInfo->savegameFolder) + 
			saveGameFileName + CONST_RNR_SAVEGAME_EXTENSION;
		const char *saveFilenameRnr = stringSaveFilenameRnr.c_str();

		return serializeGameRockNRorData(saveFilenameRnr);
	}


	// Load RockNRor-specific data from dedicated file, related to a "saved game" file.
	// saveGameFileName is the (standard) savegame filename withOUT path, with extension.
	bool loadRockNRorGameData(const char *saveGameFileName) {
		STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		assert(settings && settings->IsCheckSumValid());
		if (!settings || !settings->IsCheckSumValid()) {
			return false;
		}
		if (!saveGameFileName || (saveGameFileName[0] == '\0')) {
			return false;
		}

		std::string stringSaveFilenameRnr = std::string(settings->commandLineInfo->savegameFolder) +
			saveGameFileName + CONST_RNR_SAVEGAME_EXTENSION;
		const char *saveFilenameRnr = stringSaveFilenameRnr.c_str();

		return deserializeGameRockNRorData(saveFilenameRnr, true);
	}


	// Load RockNRor-specific data from dedicated file, related to a "saved game" file.
	// saveGameFileName is the (standard) savegame filename withOUT path, withOUT extension.
	bool loadRockNRorGameDataNoExt(const char *saveGameFileNameNoExt) {
		if (!saveGameFileNameNoExt) {
			return false;
		}
		std::string tmp = saveGameFileNameNoExt;
#ifdef GAME_IS_ROR
		tmp += ".gmx";
#else
		tmp += ".gam";
#endif
		return loadRockNRorGameData(tmp.c_str());
	}


}
}
