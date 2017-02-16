#include "../include/interface.h"


// This method makes necessary checks to know if it is relevant to modify interfac.drs filename to use our custom one
// If the game uses a custom resolution and if they are available, makes so ROR will use custom interfac.drs files (see CST_INTERFAC_DRS_CUSTOM_FILE_NAME)
void AOE_CONST_DRS::ChangeItfDRS_file() {
	char *interfac_drs_name = (char *)AOE_OFFSETS::ADDR_VAR_INTERFAC_DRS_NAME;
	long int interfac_drs_name_size = 0x0F;
	bool go_on = true;
	FILE *fileTest = NULL;
	FILE *fileLog = NULL;
	int logFileRes;
	logFileRes = fopen_s(&fileLog, "RockNRor\\RockNRor.log", "a+"); // appends (do not overwrite)
	bool hasLog = (logFileRes == 0);

	// Check if a custom resolution is enabled in game executable
	DWORD processId = GetCurrentProcessId();
	long int x, y = 0;
	if (processId == 0) {
		go_on = false;
	} else {
		x = GetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "HSize1", 0);
		y = GetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_RESOLUTION, "VSize1", 0);
	}
	if (!go_on && hasLog) {
		fprintf_s(fileLog, "Could not check if game uses a custom resolution. Game will use interfac.drs file.\n");
	}
	if (go_on) {
		go_on = ((x != 0x400) || (y != 0x300)); // 1024*768 is default (max) resolution
		if (!go_on && hasLog) {
			fprintf_s(fileLog, "The game is configured for standard resolution and will use interfac.drs file.\n");
		}
	}

	// Check if custom DRS files are present
	if (go_on) {
		int res = fopen_s(&fileTest, "data\\" CST_INTERFAC_DRS_CUSTOM_FILE_NAME, "r"); // overwrite if already existing
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
	if (go_on) {
		int res = fopen_s(&fileTest, "data2\\" CST_INTERFAC_DRS_CUSTOM_FILE_NAME, "r"); // overwrite if already existing
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
}
