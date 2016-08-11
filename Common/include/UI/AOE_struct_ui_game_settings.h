
#pragma once

#include <UI\AOE_struct_ui_screen_base.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size = 0x560. 10 78 54 00. Parent=9C 57 54 00 then BC 4B 54 00.
	// The screen before starting game to choose map type, etc ("advanced" options like map size, etc).
	// Constructor:0x49BF40
	class STRUCT_UI_GAME_SETTINGS : public STRUCT_UI_SCREEN_BASE {
	public:
		STRUCT_ANY_UI *lblScenarioSettings; // +478
		unsigned long int unknown_47C;
		// 0x480
		STRUCT_ANY_UI *lblMapSize;
		STRUCT_ANY_UI *edtMapSize; // drop down
		STRUCT_ANY_UI *lblMapType;
		STRUCT_ANY_UI *edtMapType;
		// 0x490
		STRUCT_ANY_UI *lblVictoryCondition;
		STRUCT_ANY_UI *edtVictoryCondition;
		STRUCT_ANY_UI *lblScContainsIndividualVictoryConditions;
		unsigned long int unknown_49C;
		// 0x4A0
		STRUCT_ANY_UI *lblScore;
		STRUCT_ANY_UI *edtScore;
		STRUCT_ANY_UI *lblTimeLimit;
		STRUCT_ANY_UI *edtTimeLimit;
		// 0x4B0
		STRUCT_ANY_UI *lblDifficultyLevel;
		STRUCT_ANY_UI *edtDifficultyLevel;
		STRUCT_ANY_UI *lblStartingAge;
		STRUCT_ANY_UI *edtStartingAge;
		// 0x4C0
		STRUCT_ANY_UI *lblResources;
		STRUCT_ANY_UI *edtResources;
		STRUCT_ANY_UI *lblPathFinding;
		STRUCT_ANY_UI *edtPathFinding; // checkbox
		// 0x4D0
		STRUCT_ANY_UI *lblPopLimit;
		STRUCT_ANY_UI *edtPopLimit;
		STRUCT_ANY_UI *lblUnknown_4D8;
		STRUCT_ANY_UI *edtUnknown_4DC;
		// 0x4E0
		long int scenarioListPosX;
		long int scenarioListPosY;
		long int scenarioListSizeX;
		long int scenarioListSizeY;
		// 0x4F0
		STRUCT_ANY_UI *edtScenarioList;
		unsigned long int unknown_4F4;
		unsigned long int unknown_4F8;
		unsigned long int unknown_4FC;
		// 0x500
		unsigned long int unknown_500;
		unsigned long int unknown_504;
		STRUCT_ANY_UI *lblScenarioInstructions;
		unsigned long int unknown_50C;
		// 0x510
		char unknown_510[0x524 - 0x510];
		STRUCT_ANY_UI *edtFixedPositions; // +524
		STRUCT_ANY_UI *edtRevealMap;
		STRUCT_ANY_UI *edtFullTechTree;
		// 0x530
		STRUCT_ANY_UI *edtEnableCheatMode;
		STRUCT_ANY_UI *lblFixedPositions;
		STRUCT_ANY_UI *lblRevealMap;
		STRUCT_ANY_UI *lblFullTechTree;
		// 0x540
		STRUCT_ANY_UI *lblEnableCheatMode;
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnHelp; // "?" button
		// 0x550
		unsigned long int unknown_550;
		unsigned long int unknown_554;
		unsigned long int unknown_558;
		unsigned long int unknown_55C;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_GAME_SETTINGS; }
	};
	static_assert(sizeof(STRUCT_UI_GAME_SETTINGS) == 0x560, "STRUCT_UI_GAME_SETTINGS size");

}

