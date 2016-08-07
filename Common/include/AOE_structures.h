
#pragma once

#include <math.h>
#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_empires_dat.h>
#include <AOE_structures_drs.h>
#include <AOE_struct_tech_def.h>
#include <AOE_struct_research_def.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_civilization_def.h>
#include <AOE_struct_terrain_def.h>
#include <AOE_struct_map_def.h>
#include <gameVersion.h>


// Note: as we never allocate/create unit structures ourselves, there is no problem going out of bounds as long as ROR EXE allocated enough values !
#define CST_RS_MAX_SUPPORTED_SELECTED_UNITS 100


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{
	typedef char char_array_128[0x80];


	// Pre-declarations
	class STRUCT_PLAYER_RESEARCH_STATUS;
	class STRUCT_PLAYER_RESEARCH_INFO;
	class STRUCT_PLAYER_UNKNOWN_118;
	class STRUCT_PATH_FINDING_UNKNOWN_POS_INFO;
	class STRUCT_AI_UNIT_LIST_INFO;
	class STRUCT_HUMAN_TRAIN_QUEUE_INFO;
	class STRUCT_SPOTTED_RESOURCE_INFO;
	class STRUCT_POSITION_INFO;
	class STRUCT_MP_COMMUNICATION; // Visible at global level
	class STRUCT_UNKNOWN_GAME_TIME;
	class STRUCT_COMMAND_LINE_INFO;
	class STRUCT_SOUND_DATA_ELEMENT;
	class STRUCT_MAIN_SOUND;
	class STRUCT_MAIN_MUSIC;
	class STRUCT_SCENARIO_INFO;
	class STRUCT_GAME_COMMANDS_INFO;
	class STRUCT_GAME_GLOBAL;
	class STRUCT_GAME_SETTINGS;
	class STRUCT_PER_TYPE_UNIT_LIST_ELEMENT;
	class STRUCT_PER_TYPE_UNIT_LIST_LINK;
	class STRUCT_SCORE_ELEM;
	class STRUCT_SCORE_HEADER;
	class STRUCT_PLAYER_BUILDINGS_HEADER;
	class STRUCT_PLAYER_UNKNOWN_58_AND_6C;
	class STRUCT_PLAYER_POP_HIST_INFO;
	class STRUCT_PLAYER;
	class STRUCT_INF_AI_UNIT_LIST_ELEM;
	class STRUCT_TAC_AI_BUILD_TO_TRIGGER;
	class STRUCT_VILLAGER_TASKS_ELEM;
	class STRUCT_UNIT_GROUP_ELEM;
	class STRUCT_PATH_FINDING_INTERMEDIATE_STEP;
	class STRUCT_UNIT_TARGET_POS;
	class STRUCT_UNIT_MOVEMENT_INFO;
	class STRUCT_UNIT;
	class STRUCT_UNIT_BASE; // Eye candy / common class for all units
	class STRUCT_UNIT_FLAG;
	class STRUCT_UNIT_DOPPLEGANGER;
	class STRUCT_UNIT_DEAD_FISH;
	class STRUCT_UNIT_BIRD;
	class STRUCT_UNIT_TYPE50;
	class STRUCT_UNIT_PROJECTILE;
	class STRUCT_UNIT_LIVING;
	class STRUCT_UNIT_BUILDING;
	class STRUCT_UNIT_TREE;
	class STRUCT_UNIT_BASE_LIST;

	// AI
	class STRUCT_AI;
	class STRUCT_COMMON_AI_OBJECT; // The common object included in Main decision AI, build AI, etc
	class STRUCT_BUILD_AI;
	class STRUCT_CON_AI;
	class STRUCT_DIPLOMACY_AI;
	class STRUCT_EMOTIONAL_AI;
	class STRUCT_INF_AI;
	class STRUCT_RESOURCE_AI;
	class STRUCT_STRATEGY_AI;
	class STRUCT_TAC_AI;
	class STRUCT_TRADE_AI;
	class STRUCT_STRATEGY_ELEMENT;
	class STRUCT_PLANNED_RESOURCE_NEEDS;
	class STRUCT_CON_AI_CONSTRUCTION;
	class STRUCT_INF_AI_BUILDS_HISTORY;
	class STRUCT_TAC_AI_TARGET_INFO;

	// Action / Activity
	class STRUCT_ACTION_BASE;
	class STRUCT_ACTION_MOVE;
	class STRUCT_ACTION_ATTACK;
	class STRUCT_ACTION_CONVERSION;
	class STRUCT_ACTION_LINK;
	class STRUCT_ACTION_MAKE_OBJECT;
	class STRUCT_ACTION_DISCOVERY_ARTEFACT;
	class STRUCT_ACTION_PRODUCE_FARM_FOOD;
	class STRUCT_ACTION_PROJECTILE;
	class STRUCT_UNIT_ACTIVITY_TARGET_ELEM;
	class STRUCT_UNIT_ACTIVITY_QUEUE;
	class STRUCT_UNIT_ACTIVITY;
	class STRUCT_UNIT_ACTION_INFO;

	// MAP
	class STRUCT_MAP_VISIBILITY_INFO;
	class STRUCT_MAPGEN_PLAYER_BASE_INFO;
	class STRUCT_MAPGEN_ELEVATION_DATA;
	class STRUCT_MAPGEN_ELEVATION_LINK;
	class STRUCT_MAPGEN_ELEVATION_INFO;
	class STRUCT_MAP_GENERATION_INFO;
	class STRUCT_GAME_MAP_TILE_INFO;
	class STRUCT_GAME_MAP_INFO; // TODO: Confirm exact role
	class STRUCT_NEARBY_UNIT_INFO;
	class STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYER_NEARBY_UNITS_INFO;
	class STRUCT_UNKNOWN_MAP_INFO_7D2058;
	class STRUCT_UNKNOWN_MAP_DATA_F04C;
	class STRUCT_TEMP_MAP_BUILD_LIKE_INFOS;
	class STRUCT_TEMP_MAP_POSITION_INFO;

	// UI components
	class STRUCT_UI_OBJ_UNKNOWN_INFO;
	class STRUCT_ANY_UI; // parent class
	class STRUCT_UI_LIST_ITEM;
	class STRUCT_UI_BUTTON;
	class STRUCT_UI_BUTTON_WITH_NUMBER;
	class STRUCT_UI_TEXTBOX;
	class STRUCT_UI_LISTBOX;
	class STRUCT_UI_COMBOBOX;
	class STRUCT_UI_SCROLLBAR;
	class STRUCT_UI_LABEL;
	// UI
	class STRUCT_UI_DIAMOND_MAP;
	class STRUCT_UI_IN_GAME_TEXT_ZONE;
	class STRUCT_UI_PLAYING_ZONE; // Also used in editor !
	class STRUCT_UI_UNIT_BUTTON_INFO;
	class STRUCT_UI_SCENARIO_EDITOR_MAIN; // "Scenario Editor Screen"
	class STRUCT_UI_SCENARIO_EDITOR_MENU;
	class STRUCT_UI_F11_POP_PANEL;
	class STRUCT_UI_UNIT_INFO_ZONE;
	class STRUCT_UI_EDITOR_UNIT_INFO_ZONE;
	class STRUCT_UI_IN_GAME_MAIN; // "GameScreen"
	class STRUCT_UI_IN_GAME_SUB1;
	class STRUCT_UI_IN_GAME_MENU;
	class STRUCT_UI_POPUP_DIALOG;
	class STRUCT_UI_GAME_SETTINGS;
	class STRUCT_UI_DIPLOMACY_DIALOG;
	class STRUCT_UI_IN_GAME_OPTIONS;
	class STRUCT_UI_LOAD_SAVED_GAME;
	class STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU; // welcome screen for scenario editor (with 3 buttons)
	class STRUCT_UI_CAMPAIGN_EDITOR;
	class STRUCT_UI_DIAM_MAP; // diam_map in game comments
	class STRUCT_UI_JOIN_SCREEN;
	class STRUCT_UI_WELCOME_MAIN_SCREEN;


	// Size = 8. Mostly used in temporary treatments
	class STRUCT_POSITION_INFO {
	public:
		long int posY;
		long int posX;
	};
	static_assert(sizeof(STRUCT_POSITION_INFO) == 8, "STRUCT_POSITION_INFO size");

#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment)
	// Size = 1 byte. This is a shortcut to avoid duplicating bit operations everywhere
	class TERRAIN_BYTE {
	public:
		char terrainData;
		char GetTerrainId() const {
			return this->terrainData & 0x1F;
		}
		bool SetTerrainId(char value) { // To properly update terrain, there are more complex treatments to do than this
			if ((value < 0) || (value > 31)) { return false; }
			this->terrainData = (this->terrainData & 0xE0) | value; // preserve altitude
			return true;
		}
		char GetAltitude() const {
			return (this->terrainData & 0xE0) >> 5;
		}
		bool SetAltitude(char value) { // To update altitude, there are more complex treatments to do
			if ((value < 0) || (value > 7)) { return false; }
			this->terrainData = (this->terrainData & 0x1F) | (value << 5); // preserve terrainId
			return true;
		}
	};
	static_assert(sizeof(TERRAIN_BYTE) == 1, "TERRAIN_BYTE size");

	// Size = 1 byte. This is a shortcut to avoid duplicating bit operations everywhere
	class BORDER_BYTE {
	public:
		char borderData;
		GROUND_BORDERS GetBorderId() const {
			return (GROUND_BORDERS)(this->borderData & 0x0F); // 4 low bits = border id
		}
		// UNSURE. Shape indexes can go higher, but there seems to be something with values from 0 to 12 ?
		char GetShapeIndex() const {
			return (this->borderData & 0xF0); // 4 high bits = border id
		}
	};
	static_assert(sizeof(BORDER_BYTE) == 1, "BORDER_BYTE size");
#pragma pack(pop)

	// Size=8. Represents a unit list element
	class STRUCT_UNIT_BASE_LIST {
	public:
		STRUCT_UNIT_BASE *unit;
		STRUCT_UNIT_BASE_LIST *next; // NULL=end of list
	};
	static_assert(sizeof(STRUCT_UNIT_BASE_LIST) == 8, "STRUCT_UNIT_BASE_LIST size");

	// Used in AI structures to keep lists of unit IDs. Size=0x10
	// All methods suppose that usedElemCount==arraySize (no empty slot)
	// 0x4C6880 = unitElemList.containsUnitId(pUnitId)
	// 0x4E3AF0 = unitElemList.Remove(unitId) 
	// 0x414FC0 = unitListInfo.reallocArray(newSize). Not supposed to be used to remove elements.
	// 0x4C68C0 = unitElemList.AddUnitId(unitId)
	class STRUCT_AI_UNIT_LIST_INFO {
	public:
		long int *unitIdArray; // Pointer to array of unitIDs. NULL when size==0
		long int usedElements; // +4. Number of elements in array that are actually used
		long int maxElementCount; // +8. Max or desired value ?
		long int arraySize; // +C. Number of allocated elements in array

		bool ContainsUnitId(long int unitId) {
			const unsigned long int addr = 0x4C6880;
			long int res;
			_asm {
				MOV ECX, this;
				PUSH unitId;
				CALL addr;
				MOV res, EAX;
			}
			return (res != 0);
		}
		bool Remove(long int unitId) {
			const unsigned long int addr = 0x4E3AF0;
			long int res;
			_asm {
				MOV ECX, this;
				PUSH unitId;
				CALL addr;
				MOV res, EAX;
			}
			return (res != 0);
		}
		bool Add(long int unitId) { // returns true if elem was added.
			const unsigned long int addr = 0x4C68C0;
			long int res;
			_asm {
				MOV ECX, this;
				PUSH unitId;
				CALL addr;
				MOV res, EAX;
			}
			return (res != 0);
		}
	};
	static_assert(sizeof(STRUCT_AI_UNIT_LIST_INFO) == 0x10, "STRUCT_LIST_OF_UNITID size");

	// Size=4
	class STRUCT_HUMAN_TRAIN_QUEUE_INFO {
	public:
		short int DATID; // Only relevant when unitCount > 0
		short int unitCount;
	};

	// Size=0x14. Used in InfAI structure.
	class STRUCT_SPOTTED_RESOURCE_INFO {
	public:
		long int gatherableUnitId;
		char posY;
		char posX;
		char unknown_06;
		char unknown_07;
		long int estimatedTotalTripLength; // +8. round trip estimated distance, including an arbitrary +10 value for detour (formula = distance*2 +10)
		char unknown_0C;
		char unknown_0D;
		char distanceToStorageBuilding; // +E. Actual direct distance between resource unit and its closest storage building.
		char resourceType; // +F. Always 0-3 (food wood stone gold).
		long int storageBuildingUnitID; // +10. UnitID of the closest storage building.
	};
	static_assert(sizeof(STRUCT_SPOTTED_RESOURCE_INFO) == 0x14, "STRUCT_SPOTTED_RESOURCE_INFO size");


	// Size=4 (1 dword)
	class STRUCT_MAP_VISIBILITY_INFO {
	public:
		short int fogVisibilityMask;
		short int explorationVisibilityMask;
		bool isFogVisibleForPlayer(long int playerId) {
			long int mask = 1 << playerId;
			return (this->fogVisibilityMask & mask) != 0;
		}
		bool isExploredForPlayer(long int playerId) {
			long int mask = 1 << playerId;
			return (this->explorationVisibilityMask & mask) != 0;
		}
	};
	static_assert(sizeof(STRUCT_MAP_VISIBILITY_INFO) == 4, "STRUCT_MAP_VISIBILITY_INFO size");


	/*******
	* MAP
	*/


	// Size = 0x34
	class STRUCT_MAPGEN_PLAYER_BASE_INFO {
	public:
		unsigned long int unknown_00;
		unsigned long int unknown_04;
		long int posY;
		long int posX;
		// 0x10
		unsigned long int unknown_10; // Could be AGE3 unknown4 in maps/base zone data
		long int nonBaseTerrainSizeBetweenPlayers; // REALLY unsure
		char unknown_18;
		char unused_19[3];
		unsigned long unknown_1C;
		// 0x20
		unsigned long unknown_20;
		unsigned long unknown_24;
		unsigned long mapSizeY;
		unsigned long mapSizeX;
		// 0x30
		char unknown_30;
		char unused_31[3];
	};
	static_assert(sizeof(STRUCT_MAPGEN_PLAYER_BASE_INFO) == 0x34, "STRUCT_MAPGEN_PLAYER_BASE_INFO size");


	// Size = 0xF80. Included in other bigger structs
	class STRUCT_MAPGEN_ELEVATION_DATA {
	public:
		char unknown_000[0x63 * 0x18]; // included array,elemSize=0x18, 0x63 elements
		long int unknown_004_usedElemCount; // +0x948
		unsigned long int unknown_94C; // +94C
		char unknown_950[0xF80 - 0x950]; // +950. Included array, elemSize=0x10, 0x63 elements
	};

	// Size = 8. Used to manage a list of STRUCT_MAPGEN_ELEVATION_INFO for a STRUCT_MAP_GENERATION_INFO object
	// First created is last of the list (we always insert in first position... I believe.)
	class STRUCT_MAPGEN_ELEVATION_LINK {
	public:
		STRUCT_MAPGEN_ELEVATION_INFO *mapGenElevationInfo;
		STRUCT_MAPGEN_ELEVATION_LINK *next; // Last element as a NULL next value.
	};

	// Size = 0xFB0, constructor=0x46F030
	// TO DO: there are also checksums 00545D04, 00545D10, 00545D14, 00545D4C, 00545CD8 that have the same parent ?
	class STRUCT_MAPGEN_ELEVATION_INFO {
	public:
		unsigned long int checksum; // 00 5D 54 00, parent=C8 5C 54 00 (same parent as STRUCT_MAP_GENERATION_INFO ?)
		STRUCT_MAP_GENERATION_INFO *mapGenInfo; // a "parent" mapgen object ?
		unsigned long int unknown_008;
		STRUCT_GAME_MAP_INFO *gameMapInfo;
		// 0x10
		STRUCT_GAME_MAP_TILE_INFO **pTileInfoRows; // same as gameMapInfo+8D8C
		long int mapSizeY;
		long int mapSizeX;
		float unknown_01C; // Default value = 1.5
		// 0x20
		AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO **unknown_020; // pointer 1 from mapGenInfo
		AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO **unknown_024; // pointer 2 from mapGenInfo
		AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO **unknown_028; // pointer 3 from mapGenInfo
		AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO **unknown_02C; // pointer 4 from mapGenInfo
		// 0x30
		STRUCT_MAPGEN_ELEVATION_DATA elevationData; // Size = 0xF80. Copied from mapGenInfo.unknown_3FD4 ?

		bool IsCheckSumValid() {
			return (this->checksum == 0x00545CC8) || (this->checksum == 0x00545D00);
		}
	};

	// size=0x4F5C, constructor=471D20.
	class STRUCT_MAP_GENERATION_INFO {
	public:
		unsigned long int checksum; // 18 5D 54 00, parent objects are C8 5C 54 00, DC 5C 54 00
		unsigned long int unknown_0004; // a "parent" mapgen object ?
		STRUCT_MAPGEN_ELEVATION_LINK *elevationInfoFirstLinkElem; // represents a list of elevation info objects ?
		STRUCT_GAME_MAP_INFO *gameMapInfo;
		// 0x10
		unsigned long int unknown_0010; // Same as STRUCT_GAME_MAP_INFO+8D8C
		long int mapArraySizeY;
		long int mapArraySizeX;
		unsigned long int unknown_001C;
		// 0x20
		unsigned long int unknown_0020; // pointer 1
		unsigned long int unknown_0024; // pointer 2
		unsigned long int unknown_0028; // pointer 3
		unsigned long int unknown_002C; // pointer 4
		// 0x30
		unsigned long int unknown_0030;
		long int mapType;
		long int playerCount; // NOT counting gaia, only real players.
		unsigned long int unknown_003C;
		// 0x40
		unsigned long int unknown_0040;
		STRUCT_MAPGEN_PLAYER_BASE_INFO playersBaseInfo[8]; // elemSize=0x34. (index=playerId-1?). MAYBE it starts at +3C, not 44
		char unknown_0044[0x1458 - 0x1E4];
		long int totalPlayerBaseZonesCount; // +1458. Corresponds to number of players too.
		long int nonBaseDefaultTerrainId; // +145C
		// 0x1460
		long int cornersBorderPositions[4]; // SO-NO-NE-SE. To precise exact role.
		// 0x1470
		char borderUsagePercentage;
		char unknown_1471[3];
		char unknown_1474[0x270C - 0x1474];
		STRUCT_MAPGEN_OBJECT_TO_PLACE objectsToPlaceOnMap[0x63]; // +270C
		long int numberOfMapGenObjectElems; // +399C.
		// 0x39A0
		char unknown_39A0[0x3FD4 - 0x39A0];
		STRUCT_MAPGEN_ELEVATION_DATA elevationData; // +3FD4. Size=0xF80
		unsigned long int mapTypesCount; // +4F54.
		STRUCT_MAPGEN_MAP_TYPE *mapGenMapTypesInfoArray; // +4F58.

		bool IsCheckSumValid() {
			return (this->checksum == 0x00545D18) || (this->checksum == 0x00545CC8) || (this->checksum == 0x00545CDC);
		}
	};

	// Size = 0x18. The main "actual" map info necessary to set correct terrain/altitude and transitions are terrainData, terrainBorderData and elevationGraphicsIndex
	class STRUCT_GAME_MAP_TILE_INFO {
	public:
		short int displayX; // +00. A screen X position where to display tile (independent from player's screen position). Units on this tile will be displayed accordingly.
		short int displayY; // +02. A screen Y position where to display tile (independent from player's screen position). Units on this tile will be displayed accordingly. Changing this value can generate fake elevation (display only).
		char elevationGraphicsIndex; // +4. To retrieve elevation graphics in terrain's table - 0=flat. HAS an effect on units altitude ! Please call mapInfo->RecomputeTileDisplayPos(...) after modifying this
		TERRAIN_BYTE terrainData; // +5: elevation/terrainID. 3 high bits (0x80/0x40/0x20) represent altitude 0-7. 5 low bits represent terrainId 0-31
		BORDER_BYTE terrainBorderData; // +6. Border id & shape index (for terrain transitions)
		char unknown_07; // Updated in 444980 to 0xCC, but reset to 0x0F very frequently (quite instantly). Some status ?
		char unknown_08;
		char unknown_09; // Seen 0x0F
		char tileHighlightLevel; // +0A. Tile brillance level (for tile selection in editor). Values in 0-0xB0. Default=0, 0x0F=underMouse(editor). Updated in 444980 with arg5 ?
		char unknown_0B; // maybe +A is a short int
		char unknown_0C;
		char unknown_0D;
		char unknown_0E;
		char unknown_0F;
		// 0x10
		STRUCT_UNIT_BASE_LIST *unitsOnThisTile; // +10. Array of units occupying this tile. The list ends with a NULL pointer. NULL if no unit on this tile.
		short int unitsOnThisTileCount; // +14. Number of units in unitsOnThisTile array. unitsOnThisTile=NULL if unitsOnThisTileCount==0.
		short int unknown_16; // possibly unused ?
	};

	// Size = 0xB5F8 to confirm. Init in 0x4445E0.
	class STRUCT_GAME_MAP_INFO {
	public:
		unsigned long int checksum; // F4 99 54 00 (parent class 0A 45 54 00)
		STRUCT_GAME_MAP_TILE_INFO *pTilesArray; // +04. Array base of all map tiles
		long int mapArraySizeY;
		long int mapArraySizeX;
		// 0x10
		unsigned long int unknown_0010; // = mapSizeY * [+8D96]
		unsigned long int unknown_0014; // = mapSizeX * [+8D98]
		char unknown_0018_array[0x6 * 0x13];// 0x18: array of elems size=0x6, 3 words. 0x13 elems ? Init in 444240
		short int unknown_008A;
		STRUCT_TERRAIN_DEF terrainDefinitions[0x20]; // Count is unsure
		char unknown_338C[0x8D8C - 0x338C];
		// 8808-1C... 0x10 elems and size=5A0 => start at 2E08 ? WTF. about shp
		STRUCT_GAME_MAP_TILE_INFO **pTileInfoRows; // 0x8D8C. Please use GetTileInfo(...)
		// 0x8D90
		short int terrainCount; // +8D90. Default = 0x17 = 23 different terrains in empires.dat
		short int unknown_8D92;
		unsigned short int unknown_8D94;
		unsigned short int unknown_8D96; // init 0x40 . for Y ?
		unsigned short int unknown_8D98; // init 0x20 . for X ?
		unsigned short int unknown_8D9A; // init 0x10. (posX-posY)*unknown_8D9A - (altitude*[unknown_8D9E or unknown_8D9E-1]) = tile.displayY
		unsigned short int unknown_8D9C; // init 0x20. (posX+posY)*unknown_8D9C = tile.displayX
		unsigned short int unknown_8D9E; // init 0x10. Use "unknown_8D9E+1" for tile.elevationGraphicIndex in (2,6,8,A,E,F,10), see 0x444B7B.
		char unknown_8DA0[0x8DB0 - 0x8DA0];
		char *unknown_8DB0; // ptr, map data also. Size= sizeX*sizeY*1 (x*y bytes)
		unsigned long int unknown_8DB4; // ptr, map data also ? Size=(sizeX+1)*4 bytes (ptrs ?) = rows for +8DB0
		char revealMap; // 0x8DB8
		char hasFog; // 0x8DB9
		char unknown_8DBA;
		char unknown_8DBB;
		STRUCT_MAP_GENERATION_INFO *unknown_8DBC_mapgenInfos;
		// 0x8DC0
		STRUCT_GAME_GLOBAL *globalStruct;
		unsigned long int unknown_ptrMapInfosLink; // +8DC4. Size=0C. +0=ptr_array_dwords.+4=arraySize. +8=backPtr to this. constructor 5219C0. 521C60=getItem(i)
		unsigned long int unknown_8DC8; // +8DC8. ptr, size=0x0C. +0/+4=mapSizeYX.+8=ptr_array(sizeX*sizeY*4). Constructor 517E30. Related to 7D205C
		STRUCT_UNKNOWN_MAP_INFO_7D2058 *unknown_8DCC;
		char unknown_8DD0[0x8DE4 - 0x8DD0];
		unsigned long int unknown_8DE4; // init -1
		unsigned long int unknown_8DE8; // init -1
		unsigned long int unknown_8DEC; // init -1
		// 0x8DF0
		unsigned long int unknown_8DF0; // init -1
		unsigned long int unknown_8DF4; //?
		// 0x8DF8
		char unknown_8DF8_array[0x100 * 0x28]; // Array: 0x100 elements, size=0x28. All are dwords.

		bool IsCheckSumValid() { return this->checksum == 0x005499F4; }
		STRUCT_GAME_MAP_TILE_INFO *GetTileInfo(short int x, short int y) {
			if ((x < 0) || (x >= this->mapArraySizeX) || (y < 0) || (y >= this->mapArraySizeY)) { return NULL; }
			return &this->pTileInfoRows[x][y];
		}
		// Updates a tile's displayX/Y fields according to its elevation and position.
		// Necessary to fix tile "height" display when altitude/elevationGraphicIndex changed.
		bool RecomputeTileDisplayPos(short int x, short int y) {
			if ((x < 0) || (x >= this->mapArraySizeX) || (y < 0) || (y >= this->mapArraySizeY)) { return false; }
			STRUCT_GAME_MAP_TILE_INFO *tile = this->GetTileInfo(x, y);
			if (!tile) { return false; }
			tile->displayX = (x + y) * this->unknown_8D9C;
			tile->displayY = (x - y) * this->unknown_8D9A;
			tile->displayY -= tile->terrainData.GetAltitude() * this->unknown_8D9E;
			switch (tile->elevationGraphicsIndex) {
			case 2:
			case 6:
			case 8:
			case 0xA:
			case 0xE:
			case 0xF:
			case 0x10:
				tile->displayY -= this->unknown_8D9E;
				break;
			default:
				break;
			}
			return true;
		}
	};

	// Size=8. Used in temporary treatments, for example auto-attack unit when idle...
	class STRUCT_NEARBY_UNIT_INFO {
	public:
		long int unitId; // +0
		char distance; // +4. Unsure. Not always valued ?
		char playerId; // +5
		char posY; // +6
		char posX; // +7
	};
	static_assert(sizeof(STRUCT_NEARBY_UNIT_INFO) == 0x8, "STRUCT_NEARBY_UNIT_INFO size");

	// Size = 8
	// Contains information about nearby units for temp treatments.
	// Each player has its own object
	class STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYER_NEARBY_UNITS_INFO {
	public:
		STRUCT_NEARBY_UNIT_INFO *nearbyUnitInfoArray;
		short int arraySize; // +4
		short int numberUsed; // +6
	};

	// Referenced in global variable 0x7D2058 and also from STRUCT_GAME_MAP_INFO+8DCC
	// Size = 0x24. Constructor = 0x516800
	class STRUCT_UNKNOWN_MAP_INFO_7D2058 {
	public:
		STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYER_NEARBY_UNITS_INFO **unknown_00; // Array[playerId] => pointer to array of STRUCT_UNKNOWN_MAP_INFO_7D2058_UNITINFOELEM
		char *unknown_04; // Array size = 0x800 bytes.
		char *unknown_08; // Array size = 0x800 bytes.
		char *unknown_0C; // Array size = 0x800 bytes.
		// 0x10
		char *unknown_10; // Array size = 0x800 bytes.
		char *unknown_14; // Array size = 0x800 bytes.
		long int totalPlayerCount; // Including gaia. It is also the size (number of dwords) of unknown_00 array.
		long int unknown_1C_size; // unknown_1C_size * 8 = sizeof(unknown_00[x] array)
		// 0x20
		char *sqrtTable_distanceFromDiffXY; // ptr to array of 0x10*0x10 bytes. to get a distance for diffX/Y: array[diffX*0x10 + diffY]. X,Y < 0x10.
		// Returns distance(int, int) as an integer result. Returns -1 if failed or result>0x10. Works only for x,y <= 15 (0x0F)
		char GetIntDistance(char x, char y) {
			if ((x < 0) || (x >= 16) || (y < 0) || (y >= 16)) {
				return -1;
			}
			return this->sqrtTable_distanceFromDiffXY[x * 0x10 + y];
		}
	};

	// Size 0x14. Constructor = 0x516DA0
	// Stores explored resource units (other than animal and farms) ?
	class STRUCT_PLAYER_UNKNOWN_118 {
	public:
		STRUCT_NEARBY_UNIT_INFO **arrayNearbyUnitInfos;
		long int *arrayArraySizeInNearbyUnitInfos; // +04. elem[i] represents array size of arrayNearbyUnitInfos[i]
		long int *arrayUsedElemInNearbyUnitInfos; // +08. elem[i] represents the used slot count in arrayNearbyUnitInfos[i]
		STRUCT_PLAYER *player; // Parent player
		long int arraySize; // +10. Number of (both used/alloc) elements in each array. Cf PLAYER_GATHERABLE_RESOURCE_CATEGORIES.
	};
	static_assert(sizeof(STRUCT_PLAYER_UNKNOWN_118) == 0x14, "STRUCT_PLAYER_UNKNOWN_118 size");


	// Used in path finding algorithm (included in path finding struct)
	class STRUCT_PATH_FINDING_UNKNOWN_POS_INFO {
	public:
		char posY;
		char posX;
		char unknown_2; // Maybe unused
		char unknown_3; // Maybe unused
		long int distance;
	};

	// Included structure in addresses 0x583BC8 and 0x6A1CC0
	// Constuctor 00458570
	// This structure is for path finding.
	// Note: it seems map is split into tiles (1 unit of position = 1 tile) but tiles are split into 4 "quarters".
	// In this struct, arrays indices are "quarter tiles". (use posX*4 for example).
	// Arrays value are binary (1 bit of one elementary position), which adds more precision under tile/quarter tile level ?
	class STRUCT_UNKNOWN_MAP_DATA_F04C {
	public:
		unsigned long int checksum; // F0 4C 54 00
		long int mapArraySizeY;
		long int mapArraySizeX;
		char unknown_00000C[0xFF * 0xFF]; // Access to data: 0xFF*y + x ?
		long int unknown_00FE10_dist16[0xFF * 0xFF]; // +FE10. Array of distance*16 values (?). index=y*0xFF+x. Each tile's distance to src??
		//char unknown_00FE10[0x4F614 - 0xFE10]; // size ?
		STRUCT_PATH_FINDING_UNKNOWN_POS_INFO unknown_4F614[0xFF * 0xFF]; // +4F614. Size 7F008. NOT indexed by coordinates ? Sort of list? Steps for short path finding??
		unsigned long int unknown_0CE61C;
		// 0x4F620
		unsigned long int unknown_0CE620;
		// 458870: set "has unit" at position?
		long int unknown_0CE624_mapUnitOccupiedGrid[0xFF * 0xFF]; // index posY*4*0xFF+x. Its value is a bits mask ??
		// 0x10DE28 ?
		char unknown_010DE28[0x11DC2C - 0x10DE28];
		long int unknown_011DC2C_index; // an index to access +4F614 + index*8 (byte)?
		char unknown_011DC30_posY; // current step posY ?
		char unknown_011DC31_posX; // current step posX ?
		char unknown_011DC32; // ?
		char unknown_011DC33; // ?
		long int unknown_11DC34_distanceOffset; // a distance*16 value. Current step dist*16 ? or best dist*16 ?
		long int unknown_11DC38_counter; // a counter
		unsigned long int uknown_11DC3C_ptr; // ptr to F4 99 54 00
		// 0x11DC40
		STRUCT_GAME_GLOBAL *globalStruct; // +11DC40: in old debugging times, was used as a global var to "game global", was not exactly the case.
		STRUCT_UNIT *currentUnit; // 11DC44.
		unsigned long int unknown_11DC48;
		long int destinationUnitId; // +11DC4C.
		// 0x11DC50
		float *pCurrentTerrainRestriction;
		float unknown_11DC54; // sizeRadiusY ?
		float unknown_11DC58; // sizeRadiusX ?
		long int unknown_11DC5C; // a terrainID ??? or -1 ?
		// 0x11DC60
		long int unknown_11DC60; // a terrainID ??? or -1 ?
		char unknown_11DC64[0x11DC7C - 0x11DC64];
		long int srcPosY; // +11DC7C
		// 0x11DC80
		long int srcPosX; // +11DC80
		long int destMinPosY_offset; // +11DC84. (destPosY - sizeRadius)*4 ?
		long int destMinPosX_offset; // +11DC88. (destPosX - sizeRadius2)*4 ?
		long int destMaxPosY_offset; // +11DC8C. (destPosY + sizeRadius)*4 ? then modified...
		long int destMaxPosX_offset; // +11DC90. (destPosX + sizeRadius)*4 then modified...
		long int actorMinPosY_offset; // +11DC94. (actorPosY - sizeRadius)*4 ?
		long int actorMinPosX_offset; // +11DC98. (actorPosX - sizeRadius)*4 ?
		long int actorMaxPosY_offset; // +11DC9C. (actorPosY + sizeRadius)*4 ?
		long int actorMaxPosX_offset; // +11DCA0. (actorPosX + sizeRadius)*4 ?
		char unknown_11DCA4[0x11DCC8 - 0x11DCA4];
		long int unknown_11DCC8_distance_offset; // Init=-1. Distance*4 between ? Total distance*4 (in straight line) between src and dest ?
		char unknown_11DCCC[0x11DCD8 - 0x11DCCC];
		char unknown_11DCD8_resetCounter; // A counter, +=8 at each pathFinding. When 0xF0 (30 iterations), reset and resets mapData. Value is similar to +C array
		char unknown_11DCD9[3];
		long int unknown_11DCDC; // arg15 of 00458930 call
		// 0x11DCE0
		long int unknown_11DCE0; // arg14 of 00458930 call
		long int unknown_11DCE4;
		long int unknown_11DCE8;
		long int unknown_11DCEC;
		// 0x11DCF0
		long int unknown_11DCF0;
		long int unknown_11DCF4; // seen 1
		// ...
		bool IsCheckSumValid() { return this->checksum == 0x00544CF0; }
	};


	static const unsigned char CST_MAP_BUILD_LIKE_DISABLED = (unsigned char)0xFF;
	static const unsigned char CST_MAP_BUILD_LIKE_MAX_VALUE = (unsigned char)CST_MAP_BUILD_LIKE_DISABLED - 1;
	// Included in 0x7C04A0 address.
	// Size= 0x28 ?
	// Describes a temp structure that stores information for currently-AI-managed player's build position priorities (for a given building to be constructed).
	// Note about coordinates: (posX,posY)=(i,j) is the tile where i<x<i+1 and i<y<i+1
	// So coordinates go from 0 to ("size"-1). ("grid" goes from 0 to "size").
	class STRUCT_TEMP_MAP_BUILD_LIKE_INFOS {
	public:
		unsigned long int checksum; // C8 43 54 00
		long int mapArraySizeY;
		long int mapArraySizeX;
		long int unknown_0C_toDecreaseToPosY;
		// 0x10
		long int unknown_10_toDecreaseToPosX;
		// mapLikeValuesMemoryZone[mapArraySizeX * (x) + (y)] should be the same as ptrRowsPtr[X][Y]. Strange conception.
		// Please use supplied methods if possible.
		unsigned char *mapLikeValuesMemoryZone; // Pointer to the beginning of map values memory zone. But the game accesses it via ptrRowsPtr (that gives pointers to each row (X))
		unsigned char **ptrRowsPtr; // The list of pointers to each row (X dimension) in mapLikeValues. ptrRowsPtr[X] is a pointer to the row X.
		long int unknown_1C;
		// 0x20
		long int unknown_matchCount;
		unsigned char maxLikeValue; // 0xFE generally (always ?). FF is a NO, 0->0xFE are "like values"
		char unknown_25[3];
		// to be continued... unknowns. Size ?

		bool IsCheckSumValid() { return this->checksum == 0x005443C8; }

		// Easy-to-use methods
		bool IsPositionValid(long int posX, long int posY) {
			return ((posX >= 0) && (posX < this->mapArraySizeX) &&
				(posY >= 0) && (posY < this->mapArraySizeY)
				);
		};
		bool FixPositionToGetInMapBounds(long int *posX, long int *posY) {
			if (*posX < 0) { *posX = 0; }
			if (*posY < 0) { *posY = 0; }
			if (*posX >= this->mapArraySizeX) { *posX = this->mapArraySizeX - 1; }
			if (*posY >= this->mapArraySizeX) { *posY = this->mapArraySizeX - 1; }
		}
		// Will not fail/crash if position is out of bounds
		unsigned char GetMapLikeValue(long int posX, long int posY) {
			return this->IsPositionValid(posX, posY) ? ptrRowsPtr[posX][posY] : CST_MAP_BUILD_LIKE_DISABLED;
		}
		// Set a tile like value
		// Will not fail/crash if position is out of bounds
		void SetMapLikeValue(long int posX, long int posY, long int value) {
			if (this->IsPositionValid(posX, posY)) { ptrRowsPtr[posX][posY] = (unsigned char)value; }
		}
		// Increments a tile like value with provided value (can be <0). Preserves disabled ones (0xFF)
		// Will not fail/crash if position is out of bounds
		void AddToMapLikeValue(long int posX, long int posY, long int relativeValue) {
			if (!this->IsPositionValid(posX, posY) || (ptrRowsPtr[posX][posY] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return;
			}
			long int v = ptrRowsPtr[posX][posY]; // long int to avoid overflows
			v += relativeValue;
			if (v > CST_MAP_BUILD_LIKE_MAX_VALUE) { v = CST_MAP_BUILD_LIKE_MAX_VALUE; }
			if (v < 0) { v = 0; }
			ptrRowsPtr[posX][posY] = (unsigned char)v;
			
		}
		// Set a tile like value but preserves disabled ones (0xFF)
		// Will not fail/crash if position is out of bounds.
		void SetMapLikeValueIfNotFF(long int posX, long int posY, long int value) {
			if (this->IsPositionValid(posX, posY) && (ptrRowsPtr[posX][posY] != CST_MAP_BUILD_LIKE_DISABLED)) {
				ptrRowsPtr[posX][posY] = (unsigned char)value;
			}
		}

		// Returns true if 1 of the line-end is disabled.
		// On each side, if one of the last 2 tiles is disabled, then the result is true
		bool IsAnExtremityBlocked_yLine(long int posX, long int posY_low, long int posY_high) {
			// Checks on values
			if (posY_low > posY_high) { return true; }
			if (posX < 0) { posX = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX >= this->mapArraySizeX) { posX = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			unsigned char *currentRow = this->ptrRowsPtr[posX];
			if (posY_low >= mapArraySizeY - 1) { return true; }
			if ((currentRow[posY_low] == CST_MAP_BUILD_LIKE_DISABLED) || (currentRow[posY_low + 1] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return true;
			}
			if (posY_high <= 0) { return true; }
			if ((currentRow[posY_high] == CST_MAP_BUILD_LIKE_DISABLED) || (currentRow[posY_high - 1] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return true;
			}
			return false;
		}

		// Returns true if 1 of the line-end is disabled.
		// On each side, if one of the last 2 tiles is disabled, then the result is true
		bool IsAnExtremityBlocked_xLine(long int posX_low, long int posX_high, long int posY) {
			// Checks on values
			if (posX_low > posX_high) { return true; }
			if (posY < 0) { posY = 0; }
			if (posX_low < 0) { posX_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY >= this->mapArraySizeY) { posY = mapArraySizeY - 1; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			unsigned char *currentRow = this->ptrRowsPtr[posX_low];
			if (this->ptrRowsPtr[posX_low][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (posX_low >= mapArraySizeX - 1) { return true; }
			if (this->ptrRowsPtr[posX_low + 1][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (this->ptrRowsPtr[posX_high][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (posX_high <= 0) { return true; }
			if (this->ptrRowsPtr[posX_high - 1][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			return false;
		}

		bool IsRegionAllDisabled(long int posX_low, long int posY_low, long int posX_high, long int posY_high) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return true; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrRowsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					if (currentRow[j] != CST_MAP_BUILD_LIKE_DISABLED) {
						return false;
					}
				}
			}
			return true;
		}

		// Set tile like value for all tiles in a rectangle (borders included)
		void SetMapLikeValueRectangle(long int posX_low, long int posY_low, long int posX_high, long int posY_high, long int value, bool preserveDisabled = false) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			// Treatments
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrRowsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					if (!preserveDisabled || (currentRow[j] != CST_MAP_BUILD_LIKE_DISABLED)) {
						currentRow[j] = (unsigned char)value;
					}
				}
			}
		}

		// Add like value for all tiles in a rectangle (borders included). Value can be negative.
		// This does NOT modify disabled tiles (0xFF)
		void AddMapLikeValueRectangle(long int posX_low, long int posY_low, long int posX_high, long int posY_high, long int value) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			// Treatments
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrRowsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					long int tmpvalue = currentRow[j];
					if (tmpvalue != CST_MAP_BUILD_LIKE_DISABLED) {
						tmpvalue += value; // Note: value can be <0
						if (tmpvalue > CST_MAP_BUILD_LIKE_MAX_VALUE) { tmpvalue = CST_MAP_BUILD_LIKE_MAX_VALUE; }
						if (tmpvalue < 0) { tmpvalue = 0; }
						currentRow[j] = (unsigned char)tmpvalue;
					}
				}
			}
		}

		// For each of the 4 sides of a building:
		// If the side is blocked (all neighbour tiles are disabled, side excluding corners),
		// then disable opposite side neighbour tiles (including corners)
		void ProtectBuildingOppositeSides(int unitPosX, int unitPosY, float unitRadius_f, char valueToUse = AOE_STRUCTURES::CST_MAP_BUILD_LIKE_DISABLED) {
			int unitRadius = (int)unitRadius_f;
			int unitRadiusDecreaseForHighPositions = 1 -((int)(unitRadius * 2) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
			// Get borders of the building (last tile part of the building) for simpler calculations
			int xLeftSide = unitPosX - unitRadius;
			int xRightSide = unitPosX + unitRadius - unitRadiusDecreaseForHighPositions;
			int yBottomSide = unitPosY - unitRadius;
			int yTopSide = unitPosY + unitRadius - unitRadiusDecreaseForHighPositions;
			int includeCornersInDetection = 1; // Set to 0 to ignore diagonal-corner tile, 1 to take it into account.
			//assert((includeCornersInDetection == 0) || (includeCornersInDetection == 1));

			// Is left blocked ?
			//bool isDisabled = this->IsRegionAllDisabled(xLeftSide - 1, yBottomSide, xLeftSide - 1, yTopSide);
			bool isDisabled = this->IsAnExtremityBlocked_yLine(xLeftSide - 1, yBottomSide - includeCornersInDetection, yTopSide + includeCornersInDetection);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xRightSide + 1, yBottomSide - 1, xRightSide + 1, yTopSide + 1, valueToUse, true);
			}
			// Is Right blocked ?
			isDisabled = this->IsAnExtremityBlocked_yLine(xRightSide + 1, yBottomSide - includeCornersInDetection, yTopSide + includeCornersInDetection);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yBottomSide - 1, xLeftSide - 1, yTopSide + 1, valueToUse, true);
			}
			// Is bottom blocked ?
			isDisabled = this->IsAnExtremityBlocked_xLine(xLeftSide - includeCornersInDetection, xRightSide + includeCornersInDetection, yBottomSide - 1);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yTopSide + 1, xRightSide + 1, yTopSide + 1, valueToUse, true);
			}
			// Is top blocked ?
			isDisabled = this->IsAnExtremityBlocked_xLine(xLeftSide - includeCornersInDetection, xRightSide + includeCornersInDetection, yTopSide + 1);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yBottomSide - 1, xRightSide + 1, yBottomSide - 1, valueToUse, true);
			}

			// If a corner tile if disabled, do we disable diagonal-opposed corner tile ?
		}
	};


	// Size = 0x0C. Constructor=0x4E9C60
	class STRUCT_RESEARCH_DEF_INFO {
	public:
		STRUCT_RESEARCH_DEF *researchDefArray;
		short int researchCount;
		short int unknown_06; // unused ?
		STRUCT_GAME_GLOBAL *ptrGlobalStruct;
	};

	// Size = 08
	class STRUCT_PLAYER_RESEARCH_STATUS {
	public:
		float researchingTime; // if >= researchDef.researchTime, then it's completed ! This is not =, but >=
		AOE_CONST_FUNC::RESEARCH_STATUSES currentStatus; // 2-bytes.
		short int unknown_06; // unused (probably ;)
	};

	// Size=0x10. Constructor=4EA91C
	class STRUCT_PLAYER_RESEARCH_INFO {
	public:
		STRUCT_PLAYER_RESEARCH_STATUS *researchStatusesArray;
		short int researchCount;
		short int unknown_06; // unused ?
		STRUCT_RESEARCH_DEF_INFO *ptrResearchDefInfo;
		STRUCT_PLAYER *ptrPlayer; // +C. Back pointer to player.
	};


	// This structure does not have a checksum. It is referenced by 0x580DA8
	// It is probably this structure that collects all 'common' information to be multiplayer-compatible (player names, etc).
	// pointed by GameSettings+18C ?
	// Size=0x1774
	class STRUCT_MP_COMMUNICATION {
	public:
		unsigned long int unknown_0000;
		unsigned long int unknown_0004; // ptr to float?
		unsigned long int unknown_0008;
		unsigned long int unknown_000C;
		// 0x10
		unsigned long int unknown_0010;
		unsigned long int unknown_0014;
		unsigned long int *unknown_0018_ptr; // Ptr to ? Player types ?
		unsigned long int unknown_001C; // type ?
		// 0x20
		unsigned long int unknown_0020; // Dword
		char unknown_0024[0x9D0- 0x24];
		unsigned long int unknown_09D0[8];
		char unknown_09F0[0x1000 - 0x9F0];
		// 0x1000
		// +10E0= a playerId for MP ? "localControlPlayerId ?"
		char unknown_1000[0x1100 - 0x1000];
		// +1179= byte
		char unknown_1100[0x12A0 - 0x1100];
		unsigned long int unknown_12A0;
		unsigned long int unknown_12A4;
		unsigned long int unknown_12A8;
		unsigned long int unknown_12AC;
		// 0x12B0
		unsigned long int unknown_12B0[4];
		// 0x12C0
		unsigned long int unknown_12C0;
		unsigned long int unknown_12C4;
		char_array_128 *playerNames; // +12C8. Player names, each string size=0x80
		char_array_128 *unknownNames; // +12CC. xxx names, each string size=0x80
		// 0x12D0
		unsigned long int unknown_12D0;
		AOE_CONST_INTERNAL::COMM_LINK_LEVEL commLinkLevel; // +12D4. "Link Level".
		short int maximumPlayerCount; // +12D8. Is 8
		short int unknown_12DA;
		//char unknown_12DC[0x13C0 - 0x12DC];
		char unknown_12DC[0x136C - 0x12DC];
		unsigned long int unknown_136C[10]; // +136C. 1 per (COMM) playerId (0 and 9 unused). TO VERIFY
		char unknown_1394[0x13C0 - 0x1394];
		unsigned long int unknown_13C0[10]; // +13C0. 1 per (COMM) playerId (0 and 9 unused)
		char unknown_13E8[0x1484 - 0x13E8];
		unsigned long int unknown_1484;
		unsigned long int unknown_1488;
		unsigned long int unknown_148C;
		unsigned long int unknown_1490;
		unsigned long int unknown_1494;
		unsigned long int unknown_1498;
		char unknown_149C[0x14C4 - 0x149C];
		long int *unknown_14C4; // Related to players communication. +114=ptr (player queue)?
		char unknown_14C8[0x14D4 - 0x14C8];
		unsigned long int *unknown_14D4_ptr; // Ptr to struct size=0x240, constructor=0x428990.
		unsigned long int *unknown_14D8_ptr; // Ptr to struct size=0x7EC, constructor=0x429330. +0=backptr
		char unknown_14DC[0x14E0 - 0x14DC];
		long int *unknown_14E0; // Related to players communication
		char unknown_14E4[0x1528 - 0x14E4];
		unsigned long int unknown_1528[10]; // +1528. 1 per (COMM) playerId (0 and 9 unused)
		char unknown_1550;
		char unknown_1551[3];
		unsigned long int unknown_1554;
		unsigned long int unknown_1558[3]; // ?
		unsigned long int unknown_1564[10]; // +1564. 1 per (COMM) playerId (0 and 9 unused). Some DPlay info ?
		unsigned long int unknown_158C;
		// 0x1590
		char unknown_1590;
		char unknown_1591[3]; // Probably unused
		long int unknown_1594_isComputerControlled[10]; // 1 per (COMM) playerId (0 and 9 unused). 1 if AI ? Or playerId of host, or smthing else ?
		unsigned long int unknown_15BC[10]; // +15BC. 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_15E4[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_160C[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_1634[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_165C[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_1684[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_16AC[10]; // +16AC. 1 per (COMM) playerId (0 and 9 unused)
		AOE_CONST_INTERNAL::GAME_PLAYER_TYPES playerTypes[10]; // 0x16D4.  1 per (COMM) playerId. Values: 0=gaia/other, 2=human, 4=computer
		char unknown_16FC[10];
		char unknown_1706[2]; // unused ?
		// 0x1700
		char unknown_1708[0x1718 - 0x1708];
		unsigned long int unknown_1718;
		unsigned long int unknown_171C; // ?
		unsigned long int unknown_1720[10]; // +1720. 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_1748; // ?
		unsigned long int unknown_174C[10]; // +174C ?. 1 per (COMM) playerId (0 and 9 unused)
	};
	static_assert(sizeof(STRUCT_MP_COMMUNICATION) == 0x1774, "STRUCT_UNKNOWN_P580DA8 Size");

	// Size: seems to be 0x40
	class STRUCT_SOUND_DATA_ELEMENT {
		// Object info initially provided by LucTieuPhung
	public:
		long int pSound;
		unsigned long int unknown_04;
		unsigned short int unknown_08;
		char filename[0x0C];
		unsigned long int unknown_18;
		long int lVolume; // +1C
		// 0x20
		long int dwFrequency;
		long int lPan;
		unsigned long int unknown_28;
		unsigned long int unknown_2C;
		// 0x30
		unsigned long int unknown_30;
		unsigned long int pDirectSoundBuffer; // IDirectSoundBuffer*
		unsigned long int unknown_38;
		unsigned long int unknown_3C;
	};
	static_assert(sizeof(STRUCT_SOUND_DATA_ELEMENT) == 0x40, "STRUCT_SOUND_DATA_ELEMENT Size");

	// Size 0x69C
	class STRUCT_MAIN_SOUND {
		// Object info initially provided by LucTieuPhung
	public:
		char isDSoundInitialized;
		char unknown_001; // Related to ListSoundData
		short int unknown_002;
		long int hWndToNotify;
		unsigned long int lpDirectSound; // +08. Real type=IDirectSound* (See DirectSoundCreate)
		unsigned long int lpPrimaryDsb; // +0C. Real type=IDirectSoundBuffer*
		// 0x10
		long int lastErrorCode; // Return value of mixerSetControlDetails, etc.
		long int tempSoundVolume; // -10000 to 0
		long int soundVolume; // -10000 to 0 (Registry)
		char soundFolder[0x104];
		// 0x120
		long int hasSoundVolumeCtrl; // Sound Volume Control Found Successfully
		unsigned long int hMixer; // Real type=HMIXER ?
		// 0x128 - MIXERLINE: 0xA8 bytes
		long int ml_cbStruct; // =sizeof(MIXERLINE)
		unsigned long int unknown_12C;
		unsigned long int unknown_130;
		long int ml_dwLineID;
		unsigned long int unknown_138;
		unsigned long int unknown_13C;
		// 0x140
		long int ml_dwComponentType; // = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
		char unknown_144[0x1D0 - 0x144];
		// 0x1D0 - MIXERLINECONTROLS: 0x18 bytes
		long int mlc_cbStruct; // = sizeof(MIXERLINECONTROLS)
		long int mlc_dwLineID; // = ml.dwLineID
		long int mlc_dwControlType; // = MIXERCONTROL_CONTROLTYPE_VOLUME;
		long int mlc_cControls; // = 1 ?
		// 0x1E0
		long int mlc_cbmxctrl; // = sizeof(MIXERCONTROL);
		long int mlc_pamxctrl; // = &mc
		// 0x1E8 - MIXERCONTROL: 0x94 bytes
		long int mc_cbStruct;
		long int mc_dwControlID;
		// 0x1F0
		long int mc_dwControlType;
		long int mc_fdwControl;
		long int mc_cMultipleItems;
		long int mc_szShortName[4]; // +1FC
		long int szName[0x10]; // +20C
		long int mc_dwMinimum; // +24C. =0 ?
		// 0x250
		long int mc_dwMaximum; // =65535 ?
		char unknown_254[0x27C - 0x254];
		// 0x27C - MIXERCONTROLDETAILS: 0x18 bytes
		long int mcd_cbStruct; // = sizeof(MIXERCONTROLDETAILS);
		// 0x280
		long int mcd_dwControlID; // = mc.dwControlID;
		long int mcd_cChannels;
		unsigned long int unknown_288;
		long int mcd_cbDetails; // +28C. = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		// 0x290
		long int mcd_paDetails; // = &mcdu;
		// 0x294 - MIXERCONTROLDETAILS_UNSIGNED:
		long int mcdu_dwValue;
		STRUCT_SOUND_DATA_ELEMENT soundDataArray[0x10];
		long int soundDataCount; // Number of used elements in soundDataArray ?
	};
	static_assert(sizeof(STRUCT_MAIN_SOUND) == 0x69C, "STRUCT_MAIN_SOUND Size");

	// Size = 0x3F8
	class STRUCT_MAIN_MUSIC {
		// Object info initially provided by LucTieuPhung
	public:
		char soundPath[0x104];
		char musicFormat; // AudioCD=1, MIDI=2 (WAV=3)
		char unknown_105;
		char unknown_106;
		char unknown_107;
		STRUCT_MAIN_SOUND *pSound; // +108
		long int hInstance; // = GetModuleHandle(NULL): Module handle to the file
		// 0x110
		long int hWnd; // The window to send MM_MCINOTIFY messages to.
		long int isTracksInitialized; // //Music Tracks Initialized? Music is playing?
		long int wDeviceID; // deviceID for mciSendCommand (Play, Stop, Close, etc.)
		// 0x11C - MCI_PLAY_PARMS: 0x0C bytes
		long int play_dwCallback;
		// 0x120
		long int play_dwFrom;
		long int play_dwTo;
		char isPlaying; // is music playback in progress?
		char musicFileName[0x230 - 0x129]; // "xopen.mp3", "xwon.mp3", "xlost.mp3", ""...
		// 0x230
		long int minMusicTrack;
		long int maxMusicTrack;
		unsigned long int unknown_238;
		long int seekPos; // to resume playback
		// 0x240
		long int isLooping; // to enable/disable playback loop. 0 for xopen, xwon, xlost.
		long int currentMusicTrack; // Current music number being played.
		long int nMusicTracks; // The number of xmusic#.mid or AudioCD tracks found.
		long int nMusicPlaying; // a counter. +1 for succesful MCI_PLAY. -1 after completing a playback.
		// 0x250 - music volume
		long int tempMusicVolume; // To indicate if the volume need to be set. 1 = Music Volume does not need to be set. -10000 to 0 = Music volume to be set.
		long int musicVolume; // registry value = -musicVolume
		long int isPaused; // Pause Music related
		unsigned long int unknown_25C; // related to Pause Music
		// 0x260
		unsigned long int unknown_260; // related to Pause Music
		unsigned long int unknown_264;
		unsigned long int unknown_268;
		long int isMusicVolumeStored; // indicates if music volume is stored
		// 0x270
		long int currentMusicVolume; // ?
		long int lastMusicVolumeStorageTime; // a timeGetTime value
		long int hasMusicVolumeCtrl; // 0/1 flag
		long int hMixer; // to change volume of an audio line.
		// 0x280 - MIXERLINE: 0xA8 bytes
		long int ml_cbStruct; // = sizeof(MIXERLINE);
		long int ml_dwDestination;
		long int ml_dwSource;
		long int ml_dwLineID;
		// 0x290
		unsigned long int unknown_290;
		unsigned long int unknown_294;
		unsigned long int unknown_29ml_dwComponentType;
		unsigned long int unknown_29C;
		// 0x2A0
		char unknown_2A0[0x328 - 0x2A0];
		// 0x328 - MIXERLINECONTROLS: 0x18 bytes
		long int mlc_cbStruct; // = sizeof(MIXERLINECONTROLS);
		long int mlc_dwLineID; // = ml.dwLineID;
		// 0x330
		long int mlc_dwControlType; // = MIXERCONTROL_CONTROLTYPE_VOLUME;
		long int mlc_cControls;
		long int mlc_cbmxctrl; // = sizeof(MIXERCONTROL);
		long int mlc_pamxctrl; // = &mc;
		// 0x340 - MIXERCONTROL:0x94 bytes
		//long int mc_cbStruct
		long int mc_cbStruct;
		long int mc_dwControlID;
		long int mc_dwControlType;
		long int mc_fdwControl;
		// 0x350
		long int mc_cMultipleItems;
		long int mc_szShortName[4]; // +354
		long int szName[0x10]; // +364
		long int mc_dwMinimum; // +3A4. =0 ?
		long int mc_dwMaximum; // +3A8. =65535 ?
		char unknown_3AC[0x3D4 - 0x3AC];
		// 0x3D4 - MIXERCONTROLDETAILS: 0x18 bytes
		long int mcd_cbStruct; // = sizeof(MIXERCONTROLDETAILS);
		long int mcd_dwControlID; // = mc.dwControlID;
		long int mcd_cChannels;
		// 0x3E0
		unsigned long int unknown_3E0;
		long int mcd_cbDetails; // +3E4. = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		long int mcd_paDetails; // = &mcdu;
		// 0x3EC - MIXERCONTROLDETAILS_UNSIGNED:
		long int mcdu_dwValue;
		unsigned long int unknown_3F0;
		unsigned long int unknown_3F4;
	};
	static_assert(sizeof(STRUCT_MAIN_MUSIC) == 0x3F8, "STRUCT_MAIN_MUSIC Size");


	// Size = 0x514C. Constructor=0x0506900
	// Warning: all arrays[0x10] are indexed by playerId-1, which means tribeNames[0] is player1's name.
	class STRUCT_SCENARIO_INFO {
	public:
		unsigned long int checksum; // C8 A3 54 00 ; parent struct = 3C 5E 54 00
		STRUCT_UNKNOWN_GAME_TIME *timeInfo; // ptr to 2C 99 54 00
		STRUCT_GAME_GLOBAL *globalStruct;
		char unknown_000C_generalVictory_conquest; // Init = 1 (default)
		char tribeNames[0x10][0x100]; // +D. Index=(playerId-1)
		char unknown_100E; // unused?
		char unknown_100F; // unused?
		// 0x10
		char *scenarioFileName; // +1010
		char *scenarioInstructions; // +1014
		char *scenarioTips;
		char *scenarioVictoryText;
		// 0x1020
		char *scenarioDefeatedText;
		char *scenarioHistoryText;
		//char unknown_1028[0x12C8 - 0x1028]; // 475C55
		char fullPath_tmpBuildList[0xE0];
		char unknown_1108[0x12C8 - 0x1108];
		char *strategyFileNames[0x10]; // +12C8. Index=(playerId-1). Size = 0xE0.
		char *cityPlanFileNames[0x10]; // +1308. Index=(playerId-1). Size = 0xE0.
		char *personalityFileNames[0x10]; // +1348. Index=(playerId-1). Size = 0xE0.
		char *strategyFileContent[0x10]; // +1388. pointer to AI file content. See in 408948 (read from a temp file, not from THIS). Is size limited to ~32ko ? Not sure
		char *cityPlanFileContent[0x10]; // +13C8. Pointer to an array of bytes.
		char *personalityFileContent[0x10]; // +1408. Pointer to an array of bytes. Note: The "END" keyword actually stops reading the file.
		long int strategyFileSize[0x10]; // +1448.
		long int cityPlanFileSize[0x10]; // +1488.
		long int personalityFileSize[0x10]; // +14C8.
		char scenarioBeginningAnimationFileName[0xE0]; // +1508
		char scenarioVictoryAnimationFileName[0xE0]; // +15E8
		char scenarioFailureAnimationFileName[0xE0]; // +16C8
		char scenarioBitmapFileName[0xE0]; // +17A8
		float scenarioVersion; // +1888. Corresponds to .scn/.scx version, like 1.02, 1.03, 1.04, 1.05, 1.13, 1.15(=ROR 1.0a)
		long int player_unknown_188C[0x10]; // +188C. Often referred to as +18CC-40. Default = 4 ?
		long int playerIsHumanPlayable[0x10]; // +188C. 0 if computer player only (cannot be played as human)
		long int playerCivilizationId[0x10]; // +18CC. Often referred to as +18CC+40.
		long int playerIsActive[0x10]; // +194C. 1 if player is active. Often referred to as +18CC+80.
		long int *unknown_198C; // +198C. ptr to struct size=0x20, constructor 45AF60. TO DO.
		long int startingResources[0x10][4]; // +1990. WARNING: resource order = gold/wood/food/stone here !!!
		// 0x1A90 - probably a sub-structure of size=0x3160 (last 4 bytes are unused) for victory conditions settings.
		// Those generalVictory_* are only relevant if generalVictory_mainCondition==4 (custom)
		long int generalVictory_conquest; // 1 = Standard, Conquest, Score, Time Limit, Custom if Conquest is enabled. ; 0=others
		long int generalVictory_ruinsCount; // Number of ruins to collect to win the game (for all players)
		long int generalVictory_relicsCount; // Number of relics to collect to win the game (for all players)
		long int generalVictory_discoveriesCount; // Number of discoveries to find to win the game (for all players)
		// 0x1AA0
		long int generalVictory_explorationAmount; // % of exploration to reach to win the game (for all players)
		long int generalVictory_goldAmount; // Amount of gold to collect to win the game (for all players). Hidden in scenario editor (value is never modified by editor).
		char unknown_1AA8_individual_victory_conditions[0x10][0x2D0]; // Internal structure to define
		long int diplomacySettings[0x10][0x10]; // +47A8. Refers to PLAYER_DIPLOMACY_STANCES but as DWORDs. 0=allied,1=neutral,3=enemy.
		long int hasAlliedVictory[0x10]; // +4BA8. 1=option is checked for that player
		long int artefactsRequireAll; // +4BE8. 0="at least one", 1="all". Applies to relics, ruins, discoveries ?
		unsigned long int unknown_4BEC; // Unused, it seems.
		// 0x4BF0
		unsigned long int unknown_4BF0;
		long int disableTechnologyFlags[0x10][AOE_CONST_INTERNAL::SC_ED_DISABLE_TECHNOLOGY_INDICES::CST_DTI_COUNT]; // +4BF4. Total Size =0x500 bytes. Used in 0x507E50=scenarioInfo.applyDisabledResearches(player)
		long int enableLengthenCombatMode; // +50F4. A flag 0/1. If 1, lengthen combat mode is ON (most units get *3 HP, it is technology 0x64)
		long int unknown_50F8; // +50F8 ? Unused ?
		long int fullTechTree; // +50FC
		// 0x5100
		long int playersStartingAge[0x10]; // 1=Tool, 4=post iron. Warning, this does not correspond to CST_AGE_TOOL, etc. Index is playerId-1.
		// 0x5140
		AOE_CONST_INTERNAL::GENERAL_VICTORY_CONDITION generalVictory_mainCondition; // 0=Standard, 1=Conquest, 2=Score, 3=Time Limit, 4=Custom
		long int generalVictory_scoreAmount; // Default 900
		long int generalVictory_timeAmount; // Default 9000 "years"

		bool IsCheckSumValid() { return this->checksum == 0x0054A3C8; }
	};


	// Size = ? - 04 99 54 00
	class STRUCT_GAME_COMMANDS_INFO {
	public:
		unsigned long int checksum;
		STRUCT_GAME_GLOBAL *global;
		unsigned long int currentCommand; // unsure
		long int currentCommandSize; // unsure
		// 0x10
		long int timestamp; // Value from timeGetTime
		unsigned long int *unknown_14; // Ptr to struct similar to [580DA8]

		bool IsCheckSumValid() { return this->checksum == 0x00549904; }
	};


#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_GLOBAL1 0x00554A58
#define CHECKSUM_GAME_GLOBAL2 0x00553230
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_GLOBAL1 0x005432AC
#define CHECKSUM_GAME_GLOBAL2 0x005436FC
#endif
#ifdef GAMEVERSION_ROR10b // cf 0x50C250
#define CHECKSUM_GAME_GLOBAL1 0x0054F990
#define CHECKSUM_GAME_GLOBAL2 0x0054FE58
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_GLOBAL1 0x0054A4F8
#define CHECKSUM_GAME_GLOBAL2 0x0054A954
#endif
	// Size = 0x124 - F8 A4 54 00. Parent=54 A9 54 00
	// Constructor 0x509AE0 (parent 51B140)
	class STRUCT_GAME_GLOBAL {
	public:
		unsigned long int checksum;
		unsigned long int currentGameTime; // in milliseconds
		unsigned long int unknown_008;
		unsigned long int unknown_00C;
		unsigned long int unknown_010;
		unsigned long int timeGetTimeValue;
		float gameSpeed;
		char gamePaused; // confirm exact role, do not use it directly.
		char gameRunStatus; // 0 = Game in progress. 1 = Game Over. 2 = Not ingame. (Default). 3 = Generate Map (when starting game).
		short int unknown_01E;
		// +0x20
		unsigned long int unknown_020;
		unsigned long int unknown_024;
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +28. Map related data. (F4 99 54 00)
		short int soundsArrayElemCount;
		short int unknown_02E; // maybe 2 bytes, not 1 word
		// +0x30
		unsigned long int ptrSoundsArray;
		unsigned long int unknown_034;
		unsigned long int ptrGraphicsArray; // ptr to list of elements (size=0x48) starting by a unit name?
		short int playerTotalCount; // Player count including gaia = real player count + 1
		short int unknown_03E;
		// +0x40
		STRUCT_PLAYER **ptrPlayerStructPtrTable; // points to table of player struct addresses (pointers)
		short int civCount; // +44. Number of civilizations.
		short int unknown_046;
		STRUCT_CIVILIZATION_DEF **civilizationDefinitions; // +48. A pointer to the array of pointers to civ definitions. Array size is civCount
		STRUCT_TECH_DEF_INFO *technologiesInfo; // +4C. 20 99 54 00
		// +0x50
		unsigned short int terrainRestrictionCount; // Size for ptrTerrainRestrictionArray array
		unsigned short int nbOfTerrainPerTerrainRestriction; // +52. How many terrain for each terrain restriction (common for all)
		float **ptrTerrainRestrictionArray; // +54. Array of pointers to terrain restriction info (if an array of float=0/1). Example 426951. ptrTerrainRestrictionArray[terrainRestriction][terrainId]=float : 0=inaccessible, othervalue=accessible=damagefactor
		STRUCT_GAME_COMMANDS_INFO *commandsInfo; // +58. 04 99 54 00.
		STRUCT_SCENARIO_INFO *scenarioInformation; // +5C.
		// +0x60
		unsigned short int unknown_060; // size for +64 ?
		unsigned short int unknown_062;
		unsigned long int unknown_064; // .col list ?
		long int seqUnitId; // Sequence for unit instance IDs (for creatable only ?)
		unsigned long int unknown_06C_unitIdSeqForTempUnits; // Unsure
		// +0x70
		long int forcedNextUnitId; // Unsure. Forced when loading a scenario.
		long int forceUseSpecifiedUnitId; // +74. If set (temporarily), use "+70" as next unitInstanceId instead of reading sequence.
		unsigned long int unknown_078; // random value for seed ?
		short int humanPlayerId; // +7C. PlayerId that is human controlled (controlledPlayerId)
		short int unknown_07E;
		// +0x80
		unsigned long int unknown_080; // list of unit data??
		float timerInterval;
		STRUCT_UNIT **ptrUnitPointersList; // +88. pointer to Array of unit struct pointers
		unsigned long int unknown_08C;
		// +0x90
		long int unitPointersListSize; // Size of the ptrUnitPointersList Array
		unsigned long int unknown_094;
		unsigned long int unknown_098;
		long int unknown_094_ListSize; // Size of x+0x94 array
		// +0xA0
		char unknown_0A0[0x10]; // A0: related to aoe.ply ?
		// +0xB0
		unsigned long int unknown_0B0_playerId; // currently managed playerid?
		unsigned long int unknownPlayerVar[0x09]; // 9 variables for each player, including gaia
		// +0xD8
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type10; // +D8. For unit type = eye candy
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type20; // +DC. For unit type = flag
		// +0xE0
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type30; // +E0. For unit type = killable ?
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type40; // +E4. For unit type = birds
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type50; // +E8. For unit type = type50
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type60; // +EC. For unit type = projectiles. 0x520BD0
		// +0xF0
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_doppleganger; // list of all dopplegangers ?
		unsigned long int unknown_0F4;
		long int tmp_allowedTimeForAITreatment; // +F8. For internal treatments (temporary, reset to 0 when finished). This is a time value allowed for AI treatments
		long int currentlyManagedAIPlayer; // +FC. A player id. "currentUpdateComputerPlayer"
		// +0x100
		unsigned long int unknown_100_difficultyLevel; // NOT always diff. level. See in game settings struct.
		STRUCT_RESEARCH_DEF_INFO *researchDefInfo; // +104.
		unsigned long int unknown_108;
		unsigned long int relicsCount;
		// +0x110
		unsigned long int ruinsCount;
		char unknown_114;
		char unknown_115[3]; // unused ?
		unsigned long int unknown_118;
		char WRONG_showScores; // 0x11C. WRONG ?
		char temporaryTargetPlayerId; // 11D. Used for AI players. "ConCP", CP=computerPlayer? Con=?concerned?
		short int unknown_11E;
		// 0x120
		long int unknown_120;		

		AOE_STRUCTURES::STRUCT_PLAYER ** GetPlayerStructPtrTable() const{
			return (AOE_STRUCTURES::STRUCT_PLAYER **) this->ptrPlayerStructPtrTable;
		}
		// Returns player structure corresponding to supplied ID. Returns NULL if not found
		AOE_STRUCTURES::STRUCT_PLAYER *GetPlayerStruct(int playerId) const {
			if (!this->IsCheckSumValid()) { return NULL; }
			if ((playerId < 0) || (playerId >= this->playerTotalCount) || (playerId >= 9)) { return NULL; }
			return this->ptrPlayerStructPtrTable[playerId];
		}
		// Get a technology definition, or NULL if not found
		AOE_STRUCTURES::STRUCT_TECH_DEF *GetTechDef(short int techDefId) const {
			if (!this->technologiesInfo || !this->technologiesInfo->IsCheckSumValid()) { return NULL; }
			return this->technologiesInfo->GetTechDef(techDefId);
		}
		// Returns the array of (float) values for demanded terrain restriction.
		// 0 = not accessible, >0 means accessible and is the value for armor damage multiplier
		// Returns NULL if restriction is invalid
		float *GetTerrainRestrictionValues(TERRAIN_RESTRICTION restriction) const {
			if ((restriction >= this->terrainRestrictionCount) || (restriction < 0)) { return NULL; }
			return this->ptrTerrainRestrictionArray[restriction];
		}
		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_GAME_GLOBAL1) || (this->checksum == CHECKSUM_GAME_GLOBAL2); }
	};


	// Size 0x10
	class STRUCT_PER_TYPE_UNIT_LIST_ELEMENT {
	public:
		STRUCT_UNIT *unit;
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *previousElement;
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *nextElement;
		unsigned long int unused_0C; // SEEMS to be unused
	};

	// Player's unit-per-type list
	// Size 0x0C
	class STRUCT_PER_TYPE_UNIT_LIST_LINK {
	public:
		unsigned long int checksum; // 70 9B 54 00 or D4 4A 54 00
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *lastListElement;
		short int listElemCount; // +08
		short int unused_0A;

		bool IsCheckSumValid() { return (this->checksum == 0x00549B70) || (this->checksum == 0x00544AD4); }
	};
	static_assert(sizeof(STRUCT_PER_TYPE_UNIT_LIST_LINK) == 0x0C, "STRUCT_PER_TYPE_UNIT_LIST_LINK size");

	// size = 0x20, to confirm
	class STRUCT_SCORE_ELEM {
	public:
		char unknown_00;
		char unknown_01;
		char unknown_02;
		AOE_CONST_FUNC::SCORE_CATEGORIES category;
		long int resourceId; // +04 ; RESOURCE_TYPES
		long int listCounter; // To confirm
		long int pointsPerScoreElement;
		// 0x10
		unsigned long int unknown_10;
		float value;
		STRUCT_SCORE_ELEM *next;
		unsigned long int unknown_1C; // unused ?
	};


	// Unknown size
	class STRUCT_SCORE_HEADER {
	public:
		unsigned long int checksum; // E4 A3 54 00
		unsigned long int unknown_04; // a pointer
		unsigned long int unknown_08; // a number ?
		STRUCT_PLAYER *player;
		// 0x10
		unsigned long int unknown_10;
		STRUCT_SCORE_ELEM *firstScoreElement;
		unsigned long int unknown_18;
		long int currentTotalScore;

		bool IsCheckSumValid() { return this->checksum == 0x0054A3E4; }
	};


	// Player's buildings unit list header (only buildings)
	// Size 0x1C
	class STRUCT_PLAYER_BUILDINGS_HEADER {
	public:
		STRUCT_PLAYER *player;
		unsigned long int allocatedArraySize; // this value * 4 = allocated size (in dwords) for the 4 lists below
		long int buildingsArrayElemCount;
		STRUCT_UNIT **ptrBuildingsArray; // Note: this list is always re-organized so there is not empty or obsolete element [0]=>[buildingsArrayElemCount-1]
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

	// Size = 0xB0. Added (created) in 4FC5A0.
	// Represents population info at one precise given moment of the game. (used to draw timeline graphic)
	// Referenced by timelineUI+518 (last elem?)
	class STRUCT_TIMELINE_COLUMN_INFO {
	public:
		long int unknown_00;
		long int unknown_04;
		long int unknown_08;
		long int unknown_0C;
		// 0x10
		char unknown_10;
		char unknown_11;
		char unknown_12; // default = 3 ?
		char unknown_13;
		long int totalPopulationCount;
		long int unknown_18;
		long int unknown_1C;
		long int unknown_20;
		long int unknown_24;
		long int unknown_28;
		long int unknown_2C;
		long int unknown_30;
		long int unknown_34;
		long int unknown_38;
		char totalPopulationString[0x70 - 0x3C]; // like "24"
		// 0x70
		long int unknown_70;
		char timeString[0xA8 - 0x74]; // +74. Like "0:00"
		long int unknown_A8; // timeIndex ?
		STRUCT_TIMELINE_COLUMN_INFO *previous;
	};
	static_assert(sizeof(STRUCT_TIMELINE_COLUMN_INFO) == 0xB0, "STRUCT_TIMELINE_COLUMN_INFO size");


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


	// PLAYER. Constructor=0x4EFB00
	// Standard player struct sizes are 0x85C (gaia) or 0x84C (non-gaia).
	// If selected units features is installed, size is increased to host selected unit pointers at the end of the structure.
	// Size will depend on how many maximum selected units it has been set.
	class STRUCT_PLAYER {
	public:
		unsigned long int checksum; // 0x00549B80 or 0x00549A44 or 0x00544D18 (parent class)
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
		STRUCT_DEF_UNIT **ptrStructDefUnitTable; // ptr to Array containing pointers to all player unitDefs (index=DATID). They can be NULL (not available/invalid). See also structDefUnitArraySize.
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
		AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES *ptrDiplomacyStances; // +84. [pointer+iPlayerId] = diplomacy value: 0=ally,1=neutral, 3=enemy
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
		char hasAlliedVictory;
		AOE_CONST_FUNC::CIVILIZATIONS civilizationId; // +109. 1 byte (char)
		short int unknown_10A; // unused ?
		unsigned long int ptrPlayerColorStruct; // quite unknown. +26=color(word)? +28=sub_struct
		// 0x110
		short int techTreeId; // 45BA12 : DWORD !
		short int unknown_112;
		STRUCT_PLAYER_UNKNOWN_118 *spottedResources; // +114. Contains info about resource units. Set in 0x4F1EF8. Related to +58 data. Not related to AI. Each time a tile is explored, underlying resources are added here.
		float screenPositionY; // axis southwest-northeast like /
		float screenPositionX; // axis northwest-southeast like \   [origin is left corner - max(X,Y) is right corner]
		// 0x120
		short int unknown_120_posY; // screen pos ?
		short int unknown_122_posX; // screen pos ?
		unsigned long int unknown_124;
		unsigned long int unknown_128; // split ; +0x129=?
		unsigned long int unknown_12C;
		// 0x130
		short int unknown_130;
		short int unknown_132;
		STRUCT_UNIT *selectedStructUnitTable[26]; // +0x134 is first selected unit's ptr. Last=First?. Only relevant if "selected units" features is NOT installed. See GetRelevantSelectedUnitsPointer(...).
		unsigned long int unknown_19C_mask; // a "per-player" mask. Seen 0 or 0x1FF
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
		char unknown_844[0x85C - 0x844];
		// From there, a lot of xx 00 00 00 (4 bytes) until +0x67C at least ? Maybe a lot more ?
		// 0x85C - DO NOT access this unless you checked the "selected units" feature is installed ! On standard game structure stops here.
		// See GetRelevantSelectedUnitsPointer(...)
		STRUCT_UNIT *custom_selectedUnits[CST_RS_MAX_SUPPORTED_SELECTED_UNITS]; // ONLY USE THIS if "selected units" feature is installed or you will access bad memory. See GetRelevantSelectedUnitsPointer (in commands)

		STRUCT_GAME_GLOBAL *GetGlobalStruct() const {
			return (STRUCT_GAME_GLOBAL*) this->ptrGlobalStruct;
		}
		// Obsolete getter
		STRUCT_AI * GetAIStruct() const {
			return (STRUCT_AI*) this->ptrAIStruct;
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
			if (!this->ptrResearchesStruct || !this->ptrResearchesStruct->ptrResearchDefInfo || !this->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray) {return NULL; }
			if ((researchId < 0) || (researchId >= this->ptrResearchesStruct->researchCount)) { return NULL; }
			return &this->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId];
		}
		// Securely get research status for research id. Returns NULL if input is invalid.
		STRUCT_PLAYER_RESEARCH_STATUS *GetResearchStatus(short int researchId) const {
			if (!this->ptrResearchesStruct || !this->ptrResearchesStruct->researchStatusesArray) { return NULL; }
			if ((researchId < 0) || (researchId >= this->ptrResearchesStruct->researchCount)) { return NULL; }
			return &this->ptrResearchesStruct->researchStatusesArray[researchId];
		}
	};


	// Size = 0x14
	// Constructor : 4EBBF0 (readFromFile)
	class STRUCT_UNKNOWN_GAME_TIME {
	public:
		unsigned long int checksum; // 2C 99 54 00
		STRUCT_GAME_GLOBAL *global;
		unsigned long int unknown_08;
		short int unknown_0C; // Count of... ? (+08 ptr ?)
		short int unknown_0E;
		// 0x10
		float gameTime;

		bool IsCheckSumValid() { return (this->checksum == 0x0054992C); }
	};


	// This structure is contained in stack. Maybe (probably) some fields should never be used here !
	// Size = 0x143C. "constructor" = 4436F0.
	class STRUCT_COMMAND_LINE_INFO {
	public:
		char applicationName[0x1C];
		char *unknown_01C; // pointer to stack value, inside a string ?
		// 0x20
		char *unknown_020; // pointer to stack value, inside a string ?
		unsigned long int unknown_024;
		unsigned long int unknown_028; // a pointer
		unsigned long int stack_SE_handler; // a pointer to executable code
		// 0x30
		char unknown_030[0x1F8 - 0x30];
		char relativePath_empires_dat[0x14]; // +1F8: (included) string  data2/empires.dat, copied from ADDR_VAR_EMPIRES_DAT_PATH and used in 0x501211.
		unsigned long int unknown_20C; // a pointer ?
		// 0x210
		char unknown_210[0x408 - 0x210];
		long int CDCheckFailed; // +408.
		unsigned long int unknown_40C;
		// 0x410
		long int hInstance; // +410 ; GetModuleHandle(NULL)
		char unknown_414[0x518 - 0x414];
		char unknown_518[0x890 - 0x518]; // Command-line arguments string ?
		// 0x890
		long int noStartupFlag; // NOSTARTUP command line flag
		unsigned long int unknown_894;
		unsigned long int unknown_898;
		unsigned long int unknown_89C;
		long int systemMemoryFlag; // SYSTEMMEMORY command line flag
		long int noMusicFlag; // +8A4. NOMUSIC command line flag ?
		long int noSoundFlag; // +8A8. NOSOUND command line flag
		long int isAudioCD; // +8AC. No Autodetect (force). To confirm
		unsigned long int unknown_8B0; // MIDI related ?
		long int isMidiMusic; // +8B4. Force midi. To confirm
		unsigned long int unknown_8B8; // MIDI related ?
		unsigned long int unknown_8BC;
		unsigned long int unknown_8C0;
		unsigned long int unknown_8C4;
		long int scrollSpeed; // +8C8 ; From 10 to 200
		unsigned long int unknown_8CC;
		unsigned long int unknown_8D0; // Related to scroll speed ?
		unsigned long int unknown_8D4;
		unsigned long int unknown_8D8;
		unsigned long int mouseButtonsStyle; // +8DC. 1 or 2 buttons.
		long int screenSizeX; // +8E0
		long int screenSizeY; // +8E4
		// B12 : "savegame\"
		// C17 : "scenario\"
		// D1C : "campaign\"
		// E21 : "sound\"
		// 1130 : "avi\"
		char unknown_8E8[0x143C - 0x8E8];
	};
	static_assert(sizeof(STRUCT_COMMAND_LINE_INFO) == 0x143C, "STRUCT_COMMAND_LINE_INFO size");

#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_SETTINGS1 0x005509D8
#define CHECKSUM_GAME_SETTINGS2 0x00553B78 // parent class
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_SETTINGS1 0x0054301C
#define CHECKSUM_GAME_SETTINGS2 0x0053C0A4 // parent class
#endif
#ifdef GAMEVERSION_ROR10b
#define CHECKSUM_GAME_SETTINGS1 0x0054F6E0
#define CHECKSUM_GAME_SETTINGS2 0x00547DE8 // parent class
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_SETTINGS1 0x0054A264
#define CHECKSUM_GAME_SETTINGS2 0x005430B4 // parent class
#endif
	// Size = 0x11A0 (AOE1.0b&c). Constructor 0x4F91C0 (empires.exe)
	// Size = 0x1254. Constructor 0x5004C0(1.0b), ?(1.0c)
	class STRUCT_GAME_SETTINGS {
	public:
		unsigned long int checksum;
		unsigned long int unknown_004_game_nfo_data;
		unsigned long int unknown_008_ptr_scenario_data;
		STRUCT_COMMAND_LINE_INFO *commandLineInfo;
		// 0x10
		char unknown_010[0x1C];
		unsigned long int scenarioMapSeed; // +2C. When supplied manually through scenario editor. -1 for RM / DM
		unsigned long int unknown_030;
		unsigned long int actualMapSeed; //+34.  THE actual map seed
		unsigned long int unknown_038;
		unsigned long int unknown_03C;
		// 0x40
		char unknown_040[0x1C];
		STRUCT_SLP_INFO **ptrInfosSLP; // +5C. Pointer to array slpInfo, size=3 ? index 0=shortcut numbers
		// 0x60
		STRUCT_MAIN_SOUND *pSoundStruct; // Size 69C, see 41894B. Generally =NULL, only set while being used.
		STRUCT_MAIN_MUSIC *pMusicStruct; // Size 3F8, see 418B14. Generally =NULL, only set while being used.
		long int commonSoundDataArrayElemCount; // +68. Default 0x11 (see 4FEF2D).
		unsigned long int ptrCommonSoundDataArray; // +6C.
		// 0x70
		char musicFormat; // 0=AudioCD, 1=MIDI
		char unknown_071;
		char unknown_072;
		char unknown_073;
		long int minMusicTrack;
		long int maxMusicTrack;
		long int currentMusicTrack;
		// 0x80
		char musicFilename[0x184 - 0x80]; // Chack this...
		long int musicIsLooping; // +184
		long int musicSeekPos; // Resume from minimized window
		STRUCT_MP_COMMUNICATION *MPCommunicationStruct; // +18C
		// 0x190
		char unknown_190[0x1AC - 0x190];
		unsigned long int unknown_1AC; // related to registry ?
		// 0x1B0
		AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS currentUIStatus; // 0=loading,2=inMenu,4=playing,7=editor
		AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES mouseActionType; // +1B4. Set this during game, you'll be able to edit the map !! userSelected* fields
		long int unknown_1B8; // Values 1,4,6,... ? SubMouseActionType ? Used when mouseActionType=6 (villager build menu) or in editor. Seen 1=editor_moveUnit?, 3, 4(editor_putunit?). Unsure !
		long int unknown_1BC;
		// 0x1C0
		char unknown_1C0[0x10];
		// 0x1D0
		unsigned long int unknown_1D0;
		long int hCursor; // Cursor handle, default=10003
		unsigned long int unknown_1D8;
		unsigned long int unknown_1DC;
		// 0x1E0
		long int unknown_1E0;
		long int unknown_ptrFonts; // Ptr to array, elemSize=0x0C, elemIndex = fontId
		char gameDirFullPath[0x105]; // To confirm
		char languageDLLRelativePath[0x105]; // 2ED. "language.dll"
		char unknown_3F2; // unused ?
		char unknown_3F3; // unused ?
		STRUCT_GAME_GLOBAL *ptrGlobalStruct; // +3F4
		unsigned long int unknown_3F8;
		short int editorUserSelectedUnitDefId; // +3FC: the unitDefID that user selected in "units" tab.
		short int editorUserSelectedTerrainId; // Can be changed in editor/terrain tab
		// 0x400
		short int editorUserSelectedAltitude; // in editor/terrain tab
		short int editorTerrainPencilSize; // in editor/terrain tab
		char debugString_404[0x508 - 0x404]; // "Avg view time"...
		long int unknown_508;
		long int unknown_50C; // a counter
		char unknown_510[0x528 - 0x510];
		char unknown_528[0x8E0 - 0x528]; // An included array, elemSize=0x20; getter=417430
		// +4F0 : pointer ? +100=Affects DisableMapSizeSetting
		// 0x8E0
		long int screenSizeX; // To confirm + confirm type
		long int screenSizeY; // To confirm + confirm type
		unsigned long int unknown_8E8;
		unsigned long int unknown_8EC;
		unsigned long int unknown_8F0;
		unsigned long int unknown_8F4;
		float gameSpeed; // +8F8. TO confirm - a struct starts here ?
		char isScenario; // +8FC. 1 if the game is a scenario, 0 for random game or deathMatch. (offset 0x8FC)
		char scenarioFileName[3 + 0x7C]; // +8FD.
		// 0x900
		//char unknown_900[0x7C];
		char unknown_97C;
		char isSinglePlayer;
		char isMultiplayer; // 0x97E
		char mapSizeX;
		// 0x980
		char mapSizeY;
		char unknown_981; // Related to map size. Always 8 ? Max player # ?
		char enableCheatMode; // +982. Set in 41BAE0
		char unknown_983; // +983. path finding for MP ? 0-2 default/medium/high ?
		char unknown_984; // Something to do with screen width and height??
		char revealMap; // 0x985. Set in 41BB00
		char noFog; // 0x986. Set in 41BB10
		char unknown_987; // default 1 ? Set in 41BB20
		char playerCount; // +988.
		char unknown_989; // can set speed > 16 ?
		char playerGameVersion[0x08]; // 0-127 ; 0="", 1="1.0", 2="1.0a", etc (array offset in 0-7)
		char unknown_992;
		AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL difficultyLevel; // +993. 0=hardest, 1=hard, 2, 3, 4=easiest. Set in 41BBC0
		char teamNumbers[0x08]; // player 1-8 (array offset in 0-7). Values: 1=noTeam, >1=teamId.
		char unknown_99C;
		// 0x9A0
		long int isCampaign; // get in 41B720, set in 41BA50
		long int isSavedGame; // +9A4. set in 41BA60
		long int hasSteroidsCheatCode; // +9A8. set in 50CA2B, 4E90B6. Can be cancelled !!!
		long int has_CD; //see 4169D7
		// 0x9B0
		unsigned long int chosenPlayerIndex[9]; // index in 0-8 (playerId, including gaia)
		// to 9D4...
		unsigned long int unknown_9D4;
		unsigned long int unknown_9D8[9]; // ???
		// to 9FC...
		unsigned long int gameIsRunning; // You can simply set it to 0 to pause game.
		// 0xA00
		char unknown_A00[0x0C];
		float unknown_A0C;
		char difficultyLevelChoice; // +A10. Get in 41C190. Copied into +993
		char unknown_A11;
		char unknown_A12;
		char unknown_A13;
		char pathFindingChoice; // 0xA14 (path finding value = 0-2)
		char playerIsInactive[0x09]; // +A15. Index is a "playerIndex"(???Wrong?) (cf +9B0), not playerID. 1 for player that lost/resigned..., 0 if alive
		char unknown_A1E;
		char unknown_A1F;
		// 0xA20
		char unknown_A20[0xA34 - 0xA20];
		long int unknown_A34[9]; // Indexed by playerId. Value = an int ?
		char unknown_A58[0xA7C - 0xA58];
		unsigned long int mapSizeChoice; // +A7C
		// 0xA80
		AOE_CONST_FUNC::MAP_TYPE_INDEX mapTypeChoice; // 0=small islands, 1=large islands... See empires.dat
		unsigned long int unknownA84; // +A84. set in 505340. Default=1. Unused in SP RM ?
		unsigned long int unknownA88; // +A88. set in 505360. Default=1. Unused in SP RM ?
		unsigned long int victoryConditionChoice; // 0=standard, 1=conquest, 7=time limit, 8=score
		// 0xA90
		unsigned long int victoryConditionChoice_parameter; // or float ?
		unsigned long int unknownA94; // Bytes for chosen civs ?
		unsigned long int unknownA98;
		unsigned long int unknownA9C;
		// 0xAA0
		char unknown_AA0[0x20]; // dwords index=0-7. Limits player # ? 8 Players: [0,1,2,3,4,5,6,7] 4 Players: [0,1,2,3,0,1,2,3] 2 Players: [0,1,0,1,0,1,0,1]
		// 0XAC0
		unsigned long int unknownAC0;
		char chosenPlayerNum[8]; // Corresponds to the colored number in game settings screen
		char unknown_ACC;
		// 0xACD
		char playerNameCivDLLID_Offset[8]; // per player. 4400+x=language_dll_id of a civ (player) name.
		// 0xAD5
		char unknown_AD5;
		char unknown_AD6; // +AD6. Set in 505520.
		char unknown_AD7; // +AD7. Set in 505530.
		char fixedPositions; // +AD8. Set in 505540
		char allTechs; // +AD9. Set in 505550
		char unknown_ADA;
		char unknown_ADB;
		unsigned long int initialResourcesChoice; // +ADC. 0=default, 3=large. Set in 505560.
		// 0xAE0
		AOE_CONST_INTERNAL::GAME_INITIAL_AGES initialAgeChoice; // 0=default, 1=nomad, 2=stone, 3=tool 4=bronze, 5=iron, 6=post-iron. Set in 505570
		char unknown_AE4; // +AE4. Set in 505580
		char isDeathMatch; // +AE5. Set in 505590
		char maxPopulation; // +AE6. For MP games. Set in 5055A0, Get in 5054F0. [+A9E in aoe10b]
		char unknown_AE7[0x09];
		// 0xAF0
		char unknown_AF0[0xC20 - 0xAF0];
		// 0xC20
		unsigned long int unknown_C20;
		AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *ptrGameUIStruct; // +C24
		long int lastEventPositionsY[5]; // +C28. Used when pressing HOME key
		// 0xC3C
		long int lastEventPositionsX[5]; // +C3C. Used when pressing HOME key
		// 0xC50
		long int lastEventIndex; // +C50. -1=no "last event". Or a 0-4 value
		long int lastAccessedEventIndex; // +C54. -1=no "last event". Or a 0-4 value. To remember which index was used in last "home key" press.
		// 0xC58
		char unknown_C58[0x106C - 0xC58];
		char debugString[0x1190 - 0x106C]; // Unknown_ size
		// 0x1190
		char isInitMPScreen; // +1190. Set in 5055B0. ??? to confirm. is MP game ?
		char unknown_1191;
		char unknown_1192;
		char unknown_1193;
		char unknown_1194; // default -1 ?
		char unknown_1195;
		char unknown_1196;
		char unknown_1197;
		char civPlayerNameIsUsed[0x08*0x0A]; // +1198. 8=number of players, 0x0A=number of names (0=unused, 1-9=used)
		char unknown_11E8[0x1254 - 0x11E8];

		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_GAME_SETTINGS1) || (this->checksum == CHECKSUM_GAME_SETTINGS2);
		}
	};
	//GAMEVERSION_AOE10c : size 0x11A0
#if defined(GAMEVERSION_ROR10b) || defined(GAMEVERSION_ROR10c)
	static_assert(sizeof(STRUCT_GAME_SETTINGS) == 0x1254, "STRUCT_GAME_SETTINGS size");
#endif

	// Size=0xB0 - Constructor = ?
	class STRUCT_STRATEGY_ELEMENT {
	public:
		unsigned long int checksum; // 3C 2A 54 00
		long int unitDAT_ID; // Identifies the unit definition or research. "type" term found in 408891
		long int unitInstanceId; // gID term found in 408891.
		long int elemId; // Just an internal ID to identify this strategy element in player's strategy list. "uID" term found in 408891.
		// 0x10
		char unitName[0x40]; // 0x3F + \0 maximum
		// 0x50
		float unknown_050; // posY_unused ?
		float unknown_054; // posX_unused ?
		float unknown_058;
		float sizeRadius1;
		// 0x60
		float sizeRadius2;
		unsigned long int unknown_064;
		unsigned long int unknown68_skipCount; // Unsure. "skip" term found in 408891
		STRUCT_STRATEGY_ELEMENT *next;
		// 0x70
		STRUCT_STRATEGY_ELEMENT *previous;
		AOE_CONST_FUNC::TAIUnitClass elementType; // 4-bytes. 0=building, 1=research,2=unit,4=critical_tech. "cat" (category) term found in 408891
		unsigned long int unknown_078;
		unsigned long int unknown_07C;
		// 0x80
		long int inProgressCount; // Unit count that are currently being created for this element. Can only be 0 or 1.
		long int aliveCount; // Unit count that are currently alive for this element. Can only be 0 or 1.
		long int buildAttempts; // Used in ROR internal calculations. "bldAtt"
		long int actor; // a unitId, or value for setGatherPercentage
		// Identifies the unit itself. -1 if N/A. In standard game, it is only valued for "alive" units. 
		// With customROR, we also value it for in-progress buildings => in-construction building already has its ID, use it
		// With customROR, we also value it for in-progress living units => store actor's unitID. This way, if building is destroyed, we can cancel its strategy element (otherwise it remains stuck forever)
		// 0x90
		unsigned long int unknown_090;
		unsigned long int unknown_094;
		unsigned long int unknown_098;
		unsigned long int unknown_09C;
		// 0xA0
		long int totalCount; // Total number of created units. "#blds"
		long int retrains; // Number of times the unit will be created. Then, strategy element is inactive (will never be done again). "cap" in game terminology ?
		unsigned long int unknown_0A8_skipCy; // "skipCy" term found in 408891
		char canSkip; // +AC. "permSkip" term found in 408891
		char unused_0AD;
		short int unused_0AE;

		bool IsCheckSumValid() { return this->checksum == 0x00542A3C; }
	};
	static_assert(sizeof(STRUCT_STRATEGY_ELEMENT) == 0xB0, "STRUCT_STRATEGY_ELEMENT size");


	// Included in TacAI (+E4C). Temporary info about building currently analyzed for being built.
	// Size=0x130
	class STRUCT_TAC_AI_BUILD_TO_TRIGGER {
	public:
		long int unitDefId; // DAT_ID of currently analyzed building
		float unknown_04; // SizeRadius1+SizeRadius2 ?
		char bldName_ToBuild[0x100]; // +8. Set (copied) in 0x4BE9C5.
		long int actorUnitId; // +108.
		unsigned long int minDistanceToTC; // +10C.
		unsigned long int maxDistanceToTC; // +110.
		long int infoIsValid; // +114. Flag 0/1. See 4BE350. 1 when information is valid (saved for a next AI treatment loop because it could not be completed last time)
		long int buildSitePosY; // +118.
		long int buildSitePosX; // +11C
		long int buildSiteScore; // +120. Score from "temp map like" values
		long int unknown_124; // minPosY ?????
		long int randomPlacementFactor; // To confirm
		STRUCT_STRATEGY_ELEMENT *strategyElement;
	};
	static_assert(sizeof(STRUCT_TAC_AI_BUILD_TO_TRIGGER) == 0x130, "STRUCT_TAC_AI_BUILD_TO_TRIGGER size");


	// Size = 0x18
	// For gatheres tasks (not builder/repairman/explorer?)
	class STRUCT_VILLAGER_TASKS_ELEM {
	public:
		long int unitId; // Villager unit id
		long int resourceType; // 0-3
		long int maxResourceAmount; // resourceAmount is compared to resourceAmount to known is gathering is "finished" (needs to deposit)
		long int resourceAmount; // +0C. Gathered amount at this point (compared to +08)
		// 0x10
		long int targetUnitId; // building to build, gazelle to hunt, etc
		unsigned long int timeGetTimeValue; // Last evaluation system time in milliseconds. Task changes are evaluated if > 5 seconds ?

		AOE_CONST_FUNC::RESOURCE_TYPES GetResourceType() { return (AOE_CONST_FUNC::RESOURCE_TYPES)this->resourceType; };
	};


	// Size = 0x70
	// This struct is included in TacAI
	class STRUCT_PLANNED_RESOURCE_NEEDS {
	public:
		unsigned long int checksum; // 54 2C 54 00
		unsigned long int unknown_04; // a dword
		unsigned long int unknown_08; // a dword
		long int resourceAmount[8]; // +0C. Only 4 are used, but there's room for 8 resource types. Index is resourceId.
		long int unknown_2C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int unknown_4C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int resourceTypesCount; // +0x6C ; is always 4

		bool IsCheckSumValid() { return this->checksum == 0x00542C54; }
	};
	static_assert(sizeof(STRUCT_PLANNED_RESOURCE_NEEDS) == 0x70, "STRUCT_PLANNED_RESOURCE_NEEDS size");

	// Size = 0x80. Included in ConAI
	// Some member names were copied from struct strategy element because it looks very similar. (could have same parent class)
	// But this has to be checked (maybe it's wrong)
	class STRUCT_CON_AI_CONSTRUCTION {
	public:
		unsigned long int checksum; // 94 2A 54 00. Parent = 38 2A 54 00 (same parent as stratElem !)
		long int unitDefId;
		unsigned long int unknown_08; // unit instance id ?
		unsigned long int unknown_0C; // counter ?
		// 0x10
		char unitName[0x40]; // 0x3F + \0 ?
		// 0x50
		float posY; // The position that will actually be used to build the new building
		float posX; // The position that will actually be used to build the new building
		unsigned long int unknown_58;
		unsigned long int unknown_5C;
		// 0x60
		float sizeRadius1; // TO VERIFY : +5C and 60 maybe
		float sizeRadius2; // TO VERIFY
		unsigned int unknown68_skipCount; // TO VERIFY // Unsure. "skip" term found in 408891
		STRUCT_CON_AI_CONSTRUCTION *next;
		// 0x70
		STRUCT_CON_AI_CONSTRUCTION *previous;
		unsigned long int unknown_74;
		unsigned long int unknown_78;
		unsigned long int unknown_7C;

		bool IsCheckSumValid() { return this->checksum == 0x00542A94; }
	};


	// Size = 0x08. Used in InfAI+1F8 array.
	// Elements are never reset when a building is destroyed ?
	// Exact role ? When trying to build a deposit bld, it is used to check there is not another one of same kind in same zone.
	// BUG ? status is not checked (and not up to date) when doing this !
	// Methods: 0x4C3300: infAI.addOrUpdateBldHistory(unitDefId, posY, posX, status)
	// 0x4C34C0: infAI.array1F8_setStatus(unitDefId, posY, posX, status)
	// 0x4C3550: infAI.getAddrOfUnusedElemIn1F8List(DAT_ID)
	// 0x4C3590: infAI.hasBuildingIn1F8(DAT_ID, pos, pos, allowedDistance). This is bugged in standard game because status always remains=1, even if building no longer exists! This may prevent some constructions to be started
	class STRUCT_INF_AI_BUILDS_HISTORY {
	public:
		long int unitDefId;
		AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status; // +4
		unsigned char posY; // +5
		unsigned char posX; // +6
		char unused;
	};
	static_assert(sizeof(STRUCT_INF_AI_BUILDS_HISTORY) == 0x8, "STRUCT_INF_AI_BUILDS_HISTORY size");


	// Included in tacAI structure. Size=0x2C.
	class STRUCT_TAC_AI_TARGET_INFO {
	public:
		long int targetUnitId;
		float targetEvaluation; // +04. According to SNTargetEvaluationxxx numbers
		long int unknown_08;
		long int infAIUnitElemListIndex; // +C. Index of matching element in infAI.unitListElem.
		// 0x10
		long int unknown_10;
		long int unknown_14;
		char unknown_18; // default 0. is building ? ignore speed? ? Really unsure. See 4C0ABC
		char unused_19[3];
		long int unknown_1C;
		// 0x20
		long int unknown_20;
		long int unknown_24;
		long int unknown_28; // A flag, default 1 ?
	};
	static_assert(sizeof(STRUCT_TAC_AI_TARGET_INFO) == 0x2C, "STRUCT_TAC_AI_TARGET_INFO size");


	// Size = 0x330. Constructor()=0x4CC630
	// tacAI.createGroup(useSequence)=0x4E0400 ; tacAI.removeUnitGroup(unitGroupId)=0x4E04B0
	// Organized as a loop chained list. See TacAI (first elem is included in TacAI).
	class STRUCT_UNIT_GROUP_ELEM {
	public:
		unsigned long int checksum; // E0 8C 54 00
		STRUCT_UNIT_GROUP_ELEM *next;
		STRUCT_UNIT_GROUP_ELEM *previous;
		long int unitGroupId; // A unique ID
		// 0x10
		long int unknown_resetOrg; // +10. resetOrg ?
		AOE_CONST_INTERNAL::UNIT_GROUP_TYPES unitGroupType; // +14. internal id: 64,65,66,67,6A,6B,6C,6D(=artefacts?)
		long int taskSubTypeId;
		long int myUnitsIdArray[0x28]; // +1C. 40 elements, can be non-consecutive (ignore -1 values).
		// 0xBC
		long int myUnitsHPArray[0x28]; // +BC. 40 elements. Also used as myUnitsIdArray+0xA0.
		long int unitCount; // +15C. Must correspond to valid unitIds count in unitIdArray. Warning: 4CCC90 method requires LucTieuPhung's fix to avoid breaking this assertion.
		// 0x160
		long int unitMaximumCount; // Unsure
		long int commanderUnitId; // +164. group leader
		long int totalGroupHP; // +168. or "old" value ? Not always updated: can be quite wrong !
		long int unsure_previousUnitCount; // +16C. Set from +15Cs
		// 0x170
		float unknown_170_posY;
		float unknown_174_posX;
		float unknown_178_posZ;
		unsigned long int unknown_17C; // unknown type
		// 0x180
		AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS currentTask; // TaskId, see 4CD339. Values in 0-0x15
		long int targetUnitId; // +184. including defended unit case ?
		long int targetDAT_ID;
		float targetPosY;
		// 0x190
		float targetPosX;
		float unknown_194;
		unsigned long int unknown_198; // seems int. DWORD?
		float posY; // not sure what this is exactly, NOT group "supposed" position ?
		// 0x1A0
		float posX;
		float posZ;
		long int unknown_1A8;
		float unknown_1AC_posY;
		// 0x1B0
		float unknown_1B0_posX;
		float unknown_1B4_posZ;
		unsigned long int unknown_1B8;
		unsigned long int unknown_1BC;
		// 0x1C0
		unsigned long int unknown_1C0;
		unsigned long int unknown_1C4;
		unsigned long int unknown_1C8;
		unsigned long int unknown_1CC;
		// 0x1D0
		long int attackPlayId; // attackId that is being played. -1=Non-Play-based attack
		char unknown_1D4;
		char unknown_1D5;
		short int unknown_1D6; // check type (2 bytes ?)
		// 1D8
		char unknown_1D8[0x2C8 - 0x1D8];
		char unknown_2C8; // A counter ? 0x4CD42F
		char unknown_2C9;
		char unknown_2CA; // ?
		char unknown_2CB; // ?
		long int targetUnitIdArrayUsedElemCount; // +2CC. Index of first UNused element in targetUnitIdArray (=> values 0-14). See 0x4CEBB6
		long int targetUnitIdArray[20]; // +2D0. Array of unitIDs. Total size 0x50 bytes (0x14*4)
		long int targetPlayerId; // +320. Related to targetUnitIdArray unit's playerId (last inserted? All the same player?)
		long int unknown_324; // a dword, but containing a 1-byte value. Bool, maybe.
		long int unknown_328_gameTime; // Last tasking time, consistent with global+4.
		unsigned long int unknown_gameTime_ms; // +0x32C. To confirm
		// End (0x330)
		bool IsCheckSumValid() { return this->checksum == 0x00548CE0; }
	};


	// The common object included in each AI section (with xxxAI text)
	// Size = 0xEC
	class STRUCT_COMMON_AI_OBJECT { // 00 2C 54 00 in 1.0c
	public:
		unsigned long int checksum;
		unsigned long int AITypeId; // 1000=MainDecisionAI, 1001=BuildAI, 1002=ConAI... Is this used somewhere ?
		char AIType_text[0x38]; // Exact size ?
		// 0x40
		unsigned long int unknown_040;
		unsigned long int unknown_044;
		long int playerId;
		char playerName[0x40]; // size=64
		unsigned long int unknown_08C; // (3 bytes + 1 unused?)
		// 0x90
		char unknown_080[0x10];
		// 0xA0
		char unknown_0A0[0x40];
		// 0xE0
		unsigned long unknown_0E0; // init = 5 ?
		unsigned long unknown_0E4; // maximum population ?
		unsigned long unknown_0E8;
		// 0xEC: end

		bool IsCheckSumValid() { return this->checksum == 0x00542C00; }
	};


	// Size 0x5C0 - Constructor 0x4B7650
	// Checksums: CC 8A 54 00 (normal), parents=40 2A 54 00
	class STRUCT_BUILD_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1001
		// 0xF0: 1st strategy element
		STRUCT_STRATEGY_ELEMENT fakeFirstStrategyElement; // The first strategy element (empty)
		// 0x1A0
		long int strategyElementCount; // = number of strategy elements (NOT counting fake first elem) = ID for the next element to add
		char strategyFileName[0x40]; // +1A4. Size: unsure
		char unknown_1E4[0x2A5 - 0x1E4];
		char lastBuildName[0x101]; // +2A5: text
		char currentBuildName[0x101]; // +3A6: text (desired build unit name?)
		char nextBuildName[0x101]; // +4A7: text (desired build unit name?)
		long int currentIDInStrategy; // +5A8. Corresponds to stratelem.elemId, not "index". ID 2 might NOT be 3rd element due to inserted elems.
		STRUCT_AI_UNIT_LIST_INFO autoInsertedBuildingDATIDs; // +5AC. Actually, IDs are DATID, not unit instance IDs. Automatically added buildings ?
		STRUCT_AI *mainAI; // +5BC. Set in 0x4B76E0.
		// 0x5C0: end
		bool IsCheckSumValid() const { return this->checksum == 0x00548ACC; }
	};
	static_assert(sizeof(STRUCT_BUILD_AI) == 0x5C0, "STRUCT_BUILD_AI Size");


	// size 0x318 - constructor 0x4B9660.
	// Checksums 18 8B 54 00, parents=98 2A 54 00
	// Construction AI
	class STRUCT_CON_AI {
	public:
		unsigned long int checksum; // 18 8B 54 00
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1002
		// 0xF0
		long int unknown_0F0; // Tested in 4B9A4D
		STRUCT_CON_AI_CONSTRUCTION tempFakeConstructionInfo1; // +F4. First element of a chained list. The constructions that still have to BE STARTED ?
		unsigned long int unknown_174;
		STRUCT_CON_AI_CONSTRUCTION tempFakeConstructionInfo2; // +178. First element of a chained list. Independant from tempConstructionInfo1 !
		char cityPlanName[0x100]; // +1F8. For scenario, selected city plan name. Size is unsure (maybe 101 + 3 unused bytes ?)
		unsigned long int unknown_2F8;
		float townCenter_posY;
		// 0x300
		float townCenter_posX;
		float townCenter_posZ; // unsure !
		long int mapSizeY; // +308
		long int mapSizeX; // +30C
		unsigned long int unknown_310; // unknown type
		unsigned long int ptrMainAI; // +314
		// 0x318: end
		bool IsCheckSumValid() { return this->checksum == 0x00548B18; }
	};
	static_assert(sizeof(STRUCT_CON_AI) == 0x318, "STRUCT_CON_AI size");


	// size 0x150 - constructor=0x40A780
	// Checksums : F4 2A 54 00, parents=04 2C 54 00
	class STRUCT_DIPLOMACY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1003
		// 0xF0
		unsigned long int ptrMainAI; // +F0.
		// 0xF4
		unsigned long int dislikeTable[10]; // +F4. dislike values against all players. Why is there a "player9" value ?
		//unsigned long int unknown_118;
		// 0x11C
		unsigned long int likeTable[10]; // +11C. like values against all players. Why is there a "player9" value ?
		//unsigned long int unknown_140;
		// 0x144
		char isNeutral[9]; // 0 or 1. To confirm
		char unused_14D;
		short int unused;
		// 0x150: end
		bool IsCheckSumValid() { return this->checksum == 0x00542AF4; }
	};
	static_assert(sizeof(STRUCT_DIPLOMACY_AI) == 0x150, "STRUCT_DIPLOMACY_AI size");

	// size 0x1C0 - constructor=0x40AFC0. (mainAI+0xB2C)
	// Checksums : 34 2B 54 00, parents=04 2C 54 00
	class STRUCT_EMOTIONAL_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1004
		// 0xF0
		STRUCT_AI *mainAI;
		long int attitudeValues[6]; // +F4
		char attitudeNames[6][0x1E];
		bool IsCheckSumValid() { return this->checksum == 0x00542B34; }
	};
	static_assert(sizeof(STRUCT_EMOTIONAL_AI) == 0x1C0, "STRUCT_EMOTIONAL_AI size");


	// size 0x10090 - constructor=0x4BA1C0. Offset is AI+0xCEC
	// Checksums : 74 8B 54 00, parents=04 2C 54 00, 74 2B 54 00
	class STRUCT_INF_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1005
		// 0xF0
		STRUCT_AI_UNIT_LIST_INFO unknown_0F0; // Unused ?? See 0x4DB4E2.
		// 0x100
		STRUCT_AI *ptrMainAI;
		long int YMapSize;
		long int XMapSize;
		unsigned long int unknown_10C;
		// 0x110
		long int unitElemListSize; // +110. Total allocated size of unitElemList (the array can contain some "-1")
		// Unit lists : see 0x4BDD10. In those 4 unit lists, elements are never cleaned/removed !
		STRUCT_INF_AI_UNIT_LIST_ELEM *unitElemList; // +114. Contains info about units that can be created by players + resources + artefacts
		STRUCT_AI_UNIT_LIST_INFO creatableAndGatherableUnits; // +118. units that can be created by players + resources (gazelle but not other animals) + artefacts
		STRUCT_AI_UNIT_LIST_INFO playerCreatableUnits; // +128. All player-creatable units (villager, military, buildings...). NOT resources.
		STRUCT_AI_UNIT_LIST_INFO artefactsAndFlags; // +138. Store the found flags/artefacts.
		STRUCT_AI_UNIT_LIST_INFO elementsToDefend; // +148. Can be TC, dock, relic, ruin, mines+bushes. Related to SNxxxDefendPriority
		STRUCT_TEMP_MAP_BUILD_LIKE_INFOS buildMapLikeInfo; // +158. Size to check
		// Remove unit from allMyUnits & buildingUnits: 0x4BF440.
		STRUCT_AI_UNIT_LIST_INFO allMyUnits; // +180. To confirm.
		STRUCT_AI_UNIT_LIST_INFO buildingUnits; // +190
		// 0x1A0
		char unknown_1A0[0x1F0 - 0x1A0];
		// 0x1F0
		unsigned long int unknown_1F0;
		long int buildHistoryArraySize; // +1F4. Allocated element count of +1F8 list. (default=0xC8=200)
		STRUCT_INF_AI_BUILDS_HISTORY *buildHistoryArray; // +1F8.
		long int unknown_1FC; // Element count in unknown_200_ptrSomeArray array ?
		// 0x200
		// Element size = 0x18. see 4C3719,4C3FF2. +0=dword=index(-1=bad?), +4=? +(5,6)=byte(minposYX?),(7,8)=byte(maxposYX?)
		// +9=playerIdAttacker(notme) +A=playerId_me? +C=word +E=byte +10=ptr? +14=dword
		// Related to being attacked ? add in 4C3660.
		unsigned long int *unknown_200_ptrSomeArray;
		char unknown_204[0x40C - 0x204];
		char unknown_40C[0x4D0 - 0x40C]; // 40C + x*4 = counter? arraySize=?
		// 0x4D0
		STRUCT_SPOTTED_RESOURCE_INFO *spottedGatherableUnitsByResourceTypeArrays[4]; // +4D0. Stores explored resources, their position and (closest) storage building
		long int spottedGatherableUnitsCountByResourceType[4]; // +4E0. Number of elements in +4D0+i array ? (i=resource type). Related to +540, +550
		long int spottedGatherableUnitsByResourceTypeArraySizes[4]; // +4F0. 4 Array sizes (allocated sizes ?)
		// 0x500
		unsigned long int unknown_500[4];
		unsigned long int unknown_510[4];
		unsigned long int unknown_520[4];
		unsigned long int unknown_530[4];
		// 0x540
		long int bestForageDropSiteDistance; // +540. Best distance from forage/farm to storage (granary or TC)
		long int bestWoodDropSiteDistance; // +544. Best distance from tree to storage (SP or TC)
		long int bestStoneDropSiteDistance; // +548. Best distance from stone mine to storage (SP or TC)
		long int bestGoldDropSiteDistance; // +54C. Best distance from gold mine to storage (SP or TC)
		// 0x550
		long int bestForageUnitId; // closest target unitId for food (farms & forage only, not hunting)
		long int bestWoodUnitId; // +554. closest target unitId for wood
		long int bestStoneUnitId; // +558. closest target unitId for stone
		long int bestGoldUnitId; // +55C. closest target unitId for gold
		// 0x560
		long int foundForestTiles;
		char unknown_564[0xFF64 - 0x564];
		long int humanPlayer_militaryUnitCountByType[AOE_CONST_INTERNAL::INTERNAL_MILITARY_UNIT_TYPES::CST_IMUT_COUNT]; // 0xFF64 - used only if TrackHumanHistory is ON
		char learnUHFileName[0x100]; // +0xFF90. Name of xxx.uh file. Not sure this is really used.
		// 0x10090: end

		bool IsCheckSumValid() { return this->checksum == 0x00548B74; }
	};
	static_assert(sizeof(STRUCT_INF_AI) == 0x10090, "STRUCT_INF_AI size");


	// size 0xF8 - constructor=0x4C8B50
	class STRUCT_RESOURCE_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1007
		// 0xF0
		unsigned long int ptrMainAI;
		unsigned long int resourceTypeCount; // = 4
	};


	// size 0x5F8 - constructor=0x4CB030
	class STRUCT_STRATEGY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1008
		// 0xF0
		STRUCT_AI *ptrMainAI;
		unsigned long int unknown_0F4;
		char tempPerFile[0x100]; // +F8
		char unknown_1F8[0x234 - 0x1F8];
		long int unknown_counter; // Updated each time strategy AI is run ? (in fact, always reset to 0)
		STRUCT_AI_UNIT_LIST_INFO unknown_238; // List of SN numbers (index) ? For debugging ?
		STRUCT_AI_UNIT_LIST_INFO unknown_248; // List of SN numbers (index) ? For debugging ?
		STRUCT_AI_UNIT_LIST_INFO unknown_258; // List of SN numbers (index) ? For debugging ?
		unsigned long int structPersonalityChecksum; // +268. 9C 8C 54 00
		long int SNNumber[0xE3]; // +26C. only used in init phases ?
		// 0x5F8: end
	};


	// Tactical AI. Lots of things are managed here !
	// size 0x100C - Constructor=0x4CEBE0. Corresponds to MainAI+0x1146C
	class STRUCT_TAC_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=?
		// 0xF0
		STRUCT_AI *ptrMainAI;
		STRUCT_AI_UNIT_LIST_INFO allVillagers; // +F4. Array can contain empty values (-1). Does NOT contain any boat ! 
		//long int *ptrAllVillagersUnitIdArray; // Array of unitIDs (all villagers). May contain empty values (-1) ?
		//long int myVillagerCount; // +F8. exact count of villagers in ptrAllVillagersArray
		//unsigned long int unknown_0FC;
		// 0x100
		//long int allVillagersArraySize; // Size for ptrAllVillagersArray. Can be higher than exact count (if empty spaces).
		STRUCT_AI_UNIT_LIST_INFO villagerExplorers; // +104.
		STRUCT_AI_UNIT_LIST_INFO militaryUnits; // +114. military unitIds that do NOT belong to a unitGroup. Does NOT contain boats, nor towers. Includes priests.
		STRUCT_AI_UNIT_LIST_INFO unknown_124; // NOT SURE it is a STRUCT_AI_UNIT_LIST_INFO
		STRUCT_AI_UNIT_LIST_INFO allBoats; // +134. Includes both fishing + military boats
		STRUCT_AI_UNIT_LIST_INFO warShips; // +144
		STRUCT_AI_UNIT_LIST_INFO fishingShips; // +154
		STRUCT_AI_UNIT_LIST_INFO tradeShips; // +164
		STRUCT_AI_UNIT_LIST_INFO transportShips; // +174
		STRUCT_AI_UNIT_LIST_INFO relics; // +184. Relics only ? What is sure is that is does NOT include ruins.
		long int SNNumber[0xE2]; // +194. In game, we use this one (not strategyAI's). Total size=0x388 (4*0xE2). index from 0 to E1. (missing 1 SN?)
		// 0x51C
		// Here: a list of small structures size=0x18 (0x32 elems?) = gatherers and their tasks (excluding explorers). builders/repairmen NOT in this array?
		AOE_STRUCTURES::STRUCT_VILLAGER_TASKS_ELEM gatherersTasks[0x32];
		unsigned long int villagerTasksRelevantElemCount; // +9CC. Number of RELEVANT element count in villagerTasks array (array size is fixed, it's included in this structure !)
		// 0x9D0
		long int nonExplorerVillagersCount; // +9D0. Counts all villagers BUT explorers
		AOE_STRUCTURES::STRUCT_UNIT_GROUP_ELEM fakeFirstUnitGroupElem; // +9D4. Organized as a circular list (each elem is a group) ? This one = "fake" elem (like in strategy)
		unsigned long int seqUnitGroupId; // +D04. Is = next group's ID.
		unsigned long int unitGroupsCount; // +D08. This does NOT count fakeFirstUnitGroupElem (so it CAN be 0).
		unsigned long int unknown_D0C; // a Value in milliseconds (cmp to global+4)
		// 0xD10
		unsigned long int unknown_D10;
		unsigned long int lastAttackResponseTime_ms; // +D14. Value in milliseconds. Used to compute +D24 ?
		unsigned long int unknown_D18;
		long int lastTacticalUpdateTime; // +D1C. Value in milliseconds
		// 0xD20
		unsigned long int buildFrequencyCounter; // incremented until it reaches SNBuildFrequency
		unsigned long int timeSinceLastAttackResponse_seconds; // To confirm. Compared to SNAttackResponseSeparationTime. See 4E0BC0.
		unsigned long int unknown_D28_ptrTargetInfo; //ptr to playerIdTarget? SEEMS TO BE WHAT DEFINES WHO TO ATTACK?
		unsigned long int unknown_D2C;
		// 0xD30
		unsigned long int unknown_D30;
		unsigned long int unknown_D34; // some counter ? Related to buildAI?
		unsigned long int unknown_D38;
		unsigned long int unknown_D3C;
		// 0xD40
		unsigned long int unknown_D40;
		unsigned long int unknown_D44;
		STRUCT_AI_UNIT_LIST_INFO IdleOrInRangeMilitaryUnits; // +D48. available military units (idle OR in range from TC?) for temp treatments ? See 4D8960
		STRUCT_AI_UNIT_LIST_INFO unknownUnits_D58; // TC + villagers ? + others? Builders ?
		unsigned long int gathererCount_actual[4]; // +D68. Index IS resource ID.
		long int gathererCount_desired[AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT]; // +D78. Villager count we want to assign to each type of resource (4). Index IS resource Type ID
		long int extraResourceTypeNeededByPriority[4]; // +D88 First=most needed. Use extraResourceAmount with this index to get resource amount. <0 amount = I must collect
		long int extraResourceAmount[4]; // +D98. Index is always the same (0=food, 1=food...) <0 amount = I need to collect some. "resDiff"
		STRUCT_PLANNED_RESOURCE_NEEDS plannedResourceNeeds; // +DA8, size=0x70. Related to //SNMostNeededResourceLookAhead (number of stratElem to take into account)
		// 0xE18
		long int attackEnabled; // What is exact meaning ? 1 if attack is currently being run ? Or starts with 0, once set to 1, AI can attack ?
		AOE_CONST_INTERNAL::AI_UPDATE_TYPES currentAIUpdateType; // +E1C. loop ID for TacAI treatments types.
		// 0xE20
		long int unknown_E20; // a dword, a flag - related to villagers/gathering ?
		char unknown_E24[0xE44 - 0xE24];
		long int myWonderBeingBuiltFlag; // +E44. true(1) or false(0) value.
		long int myWonderIsBuiltFlag; // +E48. true(1) or false(0) value.
		STRUCT_TAC_AI_BUILD_TO_TRIGGER constructionToTrigger; // E4C : an included temporary struct (=>F78 or F80?) about building currently analyzed for being built. +0=DATID to build. +108=actorUnitId?
		long int currentAIUpdateVillagerId; // Currently evaluated villager unitId (from allVillagers array). Set to -1 when I have no villager or out of loop ?
		long int unknown_F80; // signed dword
		long int unknown_F84; // internal ID ? 4D1CF7. Related to attacking
		long int storagePitAddedToStrategy; // +F88. 1 when a SP has been inserted into strategy.
		long int granaryAddedToStrategy; // +F8C. 1 when a granary has been inserted into strategy.
		// 0xF90
		long int unknown_F90_someTimeValue; // 4D2245,4D242A...
		long int lastPanicModeTime; // +F94. In seconds. To be compared with globalStruct+4
		long int unknown_F98_timeAboutTributes; // Related to SNCoopDemandTributeInterval
		unsigned long int unknown_F9C;
		// 0xFA0
		unsigned long int lastCoopSharAttackTime_ms; // A game time value in milliseconds.
		unsigned long int unknown_FA4; // flag ? about exploration ? Used in gatherer affectation methods
		unsigned long int unknown_FA8;
		STRUCT_TAC_AI_TARGET_INFO targetInfo; // +FAC.
		long int attacksByPlayerCount[9]; // +FD8. number of times this player attacked me ?
		long int lastUAms; // +FFC. "LastUAms" (UpdateAI). Time spent in last updateAI execution
		long int averageUpdateAITime_ms; // +1000. Updated each time unknown_1004_time_counter reaches 20 and is reset. "Average UAms" ?
		long int calcAverageUpdateAITime_counter; // Maximum value = 20 = 0x14
		long int lastAIUpdateTime_ms; // +1008. A timeGetTime value. (not a game time)

		bool IsCheckSumValid() { return this->checksum == 0x00548CF4; }
	};
	static_assert(sizeof(STRUCT_TAC_AI) == 0x100C, "STRUCT_TAC_AI size");

	// Size 0xF4 - constructor=0x40ED50
	class STRUCT_TRADE_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1010
		// 0xF0
		STRUCT_AI *ptrMainAI;
		bool IsCheckSumValid() { return this->checksum == 0x00542CD0; }
	};
	static_assert(sizeof(STRUCT_TRADE_AI) == 0xF4, "STRUCT_TRADE_AI size");

	// Size = 0x24. Update_Add=0x4BD750/0x4BD7E0
	// Organized as a (big, initially 0x1F4 elements) array in infAI structure
	// Empty slots are re-used when adding an entry.
	// In standard games, entries are NEVER reset !!!! which leads to some very bad behavior (for example, to evaluate if panic mode is needed)
	class STRUCT_INF_AI_UNIT_LIST_ELEM {
	public:
		long int unitId; // -1 means free (empty) slot. Bug in 4C41BD ?
		short int unitDATID; // +04
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClass; // +06 - short int
		unsigned char posY; // +08
		unsigned char posX;
		unsigned char posZ;
		char playerId; // 0x0B
		//float HP; // +C. unit health points
		short int HP; // +C. unit health points
		short int unused_0E;
		// 0x10
		long int unknown_10; // an int used in target rating evaluation, multiplied with //SNTargetEvaluationAttackAttempts in 4C0852
		char unknown_14;
		char unused_15;
		short int unused_16;
		float attack;
		float reloadTime1;
		// 0x20
		float maxRange;
	};

	// The main AI object. Total size = 0x125C8. Constructor = 0x4C6960.
	// Checksums: F0 8B 54 00 (normal) + parents 04 2C 54 00, B0 2B 54 00
	class STRUCT_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT structMainDecisionAI; // size 0xEC
		// 0xF0
		AOE_STRUCTURES::STRUCT_PLAYER *ptrStructPlayer;

		STRUCT_AI_UNIT_LIST_INFO allMyUnits; // +F4. Can contain empty elements ? Remove in 40B950.
		STRUCT_BUILD_AI structBuildAI; // +0x104
		STRUCT_CON_AI structConAI; // +0x6C4 ; city plan
		STRUCT_DIPLOMACY_AI structDiplAI; // +0x9DC
		STRUCT_EMOTIONAL_AI structEmotionalAI; // +0xB2C
		STRUCT_INF_AI structInfAI; // +0xCEC
		STRUCT_RESOURCE_AI structResourceAI; // +0x10D7C
		STRUCT_STRATEGY_AI structStrategyAI; // +0x10E74
		STRUCT_TAC_AI structTacAI; // +0x1146C
		STRUCT_TRADE_AI structTradeAI; // +0x12478

		STRUCT_PLAYER *player; // +1256C. Pointer to player structure
		// +12570. =1 until game start AI init has been done. Includes log like values, add dynamic strategy, resource AI bonus, tribute info init
		// Bug: this information is NOT saved in savegame files (whereas hasDiplomacyInitBeenDone is). When loading a game, it is set to 0 even if init has never been run.
		long int needGameStartAIInit;
		unsigned long int unknown_12574;
		long int nextStrategyAIExecutionCounter; // +12578. Increased, when = 0x14=20 then reset and execute strategy AI.
		unsigned long int unknown_1257C; // A milliseconds time value ? Related to diplomacy management ?
		unsigned long int unknown_12580; // +12580. A milliseconds time value ?
		unsigned long int unknown_12584; // +12584. A milliseconds time value ? Related to tributes ??
		unsigned long int tributeChatFrequency; // +12588. Including randomness. Calculated from SNTributeChatFrequency and SNTributeChatRandomness
		unsigned long int unknown_1258C;
		// 0x12590
		unsigned long int unknown_12590;
		unsigned long int unknown_12594_tributeIsNOTPersistent; // REALLY unsure.
		unsigned long int unknown_12598;
		unsigned long int hasDiplomacyInitBeenDone; // +1259C. 0/1 flag. Waits 10 seconds (0x2710=10000ms) before this init. cf 4C783F.
		unsigned long int requiredTributeToChangeDiplomacy; // +125A0
		long int receivedTributeFromPlayer[9]; // +125A4. Total tribute received from a player (no distinction on resource type). See 4C83B5


		bool IsCheckSumValid() const { return this->checksum == 0x00548BF0; }
	};


	// Size=0x40 for parent class D8 29 54 00 (*seems* to be the common parent)
	// constructor=406EA0 / 407050.
	class STRUCT_ACTION_BASE {
	public:
		unsigned long int checksum;
		AOE_CONST_FUNC::UNIT_ACTION_ID actionTypeID;
		short int unknown_006;
		STRUCT_UNIT *actor;
		AOE_CONST_INTERNAL::ACTION_STATUS actionStatus; // +0xC (byte)
		char unused[3];
		// 0x10
		STRUCT_UNIT *targetUnit;
		unsigned long int unknown_014; // actor unit pointer ???
		long int targetUnitId; // +18
		unsigned long int unknown_unitId; // +1C. A secondary target ?
		// 0x20
		float targetUnitPositionY;
		float targetUnitPositionX;
		float targetUnitPositionZ;
		float unsure_resourceValue; // For some actions (attack), it is execution time? For farms, it is "remaining food" (to be added to unit.resValue).
		// 0x30
		STRUCT_DEF_UNIT_COMMAND *command; // +30. Not always used, nullable. For gatherer, it is always set.
		STRUCT_UNIT_ACTION_INFO *requiredActionInfo; // +34. SubAction ? Link with unit/"actionLink"/action. Allows chaining actions ! This is NOT unit->actionInfo !
		unsigned long int pGraphics; // ptr to graphics structure, consistent with unit+10 ? w_lumber, etc
		char unknown_3C; // Flag 0/1, about graphics (about need to refresh graphics ?)
		char unknown_3D[3]; // Unused. Probably.
		// 0x40: not in BASE class ; it has different type/role according to child classes. (seen float, word...)
	};

	// Size = 0x44
	// Constructor 4052D0: actionMove.constructor(actor, target, maxTargetDistance, pGraphic)
	class STRUCT_ACTION_MOVE : public STRUCT_ACTION_BASE { // DC 26 54 00
	public:
		// 0x40
		float maxDistanceFromTarget; // The maximum distance from target where we accept to stop movement ?
	};
	static_assert(sizeof(STRUCT_ACTION_MOVE) == 0x44, "STRUCT_ACTION_MOVE size");

	// Size 0x5C
	// Constructor 0x401150 = actionAttack.construct(pUnitActor, pUnitTarget, arg3/4/5, blastRadius, minRange, projectileUnit)
	class STRUCT_ACTION_ATTACK : public STRUCT_ACTION_BASE { // F8 23 54 00
	public:
		// 0x40
		//short int targetDATID;
		//short int unknown_42;
		unsigned long int unknown_40; // Pointer to graphics ?
		unsigned long int unknown_44;
		unsigned long int unknown_48;
		float unknown_4C; // max distance from target I can attack from ?
		// 0x50
		long int unknown_50; // Dword
		short int unknown_54;
		short int unknown_56;
		char unknown_58; // set to 0 after attack effect is applied? 401FB7
		char unknown_59; // +59. Init=1 in 4011F1 or 0 in 4011F7
		char unknown_5A; // a counter (decremented...)?
		char unknown_5B;
		bool IsCheckSumValid() { return this->checksum == 0x005423F8; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK_9; } // really unsure :(
	};
	static_assert(sizeof(STRUCT_ACTION_ATTACK) == 0x5C, "STRUCT_ACTION_ATTACK size");

	// Size 0x48
	// Constructor = 4B1CB0
	class STRUCT_ACTION_CONVERSION : public STRUCT_ACTION_BASE {
	public:
		//unsigned long int checksum; // 1C 87 54 00
		// 0x40 = targetDATID (word) ?really unsure?
		char unknown_040[0x48 - 0x40];

		bool IsCheckSumValid() { return this->checksum == 0x0054871C; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT; } // really unsure :(
	};

	// Size 0x50
	// Constructor 04B4F10
	class STRUCT_ACTION_MAKE_OBJECT : public STRUCT_ACTION_BASE { // Train unit...
	public:
		//unsigned long int checksum; // C4 88 54 00
		// 0x40
		short int targetUnitDAT_ID; // being trained unit dat_id
		short int unknown_042;
		unsigned long int unknown_044; // maybe 2 shorts or even bytes ?
		long int strategyElementId; // +48. value from command +0x0C ?
		char unknown_04C;
		char unknown_04D[3]; // unused ?

		bool IsCheckSumValid() { return this->checksum == 0x005488C4; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT; }
	};

	// Size 0x48
	// Constructor 0x4B60B0
	class STRUCT_ACTION_MAKE_TECH : public STRUCT_ACTION_BASE {
	public:
		//unsigned long int checksum; // 90 89 54 00 ?
		// 0x40
		short int researchID;
		short int unknown_42; // Unused... probably
		long int strategyElementId; // +44

		bool IsCheckSumValid() { return this->checksum == 0x00548990; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_TECH; }
	};

	// Size = 0x44. checksum=88 87 54 00
	// Expected commandType = 0x6C (discovery)
	class STRUCT_ACTION_DISCOVERY_ARTEFACT : public STRUCT_ACTION_BASE {
	public:
		char *discoveredByPlayerTable; // +44. Pointer to array of n bytes (depending on total player count).
		bool IsCheckSumValid() { return (this->checksum == 0x00548788); }
	};

	// Size = 0x44. checksum=48 88 54 00.
	// Constructor=0x4B32C0 = actionGatherWithAttack.constructor(actor, defendCmd, targetUnit)
	// (+34) action->actionInfo has a next action=attack tree (or animal) until it is ready to be gathered ?
#define CHECKSUM_ACTION_GATHER_WITH_ATTACK 0x00548848
	class STRUCT_ACTION_GATHER_WITH_ATTACK : public STRUCT_ACTION_BASE {
	public:
		long int targetUnitDefId; // +40. Set as a dword in 4B3334
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_GATHER_WITH_ATTACK); }
	};


#define CHECKSUM_ACTION_PRODUCE_FARM_FOOD 0x0054261C
	// Size = 0x40. Checksum = 1C 26 54 00. This child class does not really have anything specific compared to base class.
	// "Farm is depleted" event is detected/triggered at 0x40482D.
	// Warning: farm "total" food is this->resourceValue (+2C) + farmUnit->resourceValue. See GetFarmCurrentTotalFood method.
	class STRUCT_ACTION_PRODUCE_FARM_FOOD : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_PRODUCE_FARM_FOOD; }
	};


	// Not very well known. Size = 8 ?
	class STRUCT_ACTION_LINK {
	public:
		STRUCT_ACTION_BASE *actionStruct; // +0. Pointer to an action struct (various types possible) = current action.
		STRUCT_ACTION_LINK *nextActionLink; // +4. Use case: Action to restore at move end.
	};


	// Size = 0x54 - Constructor=0x404990
	// Projectiles' actions (move until target or trajectory end is reached)
#define CHECKSUM_ACTION_PROJECTILE 0x0054267C
	class STRUCT_ACTION_PROJECTILE : STRUCT_ACTION_BASE {
	public:
		char unknown_40[0x54 - 0x40];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_PROJECTILE; }
	};
	static_assert(sizeof(STRUCT_ACTION_PROJECTILE) == 0x54, "STRUCT_ACTION_PROJECTILE size");

	// Not very well known. A "potential" target for a unit ? See 414B00
	// Size = 0x24.
	class STRUCT_UNIT_ACTIVITY_TARGET_ELEM {
	public:
		long int actorPlayerId;
		long int internalId;
		unsigned long int unknown_08;
		long int targetUnitId;
		// 0x10
		long int targetPlayerId;
		float posY; // target position
		float posX; // target position
		float posZ; // target position
		// 0x20
		float maxRange; // Actor's maximum range.
	};

	// Size = 0x18. No dedicated constructor? See 0x414D90 for init/adding to array.
	class STRUCT_UNIT_ACTIVITY_QUEUE {
	public:
		long int targetUnitId; // 
		long int actorUnitId; // +4. My unit id ?
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS activityId; // +8. Queued activity ID
		long int genericParam4; // +C. Can be different things ? Seen targetUnitId...
		long int genericParam5; // +10. Seen currentHP value.
		long int genericParam6; // +14. Seen unitDefMaxHP value (int).
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_QUEUE) == 0x18, "STRUCT_UNIT_ACTIVITY_QUEUE size");

	// Not very well known. Accessed via unit+0x74 pointer (+70? too)
	// Size=0x134??? May depend on sub-class
	// [EDX+0xCC]=activity.dequeue(activityQueue) ?
	class STRUCT_UNIT_ACTIVITY {
	public:
		// Known checksums: 
		// 1C 90 54 00 (predator animals?), 7C 8D 54 00 (gazelle?), 3C 8F 54 00 (elephant), 04 98 54 00 (buildings?), 
		// FC 90 54 00 (villager?gatherer?), DC 91 54 00 (priest), 64 95 54 00 (military?), 24 97 54 00 (towers?), 44 96 54 00
		// 5C 8E 54 00 (dead alligator?)
		unsigned long int checksum;
		STRUCT_UNIT *ptrUnit; // +4. actor unit.
		unsigned long int unknown_008;
		unsigned long int unknown_00C;
		// 0x10
		long int targetsInfoArrayUsedElements; // Number of "used" elements in array.
		unsigned long int targetsInfoArrayTotalSize; // Allocated elements
		STRUCT_UNIT_ACTIVITY_TARGET_ELEM *targetsInfoArray; // +18. Potential/previous? targets. THIS is used to get back to work after being attacked !
		unsigned long int unknown_01C; // elems in +24 array
		// 0x20
		unsigned long int unknown_020; // Seen only 0x0A. Size of +20 array ??
		STRUCT_UNIT_ACTIVITY_QUEUE *nextActivitiesQueue_unsure; // TO DO. ElemSize = 0x18. See 414D90
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS internalId_whenAttacked; // taskId. Auto-tasking ID ? Used when idle or attacked? If -1, then unit reacts to attack ? See 414600. Related to +30 value +0x64
		unsigned long int unknown_02C; // A distance?. 0x64 in 4DA4C9. Distance to TC ?
		// 0x30
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS currentActionId; // +30. Current activity type.
		long int targetUnitId; // +34
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES targetUnitType; // +38. Target AI type (3=building...)
		float posY;
		// 0x40
		float posX;
		float posZ;
		float maxDistance; // +48.
		unsigned long int unitIdToDefend; // +4C. Unit ID to capture or defend ?
		// 0x50
		unsigned long int previous_whenAttackedInternalId; // +50. Backup for +28
		long int previousActionId; // Backup for currentActionId. set in 40F6D0 method, 411D00
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES previousTargetUnitId; // +58. Previous target class
		long int previousTargetUnitType; // +5C
		// 0x60
		STRUCT_AI_UNIT_LIST_INFO unknown_060_unitIdList_targets; // +60. Warning, arraySize can be >usedElemCount here.
		// 0x70
		float unknown_070_posY;
		float unknown_074_posX;
		unsigned long int unknown_078; // a unitId. Commander???
		unsigned long int unknown_07C; // unknown type
		// 0x80
		float unknown_080_posY;
		float unknown_084_posX;
		unsigned long int unknown_088; // unknown type
		unsigned long int unknown_08C; // unknown type
		// 0x90
		float unknown_090_posY;
		float unknown_094_posX;
		unsigned long int unknown_098; // unknown type
		unsigned long int unknown_09C; // unknown type
		// 0xA0
		float unknown_0A0_posY;
		float unknown_0A4_posX;
		unsigned long int unknown_0A8; // unknown type
		unsigned long int unknown_0AC; // unknown type
		// 0xB0
		float unknown_0B0_posY;
		float unknown_0B4_posX;
		unsigned long int unknown_0B8; // unknown type
		unsigned long int unknown_0BC; // unknown type
		// 0xC0
		float unknown_0C0_posY;
		float unknown_0C4_posX;
		unsigned long int unknown_0C8; // unknown type
		unsigned long int unknown_0CC; // unknown type
		// 0xD0
		float unknown_0D0_posY;
		float unknown_0D4_posX;
		unsigned long int unknown_0D8; // unknown type
		unsigned long int unknown_0DC; // unknown type
		// 0xE0
		float unknown_0E0_posY;
		float unknown_0E4_posX;
		unsigned long int unknown_0E8; // unknown type
		unsigned long int unknown_0EC; // unknown type
		// 0xF0
		unsigned long int unknown_0F0;
		unsigned long int unknown_0F4; // int, consistent with +0FC and global.playerVar ?
		unsigned long int unknown_0F8; // int, consistent with +0FC and global.playerVar ?
		unsigned long int unknown_0FC; // int, consistent with +0F8 and global.playerVar ?
		// 0x100
		unsigned long int unknown_100;
		unsigned long int unknown_104;
		unsigned long int unknown_108; // int, consistent with +10C
		unsigned long int unknown_10C; // int, consistent with +108
		// 0x110
		float currentPosY;
		float currentPosX;
		unsigned long int unknown_118;
		unsigned long int unknown_11C;
		// 0x120
		float unknown_120; // confirm type
		long int *playerNearbyUnitsInfoIndexes; // +124. ptr to array of indexes to reference (nearby) units in PLAYER_NEARBY_UNITS_INFO object
		long int playerNearbyUnitsInfoIndexesArraySize; // +128. number of elements in +124.
		unsigned long int unknown_12C;
		// 0x130
		char unknown_130; // A flag ? 410C00
		char unknown_131; // unused ?
		char unknown_132; // unused ?
		char unknown_133; // unused ?
	};

	// Accessed via unit+0x184 pointer or from action itself
	// Size = 0x0C.
	// Warning: there are 2 usages ("from unit" or "from action")
	// Example: action cut tree has an action.actionLink = "attack tree" (before it can be gathered)
	// Chain example: action hunt elephant => subaction attack elephant => subaction move (close enough) to elephant
	// Note: actions & action chains do NOT handle "resume working" after being attacked, see activity.targetsArray.
	class STRUCT_UNIT_ACTION_INFO {
	public:
		unsigned long int checksum; // A8 88 54 00 (from unit) or 00 26 54 00 (from action)
		STRUCT_UNIT *ptrUnit;
		STRUCT_ACTION_LINK *ptrActionLink;

		bool IsCheckSumValid() {
			return (this->checksum == 0x005488A8) ||
				(this->checksum == 0x00542600);
		}
	};


	// Size = 0x10
	class STRUCT_PATH_FINDING_INTERMEDIATE_STEP {
	public:
		float posY;
		float posX;
		float posZ; // Unsure
		char remainingSteps; // -1=initial position, 0 for final position, 1 for last before final position...
	};

	// Size = 0x3 ! (yes !)
	class STRUCT_UNIT_TARGET_POS {
	public:
		char posY;
		char posX;
		char posZ;
	};
	static_assert(sizeof(STRUCT_UNIT_TARGET_POS) == 3, "STRUCT_UNIT_TARGET_POS size");

	// Size = 0x34
	// (free=0x4580A0). An interesting method: 0x4581B0=moveInfo.replaceByNewDestination(STRUCT_POSITION_INFO*)
	// About movement/path finding
	class STRUCT_UNIT_MOVEMENT_INFO {
	public:
		unsigned long int unknown_00;
		unsigned long int unknown_04;
		unsigned long int unknown_08;
		unsigned long int unknown_0C;
		// 0x10
		unsigned long int unknown_10; // unknown type/size
		unsigned long int unknown_14;
		unsigned long int unknown_18;
		unsigned long int unknown_1C;
		// 0x20
		unsigned long int unknown_20; // unknown type/size
		STRUCT_PATH_FINDING_INTERMEDIATE_STEP *intermediatePositions; // +24
		long int intermediatePositionsArraySize; // +28
		long int intermediatePositionsUsedElementsCount; // +2C
		// 0x30
		long int intermediatePositionsCurrentIndex; // +30. To confirm. See 4583D0
	};


	// Eye candy (type10) / common class for all units
	// A8 7D 54 00. Size=0x88 (constructor 0x04A64B0)
	class STRUCT_UNIT_BASE {
	public:
		unsigned long int checksum;
		long int unitInstanceId; // Can be negative (temporary units like smoke). Can be something else (or unused?) (seen for flare, type=10)
		STRUCT_DEF_UNIT *ptrStructDefUnit;
		STRUCT_PLAYER *ptrStructPlayer;
		// 0x10
		char *pCurrentGraphics; // Pointer to a structure about graphics...
		unsigned long int unknown_014;
		unsigned long int unknown_018; // ptr to struct 00 30 54 00 - size=0x0C Related to graphics
		STRUCT_GAME_MAP_TILE_INFO *myTile; // +1C. To confirm. +5=byte=altitude&terrain bits, see TERRAIN_BYTE class
		// 0x20
		STRUCT_UNIT *transporterUnit; // Transport boat the unit is in
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unknown_024; // +24.
		short int unknown_028;
		short int unknown_02A; // for graphics ?
		short int unknown_02C; // for graphics ?
		short int unknown_02E; // for graphics ?
		// 0x30
		float remainingHitPoints;
		char currentGraphicsDamageIndex; // +34. According to remaining HP ?
		char orientationIndex; // +35. Also used as building step (<25%, <50%, <75%, >75%). For types 30+, step orientationAngle(+9C), not this. See 0x44BBD0(unit+[0xD4]).
		char mouseSelectionStatus; // +36. 1=selected, 2=right-click-target. Is it a Mask, not an enum !
		char shortcutNumber; // 0-10, 10+=regrouped units ; 0=none, 10=0x0A="0" key.
		float positionY; // (+38) bottom-left to upper-right
		float positionX; // (+3C) top-left to bottom-right
		// 0x40
		float positionZ; // Read only, because it's updated automatically according to map position(X,Y)
		float resourceValue; // See resourceValue. Faith (0-100) for priests, food for hunter...
		char unitStatus; // 0=being_built, 2=ready, at least for buildings... To confirm exact roles. Values=0,1,2,3,5,7,8=max?
		char isNotCreatable; // +49. 1 if the unit is a "not-creatable" unit, like cliffs, etc. Always 1 in parent constructors, set to 0 in "creatable-level" constructor.
		char isDoppleGanger; // +4A. Is temp unit ? Not 100% sure.
		char unknown_04B;
		short int resourceTypeId; // A unit can hold only 1 resource. =0x22=34 for priest.
		char unitType; // +4E. Warning: this value is often WRONG. See unitDef value instead. Only 1-byte here, whereas it is 4-bytes in unitDef struct. 70=living, etc. See GLOBAL_UNIT_TYPES
		char unitCountThatAreTargettingMe; // Number of (other)units that have "me" as target. See 4AADB0.
		// 0x50
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *ptrElemInPlayerUnitList; // +50.
		unsigned long int unknown_054; // +54 a ptr. An array. Elemsize=4? about movement ?
		long int unknown_054_usedElemCount; // +58. (to confirm)
		long int unknown_054_maxElemCount; // +5C. (to confirm)
		// 0x60
		long int unknown_054_arraySize; // +60. Number of allocated elements in +54 (to confirm)
		char unknown_064[0x70 - 0x64];
		// +50 a ptr. ? +4,+8=dword. Related to playerUnitListStruct?
		// +54 a ptr. An array. Elemsize=4? about movement ?
		// +58: number of used elements in +54 ??
		// +60 size of array +54 ?
		// 0x70
		unsigned long int unknown_070; //same type as 74 ?
		STRUCT_UNIT_ACTIVITY *currentActivity; // +74
		short int unknown_078_unitInstanceId; // ?
		short int unknown_07A;
		unsigned long int unknown_07C; // Related to terrain restriction ????
		// 0x80
		unsigned long int unknown_080;
		char unknown_084; // related to movement ?
		char unknown_085; // related to movement ? Previous value of +84 ?
		char unknown_086_hasTarget; // ?
		char unknown_087;

		bool IsCheckSumValid() { return (this->checksum == 0x00547DA8); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_EYE_CANDY); }
		// Returns true if checksum is valid for this class OR a child class
		bool IsCheckSumValidForAUnitClass() {
			return (this->checksum == 0x00547DA8) || // eye candy (including cliffs...)
				(this->checksum == 0x00542E14) || // Flag
				(this->checksum == 0x005441BC) || // Doppleganger
				(this->checksum == 0x00544838) || // Dead/fish (destroyable?)
				(this->checksum == 0x00542748) || // Birds
				(this->checksum == 0x0054324C) || // Type50
				(this->checksum == 0x005445E8) || // Projectiles
				(this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0) || // Building
				(this->checksum == 0x00548474) // Tree
				;
		}
		STRUCT_UNITDEF_BASE *GetUnitDefinition() { return (STRUCT_UNITDEF_BASE *) this->ptrStructDefUnit; }
		// Returns true if the unit definition is a flag (20) or a child class (all but eye candy and trees)
		bool DerivesFromFlag() { 
			return (this->IsCheckSumValidForAUnitClass() && (this->checksum != 0x00547DA8) && (this->checksum != 0x00548474)); // all but 10 and 90
		}
		// Returns true if the unit definition is dead/fish (30) or a child class
		bool DerivesFromDead_fish() {
			return this->DerivesFromBird() || (this->checksum == 0x00544838);
		}
		// Returns true if the unit definition is a bird (40) or a child class
		bool DerivesFromBird() {
			return this->DerivesFromType50() || (this->checksum == 0x00542748);
		}
		// Returns true if the unit definition is a living unit or a child class (building)
		bool DerivesFromLiving() {
			return (this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0); // Building
		}
		// Returns true if the unit definition is type50 or one of its child classes (projectile, living/building).
		bool DerivesFromType50() { 
			return (this->checksum == 0x0054324C) || // Type50
				(this->checksum == 0x005445E8) || // Projectiles
				(this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0); // Building
		}
		// Deletes a units by calling destructor. You should provide freeMemory=true
		// AOE destructor calls all necessary underlying updates of removing the unit (map, player, AI...)
		// Unit will disappear without animation.
		void AOE_destructor(bool freeMemory) {
			long int doFree = freeMemory ? 1 : 0;
			unsigned long int myaddr = (unsigned long int)this;
			_asm {
				MOV ECX, myaddr;
				MOV EDX, DS:[ECX];
				PUSH doFree;
				CALL DS:[EDX];
			}
		}
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE) == 0x88, "STRUCT_UNIT_BASE size");

	

	// 14 2E 54 00 = flag (type20). Size=0x8C. constructor=0x0415020 - derives from type10
	class STRUCT_UNIT_FLAG: public STRUCT_UNIT_BASE {
	public:
		float unknown_088;

		bool IsCheckSumValid() { return (this->checksum == 0x00542E14); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_FLAGS); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG) == 0x8C, "STRUCT_UNIT_FLAG size");


	// BC 41 54 00 = doppleganger (type25). Size=0xB0 (constructor=0x0436030) - derives from type20
	class STRUCT_UNIT_DOPPLEGANGER : public STRUCT_UNIT_FLAG {
	public:
		unsigned long int unknown_08C;
		// 0x90
		unsigned long int unknown_90_color; // ptr to struct 0C 32 54 00 (+4=colorName). +28=sub-struct
		char unknown_94[0xA4 - 0x94];
		unsigned long int unknown_A4; // bool ?
		char unknown_A8; // value 1-3 ?
		char unknown_A9; // Seen 0xDF ?
		char unknown_AA[0xB0 - 0xAA];

		bool IsCheckSumValid() { return (this->checksum == 0x005441BC); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DOPPLEGANGER); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER) == 0xB0, "STRUCT_UNIT_DOPPLEGANGER size");


	// 38 48 54 00 = dead/fish (type30). Size=0x180 (constructor=0x044AD30) - Derives from type20
	class STRUCT_UNIT_DEAD_FISH : public STRUCT_UNIT_FLAG {
	public:
		unsigned long int unknown_08C;
		// 0x90
		float unknown_090; // Used for movement computation ?
		float unknown_094; // Used for movement computation ?
		unsigned long int unknown_098;
		float orientationAngle; // +9C. unit orientation angle. Updating this impacts unit.orientationIndex (+0x35). Angle [0, 2*PI[. 0=heading to northEast (increasing Y)
		unsigned long int unknown_0A0;
		STRUCT_UNIT_MOVEMENT_INFO movementInfo; // +A4. Movement info / path finding.
		STRUCT_UNIT_MOVEMENT_INFO temp_AI_movementInfo; // +D8. Only used in AI treatments(?) to make checks before assigning tasks ? Used when unknown_154_tempFlag_calculatingPath=1
		float unknown_10C; // +10C.
		// 0x110
		float move_initialPosX; // wrong ?
		float move_initialPosZ; // wrong ?
		unsigned long int unknown_118;
		float move_targetPosY; // updated from action struct on action updates.
		// 0x120
		float move_targetPosX;
		float move_targetPosZ;
		unsigned long int unknown_128;
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		float move_distanceFromTargetToReach; // The maximum distance to target to reach before movement is considered done.
		long int move_targetUnitId;
		float targetSizeRadiusY;
		// 0x140
		float targetSizeRadiusX;
		long int unknown_144; // =targetPositionsArrayUsedElements+0xF ?
		char unknown_148[0x154 - 0x148];
		char unknown_154_tempFlag_calculatingPath; // 1 while in treatment (computing a move path ?). If 1, use temp_AI_movementInfo instead of movementInfo ?
		char unknown_155; // Related to action, consistent with action+0xC ? (used when action+C=0 or no action?)
		short int unknown_156; // or 2 bytes ?
		long int unknown_158_posY1;
		long int unknown_15C_posX1;
		long int unknown_160_posY2;
		long int unknown_164_posX2;
		STRUCT_UNIT_TARGET_POS *targetPositionsArray; // +168. ptr to list of 3-bytes elements posYXZ, related to movement. See 44EB70 (add pos in array)
		long int targetPositionsArrayUsedElements; // size of unknown_168_ptrList
		// 0x170
		unsigned long int targetPositionsArrayTotalSize; // allocated number of elements (elemSize=3, warning, it is unusual).
		char isInMovement; // Not sure
		char unknown_175;
		char unknown_176; // ?
		char unknown_177; // ?
		unsigned long int unknown_178;
		unsigned long int unknown_17C;

		bool IsCheckSumValid() { return (this->checksum == 0x00544838); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DEAD_UNITS); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH) == 0x180, "STRUCT_UNIT_DEAD_FISH size");


	// 48 27 54 00 = bird (type40). Size=0x18C (constructor=0x04058A0) - Derives from type30
	class STRUCT_UNIT_BIRD : public STRUCT_UNIT_DEAD_FISH {
	public:
		// 0x180
		unsigned long int unknown_180;
		STRUCT_UNIT_ACTION_INFO *ptrActionInformation; // +184. Useful to retrieve unit's action.
		unsigned long int unknown_188;

		bool IsCheckSumValid() { return (this->checksum == 0x00542748); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BIRD); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD) == 0x18C, "STRUCT_UNIT_BIRD size");


	// 4C 32 54 00 = type50 (type50). Size=0x1BC? (constructor=00425F30) - Derives from type40
	class STRUCT_UNIT_TYPE50 : public STRUCT_UNIT_BIRD {
	public:
		unsigned long int unknown_18C_table; // table of 8 (ptr) elements "Y". Y+0=unitID,Y+5=numPlayerOwner,Y+6=byte(pos?),Y+7=byte(pos?)???
		// 0x190
		unsigned long int unknown_190;
		unsigned long int *unknown_194; // pointer to unit ID (myself ?)
		unsigned long int unknown_198;
		unsigned long int unknown_19C;
		unsigned long int unknown_1A0;
		unsigned long int unknown_1A4;
		unsigned long int unknown_1A8;
		unsigned long int unknown_1AC;
		// 0x1B0
		STRUCT_MAP_VISIBILITY_INFO unknownVisibility_1B0; // Same "nature" object as +0x1E4. A mask for map visibility (visible for ...)?
		float unknown_1B4; // Time to next xxx?
		char stillToBeDiscoveredByHuman; // 1 for gaia units that have not been discovered yet (for gaia units capture system)
		char unknown_1B9[3]; // ?

		bool IsCheckSumValid() { return (this->checksum == 0x0054324C); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TYPE50); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50) == 0x1BC, "STRUCT_UNIT_TYPE50 size");


	// E8 45 54 00 = projectile (type60). Size=1C4 (constructor=0x0443D30) - Derives from type50
	class STRUCT_UNIT_PROJECTILE : public STRUCT_UNIT_TYPE50 {
	public:
		char unknown_1BC; // byte
		char unknown_1BD;
		short int unknown_1BE;
		// 0x1C0
		unsigned long int unknown_1C0;

		bool IsCheckSumValid() { return (this->checksum == 0x005445E8); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_PROJECTILE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE) == 0x1C4, "STRUCT_UNIT_PROJECTILE size");
	

	// 0C 82 54 00 = living (type70). Size=0x1C0 (constructor=0x04AE2D0) - Derives from type50
	class STRUCT_UNIT_LIVING : public STRUCT_UNIT_TYPE50 {
	public:
		char hasDedicatedUnitDef; // +1BC. If true, this->unitDef is a "temporary" copy created when unit was converted, which means it must be freed when unit dies (or changes, for villager mode). See 4AE435 for delete.
		char unknown_1BD;
		short int unknown_1BE;

		bool IsCheckSumValid() { return (this->checksum == 0x0054820C); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING) == 0x1C0, "STRUCT_UNIT_LIVING size");
	

	// A0 7F 54 00 = building (type80). Size=0x1FC (constructor=0x04AC550) - Derives from type70
	class STRUCT_UNIT_BUILDING : public STRUCT_UNIT_LIVING {
	public:
		// 0x1C0
		//short int *ptrHumanTrainQueueInformation; // [+00]=Word=unit_DAT_ID, [+02]=trainQueueCount, if 0 then DATID has no meaning.
		STRUCT_HUMAN_TRAIN_QUEUE_INFO *ptrHumanTrainQueueInformation; // +1C0. Only relevant for human-triggered "train unit".
		short int unknown_1C4; // Max number of different units in queue ?
		short int trainUnitQueueCurrentElemCount; // +1C6. Number of different queued unit types. Warning: it is >=1 only when TRAINING a unit that was "human-asked" ? This flag does NOT mean "busy" !
		unsigned long int unknown_1C8;
		STRUCT_UNIT_ACTION_INFO *trainUnitActionInfo; // +1CC. Only for makeobject actions (not research).
		// 0x1D0
		char queueActionsCounter; // Increments when I click to train/enqueue a unit or cancel it. (if I cancel 3 queued units, it decreases by 3)
		char unknown_1D1;
		short int unknown_1D2; // see 004ADE3C, 4AE1C3. Values -1, 4 ?
		float constructionProgress;
		long int strategyElementId; // +1D8. Strategy element ID (building's) : used to update strategy element when construction ends.
		char unknown_1DC; // Consistent with player+0x54. An index for ?
		char unknown_1DD; // value = 0 or 100 ? Switched when construction ends?
		short int unknown_1DE;
		// 0x1E0
		unsigned long int unknown_1E0;
		STRUCT_MAP_VISIBILITY_INFO unknown_1E4; // Do not modify (automatically refreshed)
		STRUCT_MAP_VISIBILITY_INFO unknown_1E8; // Do not modify (automatically refreshed)
		STRUCT_MAP_VISIBILITY_INFO unknown_1EC; // Do not modify (automatically refreshed)
		// 0x1F0
		unsigned long int unknown_1F0;
		unsigned long int unknown_1F4;
		unsigned long int unknown_1F8;

		// Returns true if checksum is valid FOR A BUILDING only.
		bool IsCheckSumValid() { return (this->checksum == 0x00547FA0); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING) == 0x1FC, "STRUCT_UNIT_BUILDING size");
	

	// 74 84 54 00 = tree (type90). Size=0x88 (constructor=0x04B0D20) - Derives from type10
	class STRUCT_UNIT_TREE : public STRUCT_UNIT_BASE {
	public:
		bool IsCheckSumValid() { return (this->checksum == 0x00548474); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TREE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_TREE) == 0x88, "STRUCT_UNIT_TREE size");


	/* Object sizes according to unit type (@constructor in parenthesis)
	A8 7D 54 00 = eye candy (type10). Size=0x88 (004A64B0) - Base class
	14 2E 54 00 = flag (type20). Size=0x8C (00415020) - Derives from type10
	BC 41 54 00 = doppleganger (type25). Size=0xB0 (00436030) - Derives from type20
	38 48 54 00 = dead/fish (type30). Size=0x180 (0044AD30) - Derives from type20
	48 27 54 00 = bird (type40). Size=0x18C (004058A0) - Derives from type30
	4C 32 54 00 = type50 (type50). Size=0x1BC? (00425F30) - Derives from type40
	E8 45 54 00 = projectile? (type60). Size=1C4 (00443D30) - Derives from type50
	0C 82 54 00 = living (type70). Size = 1C0 (004AE2D0) - Derives from type50
	A0 7F 54 00 = building (type80). Size = 1FC (004AC550) - Derives from type70
	74 84 54 00 = tree (type90). Size = 0x88 (004B0D20) - Derives from type10
	*/
	// Old common type for units
	class STRUCT_UNIT {
	public:
		unsigned long int checksum;
		long int unitInstanceId; // Can be negative (temporary units like smoke). Can be something else (or unused?) (seen for flare, type=10)
		STRUCT_DEF_UNIT *ptrStructDefUnit;
		STRUCT_PLAYER *ptrStructPlayer;
		// 0x10
		char *pCurrentGraphics; // Pointer to a structure about graphics...
		unsigned long int unknown_014;
		unsigned long int unknown_018; // ptr to struct 00 30 54 00 - size=0x0C Related to graphics
		unsigned long int unknown_01C; // ptr. +5=byte
		// 0x20
		STRUCT_UNIT *transporterUnit; // Transport boat the unit is in
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unknown_024; // List of ??
		short int unknown_028;
		short int unknown_02A;
		short int unknown_02C;
		short int unknown_02E;
		// 0x30
		float remainingHitPoints;
		char currentGraphicsAge; // 0=stone/tool, 1=bronze, 2=iron
		char orientation; // Also used as building step (<25%, <50%, <75%, >75%)
		char mouseSelectionStatus; // +36. 1=selected, 2=right-click-target. Is it a Mask, not an enum !
		char shortcutNumber; // 0-10, 10+=regrouped units ; 0=none, 10=0x0A="0" key.
		float positionY; // (+38) bottom-left to upper-right
		float positionX; // (+3C) top-left to bottom-right
		// 0x40
		float positionZ; // Read only, because it's updated automatically according to map position(X,Y)
		float resourceValue; // See resourceValue. Faith (0-100) for priests, food for hunter...
		AOE_CONST_INTERNAL::UNIT_STATUS unitStatus; // +48. 0=not built, 2=ready, etc
		char isNotCreatable; // +49. 1 if the unit is a "not-creatable" unit, like cliffs, etc. Always 1 in parent constructors, set to 0 in "creatable-level" constructor.
		char isDoppleGanger; // Is temp unit ? Not 100% sure.
		char unknown_04B;
		short int resourceTypeId; // A unit can hold only 1 resource. =0x22=34 for priest.
		char unitType; // +4E. Only 1-byte here, whereas it is 4-bytes in unitDef struct. 70=living, etc. See GLOBAL_UNIT_TYPES
		char unitCountThatAreTargettingMe; // Number of (other)units that have "me" as target. See 4AADB0.
		// 0x50
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *ptrElemInPlayerUnitList; // +50.
		unsigned long int unknown_054; // +54 a ptr. An array. Elemsize=4? about movement ?
		long int unknown_054_usedElemCount; // +58. (to confirm). (see 4AC330, reset for AI player only after building?)
		long int unknown_054_maxElemCount; // +5C. (to confirm)
		// 0x60
		long int unknown_054_arraySize; // +60. Number of allocated elements in +54 (to confirm)
		char unknown_064[0x70 - 0x64];
		// 0x70
		unsigned long int unknown_070; //same type as 74 ?
		STRUCT_UNIT_ACTIVITY *currentActivity; // +74
		short int unknown_078_unitInstanceId; // ?
		short int unknown_07A;
		unsigned long int unknown_07C; // Related to terrain restriction ???? NO ?
		// 0x80
		unsigned long int unknown_080;
		char unknown_084; // related to movement ? IsMoving?
		char unknown_085; // related to movement ? IsMoving? Previous value of +84 ? Or "temp" value (like temp_AI_movementInfo) ?
		char unknown_086_hasTarget; // ?
		char unknown_087;
		float unknown_088; // a resource value ? Max value ?
		unsigned long int unknown_08C;
		// 0x90
		float unknown_090; // Used for movement computation ?
		float unknown_094; // Used for movement computation ?
		char unknown_098[0xA4 - 0x98];
		STRUCT_UNIT_MOVEMENT_INFO movementInfo; // +A4. Movement info / path finding.
		STRUCT_UNIT_MOVEMENT_INFO temp_AI_movementInfo; // +D8. Only used in temp path finding for checking movements. Used when unknown_154_tempFlag_calculatingPath=1
		float move_initialPosY; // +10C. WRONG orientationAngle !: Angle [0, 2*PI[. 0=heading to northEast (increasing Y)
		// 0x110
		float move_initialPosX; // wrong ?
		float move_initialPosZ; // wrong ?
		unsigned long int unknown_118;
		float move_targetPosY; // updated from action struct on action updates.
		// 0x120
		float move_targetPosX;
		float move_targetPosZ;
		unsigned long int unknown_128;
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		float move_distanceFromTargetToReach; // The maximum distance to target to reach before movement is considered done.
		long int move_targetUnitId;
		float targetSizeRadiusY;
		// 0x140
		float targetSizeRadiusX;
		long int unknown_144; // Compared to 0x0A in 458CCF. =targetPositionsArrayUsedElements+0xF ?
		char unknown_148[0x154 - 0x148];
		char unknown_154_tempFlag_calculatingPath; // 1 while in treatment (computing a move path ?). If 1, use temp_AI_movementInfo instead of movementInfo ?
		char unknown_155; // Related to action, consistent with action+0xC ? (used when action+C=0 or no action?)
		short int unknown_156; // or 2 bytes ?
		long int unknown_158_minPosY; // used for path finding ?
		long int unknown_15C_minPosX;
		long int unknown_160_maxPosY;
		long int unknown_164_maxPosX;
		STRUCT_UNIT_TARGET_POS *targetPositionsArray; // +168. ptr to list of 3-bytes elements posYXZ, related to movement. See 44EB70 (add pos in array)
		long int targetPositionsArrayUsedElements; // size of unknown_168_ptrList
		// 0x170
		unsigned long int targetPositionsArrayTotalSize; // allocated number of elements (elemSize=3, warning, it is unusual).
		char isInMovement; // Not sure
		char unknown_175;
		char unknown_176; // ?
		char unknown_177; // ?
		unsigned long int unknown_178;
		unsigned long int unknown_17C;
		// 0x180
		unsigned long int unknown_180;
		STRUCT_UNIT_ACTION_INFO *ptrActionInformation; // +184. Useful to retrieve unit's action.
		unsigned long int unknown_188;
		unsigned long int unknown_18C_table; // table of 8 (ptr) elements "Y". Y+0=unitID,Y+5=numPlayerOwner,Y+6=byte(pos?),Y+7=byte(pos?)???
		// 0x190
		char unknown_190[0x1B0 - 0x190];
		// 0x1B0
		STRUCT_MAP_VISIBILITY_INFO unknown_1B0; // Same "nature" object as +0x1E4. A mask for map visibility (visible for ...)?
		float unknown_1B4; // Time to next xxx?
		char stillToBeDiscoveredByHuman; // 1 for gaia units that have not been discovered yet (for gaia units capture system ?)
		char unknown_1B9[3]; // ?
		char hasDedicatedUnitDef; // Set to 1 when converted. Means unitDef must be freed when unit dies/changes unitDef. See 4AE435 for delete.
		char unknown_1BD;
		short int unknown_1BE;
		// 0x1C0 - Only buildings have those members
		unsigned long int ptrHumanTrainQueueInformation; // [+00]=Word=unit_DAT_ID, [+02]=trainQueueCount, if 0 then DATID has no meaning.
		short int unknown_1C4; // Max number of different units in queue ? Unused because always 1 ?
		short int isCurrentlyTrainingUnit; // +1C6. Warning: it is 1 only when TRAINING a unit that was "human-asked" ? This flag does NOT mean "busy" !
		unsigned long int unknown_1C8;
		STRUCT_UNIT_ACTION_INFO *trainUnitActionInfo; // +1CC. Only for makeobject actions (not research).
		// 0x1D0
		char unknownClickCounter; // Increments when I click to train/enqueue a unit or cancel it. (if I cancel 3 queued units, it decreases by 3)
		char unknown_1D1;
		short int unknown_1D2; // see 004ADE3C, 4AE1C3. Values -1, 4 ?
		float constructionProgress;
		unsigned long int unknown_1D8; // Build item, for buildings ??
		char unknown_1DC; // Consistent with player+0x54
		char unknown_1DD; // value = 0 or 100 ? Switched when construction ends?
		short int unknown_1DE;
		// 0x1E0
		unsigned long int unknown_1E0;
		STRUCT_MAP_VISIBILITY_INFO unknown_1E4;
		STRUCT_MAP_VISIBILITY_INFO unknown_1E8;
		STRUCT_MAP_VISIBILITY_INFO unknown_1EC;

		bool IsCheckSumValid() {
			return ((this->checksum == 0x00547DA8) || // eye candy
				(this->checksum == 0x00542E14) || // Flag
				(this->checksum == 0x005441BC) || // Doppleganger
				(this->checksum == 0x00544838) || // Dead/fish
				(this->checksum == 0x00542748) || // Birds
				(this->checksum == 0x005445E8) || // Projectiles
				(this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0) || // Building
				(this->checksum == 0x00548474) // Tree
				);
		}
		STRUCT_UNITDEF_BASE *GetUnitDefBase() { return (STRUCT_UNITDEF_BASE *)this->ptrStructDefUnit; }
	};


	class STRUCT_UI_OBJ_UNKNOWN_INFO {
	public:
		unsigned long int unknown_00; // A pointer
		long int hWnd;
		unsigned long int unknown_08;
		unsigned long int unknown_0C; // A pointer
		// 0x10
	};

	// A shortcut to be used with UI objects when exact type is unknown. Also used as parent class for UI objects.
	// Size=0xF4 for this class, but all child classes are larger !
	// Constructor: 004523F0 for "screen" objects, 00452580 for components
	// In general, you should not modify these members directly. You are supposed to use methods (few are implemented in this solution).
	// D8 4A 54 00 for this base (parent) class ??
	class STRUCT_ANY_UI {
	public:
		unsigned long int checksum; // Depends on UI object type
		unsigned long int unknown_004; // pointer ?
		unsigned long int unknown_008;
		unsigned long int unknown_00C_posX; // unsure
		// 0x10
		unsigned long int unknown_010_posY; // unsure
		long int sizeX;
		long int sizeY;
		char *screenName;
		// 0x20
		STRUCT_UI_OBJ_UNKNOWN_INFO *unknown_020; // pointer. +04=hWnd +13C=evtStatus?? +1F6=objectIndex,+F4+objectIndex*4=ButtonId +17C=hObject?
		long int minPosX; // +24. Min X position for this UI object. +24=a pRect object (4 position dwords)
		long int minPosY; // +28
		long int maxPosX; // +2C
		// 0x30
		long int maxPosY; // +30
		unsigned long int unknown_034;
		unsigned long int unknown_038; // init = 2
		STRUCT_ANY_UI *focusedComponent;
		// 0x40
		STRUCT_ANY_UI *ptrParentObject; // +40
		unsigned long int unknown_044;
		unsigned long int unknown_048;
		unsigned long int unknown_04C;
		// 0x50
		unsigned long int unknown_050;
		unsigned long int unknown_054; // ptr ?
		unsigned long int unknown_058; // ptr ?
		unsigned long int unknown_05C; // A ptr to a list: elem= {+0=STRUCT_ANY_UI*, +4=nextElem}. Used in 453360
		// 0x60
		unsigned long int unknown_060;
		unsigned long int unknown_064;
		unsigned long int unknown_068; // 1 when being clicked ???
		long int visible; // +6C. For a dialog it is 0 ? Do not edit directly.
		// 0x70
		unsigned long int unknown_070;
		unsigned long int editable; // TO CONFIRM !! seen for text components. What about others ? For button, it is text color when "active"?
		unsigned long int unknown_078;
		unsigned long int unknown_07C; // byte ?
		// 0x80
		unsigned long int unknown_080; // Related to "!readonly" ??? But it is NOT "enabled"
		unsigned long int unknown_084; // type ?
		unsigned long int unknown_088;
		long int unknown_08C_minPosX; // MinPosX ? similar to +24 ?
		// 0x90
		long int unknown_090_minPosY; // minPosY ? similar to +28 ? Example 0x14 (size of top banner, with resources, current age, menu)
		long int unknown_094_maxPosX; // maxPosX ? similar to +2C
		long int unknown_098_maxPosY; // maxPosY ? similar to +30
		unsigned long int unknown_09C; // similar to +24, +8C ?
		// 0xA0
		unsigned long int unknown_0A0; // similar to +28, +90 ?
		unsigned long int unknown_0A4;
		unsigned long int unknown_0A8;
		unsigned long int unknown_0AC; // similar to +14 ? (X pos or size ?)
		// 0xB0
		unsigned long int unknown_0B0; // similar to +14 ? (X pos or size ?)
		unsigned long int unknown_0B4; // similar to +18 ? (Y pos or size ?)
		unsigned long int unknown_0B8; // similar to +18 ? (Y pos or size ?)
		unsigned long int unknown_0BC; // init=0xFA ?
		// 0xC0
		unsigned long int unknown_0C0;
		unsigned long int unknown_0C4;
		unsigned long int unknown_0C8;
		unsigned long int unknown_0CC;
		// 0xD0
		unsigned long int unknown_0D0;
		unsigned long int unknown_0D4;
		unsigned long int unknown_0D8;
		unsigned long int unknown_0DC;
		// 0xE0
		unsigned long int unknown_0E0;
		long int helpDllId; // +E4. Dll string Id for help (or tooltip?) ?
		long int winHelpDataDllId; // +E8. a DllID, used when clicking on help/(object)/details = data for winHelp call.
		char unknown_0EC;
		char unknown_0ED;
		char unknown_0EE;
		char unknown_0EF;
		// 0xF0
		char unknown_0F0;
		char unknown_0F1; // is this used ?
		char unknown_0F2;
		char unknown_0F3;
		// 0xF4 and after: different according to 2 main child (intermediate) classes: UI components and UI screens ?
		// Screens: F4, F8 are dwords (some X/Y size/pos). classes Size = 0x478/0x490 ? See 454B60/460730
		// Components: F4 is 2 words...?

		long int GetHWnd() {
			if (!this->unknown_020) { return 0; }
			return this->unknown_020->hWnd;
		}
	};


	// Size 0x2B8. Also for checkboxes.
	// Can be created in 0x0456240
#define CHECKSUM_UI_BUTTON 0x00544E30
#define CHECKSUM_UI_BUTTON_WITH_NUMBER 0x00549CAC
	class STRUCT_UI_BUTTON : public STRUCT_ANY_UI {
	public: // 30 4E 54 00 (a child class is AC 9C 54 00(size=0x3D4), rarely used. 16 are created in game screen=unit-commands)
		long int commandIDs[9]; // +F4. Only index 0 is really used ?? Various types (enums) (GAME_SCREEN_BUTTON_IDS, INGAME_UI_COMMAND_ID, etc)
		long int buttonInfoValue[9]; // +118. For example, a DATID.
		unsigned long int unknown_13C;
		unsigned long int unknown_140;
		unsigned long int unknown_144;
		unsigned long int* unknown_148_wav; // +148: sound button.wav info? (always 0 except when playing sound ?)
		unsigned long int* unknown_14C[9]; // Pointers, but not UI structs ?
		short int unknown_170[9]; // +170. Default -1.
		char unknown_182[0x198 - 0x182];
		char* unknown_198[9];
		char* unknown_1BC[9];
		char unknown_1E0[0x1F6 - 0x1E0];
		short int checked; // +1F6. To update value, see AOE_CheckBox_SetChecked. Index for commandIDs and buttonInfoValue
		unsigned long int unknown_1F8;
		STRUCT_ANY_UI **unknown_1FC; // ptr array groupedObjects ? really unsure
		// 0x200
		short int unknown_200; // groupedObjectCount ? really unsure (like radiobuttons?)
		char unknown_202[0x208 - 0x202];
		unsigned long int unknown_208[9]; // +208. default 0x00FFFFFF
		unsigned long int unknown_22C[9];
		long int unknown_250[9]; // +250. default 0xFFFF
		unsigned long int unknown_274[9];
		long int hotkey; // +298. Ascii code of hotkey: 65='A', etc.
		long int unknown_29C; // +29C. Related to hotkey ? Not sure.
		unsigned long int unknown_2A0;
		char unknown_2A4;
		char unknown_2A5;
		char unknown_2A6;
		char unknown_2A7;
		char unknown_2A8;
		char unknown_2A9;
		char unknown_2AA_unused; // +2AA : unused ?
		char unknown_2AB_unused; // +2AB : unused ?
		char unknown_2AD_unused[4]; // +2AC unsure
		long int readOnly; // +2B0. 45F3A0 to modify ?
		long int unknown_2B4; // +2B4. 4606D0 = setter ?

		bool IsChecked() { return this->checked != 0; }
		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER) || (this->checksum == CHECKSUM_UI_BUTTON);
		}
	};
	static_assert(sizeof(STRUCT_UI_BUTTON) == 0x2B8, "STRUCT_UI_BUTTON size");

	// Size 0x3D4. Constructor=0x4F7420
	class STRUCT_UI_BUTTON_WITH_NUMBER : public STRUCT_UI_BUTTON {
	public:
		// Starts at +2B8
		long int showNumber; // +2B8. If true, numberToDisplay will be displayed. Set in 0x4F72B0. Call refresh after changing it.
		long int numberToDisplay; // +2BC. The number to show on button icon, like queued item count. Call refresh after changing it.
		// 0x2C0
		char unknown_2C0; // Values 0, 3 .. ?
		char unknown_2C1[3];
		unsigned long int *unknown_2C4;
		unsigned long int *unknown_2C8;
		unsigned long int unknown_2CC;
		char contextHelpText[0x100]; // +2D0.
		long int unknown_3D0; // see 483785
		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER);
		}
	};
	static_assert(sizeof(STRUCT_UI_BUTTON_WITH_NUMBER) == 0x3D4, "STRUCT_UI_BUTTON_WITH_NUMBER size");


	// Size ?
	// Can be created in 0x0456A30
#define CHECKSUM_UI_TEXTBOX 0x00545220
	class STRUCT_UI_TEXTBOX : public STRUCT_ANY_UI { // 20 52 54 00
	public:
		long int hWnd;
		short int maxTextSize; // +F8. =constructor_maxTextSize+1
		short int unknown_0FA; // +FA
		char *pTypedText; // +FC.
		// 0x100
		char unknown_100[0x128 - 0x100];
		long int inputType; // 0=normal, 3=numberOnly, 7=multiline

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_TEXTBOX; }
	};


	// Size = 0x10
	class STRUCT_UI_LIST_ITEM {
	public:
		char *itemText;
		unsigned long int *unknown_04;
		long int optionalId;
		STRUCT_UI_LIST_ITEM *next;
	};

	// Size = 0x1A0  // F8 53 54 00
	// Constructor = 0x464A50 (also 00456F20?). Parent class is label (90 5A 54 00).
	// See also STRUCT_UI_COMBOBOX (that contains a listbox)
#define CHECKSUM_UI_LISTBOX 0x005453F8
	class STRUCT_UI_LISTBOX : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LIST_ITEM *firstItem; // +F4. First element of list of items.
		short int itemCount; // +F8
		short int unknown_0FA;
		short int unknown_0FC; // minVisibleIndex??
		short int unknown_0FE; // maxVisibleIndex??
		short int selectedIndex; // +100.
		short int unknown_102; // +102
		// +138=word

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LISTBOX; }
	};

	// Size = 0x144 - 50 50 54 00. Direct parent is STRUCT_ANY_UI
	// Constructor = 0x460A00
#define CHECKSUM_UI_COMBOBOX 0x00545050
	class STRUCT_UI_COMBOBOX : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned long int unknown_0F8;
		STRUCT_UI_LISTBOX *underlyingListBox;
		// 0x100
		unsigned long int unknown_100;
		unsigned long int unknown_104;
		unsigned short int unknown_108;
		unsigned short int unknown_10A;
		unsigned long int unknown_10C; // unknown type
		char unknown_110[0x130 - 0x110];
		// 0x130
		char unknown_130;
		char unknown_131;
		short int unknown_132; // unknown type
		unsigned long int unknown_134; // unknown type
		unsigned long int unknown_138; // unknown type
		unsigned long int unknown_13C;
		// 0x140
		unsigned long int unknown_140; // init=1

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_COMBOBOX; }
		long int GetValue() {
			if (!this->IsCheckSumValid()) { return -1; }
			unsigned long int tmp = this->checksum;
			_asm {
				MOV ECX, tmp
				MOV EAX, 0x461E50
				CALL EAX
				MOV tmp, EAX
			}
			return tmp;
		}
	};

	// Size = 0x1C4 // B8 56 54 00
	// Constructor = 0x466E30.
#define CHECKSUM_UI_SCROLLBAR 0x005456B8
	class STRUCT_UI_SCROLLBAR : public STRUCT_ANY_UI {
	public:
		STRUCT_ANY_UI *objectToScroll; // +F4

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCROLLBAR; }
	};


	// Size 0x188
	// Constructor=0x4685B0
#define CHECKSUM_UI_LABEL 0x00545A90
	class STRUCT_UI_LABEL : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned short int unknown_0F8;
		unsigned short int unknown_0FA;
		unsigned short int unknown_0FC;
		unsigned short int unknown_0FE;
		// 0x100
		unsigned short int unknown_100;
		unsigned short int unknown_102;
		unsigned long int unknown_104;
		unsigned long int unknown_108;
		unsigned long int unknown_10C;
		// 0x110
		unsigned long int unknown_110;
		unsigned long int unknown_114;
		unsigned long int unknown_118;
		unsigned long int unknown_11C; // default 2 ?
		// 0x120
		unsigned long int unknown_120;
		unsigned long int unknown_124;
		unsigned long int unknown_128; // type ?
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		unsigned long int unknown_134;
		unsigned short int unknown_138;
		unsigned short int unknown_13A; // type ?
		unsigned long int unknown_13C;
		// 0x140
		unsigned long int unknown_140;
		unsigned long int unknown_144;
		unsigned short int unknown_148;
		unsigned short int unknown_14A;
		unsigned long int unknown_14C;
		// 0x150
		char unknown_150; // init -1
		char unknown_151; // is it used ?
		short int unknown_152; // is it used ?
		unsigned long int unknown_154;
		char unknown_158;
		char unknown_159; // is it used ?
		short int unknown_15A; // is it used ?
		unsigned long int unknown_15C;
		// 0x160
		unsigned long int unknown_160;
		unsigned long int unknown_164;
		unsigned long int unknown_168;
		unsigned long int unknown_16C;
		// 0x170
		unsigned long int unknown_170;
		char unknown_174;
		char unknown_175;
		char unknown_176;
		char unknown_177;
		char unknown_178;
		char unknown_179;
		char unknown_17A; // unused ?
		char unknown_17B; // unused ?
		unsigned long int unknown_17C;
		// 0x180
		unsigned long int unknown_180;
		unsigned long int unknown_184;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LABEL; }
	};


	// Size ?
/*#define CHECKSUM_UI_IN_GAME_SUB1 0x00546688
	class STRUCT_UI_IN_GAME_SUB1 : public STRUCT_ANY_UI { // TO DO - name to fix - role to determine
	public:
		char unknown_0F4[0x100 - 0x0F4];
		STRUCT_PLAYER *humanControlledPlayer;
		long int unknown_104_link_map_infos; // like global+28 ?
		// 340 = unitId (under mouse ?)

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_SUB1; }
	};
	*/

	// Size ?
#define CHECKSUM_UI_DIAMOND_MAP 0x0054A73C
	class STRUCT_UI_DIAMOND_MAP : public STRUCT_ANY_UI { // 3C A7 54 00 (editor). Also F4 A3 54 00(game) ?
	public:
		STRUCT_GAME_GLOBAL *global; // +F4
		STRUCT_PLAYER *controlledPlayer;
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +FC
		// 0x100
		char *unknown_100[0x128 - 0x100];
		unsigned long int unknown_128; // array map coordinates per diammap "index" ?? ElemSize=6 (3 words) [+0]+[+2]=constant(6E)?
		unsigned long int unknown_12C; // pointer to diam map data? Alloc_size = mapSizeX_plus_Y_minus_1 * 0x24. +0=word(an index?)+2=byte=relevant,+4=word,+8=float,+C=dword,+10=sizeY-?-1,+12=word,+14=word,+18=float,+1C=word,+20=float
		// 0x130
		short int mapSizeY;
		short int mapSizeX; // +132
		unsigned long int unknown_134;
		short int unknown_138; // Number of "diam_map" indexes ? =elemCount for +128 ?
		short int mapSizeX_plus_Y_minus_1; // +13A. = mapSizeY+mapSizeX-1
		float unknown_pixelFactor; // +13C. = mapSizeX_plus_Y_minus_1 / unknown_138
		// 0x140
		char *unknown_140[0x168 - 0x140];
		unsigned long int unknown_168; // ptr to diamMap data ? (color to display...) ? +8=ptr to array[byte=index]=colorPerPixel

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_DIAMOND_MAP; }
	};

	// Size 0x380. Constructor=0x465730
	// class for chat text lines (1 for each chat line), yellow/orange centered in-game error messages
	class STRUCT_UI_IN_GAME_TEXT_ZONE : public STRUCT_ANY_UI {
	public:
		//unsigned long int checksum; // EC 54 54 00
		// F8 : text?
		bool IsCheckSumValid() { return this->checksum == 0x005454EC; }
	};

	// Size 0x0x4BC. Constructor = 0x433C30 (create and show). B8 3B 54 00. Parent=1C 4F 54 00
#define CHECKSUM_UI_IN_GAME_MENU 0x00543BB8
	class STRUCT_UI_IN_GAME_MENU : public STRUCT_ANY_UI{
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_MENU; }
	};

	// Size = 0x4A4
	// Constructor=0x434550
	class STRUCT_UI_POPUP_DIALOG : public STRUCT_ANY_UI {
	public:
		// Note on isActive: For a dialog it is probably always 0. Parent UI object is active, not the dialog.
		char unknown_0F4[0x498 - 0x0F4];
		STRUCT_UI_BUTTON *btnYes;
		STRUCT_UI_BUTTON *btnNo;
		// 4A0
		STRUCT_UI_BUTTON *btnCancel;
		// End.
		bool IsCheckSumValid() { return this->checksum == 0x00543CEC; }
	};

#define CHECKSUM_UI_PLAYING_ZONE 0x00546688
	// Size 0x380
	// Constructor 0x518690
	// Parent can be STRUCT_UI_IN_GAME_MAIN, or editor UI?
	// Also used in editor !
	// 88 66 54 00, parent = 40 A8 54 00, 1C A6 54 00, then UI base
	class STRUCT_UI_PLAYING_ZONE : public STRUCT_ANY_UI {
	public:
		long int hWnd; // TO CONFIRM. +F4
		unsigned long unknown_0F8;
		STRUCT_GAME_GLOBAL *globalStruct; // +FC
		// 0x100
		STRUCT_PLAYER *controlledPlayer;
		STRUCT_GAME_MAP_INFO *globalGameMapInfo; // +104. To confirm
		short int unknown_108; // some value for Y
		short int unknown_10A;
		short int unknown_10C;
		short int unknown_10E; // some value for X
		unsigned long int unknown_110;
		short int unknown_114;
		short int unknown_116;
		short int unknown_118;
		short int unknown_11A;
		short int unknown_11C; //?
		short int unknown_11E; //?
		short int unknown_120;
		short int unknown_122_screenGamePosY_word; // +122. From player.screenPosY
		short int unknown_124_screenGamePosX_word; // +124. From player.screenPosX
		short int unknown_126;
		short int unknown_128;
		short int unknown_12A;
		short int unknown_12C;
		short int unknown_12E;
		char unknown_130[0x138 - 0x130];
		float screenGamePosY; // +138.
		float screenGamePosX; // +13C.
		char unknown_140;
		char unknown_141[3];

		char unknown_144[0x334 - 0x144];
		// +168 : byte
		// +304, 318, 31C, 320: dwords related to mouse pos?

		unsigned long int unknown_334_ptr; // Ptr to obj size=0x70, ccor=516110. +5C=array size 4*(maxTypeId+1) of ptrs to obj. obj:+4=next,+8=unitId,+30/32/34/36=min/maxPosXY? +C,+20=ptrGraphicsShp?
		// Note: +4C=maxTypeId, typeIDs: 00=eyeCandy,10=birds,20=flag/deadfish/living/building/tree,30,40=?
		unsigned long int unknown_338_ptr; // Ptr to same kind of obj as +334
		unsigned long int unknown_33C;
		// 0x340
		long int unknown_340_unitId; // Target ? Selected ? Under mouse ?
		unsigned long int unknown_344;
		unsigned long int unknown_348;
		unsigned long int unknown_34C;
		char unknown_350[0x36C - 0x350];
		long int unsure_gamePosYUnderMouse; // +36C. mouse pos or game pos (or temp variable ?)
		long int unsure_gamePosXUnderMouse; // +370. mouse pos or game pos (or temp variable ?)

		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_UI_PLAYING_ZONE) || (this->checksum == 0x0054A840); }
	};


	// Size=0x28. No constructor (this is a temporary struct). See init in 4F0EEC for example.
	class STRUCT_UI_UNIT_BUTTON_INFO {
	public:
		char *name;
		short int DATID; // +4. Can be unitDefId, researchId
		short int iconId; // +6.
		short int costType1; // +8. A resourceId
		short int unused_0A;
		float costAmount1; // +C
		// 0x10
		short int costType2; // +10. A resourceId
		short int unused_12;
		float costAmount2; // +14
		short int costType3; // +18. A resourceId
		short int unused_1A;
		float costAmount3; // +1C
		// 0x20
		long int languageDllHelp; // +20. Id for language(x).dll
		long int languageDllHotkeyText; // +24. Id of hotkey for language(x).dll
	};
	static_assert(sizeof(STRUCT_UI_UNIT_BUTTON_INFO) == 0x28, "STRUCT_UI_UNIT_BUTTON_INFO size");


	// Size 0x948. Constructor 0x48FC40
	// This is the parent UI object of in-game screen.
#define CHECKSUM_UI_SCENARIO_EDITOR_MAIN 0x00547360
	class STRUCT_UI_SCENARIO_EDITOR_MAIN : public STRUCT_ANY_UI { // 60 73 54 00
	public:
		long int hWnd; // TO CONFIRM
		char unknown_0F8[0x4A8 - 0x0F8];
		long int currentMapGenerationChoice; // +4A8. 1=empty, 2=random, 3=from seed
		long int unknown_04AC; // default 2
		unsigned long int unknown_04B0;
		unsigned long int unknown_04B4;
		long int selectedPlayerId; // +4B8. Default 1
		unsigned long int unknown_04BC;
		// 0x4C0
		char unknown_04C0; // default 0
		char unknown_04C1; // default 0
		short int unknown_04C2; // unused??
		unsigned long int unknown_04C4;
		STRUCT_UI_PLAYING_ZONE *gamePlayUIZone; // +4C8. 88 66 54 00.
		STRUCT_UI_DIAMOND_MAP *diamondMap; // +4CC. ptr to 3C A7 54 00.
		// 0x4D0
		unsigned long int unknown_04D0;
		STRUCT_ANY_UI *unknown_04D4; // *Really* UI base class (D8 4A 54 00)
		STRUCT_UI_BUTTON *btnTabs[10]; // +4D8. Map, terrain, players, units, dipl // vict1, vict2, options, messages, animation
		// 0x500
		STRUCT_UI_BUTTON *btnMenu; // +500. Top right.
		STRUCT_UI_BUTTON *btnHelp; // +504. In bottom-right corner.
		STRUCT_UI_BUTTON *map_btn_emptyMap; // +508. Checkbox
		STRUCT_UI_BUTTON *map_btn_random; // +50C. Checkbox
		STRUCT_UI_BUTTON *map_btn_fromSeed; // +510. Checkbox
		STRUCT_UI_LABEL *map_lbl_btnEmpty; // +514
		STRUCT_UI_LABEL *map_lbl_btnRandom; // +518
		STRUCT_UI_LABEL *map_lbl_btnSeed; // +51C
		STRUCT_UI_LABEL *map_lbl_map; // +520
		STRUCT_UI_LABEL *map_lbl_cbbDefaultTerrain; // +524
		STRUCT_ANY_UI *map_cbb_defaultTerrain; // +528. Combobox
		STRUCT_UI_LABEL *map_lbl_mapSize; // +52C
		STRUCT_UI_COMBOBOX *map_cbb_mapSize; // +530. Combobox
		STRUCT_UI_LABEL *map_lbl_mapType; // +534
		STRUCT_UI_COMBOBOX *map_cbb_mapType; // +538
		STRUCT_UI_LABEL *map_lbl_seed_whenEditable; // +53C
		STRUCT_ANY_UI *map_edt_seed; // +540
		STRUCT_UI_LABEL *map_lbl_seed_whenReadOnly; // +544
		STRUCT_UI_LABEL *map_edt_seed_whenReadOnly; // +548
		STRUCT_UI_BUTTON *map_btn_generateMap; // +54C
		STRUCT_UI_LABEL *map_lbl_bigGeneratingMapNotification; // +550. The big centered message while generating.
		STRUCT_UI_LABEL *trn_lbl_pencilType; // +554
		STRUCT_UI_BUTTON *trn_btn_pencilSmallest; // +558
		STRUCT_UI_BUTTON *trn_btn_pencilSmall; // +55C
		STRUCT_UI_BUTTON *trn_btn_pencilMedium; // +560
		STRUCT_UI_BUTTON *trn_btn_pencilLarge; // +564
		STRUCT_UI_BUTTON *trn_btn_pencilLargest; // +568
		STRUCT_UI_LABEL *trn_lbl_pencilSmallest; // +56C
		STRUCT_UI_LABEL *trn_lbl_pencilSmall; // +570
		STRUCT_UI_LABEL *trn_lbl_pencilMedium; // +574
		STRUCT_UI_LABEL *trn_lbl_pencilLarge; // +578
		STRUCT_UI_LABEL *trn_lbl_pencilLargest; // +57C
		STRUCT_UI_LABEL *unknown_580;
		STRUCT_UI_BUTTON *trn_btn_map; // +584. To edit terrain
		STRUCT_UI_BUTTON *trn_btn_elevation; // +588. To edit elevation
		STRUCT_UI_BUTTON *trn_btn_cliffs; // +58C. cliffs button
		STRUCT_UI_LABEL *trn_lbl_map; // +590. "Map" in terrain tab (actually, to edit terrain)
		STRUCT_UI_LABEL *trn_lbl_elevation;
		STRUCT_UI_LABEL *trn_lbl_cliffs;
		STRUCT_UI_LABEL *unknown_59C;
		STRUCT_UI_LISTBOX *trn_lst_terrainList; // +5A0. List of terrains
		STRUCT_ANY_UI *trn_scroll_terrainList; // scroll for trn_lst_terrainList ? B8 56 54 00
		STRUCT_UI_LISTBOX *trn_lst_elevationList; // +5A8
		STRUCT_ANY_UI *unknown_5AC; // 56b8
		STRUCT_UI_LABEL *pl_lbl_playerCount;
		STRUCT_UI_COMBOBOX *pl_cbb_selectedPlayer; // +5B4. In Players tab, current player selection.
		STRUCT_UI_COMBOBOX *pl_cbb_playerCount; // +5B8. Number of players combobox.
		STRUCT_ANY_UI *unknown_5BC[(0x640 - 0x5BC) / 4]; // TO DO
		unsigned long int unknown_640; // +640 healthSlp ?
		unsigned long int unknown_644; // +644 itemiconSLP ?
		STRUCT_SLP_INFO *iconsForUnits; // +648.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet0; // +64C.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet1; // +650.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet2; // +654.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet3; // +658.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet4; // +65C. Not well supported ?
		STRUCT_UI_EDITOR_UNIT_INFO_ZONE *selectedUnitInfoZone; // +660.
		long int unitListElementCount; // +664
		STRUCT_UI_BUTTON *gv_btn_custom_exploration; // +668
		STRUCT_UI_BUTTON *gv_btn_custom_ruins; // +66C
		STRUCT_UI_BUTTON *gv_btn_custom_relics; // +670
		STRUCT_UI_BUTTON *gv_btn_custom_discoveries; // +674
		STRUCT_ANY_UI *unknown_678[(0x928 - 0x678) / 4]; // TO DO
		STRUCT_GAME_GLOBAL *global; // +928
		long int unknown_92C; // Default 200
		long int unknown_930_timeGetTimeValue; // +930. TimeGetTime result value
		long int unknown_934; // Default 1000
		long int unknown_938_timeGetTimeValue; // +938. TimeGetTime result value
		unsigned long int unknown_93C;
		unsigned long int unknown_940;
		unsigned long int unknown_944;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN; }
	};
	static_assert(sizeof(STRUCT_UI_SCENARIO_EDITOR_MAIN) == 0x948, "STRUCT_UI_SCENARIO_EDITOR_MAIN size");


	// Size=0x164. Constructor=0x4FA150?
#define CHECKSUM_UI_F11_POP_PANEL 0x00549F6C
	class STRUCT_UI_F11_POP_PANEL : public STRUCT_ANY_UI {
	public:
		long int unknown_0F4_font; // +F4.
		long int unknown_0F8;
		long int colorRGB; // +0xFC
		char unknown_100[0x110 - 0x100];
		// 0x110
		long int currentPopulation; // +110
		long int currentMaxHostedPopulation; // +114. Maximum population currently supported by houses
		char unknown_118;
		char unknown_119[3]; // unused ?
		char unknown_11C[0x164 - 0x11C];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_F11_POP_PANEL; }
	};
	static_assert(sizeof(STRUCT_UI_F11_POP_PANEL) == 0x164, "STRUCT_UI_F11_POP_PANEL size");


#define CHECKSUM_UI_UNIT_INFO_ZONE 0x00549E7C
#define CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE 0x00547CC0
	// Size=0x214. Constructor=0x4F83D0 = unitInfoZone.constructor(arg1, mainGameUI, arg3, arg4, arg5, iconsForTrainUnit, iconsForBuildings, controlledPlayer)
	// Methods: 0x4F9970=unitInfoZone.refreshIfNeeded(), 0x4F8740=UnitInfoZone.showInfos(self?, mainGameZone?)
	class STRUCT_UI_UNIT_INFO_ZONE : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4; // +F4 : font info. (=fontObj+0)
		unsigned long int unknown_0F8; // +F8 : font info. (=fontObj+4)
		unsigned long int unknown_0FC; // +FC : font info. (=fontObj+8)
		STRUCT_SLP_INFO *healthSlp; // +100. Health.shp, slpId=0xC639
		STRUCT_SLP_INFO *itemIconSlp; // +104. itemicon.shp, slpid=0xC62B
		STRUCT_SLP_INFO *iconsForTrainUnits; // +108. Check type (STRUCT_SLP_INFO ok, but is pointer ok ?)
		STRUCT_SLP_INFO **iconsForBuildings; // +10C. ArraySize=5 (tilesets)? Check type (STRUCT_SLP_INFO ok, but is pointer ok ?)
		STRUCT_PLAYER *controlledPlayer; // +110.
		STRUCT_UNIT_BASE *currentUnit; // +114
		long int unknown_118;
		long int unknown_11C; // Init = 0x1F4.
		char unknown_120; // +120. 0,1, 2 (enum ?)
		char unknown_121[3]; // unused?
		STRUCT_UNITDEF_BASE *currentUnitDef; // +124. Unit definition of selected unit
		unsigned long int unknown_128;
		short int currentUnitResourceTypeId; // +12C
		short int unknown_12E;
		float currentUnitResourceAmount; // +130
		char unknown_134;
		char unknown_135[3]; // unused?
		char unknown_138[0x13C - 0x138];
		unsigned long int *unknown_13C; // +13C. Pointer to ? First garrisoned unit ?
		short int garrisonedUnitCount; // +140. To confirm
		short int unknown_142;
		unsigned long int unknown_144;
		unsigned long int unknown_148;
		float currentUnitHP; // +14C
		short int displayedMeleeArmorValue; // +150. Only used to know if refresh is needed, not used for actual display.
		short int displayedPierceArmorValue; // +152. Only used to know if refresh is needed, not used for actual display.
		short int displayedAttack; // +154.  Only used to know if refresh is needed, not used for actual display.
		short int unknown_156; // +156. unused ?
		long int maxRange; // +158. The TOTAL unit range. Only used to know if refresh is needed, not used for actual display.
		float reloadTime1; // +15C. 
		short int unknown_160;
		short int unknown_162;
		short int unknown_164;
		char unknown_166;
		char unknown_167; // unused ?
		char unknown_168[0x1C8 - 0x168];
		short int unknown_1C8;
		char unknown_1CA;
		char unknown_1CB; // unused ?
		long int unknown_1CC;
		float currentUnitLineOfSight; // +1D0
		long int unknown_1D4;
		long int unknown_1D8;
		char currentUnitShortcutNumber; // +1DC
		char unknown_1DD[3];
		STRUCT_PLAYER *currentUnitPlayer; // +1E0.
		unsigned long int unknown_1E4;
		char currentUnitHasAnAction; // +1E8
		char unknown_1E9[3];
		STRUCT_SLP_INFO *unknown_1EC_SLP[9]; // +1EC.
		long int unknown_210; // +210

		bool IsCheckSumValid() { return
			(this->checksum == CHECKSUM_UI_UNIT_INFO_ZONE) || (this->checksum == CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE);
		}
	};
	static_assert(sizeof(STRUCT_UI_UNIT_INFO_ZONE) == 0x214, "STRUCT_UI_UNIT_INFO_ZONE size");

	// Size 0x21C Constructor 0x4A42A0
	class STRUCT_UI_EDITOR_UNIT_INFO_ZONE : public STRUCT_UI_UNIT_INFO_ZONE {
		long int currentUnitDefId; // +214
		long int currentTileSet; // +218. 0-4.
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE; }
	};
	static_assert(sizeof(STRUCT_UI_EDITOR_UNIT_INFO_ZONE) == 0x21C, "STRUCT_UI_EDITOR_UNIT_INFO_ZONE size");


	// Size 0x7C8
	// Constructor 0x47D440
	// This is the parent UI object of in-game screen.
	// Pointed by 005830F4
#define CHECKSUM_UI_IN_GAME_MAIN 0x0054679C
	class STRUCT_UI_IN_GAME_MAIN : public STRUCT_ANY_UI { // "Game Screen"
	public:
		long int hWnd; // TO CONFIRM
		char unknown_0F8[0x48C - 0x0F8];
		STRUCT_SLP_INFO *iconsForUnitCommands; // +48C. Pointer to SLP data... Cf SLP 50721
		STRUCT_SLP_INFO *iconsForResearches; // +490. Pointer to SLP data... Cf SLP 50729
		STRUCT_SLP_INFO *iconsForTrainUnits; // +494. Pointer to SLP data... Cf SLP 50730.
		STRUCT_SLP_INFO *iconsForBuildings[5]; // +498 + tileset*4. There is no free slot for more tilesets. SLPId= C610+i except tileset4=>CF0D.
		STRUCT_SLP_INFO *unknown_4AC_icons; // +4AC. Used in 48250F. Includes the "cancel" icon, id=10. SLP 50725 ?
		short int unknown_4B0_iconId; // +4B0. Icon id for Next page button ?
		short int unknown_4B2_iconId; // +4B2. Icon id for unselect button ?
		unsigned long int unknown_4B4;
		STRUCT_UI_PLAYING_ZONE *gamePlayUIZone; // +4B8. 88 66 54 00.
		STRUCT_UI_DIAMOND_MAP *diamondMap; // +4BC. ptr to F4 A3 54 00.
		// 0x4C0
		unsigned long int unknown_4C0; // Struct size=0x118, constructor=4F7C00. 98 9D 54 00
		unsigned long int unknown_4C4;
		STRUCT_UI_BUTTON_WITH_NUMBER *unitCommandButtons[12]; // +4C8. 2 rows of 6 slpButtons for command buttons in bottom center zone.
		STRUCT_UI_BUTTON_WITH_NUMBER *btnChat; // +4F8. Also referred as commandButtons[0xC]. Not visible in SP games.
		STRUCT_UI_BUTTON_WITH_NUMBER *btnDiplomacy; // +4FC. Also referred as commandButtons[0xD]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnMenu; // +500. Also referred as commandButtons[0xE]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnHelp; // +504. Also referred as commandButtons[0xF]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnShowScores; // +508. Also referred as commandButtons[0x10]
		char unknown_50C[0x518 - 0x50C];
		STRUCT_ANY_UI *unknown_518[10]; // cf 47F753
		STRUCT_UI_F11_POP_PANEL *populationInfoPanel; // 0x540. F11 "pop : x/y" zone ?
		STRUCT_UI_IN_GAME_TEXT_ZONE *ingameErrorTextZone; // 0x544
		char unknown_548[0x55C - 0x548];
		STRUCT_UI_IN_GAME_TEXT_ZONE *ingameChatTextZone[8]; // 0x55C ; check count !
		STRUCT_ANY_UI *unknown_57C; // 50 5D 54 00
		char unknown_580[0x5B4 - 0x580];
		STRUCT_GAME_GLOBAL *gameGlobal; // +5B4.
		char unknown_5B8[0x5C8 - 0x5B8];
		STRUCT_UNIT *panelSelectedUnit; // 0x5C8. Pointer to unit selected in unit info zone.
		char unknown_5CC[0x5D0 - 0x5CC];
		STRUCT_UI_UNIT_BUTTON_INFO *tmpUnitBtnCommandsInfo_researches; // +5D0. Pointer to array[]. Only for researches ! Used in internal treatments when showing buttons.
		STRUCT_UI_UNIT_BUTTON_INFO *tmpUnitBtnCommandsInfo_units; // +5D4. Pointer to array[]. Only for train units ! Used in internal treatments when showing buttons.
		unsigned long int unknown_5D8;
		unsigned long int unknown_5DC;
		// 0x5E0
		short int panelDisplayedButtonCount; // Total number of displayed buttons in button bar (unit commands)
		short int panelButtonIdPageOffset; // +5E2. 0=first page, 10=second page. No other possibility in standard game.
		short int panelLastButtonId; // +5E4. Maximum button id for last page.
		short int unknown_5E6; // unused ?
		char unknown_5E8[0x724 - 0x5E8]; // includes unused fields ?
		long int currentChatTextLine;
		char unknown_728[0x734 - 0x728];
		long int unknown_debugText_strlen; // +734
		char unknown_debugText[0x34];
		char shortcutKeySelectTownCenter[4]; // +76C. = 'H' = select TC and center screen.
		char shortcutKeyunused_villagerWork[4]; // +770. = 0 (unused) = Work, for a villager.
		char shortcutKeyMove[4]; // +774. = 'M'. Key to tell a unit to move.
		char shortcutKeyUnused_attack[4]; // +778. = 0 (unused) = Attack a unit (click to select target)
		char shortcutKeyUngroupUnits[4]; // +77C. = 'D' = ungroup units
		char shortcutKeyA_780[4]; // +780. = 'A'. Unused ?
		char shortcutKeyN[4]; // +784. = 'N'
		char shortcutKeyC[4]; // +788. = 'C'
		char shortcutKeyP[4]; // +78C. = 'P'
		char shortcutKeyA_790[4]; // +790. = 'A'
		char shortcutKeyS[4]; // +794. = 'S'
		char shortcutKeyE[4]; // +798. = 'E'
		char shortcutKeyT[4]; // +79C. = 'T'
		char shortcutKeyI[4]; // +7A0. = 'I'
		char unknown_7A4[0x7C4 - 0x7A4];
		long int panelSelectedUnitId; // +7C4. Selected unit ID (in unit details zone)

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_MAIN; }
	};
	static_assert(sizeof(STRUCT_UI_IN_GAME_MAIN) == 0x7C8, "STRUCT_UI_IN_GAME_MAIN size");


	// Size = 0x560. 10 78 54 00. Parent=9C 57 54 00 then BC 4B 54 00.
	// The screen before starting game to choose map type, etc ("advanced" options like map size, etc).
	// Constructor:0x49BF40
#define CHECKSUM_UI_GAME_SETTINGS 0x00547810
	class STRUCT_UI_GAME_SETTINGS : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblScenarioSettings;
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

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_GAME_SETTINGS; }
	};

	// In-game diplomacy menu
	// Size = 0x6B8
#define CHECKSUM_UI_DIPLOMACY_DIALOG 0x00543814
	class STRUCT_UI_DIPLOMACY_DIALOG : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x490 - 0x0F4];
		// 0x490
		STRUCT_ANY_UI *lblDiplomacy;
		STRUCT_ANY_UI *lblName;
		STRUCT_ANY_UI *lblCivilization;
		STRUCT_ANY_UI *lblAlly;
		// 0x4A0
		STRUCT_ANY_UI *lblNeutral;
		STRUCT_ANY_UI *lblEnemy;
		STRUCT_ANY_UI *lblPayTribute;
		STRUCT_ANY_UI *lblAlliedVictory;
		// 0x4B0
		char unknown_4B0[0x510 - 0x4B0];
		// 0x510
		STRUCT_ANY_UI *lblNeedMarketToPayTribute;
		char unknown_514[0x5F4 - 0x514];
		STRUCT_ANY_UI *edtAlliedVictory; // +5F4. Checkbox
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnClearTributes;
		// 0x600
		STRUCT_UI_BUTTON *btnCancel;
		unsigned long int unknown_604;
		char unknown_608[0x620 - 0x608];
		// 0x620
		STRUCT_ANY_UI *lblFoodTribute;
		STRUCT_ANY_UI *lblGoldTribute;
		STRUCT_ANY_UI *lblWoodTribute;
		STRUCT_ANY_UI *lblStoneTribute;
		// 0x630
		char unknown_630[0x6B8 - 0x630];

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_DIPLOMACY_DIALOG; }
	};


	// Size=0x564. In-game Options popup.
	// Constructor = 0x4300D0
#define CHECKSUM_UI_IN_GAME_OPTIONS 0x005436E0
	class STRUCT_UI_IN_GAME_OPTIONS : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x490 - 0x0F4];
		// 0x490
		unsigned long int unknown_490; // Init with constructor's arg1
		STRUCT_ANY_UI *lblTitle;
		STRUCT_ANY_UI *lblSpeed;
		STRUCT_ANY_UI *lblNormalSpeed;
		// 0x4A0
		STRUCT_ANY_UI *lblFastSpeed;
		STRUCT_ANY_UI *lblVeryFastSpeed;
		STRUCT_ANY_UI *edtNormalSpeed; // checkbox
		STRUCT_ANY_UI *edtFastSpeed;
		// 0x4B0
		STRUCT_ANY_UI *edtVeryFastSpeed;
		STRUCT_ANY_UI *lblMusicVolume;
		STRUCT_UI_BUTTON *btnMusicVolumeMore; // ?
		STRUCT_UI_BUTTON *btnMusicVolumeLess;
		// 0x4C0
		STRUCT_UI_BUTTON *btnMusicVolumeSelector; // scroller
		STRUCT_ANY_UI *lblSoundVolume;
		STRUCT_UI_BUTTON *btnSoundVolumeMore;
		STRUCT_UI_BUTTON *btnSoundVolumeLess;
		// 0x4D0
		STRUCT_UI_BUTTON *btnSoundVolumeSelector; // scroller
		STRUCT_ANY_UI *lblScrollSpeed;
		STRUCT_UI_BUTTON *btnScrollSpeedFaster;
		STRUCT_UI_BUTTON *btnScrollSpeedSlower;
		// 0x4E0
		STRUCT_UI_BUTTON *btnScrollSpeedSelector; // scroller
		STRUCT_ANY_UI *lblScreenSize;
		STRUCT_ANY_UI *lblScreenSize1; // 640*480
		STRUCT_ANY_UI *lblScreenSize2; // 800*600
		// 0x4F0
		STRUCT_ANY_UI *lblScreenSize3; // 1024*768
		STRUCT_ANY_UI *edtScreenSize1; // checkbox
		STRUCT_ANY_UI *edtScreenSize2;
		STRUCT_ANY_UI *edtScreenSize3;
		// 0x500
		STRUCT_ANY_UI *lblMouseInterface;
		STRUCT_ANY_UI *lblTwoButtons;
		STRUCT_ANY_UI *lblOneButton;
		STRUCT_ANY_UI *edtTwoButtons;
		// 0x510
		STRUCT_ANY_UI *edtOneButton;
		STRUCT_ANY_UI *lblContextHelp;
		STRUCT_ANY_UI *lblContextHelpOn;
		STRUCT_ANY_UI *lblContextHelpOff;
		// 0x520
		STRUCT_ANY_UI *edtContextHelpOn;
		STRUCT_ANY_UI *edtContextHelpOff;
		STRUCT_ANY_UI *lblPathFinding;
		STRUCT_ANY_UI *lblPathFindingDefault;
		// 0x530
		STRUCT_ANY_UI *lblPathFindingMedium;
		STRUCT_ANY_UI *lblPathFindingHigh;
		STRUCT_ANY_UI *edtPathFindingDefault;
		STRUCT_ANY_UI *edtPathFindingMedium;
		// 0x540
		STRUCT_ANY_UI *edtPathFindingHigh;
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		long int musicInitialVolume;
		// 0x550
		long int soundInitialVolume;
		unsigned long int unknown_554;
		unsigned long int unknown_558;
		long int musicVolumeLevelValue; // values 0-99 (99=lowest)
		// 0x560
		long int soundVolumeLevelValue; // values 0-99 (99=lowest)
		// 0x564: end
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_OPTIONS; }
	};


	// Size ?
	class STRUCT_UI_LOAD_SAVED_GAME : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblSavedGames;
		STRUCT_ANY_UI *lblSelectSavedGame;
		// 0x480
		STRUCT_ANY_UI *edtSavedGameList;
		STRUCT_ANY_UI *unknown_484; // type a name for the game?
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		// 0x490
		STRUCT_UI_BUTTON *btnDelete;

		//bool IsCheckSumValid() { return this->checksum == 0x0054xxxx; }
	};


	// Size - Constructor = 
	// This is the "welcome" screen with 3 buttons (NOT menu in scenario editor screen)
	class STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblScenarioBuilder;
		STRUCT_UI_BUTTON *btnCreateScenario;
		// 0x480
		STRUCT_UI_BUTTON *btnEditScenario;
		STRUCT_UI_BUTTON *btnCampaignEditor;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnUnknown_48C;

		//bool IsCheckSumValid() { return this->checksum == ; }
	};


	// Size 0x4B4 - Constructor = 0x4349A0 - 20 3E 54 00
	// "Scenario Menu Dialog"
#define CHECKSUM_UI_SCENARIO_EDITOR_MENU 0x00543E20
	class STRUCT_UI_SCENARIO_EDITOR_MENU : public STRUCT_ANY_UI {
	public:
		// +F4...
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MENU; }
	};


	// Size ?
	class STRUCT_UI_CAMPAIGN_EDITOR : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x47C - 0x0F4];
		STRUCT_ANY_UI *lblCampaignEditor;
		// 0x480
		STRUCT_ANY_UI *lblCampaignFilename;
		STRUCT_ANY_UI *lblScenarios;
		STRUCT_ANY_UI *lblCampaignScenarios;
		unsigned long int unknown_48C;
		// 0x490
		unsigned long int unknown_490;
		unsigned long int unknown_494;
		unsigned long int unknown_498;
		STRUCT_ANY_UI *unknown_49C_defaultCampaignInputDialog;
		// 0x4A0
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnSave;
		STRUCT_UI_BUTTON *btnAdd;
		// 0x4B0
		STRUCT_UI_BUTTON *btnRemove;
		STRUCT_UI_BUTTON *btnUp;
		STRUCT_UI_BUTTON *btnDown;

		//bool IsCheckSumValid() { return this->checksum == 0x0054xxxx; }
	};


	class STRUCT_UI_DIAM_MAP : public STRUCT_ANY_UI { // F4 A3 54 00
	public:
		char unknown_0F4[0x130 - 0x0F4];
		// 0x130
		short int mapSizeX; // unsure
		short int mapSizeY; // unsure
		

		bool IsCheckSumValid() { return this->checksum == 0x0054A3F4; }
	};

	// Size = 0x4B0. Constructor = 0x47B440. 08 62 54 00
	// "Join Screen" = MP search game screen.
	class STRUCT_UI_JOIN_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x00546208; }
	};

	// Size = . Constructor = . F8 69 54 00
	// OnKeyPress = 0x486C10
	class STRUCT_UI_WELCOME_MAIN_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x005469F8; }
	};

	// Size = 0x8B8. Constructor = 0x47C510. 68 64 54 00
	class STRUCT_UI_CREDITS_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x00546468; }
	};

	// Size 0x14 ?
	// This struct is used internally to get position info (when clicking, for example)
	// Often it is in stack, (SUB ESP, ...), not allocated
	class STRUCT_TEMP_MAP_POSITION_INFO {
	public:
		float posY;
		float posX;
		unsigned short int unknown_08;
		unsigned short int unknown_0A;
		STRUCT_UNIT_BASE *unit;
		unsigned long int unknown_10;
	};




	static long int GetUnitStructRealSize(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
		if (unit->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_BIRD*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_FLAG*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_DEAD_FISH*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_LIVING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_TYPE50*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		// unit->unitType is often erroneous ! If not found, try again using only checksum.
		if (unit->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_BIRD*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_FLAG*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_DEAD_FISH*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_LIVING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_TYPE50*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		return 0;
	}

}

