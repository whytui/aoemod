#include "../include/interface.h"
#include <string>

namespace ROCKNROR {
namespace PATCHER {
;

// Retrieve the highest resolution sizes (x/y)
// Returns true if the variables were successfully updated
bool ROCKNROR::PATCHER::GetHighestResolutionValues(long int &x, long int &y) {
	x = ROCKNROR::PATCHER::GetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HSize1", 0);
	y = ROCKNROR::PATCHER::GetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VSize1", 0);
	if ((x == 0) || (y == 0)) { return false; }
	return true;
}


// Returns true if game has been patched to use custom interfac.drs files (see CST_INTERFAC_DRS_CUSTOM_FILE_NAME)
// This method makes necessary checks to know if it is relevant to modify interfac.drs filename to use our custom one
// If the game uses a custom resolution and if they are available, makes so ROR will use custom interfac.drs files (see CST_INTERFAC_DRS_CUSTOM_FILE_NAME)
bool ROCKNROR::PATCHER::ChangeItfDRS_file() {
	char *interfac_drs_name = (char *)AOE_OFFSETS::ADDR_VAR_INTERFAC_DRS_NAME;
	long int interfac_drs_name_size = 0x0F;
	bool go_on = true;
	FILE *fileTest = NULL;
	FILE *fileLog = NULL;
	int logFileRes;
	logFileRes = fopen_s(&fileLog, MOD_NAME "\\" MOD_NAME ".log", "a+"); // appends (do not overwrite)
	bool hasLog = (logFileRes == 0);

	// Check if custom DRS files are present
	if (go_on) {
		int res = fopen_s(&fileTest, "data\\" CST_INTERFAC_DRS_CUSTOM_FILE_NAME, "r");
		if (res) {
			// File does not exist (or is not readable by ROR process ?)
			go_on = false;
			if (hasLog) {
				fprintf_s(fileLog, "Could not find data\\" CST_INTERFAC_DRS_CUSTOM_FILE_NAME ". Game will use interfac.drs file.\n");
			}
		} else {
			fclose(fileTest);
		}
	}
	bool requiresData2 = false; // true if we want data2/itf_custom.drs to be necessary
	if (go_on && requiresData2) {
		int res = fopen_s(&fileTest, "data2\\" CST_INTERFAC_DRS_CUSTOM_FILE_NAME, "r");
		if (res) {
			// File does not exist (or is not readable by ROR process ?)
			go_on = false;
			if (hasLog) {
				fprintf_s(fileLog, "Could not find data2\\" CST_INTERFAC_DRS_CUSTOM_FILE_NAME ". Game will use interfac.drs file.\n");
			}
		} else {
			fclose(fileTest);
		}
	}
	if (go_on) {
		// All checks are OK, modify filename in ROR variables
		strcpy_s(interfac_drs_name, interfac_drs_name_size, CST_INTERFAC_DRS_CUSTOM_FILE_NAME);
		if (hasLog) {
			fprintf_s(fileLog, "The game will use " CST_INTERFAC_DRS_CUSTOM_FILE_NAME " instead of interfac.drs.\n");
		}
	}

	if (!logFileRes) {
		fclose(fileLog);
	}
	return go_on;
}

// Patches ROR executable for a new (max) screen resolution
// Returns true if successful
bool ROCKNROR::PATCHER::ChangeExeResolution(long int sizeX, long int sizeY) {
	bool result = true;
	FILE *fileLog = NULL;
	int logFileRes;
	logFileRes = fopen_s(&fileLog, MOD_NAME "\\" MOD_NAME ".log", "a+"); // appends (do not overwrite)
	bool hasLog = (logFileRes == 0);
	try {
		EnableWritingInRData(true);
		RORProcessEditor pe;
		aoeBinData.SetCurrentVersion(GetBuildVersion());

		// Apply resolution changes with correct parameter values
		// Too bad duplicate code with RockNRorAdmin
		
		for (int i = 1; i <= 21; i++) {
			std::string seqDefName = std::string("HSize") + std::to_string(i);
			ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, seqDefName, 0, sizeX);
		}
		for (int i = 1; i <= 11; i++) {
			std::string seqDefName = std::string("VSize") + std::to_string(i);
			ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, seqDefName, 0, sizeY);
		}

		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_ScoreButton", 0, sizeX - 35);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_HelpButton", 0, sizeX - 35);
		//ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_Map", 0, sizeX - 229); // update map pos
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_Map", 0, 1024 - 229); // preserve map pos
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_MenuButton", 0, sizeX - 72);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_DiplButton", 0, sizeX - 180);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_ButtonsZone", 0, sizeX - 252);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HSize_AgePanel", 0, sizeX - 532);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HPos_PopLimit", 0, sizeX / 2 - 80);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "SI_HPos_TopLabel", 0, sizeX - 6);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "SI_HSize_TextArea", 0, sizeX - 80);

		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_GameZoneBottom", 0, sizeY - 146);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_Map", 0, sizeY - 120);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_UnitInfoZone", 0, sizeY - 119);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_ScoreInformation", 0, sizeY - 118);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_HelpButton", 0, sizeY - 36);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_ScoreButton", 0, sizeY - 118);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_BottomPanel", 0, sizeY - 126);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VPos_PanelImage", 0, sizeY - 126);

		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VSize_GameZoneMinus30", 0, sizeY - 176);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "SI_VPos_HistButton", 0, sizeY - 78);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "SI_VPos_InstrButton", 0, sizeY - 78);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "SI_VPos_HintsButton", 0, sizeY - 78);
		ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "SI_VPos_OkButton", 0, sizeY - 78);

		if (hasLog) {
			std::string msg = "Patching the game to apply custom resolution : ";
			msg.append(std::to_string(sizeX));
			msg.append("*");
			msg.append(std::to_string(sizeY));
			msg.append("\n");
			fprintf_s(fileLog, msg.c_str());
		}
	}
	catch (std::exception e) {
		traceMessageHandler.WriteMessage(e.what());
		EnableWritingInRData(false);
		result = false;
	}
	if (!logFileRes) {
		fclose(fileLog);
	}

	return result;
}

}
}
