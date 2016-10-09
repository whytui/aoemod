
#pragma once

#include <assert.h>
//#include <AOE_const_functional.h>
#include <AOE_struct_research_def.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_score.h>
#include <AOE_struct_map_base_common.h>
#include <AOE_struct_game_map_info.h>
#include <AOE_struct_map_visibility_info.h>

// Note: as we never allocate/create unit structures ourselves, there is no problem going out of bounds as long as ROR EXE allocated enough values !
#define CST_RS_MAX_SUPPORTED_SELECTED_UNITS 100


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// External dependencies
	class STRUCT_GAME_GLOBAL;
	class STRUCT_AI;


	// Size = 0x0C. Constructor=0x4E9C60
	class STRUCT_RESEARCH_DEF_INFO {
	private:
		STRUCT_RESEARCH_DEF *researchDefArray;
	public:
		short int researchCount;
		short int unknown_06; // unused ?
		STRUCT_GAME_GLOBAL *ptrGlobalStruct;
		
		// Securely gets a pointer to a research definition from its ID. Returns NULL if invalid (does asserts on ID)
		STRUCT_RESEARCH_DEF *GetResearchDef(short int researchId) const {
			assert(researchId >= 0);
			assert(researchId < this->researchCount);
			if ((researchId < 0) || (researchId >= this->researchCount)) { return NULL; }
			return &this->researchDefArray[researchId];
		}
	};
	static_assert(sizeof(STRUCT_RESEARCH_DEF_INFO) == 0x0C, "STRUCT_RESEARCH_DEF_INFO size");

	// Size = 08
	class STRUCT_PLAYER_RESEARCH_STATUS {
	public:
		float researchingTime; // if >= researchDef.researchTime, then it's completed ! This is not =, but >=
		AOE_CONST_FUNC::RESEARCH_STATUSES currentStatus; // 2-bytes.
		short int unknown_06; // unused (probably ;)
	};
	static_assert(sizeof(STRUCT_PLAYER_RESEARCH_STATUS) == 8, "STRUCT_PLAYER_RESEARCH_STATUS size");

	// Size=0x10. Constructor=4EA91C
	class STRUCT_PLAYER_RESEARCH_INFO {
	public:
		STRUCT_PLAYER_RESEARCH_STATUS *researchStatusesArray;
		short int researchCount;
		short int unknown_06; // unused ?
		STRUCT_RESEARCH_DEF_INFO *ptrResearchDefInfo;
		STRUCT_PLAYER *ptrPlayer; // +C. Back pointer to player.
	};
	static_assert(sizeof(STRUCT_PLAYER_RESEARCH_INFO) == 0x10, "STRUCT_PLAYER_RESEARCH_INFO size");


	// Player's buildings unit list header (only buildings)
	// Size 0x1C
	class STRUCT_PLAYER_BUILDINGS_HEADER {
	public:
		STRUCT_PLAYER *player;
		unsigned long int allocatedArraySize; // this value * 4 = allocated size (in dwords) for the 4 lists below
		long int buildingsArrayElemCount;
		STRUCT_UNIT_BASE **ptrBuildingsArray; // Note: this list is always re-organized so there is not empty or obsolete element [0]=>[buildingsArrayElemCount-1]
		// 0x10
		STRUCT_MAP_VISIBILITY_INFO **ptrMapVisibilityInfoPointersArray; // indexes corresponds to ptrBuildingsArray. Data correspond to [0x7D205C] structure.
		STRUCT_MAP_VISIBILITY_INFO *ptrMapVisibilityInfoArray; // indexes corresponds to ptrBuildingsArray. It seems ptrMapVisibilityInfoArray[i] = *ptrMapVisibilityInfoPointersArray[i]
		long int *ptrBuildingsUnitIDs; // +18. Array containing buildings' unitId (corresponding to ptrBuildingsArray)
	};


	// Size = 0x38. Constructor = 005173D0
	// Probably used to stored player's exploration ?
	class STRUCT_PLAYER_MAP_INFO {
	public:
		char **unitCountThatSeeTile_row; // "sizeX" dwords. DS:[myMapRows+x*4] => DS:[...]+y=byte = unitCount that see this tile ?
		STRUCT_GAME_GLOBAL *global;
		STRUCT_PLAYER *player;
		STRUCT_GAME_MAP_INFO *globalGameMapInfo;
		// 0x10
		char *myTilesVisibility; // size = totalTilesCount bytes. Initialized content to 0xFF. 1 if fog-visible, 0 if explored but not visible ?
		long int mapSizeY;
		long int mapSizeX;
		long int unknown_1C_totalTilesCount;
		// 0x20
		long int totalTilesCount; // = mapSizeX * mapSizeY
		unsigned long int playerIdMask_low; // (p0=1,p1=2,p2=4...). = (1 << playerID)
		unsigned long int playerIdNegMask_low; // = NOT playerIdMask_low
		unsigned long int playerIdMask_high; // =(1 << playerID) << 8
		// 0x30
		unsigned long int playerIdNegMask_high; // = NOT  playerIdMask_high
		unsigned long int unknown_34;

		unsigned long int GetPlayerIdMask() { return this->playerIdMask_high | this->playerIdMask_high; }
	};
	static_assert(sizeof(STRUCT_PLAYER_MAP_INFO) == 0x38, "STRUCT_PLAYER_MAP_INFO size");

	// Size = 0x10. Positions to refresh in UI ??? Just a guess
	// 45EAC0=addElement(posY, posX)
	class STRUCT_PLAYER_UNKNOWN_58_AND_6C {
	public:
		unsigned long int arraySize; // Size for unknown_010
		unsigned long int usedElements; // +4. Number of used slots in array ?
		unsigned long int unknown_08; // +8. Same as +4.. One is usedcount, other is currentMaxIndex?
		unsigned long int unknown_0C; // 
		STRUCT_POSITION_INFO *positionsArray; // +10 = array to (posY, posX) as 2 dwords
	};


	// Size = 0x14 - Constructor = 0x4B09F0
	class STRUCT_PLAYER_POP_HIST_INFO {
	public:
		char *populationCountByTimeIndex; // Pointer to array of bytes = my population at a given moment.
		long int *unknown_04; // Ptr to ? +4=dword(int) +C=ptr=next
		long int unknown_08; // Min index ???
		long int currentTimeIndex; // +0C. Corresponds to current "maxTimeIndex" !
		long int arraySize;
	};
	static_assert(sizeof(STRUCT_PLAYER_POP_HIST_INFO) == 0x14, "Size of STRUCT_PLAYER_POP_HIST_INFO");


	// Size 0x14. Constructor = 0x516DA0
	// Stores explored resource units (that can NOT move = other than animal and farms)
	class STRUCT_PLAYER_MOTIONLESS_SPOTTED_GATHERABLE {
	public:
		STRUCT_NEARBY_UNIT_INFO **arrayNearbyUnitInfos;
		long int *arrayArraySizeInNearbyUnitInfos; // +04. elem[i] represents array size of arrayNearbyUnitInfos[i]
		long int *arrayUsedElemInNearbyUnitInfos; // +08. elem[i] represents the used slot count in arrayNearbyUnitInfos[i]
		STRUCT_PLAYER *player; // Parent player
		long int arraySize; // +10. Number of (both used/alloc) elements in each array. Cf PLAYER_GATHERABLE_RESOURCE_CATEGORIES.
	};
	static_assert(sizeof(STRUCT_PLAYER_MOTIONLESS_SPOTTED_GATHERABLE) == 0x14, "STRUCT_PLAYER_UNKNOWN_118 size");


	// PLAYER. Constructor=0x4EFB00
	// Standard player struct sizes are 0x85C (gaia) or 0x84C (non-gaia).
	// If selected units features is installed, size is increased to host selected unit pointers at the end of the structure.
	// Size will depend on how many maximum selected units it has been set.
	class STRUCT_PLAYER {
	public:
		unsigned long int checksum; // 0x00549B80 or 0x00549A44 (normal player) or 0x00544D18 (parent class)
		unsigned long int isComputerControlled; // 0/1. Mostly for "military" behaviours, not for MainAI-related behaviours.
		unsigned long int unknown_008; // value is from [55473C] ?
		unsigned long int unknown_00C;
		// 0x10
		unsigned long int unknown_010;
		unsigned long int unknown_014;
		unsigned long int unknown_018;
		unsigned long int unknown_01C;
		// 0x20
		short int unknown_020;
		short int structDefUnitArraySize; // +22. Number of elements in ptrStructDefUnitTable array = number of "unit definitions" for this player.
		STRUCT_UNITDEF_BASE **ptrStructDefUnitTable; // ptr to Array containing pointers to all player unitDefs (index=DATID). They can be NULL (not available/invalid). See also structDefUnitArraySize.
		STRUCT_PER_TYPE_UNIT_LIST_LINK *ptrCreatableUnitsListLink; // +28. Only building/living + smoke, dead units...?
		STRUCT_PER_TYPE_UNIT_LIST_LINK *ptrNonCreatableUnitsListLink; // Others than building/living ?
		// 0x30
		STRUCT_PER_TYPE_UNIT_LIST_LINK *ptrTempUnitsListLink; // ptr to list of temporary units (with <0 id)
		STRUCT_SCORE_HEADER *ptrScoreInformation; // +34. 
		STRUCT_PLAYER_MAP_INFO *myMapInfo;
		STRUCT_GAME_GLOBAL *ptrGlobalStruct;
		// 0x40
		STRUCT_PLAYER_BUILDINGS_HEADER *ptrBuildingsListHeader;
		char *playerName_length16max; // pointer to player name (max length=0x10)
		char AIControlMode; // 1=human, 2=gaia, 3=computer. Do not customize it. It is important when loading/saving, impacts destructor...
		char unknown_049;
		short int playerId; // 0x4A
		short int unknown_04C; // something's size ? related to civ ?
	private:
		// 0x4E - Unused in standard game. Used by "ManageAI" mod to enable(1)/disable(0) AI control.
		// Check if "ManageAI" feature is installed before using this. => Use IsAIActive(...) instead of using this directly
		char unused_customAIFlag;
	public:
		char unused_04F;
		// 0x50
		float *ptrResourceValues; // To get a resource value: player->ptrResourceValues[CST_RES_ORDER_xxx])
		char tileSet; // +54. Civilization tileset, 5 max (0-4). 4 is ROR's added tileset.
		char unknown_055_unused[3];
		STRUCT_PLAYER_UNKNOWN_58_AND_6C unknown_058;
		STRUCT_PLAYER_UNKNOWN_58_AND_6C unknown_06C;
		// 0x80
		char aliveStatus; //0=alive, 1=win 2=lost.
		char isInactive; // +81. 1 for resigned/disconnected ? 45BBE3
		short int unknown_082; // unused ?
		AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES *ptrDiplomacyStances; // +84. [pointer+iPlayerId] = diplomacy value: 0=ally,1=neutral, 3=enemy. gaia&self are neutral !
		char unknown_088;
		char unknown_089[3]; // unused ?
		AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES diplomacyVSPlayers[9]; // +8C. Diplomacy against gaia(p0), p1, p2... 1=self, 2=allied, 3=neutral, 4=enemy. Long ints (4bytes each)
		// 0xB0
		long int unknown_0B0[9]; // B0: bool (dword) per playerId, related to visibility/exploration ?
		short int unknown_0D4; // +D4. some mask for exploration. In fact, +D4 and D6 are 1 field (dword)
		short int greyedMapToShowPlayerMask; // +D6. Bit-to-bit mask. bit1(low)=gaia, bit2=player1... Max possible value=0x1FF
		short int clearMapToShowPlayerMask; // +D8. Bit-to-bit mask. bit1(low)=gaia, bit2=player1... Max possible value=0x1FF
		short int unknown_0DA; // a mask too. In fact, +D8 and DA are 1 field (dword)
		long int unknown_0DC; // A mask too ?
		// 0xE0
		long int unknown_0E0[9]; // A dword per player 0-8
		long int sharedExploration; // +104. 1=shared exploration (writing tech).
		char hasAlliedVictory; // +108.
		AOE_CONST_FUNC::CIVILIZATIONS civilizationId; // +109. 1 byte (char)
		short int unknown_10A; // unused ?
		unsigned long int ptrPlayerColorStruct; // quite unknown. +4=name(x.col), +26=color(word)? +28=sub_struct
		// 0x110
		short int techTreeId; // 45BA12 : DWORD !
		short int unknown_112;
		STRUCT_PLAYER_MOTIONLESS_SPOTTED_GATHERABLE *spottedResources; // +114. Contains info about resource units. Set in 0x4F1EF8. Related to +58 data. Not related to AI. Each time a tile is explored, underlying resources are added here.
		float screenPositionY; // axis southwest-northeast like /
		float screenPositionX; // axis northwest-southeast like \   [origin is left corner - max(X,Y) is right corner]
		// 0x120
		short int unknown_120_posY; // screen pos ?
		short int unknown_122_posX; // screen pos ?
		long int unknown_124;
		long int unknown_128; // split ; +0x129=?
		long int unknown_12C;
		// 0x130
		short int unknown_130;
		short int unknown_132;
		STRUCT_UNIT_BASE *selectedStructUnitTable[26]; // +0x134 is first selected unit's ptr. Last=First?. Only relevant if "selected units" features is NOT installed. See GetRelevantSelectedUnitsPointer(...).
		// +19C : a mask for interactions (depending on selected units). Bits: Refer to UNIT_INTERACTION_MASK_BIT.
		unsigned long int currentInteractionMask;
		// 0x1A0
		short int selectedUnitCount; // Count of currently selected units.
		char groupNumberIsUsed[0x7E]; // In fact, indices 0-0x0A are unused because 0 is unused and 1-0x0A are shortcut numbers, not groups.
		// 1FF byte is NOT initialized, it is a bug but only minor (we this byte is reached, game resets all groups and this time it will be initialized... But will also clear +220 byte !)
		// after 0x220, something different starts here (different initialization)?
		unsigned long int unknown_220; // Warning, if not part of groupNumberIsUsed, then there is a bug, +220 byte can be reset when group max number count is reached
		STRUCT_PLAYER_RESEARCH_INFO *ptrResearchesStruct; // +224.
		unsigned long int unknown_228;
		unsigned long int unknown_22C; // a counter (decreased...?)
		// 0x230
		unsigned long int unknown_230; // related to +0x238 ? Something about time (seconds)
		unsigned long int unknown_234; // a counter (decreased...?)
		float unknown_238; // float, related to timer interval. When it reaches 1, reset to 0 and decrement "+0x234" value.
		STRUCT_AI *ptrAIStruct; // Pointer to the main structure for AI
		// 0x240
		char AIDebugTextLine1[0x100]; // +240.
		char AIDebugTextLine2[0x100]; // +340.
		char AIDebugTextLine3[0x100]; // +440.
		char AIDebugTextLine4[0x100]; // +540.
		char AIDebugTextLine5[0x100]; // +640.
		char AIDebugTextLine6[0x100]; // +740.
		STRUCT_PLAYER_POP_HIST_INFO *populationHistoryInfo; // +840. Information for achievements screen (timeline)
		float remainingTimeToAllRuinsVictory; // +844. -1 means not relevant (I do not have all ruins). See 0x50BF00 method
		float remainingTimeToAllRelicsVictory; // +848. -1 means not relevant (I do not have all relics). See 0x50BF00 method
		char unknown_84C[0x85C - 0x84C];
		// 0x85C - DO NOT access this unless you checked the "selected units" feature is installed ! On standard game structure stops here.
		// See GetRelevantSelectedUnitsPointer(...)
		STRUCT_UNIT_BASE *custom_selectedUnits[CST_RS_MAX_SUPPORTED_SELECTED_UNITS]; // ONLY USE THIS if "selected units" feature is installed or you will access bad memory. See GetRelevantSelectedUnitsPointer (in commands)

		STRUCT_GAME_GLOBAL *GetGlobalStruct() const {
			return (STRUCT_GAME_GLOBAL*) this->ptrGlobalStruct;
		}
		bool IsCheckSumValid() const {
			return (this->checksum == 0x00549B80) || (this->checksum == 0x00549A44) ||
				(this->checksum == 0x00544D18); // parent class checksum
		}
		// Returns true if AI "economy" control is active for the player, depending on game EXE (is "ManageAI" feature installed ?)
		bool IsAIActive(bool hasManageAIFeatureON) {
			if (hasManageAIFeatureON) {
				return (this->unused_customAIFlag != 0);
			}
			return (this->AIControlMode == 3);
		}
		char GetCustomAIFlag() const { return this->unused_customAIFlag; } // please use IsAIActive instead unless you have a good reason.
		float GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES resourceIndex) {
			if ((resourceIndex < 0) || (resourceIndex > AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_ALL_RELICS)) { return -1; }
			return this->ptrResourceValues[resourceIndex];
		}
		void SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES resourceIndex, float value) {
			if ((resourceIndex < 0) || (resourceIndex > AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_ALL_RELICS)) { return; }
			this->ptrResourceValues[resourceIndex] = value;
		}
		void SetCustomAIFlag(char value) { this->unused_customAIFlag = value; } // Sets customROR's AI flag.
		STRUCT_UNITDEF_BASE *GetUnitDefBase(short int unitDefId) const {
			if ((unitDefId < 0) || (unitDefId >= this->structDefUnitArraySize)) { return NULL; }
			return (STRUCT_UNITDEF_BASE *)this->ptrStructDefUnitTable[unitDefId];
		}
		STRUCT_RESEARCH_DEF *GetResearchDef(short int researchId) const {
			if (!this->ptrResearchesStruct || !this->ptrResearchesStruct->ptrResearchDefInfo) { return NULL; }
			if ((researchId < 0) || (researchId >= this->ptrResearchesStruct->researchCount)) { return NULL; }
			return this->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(researchId);
		}
		// Securely get research status for research id. Returns NULL if input is invalid.
		STRUCT_PLAYER_RESEARCH_STATUS *GetResearchStatus(short int researchId) const {
			if (!this->ptrResearchesStruct || !this->ptrResearchesStruct->researchStatusesArray) { return NULL; }
			if ((researchId < 0) || (researchId >= this->ptrResearchesStruct->researchCount)) { return NULL; }
			return &this->ptrResearchesStruct->researchStatusesArray[researchId];
		}
	};

}
