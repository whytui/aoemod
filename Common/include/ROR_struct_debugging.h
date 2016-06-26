#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <ROR_structures.h>
#include <yvals.h> // for __STRINGIZE
#include <Windows.h>
#include <WinUser.h>
#include <AOE_offsets.h>
#include <assert.h>
#include <string>
#include <ROR_struct_export.h>
#include <ROR_struct_export_remote.h>

/* This file does not provide methods to do any treatments, only tools for debugging.
*/

namespace ROR_STRUCTURES_10C
{
	long int GuessRORStructureSize(void *pData);
	char *GuessRORStructure(void *pData);

	char *DebugStructAtAddress(HANDLE h, unsigned long int address);


	class RORDebugger {
	public:
		// Handle to ROR process. Set this before calling various methods.
		HANDLE handleROR;
		// Copy of game settings structure.
		unsigned long int gameSettingsAddr;
		ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS gameSettings;
		// Copy of game global structure
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL gameGlobal;
		// Copy of game map info
		ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO gameMapInfo;
		// Copy of map visibility info
		ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO *mapVisibilityInfoRows[255];
		ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO mapVisibilityInfo[255][255];
		// Copy of map build like info (tmp)
		ROR_STRUCTURES_10C::STRUCT_TEMP_MAP_BUILD_LIKE_INFOS tempMapBuildLikeInfo;
		// Copy of active UI struct (base class info only)
		ROR_STRUCTURES_10C::STRUCT_ANY_UI currentUIStruct;
		// Copies of player structures
		unsigned long int playerAddresses[9];
		ROR_STRUCTURES_10C::STRUCT_PLAYER players[9];
		ROR_STRUCTURES_10C::STRUCT_AI playersAI[9];
		// Copy of scenario info structure
		ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO scenarioInfo;
		// TO DO
		STRUCT_MP_COMMUNICATION unknown_0x580DA8;
		long int unknown_0x6A18C0[0x100];

		RORDebugger();

		bool RefreshMainGameStructs();
		// Creates a temporary copy of desired unit (from its ID).
		// Returns NULL if not found / failed.
		// You MUST free the object when you no longer need it.
		ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *GetUnitFromRORData(long int unitId, unsigned long int &out_RORAddress);
		// Creates a temporary copy of desired unit definition (from its ID/player).
		// Returns NULL if not found / failed.
		// You MUST free the object when you no longer need it.
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *GetUnitDefFromRORData(long int playerId, long int unitDefId, unsigned long int &out_RORAddress);
		// Export to text main structures information
		std::string ExportMainStructInfo();
		// Export to text all info about an object (copied in a local object, but underlying pointers still refer to ROR process objects)
		template<class T> std::string ExportHostedRORObject(T *object, unsigned long int actualRORAddress) {
			ROR_STRUCTURES_10C::ROR_STRUCT_EXPORTER_REMOTE exporter;
			exporter.handleROR = this->handleROR;
			return exporter.ExportHostedStruct(object, actualRORAddress);
		}
		// Export to text all info about an object using a ROR pointer (*object has no meaning from this code's point of view, but only in ROR process context)
		template<class T> std::string ExportRemoteRORObject(T *object) {
			ROR_STRUCTURES_10C::ROR_STRUCT_EXPORTER_REMOTE exporter;
			exporter.handleROR = this->handleROR;
			return exporter.ExportStruct(object);
		}

		std::string GetUnitName(long int unitId);

		// Return a string with list of <unitID>(<plId>.<name>)
		std::string GetUnitNamesFromArrayOfID(unsigned long int RORUnitIdArrayAddress, long int elemCount);

	private:
		bool RefreshGameSettings();
		bool RefreshGameGlobal();
		bool RefreshGameMapInfo();
		bool RefreshMapVisibilityInfo();
		bool RefreshTempMapBuildLikeInfo();
		bool RefreshCurrentUIObjectBase();
		bool RefreshScenarioInfo();
		bool RefreshPlayers();
		bool RefreshUnknown();
	};
}

