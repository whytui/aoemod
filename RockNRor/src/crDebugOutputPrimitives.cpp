#include "../include/crDebugOutputPrimitives.h"

/* This file contains various primitives for debugging */


using namespace AOE_STRUCTURES;

namespace CR_DEBUG {
;

// Writes text to RockNRor log file (appends).
// Returns true if successful
bool AppendTextToLogFile(const char *text, bool addNewLineAfterwards) {
	if (!text) { return true; }
	FILE *f;
	int res = fopen_s(&f, MOD_NAME "\\" MOD_NAME ".log", "a+"); // append (do not overwrite)
	if (res) {
		// Error
		return false;
	}
	fprintf_s(f, "%s%s", text, addNewLineAfterwards ? "\n" : "");
	fclose(f);
	return true;
}


#ifdef _DEBUG


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
#undef thisbufSize


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
bool HandleRockNRorInGameF5DebugInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings) {
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
	if (ROCKNROR::crInfo.configInfo.useF5LabelZoneForCustomDebugInfo) {
		AOE_METHODS::UI_BASE::GameMainUI_writeF5DebugInfo(settings->ptrGameUIStruct, showUnitGroups ? "RockNRor Unit group tasking" : "Player targeting");
	}
	return true;
}


#endif



// Automatically detects issues in empires.dat (requires to have been loaded already) and writes logs about errors in trace message handler.
bool AnalyzeEmpiresDatQuality() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }
	if (!global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) {
		traceMessageHandler.WriteMessage("ERROR: missing or invalid techInfo structure");
	}
	if (!global->researchDefInfo) {
		traceMessageHandler.WriteMessage("ERROR: missing or invalid researchInfo structure");
	}
	std::string msg;
	int civResCount = -1;
	int civUnitDefCount = -1;
	std::set<short int> techTreeIds; // stores all technology tree IDs
	// CivDef
#if 1
	for (int civId = 0; civId < global->civCount; civId++) {
		if (civResCount == -1) {
			civResCount = global->civilizationDefinitions[civId]->civResourcesCount;
		}
		if (global->civilizationDefinitions[civId]->civResourcesCount != civResCount) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has not the same resources count (";
			msg += std::to_string(global->civilizationDefinitions[civId]->civResourcesCount);
			msg += ") as previous civilizations (";
			msg += std::to_string(civResCount);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
		}
		if (civUnitDefCount == -1) {
			civUnitDefCount = global->civilizationDefinitions[civId]->civUnitDefCount;
		}
		if (global->civilizationDefinitions[civId]->civUnitDefCount != civUnitDefCount) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has not the same unit definitions count (";
			msg += std::to_string(global->civilizationDefinitions[civId]->civUnitDefCount);
			msg += ") as previous civilizations (";
			msg += std::to_string(civUnitDefCount);
			msg += "). This can provoke crashes.";
			traceMessageHandler.WriteMessage(msg);
		}
		if (global->civilizationDefinitions[civId]->techTreeId >= global->technologiesInfo->technologyCount) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has an invalid tech tree ID (";
			msg += std::to_string(global->civilizationDefinitions[civId]->techTreeId);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
		}
		if (global->civilizationDefinitions[civId]->techTreeId < 0) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " does not have a tech tree (tech tree id=-1). This is not an error and is acceptable.";
			traceMessageHandler.WriteMessage(msg);
		} else {
			techTreeIds.insert(global->civilizationDefinitions[civId]->techTreeId);
		}
		if ((global->civilizationDefinitions[civId]->graphicSetId > 4) || (global->civilizationDefinitions[civId]->graphicSetId < 0)) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has an invalid graphic set ID (";
			msg += std::to_string(global->civilizationDefinitions[civId]->graphicSetId);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
		}
	}
#endif
	// Technologies
#if 1
	for (int techId = 0; techId < global->technologiesInfo->technologyCount; techId++) {
		AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techId);
		assert(techDef != NULL);
		if (techDef) {
			for (int effectId = 0; effectId < techDef->effectCount; effectId++) {
				if (!techDef->ptrEffects[effectId].HasValidEffect()) {
					msg = "Technology ";
					msg += std::to_string(techId);
					msg += "/effect #";
					msg += std::to_string(effectId);
					msg += " does not have an effective impact";
					traceMessageHandler.WriteMessage(msg);
				}
				if (techTreeIds.find(techId) == techTreeIds.end()) {
					if (techDef->ptrEffects[effectId].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
						msg = "Technology ";
						msg += std::to_string(techId);
						msg += "/effect #";
						msg += std::to_string(effectId);
						msg += " has 'set attribute' type, which should not be used. Try to use add or multiply instead";
						traceMessageHandler.WriteMessage(msg);
					}
					if ((techDef->ptrEffects[effectId].effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) &&
						(techDef->ptrEffects[effectId].effectClass == 0)) { // mode = 0 = set
						msg = "Technology ";
						msg += std::to_string(techId);
						msg += "/effect #";
						msg += std::to_string(effectId);
						msg += " has 'set resource' type, which should not be used. Try to use add or multiply instead";
						traceMessageHandler.WriteMessage(msg);
					}
				}
				// TODO: Negative tech effects should be avoided?
			}

		}
	}
#endif
	// Researches
#if 1
	for (int researchId = 0; researchId < global->researchDefInfo->researchCount; researchId++) {
		STRUCT_RESEARCH_DEF *resDef = global->researchDefInfo->GetResearchDef(researchId);
		if (!resDef) {
			msg = "ERROR: Research #";
			msg += std::to_string(researchId);
			msg += " is NULL";
			traceMessageHandler.WriteMessage(msg);
			continue;
		}
		int remainingRequirementsCount = resDef->minRequiredResearchesCount;
		if ((remainingRequirementsCount < 0) || (remainingRequirementsCount > 4)) {
			msg = "Research #";
			msg += std::to_string(researchId);
			msg += " has an invalid requirements count (";
			msg += std::to_string(remainingRequirementsCount);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
			remainingRequirementsCount = 0;
		}
		int validReqResearchCount = 0;
		for (int index = 0; index < 4; index++) {
			short int reqResearchId = resDef->requiredResearchId[index];
			if (reqResearchId >= 0) {
				validReqResearchCount++; // found a valid required research id
				if (reqResearchId >= global->researchDefInfo->researchCount) {
					msg = "Research #";
					msg += std::to_string(researchId);
					msg += " has an invalid required research (id=";
					msg += std::to_string(reqResearchId);
					msg += ")";
					traceMessageHandler.WriteMessage(msg);
				}
			}
		}
		if (validReqResearchCount > remainingRequirementsCount) {
			msg = "Research #";
			msg += std::to_string(researchId);
			msg += " has optional requirements (more required researches than minRequiredResearch). This should be avoided (used in standard game for 'age' researches). You can set required research IDs to -1 to disable them.";
			traceMessageHandler.WriteMessage(msg);
		}
		bool hasDuplicateRequirement = false;
		for (int index = 0; index < 4; index++) {
			short int reqResearchId = resDef->requiredResearchId[index];
			if (reqResearchId >= 0) {
				for (int index2 = index + 1; index2 < 4; index2++) {
					if (reqResearchId == resDef->requiredResearchId[index2]) {
						hasDuplicateRequirement = true;
					}
				}
			}
		}
		if (hasDuplicateRequirement) {
			msg = "Research #";
			msg += std::to_string(researchId);
			msg += " has duplicate requirements (other than -1).";
			traceMessageHandler.WriteMessage(msg);
		}
		if (resDef->researchLocation >= 0) {
			// If research is a shadow research, error
			if ((resDef->researchTime <= 0) || (resDef->buttonId <= 0)) {
				msg = "Research #";
				msg += std::to_string(researchId);
				msg += " is a shadow research but has a research location. If this really is a shadow research, you should set research location to -1.";
				traceMessageHandler.WriteMessage(msg);
			}
		}
	}
#endif
	// Units
#if 1
	std::set<short int> badBlastLevelUnits;
	std::set<pair<short int, short int>> unitDefCommandAlreadyReported;
	for (int civId = 0; civId < global->civCount; civId++) {
		for (int unitDefId = 0; unitDefId < global->civilizationDefinitions[civId]->civUnitDefCount; unitDefId++) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = global->civilizationDefinitions[civId]->GetUnitDef(unitDefId);
			if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass()) {
				if (unitDefBase->unitAIType < 0) {
					msg = "UnitDef ";
					msg += std::to_string(unitDefId);
					msg += " is missing an AI type (class = -1)";
					traceMessageHandler.WriteMessage(msg);
				}
			}
			// Blast type / level consistency (tons of errors)
			if (unitDefBase && unitDefBase->DerivesFromAttackable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDefBase;
				if ((type50->blastLevel != CST_BL_DAMAGE_TARGET_ONLY) && (type50->blastRadius == 0)) {
					badBlastLevelUnits.insert(unitDefId);
					if (false) { // generates too many errors
						msg = "UnitDef ";
						msg += std::to_string(unitDefId);
						msg += " has an inactive blast level set (";
						msg += std::to_string((int)type50->blastLevel);
						msg += ") because blast radius is 0 (no blast damage). You should set blast level to 3 when no blast damage is used.";
						traceMessageHandler.WriteMessage(msg);
					}
				}
			}
			// Gather Commands using unitId instead of class (miners) => adding custom stone/gold mines wouldn't work
			if (unitDefBase && unitDefBase->DerivesFromCommandable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *commandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDefBase;
				if (commandable->ptrUnitCommandHeader && commandable->ptrUnitCommandHeader->IsCheckSumValid()) {
					for (int i = 0; i < commandable->ptrUnitCommandHeader->commandCount; i++) {
						if ((commandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK) ||
							(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK)) {
							if ((commandable->ptrUnitCommandHeader->ptrCommandArray[i]->classId == -1) &&
								(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId >= 0) &&
								(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId != CST_UNITID_FARM) // Allow farm because they don't have a dedicated AI type (class)
								) {
								pair<short int, short int> p;
								p.first = unitDefId;
								p.second = i;
								if (unitDefCommandAlreadyReported.find(p) == unitDefCommandAlreadyReported.end()) {
									msg = "UnitDef ";
									msg += std::to_string(unitDefId);
									msg += ": command #";
									msg += std::to_string(i);
									msg += " should not use unitDefId (";
									msg += std::to_string(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId);
									msg += "). Please use unit class instead (civ #";
									msg += std::to_string(civId);
									msg += " - the message won't be repeated for other civs)";
									traceMessageHandler.WriteMessage(msg);
									unitDefCommandAlreadyReported.insert(p);
								}
							}
						}
					}
				}
			}
		}
	}
	if (badBlastLevelUnits.size() > 0) {
		msg = "Those units have an inactive blast level set because blast radius is 0 (no blast damage). You should set blast level to 3 when no blast damage is used. ";
		for each (short int unitDefId in badBlastLevelUnits)
		{
			msg += std::to_string(unitDefId);
			msg += ", ";
		}
		traceMessageHandler.WriteMessage(msg);
	}

#endif
	// Search for techtree-unavailable researches
	for (int civId = 0; civId < global->civCount; civId++) {
		DetectDatImpossibleResearches(global, civId);
	}

	return true;
}


}

