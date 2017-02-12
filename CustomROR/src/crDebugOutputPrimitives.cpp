
#ifdef _DEBUG
#include "../include/crDebugOutputPrimitives.h"

/* This file contains various primitives for debugging */


using namespace AOE_STRUCTURES;

namespace CR_DEBUG {


void DumpDebugInfoToFile() {
	char buf[] = "Error opening debug file :       ";
	FILE *f;
	int res = fopen_s(&f, "F:\\AOEDebug.txt", "w+"); // overwrite if already existing
	if (res) {
		sprintf_s(buf + 28, 5, "%d", res);
		//CallWriteText(buf);
		return;
	}

	AOE_STRUCTURES::AOE_STRUCT_EXPORTER writer;
	std::string objToString = "";

	fprintf_s(f, "AOE Debug information\n\n");
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settingsStruct = *ROR_gameSettings;
	fprintf_s(f, "Global struct = 0x%08lX - %d players (including gaia)\n", (long int)globalStruct, globalStruct->playerTotalCount);
	fprintf_s(f, "Game settings = 0x%08lX - Map seed=%d - Deathmatch=%d\n", (long int)settingsStruct, settingsStruct->actualMapSeed, settingsStruct->isDeathMatch);
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		//fprintf_s(f, "Player %d struct=0x%08lX - Resources=0x%08lX - AI=0x%08lX - %-16s\n", i, (long int)currentPlayer, currentPlayer->ptrResourceValues, currentPlayer->ptrAIStruct, currentPlayer->playerName_length16max);
		
		objToString = writer.ExportStruct(currentPlayer);
		fprintf_s(f, objToString.c_str());
	}

	// Diplomacy values
	fprintf_s(f, "\n*** Diplomacy values ***\n");
	fprintf_s(f, "\nDislike vs p0  p1  p2  p3  p4  p5  p6  p7  p8  - Like vs p0  p1  p2  p3  p4  p5  p6  p7  p8  \n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		AOE_STRUCTURES::STRUCT_AI *ai = currentPlayer->ptrAIStruct;
		if (ai == NULL) {
			fprintf_s(f, "[No AI structure for player #%d.]\n", i);
		} else {
			fprintf_s(f, "player %d :", i);
			for (int iTargetPlayer = 0; iTargetPlayer < 9; iTargetPlayer++) {
				fprintf_s(f, " %-3ld", ai->structDiplAI.dislikeTable[iTargetPlayer]);
			}
			fprintf_s(f, " -        ");
			for (int iTargetPlayer = 0; iTargetPlayer < 9; iTargetPlayer++) {
				fprintf_s(f, " %-3ld", ai->structDiplAI.likeTable[iTargetPlayer]);
			}
			fprintf_s(f, "\n");
		}
	}

	// Strategy
	fprintf_s(f, "\n*** Strategies ***\n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		fprintf_s(f, "\nPlayer %d:\nCounter Class DAT_ID Name                           Actor Unitid InProgress Alive Attempts #created Retrains Ptr\n", i);
		AOE_STRUCTURES::STRUCT_AI *ai = currentPlayer->ptrAIStruct;
		if (ai == NULL) {
			fprintf_s(f, "No AI structure for this player.\n");
		} else {
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &ai->structBuildAI.fakeFirstStrategyElement;
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
			while (currentStratElem && (currentStratElem != fakeFirstStratElem)) {
				fprintf_s(f, "%03ld     %1ld     %-3ld    %-30s %-3ld   %-6ld %-2ld         %-2ld    %-4ld     %-4ld     %-3ld      0x%08lX\n", currentStratElem->elemId, currentStratElem->elementType, currentStratElem->unitDAT_ID,
					currentStratElem->unitName, currentStratElem->actor, currentStratElem->unitInstanceId, currentStratElem->inProgressCount,
					currentStratElem->aliveCount, currentStratElem->buildAttempts, currentStratElem->totalCount, currentStratElem->retrains, (long int)currentStratElem
					);
				currentStratElem = currentStratElem->next;
			}
		}
	}

	fprintf_s(f, "\n*** SN values ***\n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		fprintf_s(f, "\nPlayer %d:\n", i);
		AOE_STRUCTURES::STRUCT_AI *ai = currentPlayer->ptrAIStruct;
		if (ai == NULL) {
			fprintf_s(f, "No AI structure for this player.\n");
		} else {
			for (int sn = 0; sn < 227; sn++) {
				fprintf_s(f, "SN%-3ld   value=%ld\n", sn, ai->structTacAI.SNNumber[sn]);
			}
		}
	}
	fclose(f);
}


// Export InfAI's exploration status map data to a bitmap using color codes
bool exportInfAIExplorationToBitmap(STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	long int mapSizeX = player->ptrAIStruct->structInfAI.XMapSize;
	long int mapSizeY = player->ptrAIStruct->structInfAI.YMapSize;
	if (player->ptrAIStruct && (mapSizeX > 0) && (mapSizeY > 0)) {
		const long int paletteSize = 4 * 4; // 4 colors, 4 bytes/DWORD
		unsigned char myPalette[paletteSize];
		memset(myPalette, 0, paletteSize);
		for (int i = 0; i < 3; i++) {
			myPalette[0 * 4 + i] = 127; // grey for -1 (unexplored)
		}
		myPalette[1 * 4 + 1] = 255; // Green for 0 (explored)
		myPalette[2 * 4 + 2] = 255; // Red for 1 (failed)
		myPalette[3 * 4 + 0] = 255; // Blue for 2 (to reexplore)

		_BITMAP::BitmapExporter::ExportDataColumnsAsBitmapUsingPalette("D:\\test.bmp", mapSizeX, mapSizeY,
			player->ptrAIStruct->structInfAI.mapExplorationInfo.ptrColsPtr, -1, myPalette, paletteSize);
	}
	return true;
}


// Exports the STRUCT_GAME_TERRAIN_RESTRICTION_INFO data as a bitmap
bool exportGameTerrainRestrictionValuesToBitmap() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	if (!global->gameMapInfo->terrainZonesInfoLink) { return false; }
	if ((global->gameMapInfo->terrainZonesInfoLink->ptrTerrainZoneInfoArray == NULL) || (
		global->gameMapInfo->terrainZonesInfoLink->arrayElemCount <= 0)) {
		return false;
	}
	STRUCT_GAME_TERRAIN_ZONES_INFO *gameTerrainRestrInfo = NULL;
	int index = 0;
	// Find the terrain restriction info that matches "game global" : number of terrain per terrain restriction must match
	while ((index < global->gameMapInfo->terrainZonesInfoLink->arrayElemCount) && (gameTerrainRestrInfo == NULL)) {
		gameTerrainRestrInfo = global->gameMapInfo->terrainZonesInfoLink->ptrTerrainZoneInfoArray[index];
		if (gameTerrainRestrInfo) {
			if ((gameTerrainRestrInfo->terrainsCountInTerrainRestrictions <= 0) ||
				(gameTerrainRestrInfo->terrainsCountInTerrainRestrictions != global->nbOfTerrainPerTerrainRestriction)) {
				gameTerrainRestrInfo = NULL;
			}
		}
		index++;
	}
	if (gameTerrainRestrInfo == NULL) { return false; }

	// Found the correct object. Export its map data.
	int sizeX = global->gameMapInfo->mapArraySizeX;
	int sizeY = global->gameMapInfo->mapArraySizeY;
	assert((sizeX > 0) && (sizeY > 0));
	if ((sizeX <= 0) || (sizeY <= 0)) { return false; }

	int maxValue = -1;
	for (int i = 0; i < sizeX * sizeY; i++) {
		int v = gameTerrainRestrInfo->landAndWaterIdentifiersArray[i];
		if (v > maxValue) {
			maxValue = v;
		}
	}

	_BITMAP::BitmapExporter::ExportDataAsBitmapGreyShades("D:\\testtrn0.bmp", sizeX, sizeY,
		gameTerrainRestrInfo->landAndWaterIdentifiersArray, 0, maxValue, true);

	/*_BITMAP::BitmapExporter::ExportDataColumnsAsBitmapUsingPalette("D:\\testtrn.bmp", sizeX, sizeY,
		gameTerrainRestrInfo->landAndWaterIdentifiersCols, 0, myPalette, paletteSize);*/
	return true;
}


// Export map visibility values to a bitmap.
// fogVisibility : true=export fog visibility, false=export exploration
// playerIdFilter : if -1, export for all players (value=mask), otherwise, export for supplied player only
bool exportVisibilityToBitmap(bool fogVisibility, long int playerIdFilter) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }
	if ((playerIdFilter < -1) || (playerIdFilter >= global->playerTotalCount)) { return false; } // -1 is allowed (joker)
	const int mask = (playerIdFilter >= 0) ? (1 << playerIdFilter) : 0; // only used if playerId is not -1.
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_LINK *visLink = global->gameMapInfo->mapVisibilityLink;
	if (!visLink || !visLink->tilesVisibilityArray) { return false; }
	if ((visLink->mapSizeX <= 0) || (visLink->mapSizeY <= 0)) { return false; }
	unsigned char *valuesArray = (unsigned char*)malloc(visLink->mapSizeX * visLink->mapSizeY);
	if (!valuesArray) { return false; }
	for (int i = 0; i < visLink->mapSizeX * visLink->mapSizeY; i++) {
		if (fogVisibility) {
			valuesArray[i] = (visLink->tilesVisibilityArray[i].fogVisibilityMask) & 0xFF;
		} else {
			valuesArray[i] = (visLink->tilesVisibilityArray[i].explorationVisibilityMask) & 0xFF;
		}
		if (playerIdFilter >= 0) {
			valuesArray[i] = valuesArray[i] & mask;
		}
	}
	char *filename = fogVisibility ? "D:\\testfog.bmp" : "D:\\testexpl.bmp";
	_BITMAP::BitmapExporter::ExportDataAsBitmapUsingPalette(filename, global->gameMapInfo->mapArraySizeX,
		global->gameMapInfo->mapArraySizeY, valuesArray, 0, _BITMAP::itfDrsPalette50509, _countof(_BITMAP::itfDrsPalette50509), true);
	free(valuesArray);
	return true;
}



void DebugDumpAllUnits() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) return;
	for (int i = 0; (i < GetGameGlobalStructPtr()->seqUnitId - 1) && (i < GetGameGlobalStructPtr()->unitPointersListSize); i++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = GetGameGlobalStructPtr()->ptrUnitPointersList[i];
		if (unit && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
			char buf[200];
			char buf2[10];
			buf2[0] = '-';
			buf2[1] = 0;
			AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *unitLiving = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)unit;
			if (unitLiving->DerivesFromAttackable()) {
				sprintf_s(buf2, "%d", unitLiving->stillToBeDiscoveredByHuman);
			}
			sprintf_s(buf, "unit %ld\tType=%ld\tDATID=%ld\t1B8=%s\tname=%s\t%d\t%d\t%d\t%d\n", unit->unitInstanceId, unit->unitType,
				unit->unitDefinition->DAT_ID1, buf2, unit->unitDefinition->ptrUnitName,
				unit->unitStatus, unit->isNotCreatable, unit->isDoppleGanger, unit->unknown_04B);
			WriteToFile(buf, "D:\\AOEUnits.txt", true);
		}
	}
}


// For testing purpose
#define thisbufSize 20000
static char buffer[thisbufSize];
char *DumpPosToTextBuffer(AOE_STRUCTURES::STRUCT_MAP_TILE_VALUES *mapInfosStruct, long int posX, long int posY, int radius) {
	if (((radius * 2 + 1) * (radius * 2 + 1) * 3) >= thisbufSize) { return "Buffer is not large enough for this radius."; }
	int pos = 0;
	for (long int i = -radius; i <= radius; i++) {
		for (long int j = -radius; j <= radius; j++) {
			unsigned char value = (unsigned char)mapInfosStruct->GetTileValue(posX + i, posY + j, CST_MAP_BUILD_LIKE_DISABLED);
			if (value == 0xFF) {
				sprintf_s(&buffer[pos], 4, "-1 ");
			} else {
				sprintf_s(&buffer[pos], 4, "%02X ", value);
			}
			pos += 3; // we will overwrite sprintf_s' ending \n
		}
		buffer[pos] = '\n';
		pos++;
	}
	buffer[pos] = '\n'; pos++; // Better with a (heavy) chariot return at end ;)
	buffer[pos] = '\0'; pos++;
	assert(pos < thisbufSize);
	return buffer;
#undef thisbufSize
}


bool debugSerialization = false;
// Write deserialization data into a buffer, then to a log file.
// This affects greatly performance !!! Debug only.
// This uses a buffer system, which is flushed when exiting game.
void WriteDebugLogForDeserializedData(unsigned long int callAddr, unsigned char *buffer, long int bufferSize) {
	static std::string test;
	static int writtenLines = 0;
	if (!buffer || (bufferSize <= 0)) {
		// Flush
		if (writtenLines > 0) {
			writtenLines = 0;
			WriteToFile(test, serializationLogFilename, true);
			test.clear();
		}
		return;
	}
	static long int totalBufferSize = 0; // Gives (next) offset
	static long int previousSerzOffset = 0;
	static long int previousSerzEndOffset = 0; // last "end position, =this begin position (unless object changed)
	long int serzOffset = *AOE_OFFSETS::AOE_CURRENT_OFFSET_IN_FILE_DESERIALIZATION;
	long int tmpEndOffset = serzOffset;
	serzOffset -= bufferSize;
	totalBufferSize += bufferSize;
	if (previousSerzOffset + bufferSize > 0x10000) {
		//test += GetHexStringAddress(totalBufferSize - bufferSize);
		//test += " (reload buffer)\n";
		serzOffset = 0;
		previousSerzEndOffset = previousSerzOffset + bufferSize - 0x10000;
		previousSerzOffset = serzOffset;
	} else {
		bool objectChanged = (serzOffset <= previousSerzOffset);
		if (objectChanged) {
			test += "*****\n"; // indicate we now deserialize another file (?)
			totalBufferSize = bufferSize;
		}
		previousSerzOffset = serzOffset;
		previousSerzEndOffset = tmpEndOffset;
	}


	test += "{";
	test += GetHexStringAddress(callAddr);
	test += "}\t";
	test += "[";
	test += GetHexStringAddress((unsigned long int)buffer);
	test += "]\t[+";
	//test += GetHexStringAddress(serzOffset + totalOffsetCountingResets, 5);
	test += GetHexStringAddress(totalBufferSize - bufferSize, 5);
	test += "]\tsz=";
	test += std::to_string(bufferSize);
	test += "\t";
	for (int i = 0; i < bufferSize; i++) {
		test += GetHexStringAddress(buffer[i], 2);
		test += " ";
	}
	test += "\t\"";
	for (int i = 0; i < bufferSize; i++) {
		char c = (char)buffer[i];
		if ((c < 32) || (c > 126)) { c = ' '; }
		test += c;
	}
	test += "\"";
	if (bufferSize == 4) {
		float f = 0;
		long int *fakePtr = (long int*)&f;
		*fakePtr = *((long int*)buffer);
		test += "\tfloat=";
		test += std::to_string(f);
	}
	test += "\n";
	writtenLines++;
	if (writtenLines >= 500) {
		writtenLines = 0;
		WriteToFile(test, serializationLogFilename, true);
		test.clear();
	}
}


// Displays debug info in F5 debug zone and/or in in-game bottom text lines.
bool HandleCustomRORInGameF5DebugInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return false; }
	std::string msg[9];
	static bool showUnitGroups = true;
	for (int i = 2; i <= 7; i++) {
		if (!showUnitGroups) {
			CUSTOM_AI::AIPlayerTargetingInfo *info = CUSTOM_AI::playerTargetingHandler.GetPlayerInfo(i);
			if (!info) { msg[i] = ""; continue; }
			int curTargetPlayerId = info->GetCurrentTacAITargetPlayerId(global->GetPlayerStruct(i));
			msg[i] = std::string("p#") + std::to_string(i) + std::string(" targt=") + std::to_string(curTargetPlayerId) +
				std::string(" ; subdslk vs");
			for (int j = 1; j < global->playerTotalCount; j++) {
				msg[i] += std::string(" p") + std::to_string(j) + std::string("=");
				msg[i] += to_string(info->lastComputedDislikeSubScore[i]);
			}
		} else {
			if (CUSTOM_AI::customAIHandler.IsAliveAI(i)) {
				int cnt = CUSTOM_AI::customAIHandler.GetCustomPlayerAI(i)->unitGroupAI.last5TaskingByUGAI.size();
				if (cnt > UNITGROUP_AI_LAST_TASK_TRACE_COUNT) { cnt = UNITGROUP_AI_LAST_TASK_TRACE_COUNT; }
				msg[i] = std::string("[p") + std::to_string(i) + std::string("] grpId,tsk,targ,time ");
				for (auto it = CUSTOM_AI::customAIHandler.GetCustomPlayerAI(i)->unitGroupAI.last5TaskingByUGAI.begin();
					(it != CUSTOM_AI::customAIHandler.GetCustomPlayerAI(i)->unitGroupAI.last5TaskingByUGAI.end()); it++) {
					std::tuple<long int, long int, long int, long int> t = *it;
					long int unitGrpId = std::get<0>(t);
					long int taskId = std::get<1>(t);
					long int targetId = std::get<2>(t);
					long int time_ms = std::get<3>(t);
					msg[i] += std::string(" ") + std::to_string(unitGrpId) +
						std::string(",") + std::to_string(taskId) +
						std::string(",") + std::to_string(targetId) +
						std::string(",") + std::to_string(time_ms);
				}
			}
		}
	}
	AOE_METHODS::UI_BASE::GameMainUI_writeTextDebugLines(settings->ptrGameUIStruct, msg[2].c_str(),
		msg[3].c_str(),
		msg[4].c_str(),
		msg[5].c_str(),
		msg[6].c_str(),
		msg[7].c_str()
		);
	if (CUSTOMROR::crInfo.configInfo.useF5LabelZoneForCustomDebugInfo) {
		AOE_METHODS::UI_BASE::GameMainUI_writeF5DebugInfo(settings->ptrGameUIStruct, showUnitGroups ? "CustomROR Unit group tasking" : "Player targeting");
	}
	return true;
}


}

#endif
