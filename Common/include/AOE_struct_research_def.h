
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// size = 0x3C. "Player tech" ?
	// Remark: a research is automatically triggered (developed)
	// when (number of "done" requiredResearchId >= minRequiredResearchesCount) AND researchTime==0 (trick in 0x4EB4C0: non-BEING_RESEARCHED researches have "progress time=0)
	// when a building is constructed for the first time (if building.unitDef.initiateResearch is set)
	// Otherwise, research status must be CST_RESEARCH_STATUS_AVAILABLE (1), and player has to run development in appropriate building.
	// See 0x4EB38D = playerResearchInfo.updateResearchStatusesWithRequirements
	class STRUCT_RESEARCH_DEF {
	public:
		char *researchName;
		short int requiredResearchId[4]; // +04. 4 possible research IDs (required researches)
		short int minRequiredResearchesCount; // +0C. Min number of "requiredResearchId" that need to be satisfied to enable this research. NOT number of used elements in array !
		short int costType1;
		// 0x10
		short int costType2;
		short int costType3;
		short int costAmount1; // +14
		short int costAmount2;
		short int costAmount3; // +18
		char costUsed1; // +1A
		char costUsed2;
		char costUsed3; // +1C
		char unused_1D0;
		short int researchTime; // 1E
		// 0x20
		short int technologyId;
		short int researchType; // 1=dock, 2=granary,etc (see AGE3 tooltip)
		short int iconId; // 24
		char buttonId; // 26. 1-5 for first row, 6-10 for second row, 11-15 and 16-20 for 2nd page (2 rows)
		char unknown_27;
		short int researchLocation; // 28
		short int languageDLLName; // 2A.
		short int languageDLLDescription; // 2C. Description of research (with effect details like "+1 for xxx", etc)
		unsigned short int unknown_2E; // unused ?
		// 0x30
		long int languageDLLCreation;
		long int languageDLLHelp; // +34
		unsigned long int unknown_38;
	};
	static_assert(sizeof(STRUCT_RESEARCH_DEF) == 0x3C, "STRUCT_RESEARCH_DEF size");

}
