
#ifdef _DEBUG
#include "../include/crDebugOutputPrimitives.h"

/* This file contains various primitives for debugging */


using namespace AOE_STRUCTURES;

namespace CR_DEBUG {

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
		myPalette[1 * 4 + 1] = 255; // Green for 0 (explored
		myPalette[2 * 4 + 2] = 255; // Red for 1 (failed)
		myPalette[3 * 4 + 0] = 255; // Blue for 2 (to reexplore)

		_BITMAP::BitmapExporter::ExportDataColumnsAsBitmapUsingPalette("D:\\test2.bmp",
			mapSizeX, mapSizeY,
			(const unsigned char**)player->ptrAIStruct->structInfAI.mapExplorationInfo.ptrColsPtr, -1, myPalette, paletteSize);
	}
	return true;
}


bool exportGameTerrainRestrictionValuesToBitmap() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	if (!global->gameMapInfo->unknown_ptrMapInfosLink) { return false; }
	if ((global->gameMapInfo->unknown_ptrMapInfosLink->ptrArray == NULL) || (
		global->gameMapInfo->unknown_ptrMapInfosLink->arrayElemCount <= 0)) {
		return false;
	}
	STRUCT_GAME_TERRAIN_RESTRICTION_INFO *gameTerrainRestrInfo = NULL;
	int index = 0;
	// Find the terrain restriction info that matches "game global" : number of terrain per terrain restriction must match
	while ((index < global->gameMapInfo->unknown_ptrMapInfosLink->arrayElemCount) && (gameTerrainRestrInfo == NULL)) {
		gameTerrainRestrInfo = global->gameMapInfo->unknown_ptrMapInfosLink->ptrArray[index];
		if (gameTerrainRestrInfo) {
			if ((gameTerrainRestrInfo->terrainsCountInTerrainRestrictions <= 0) ||
				(gameTerrainRestrInfo->terrainsCountInTerrainRestrictions != global->nbOfTerrainPerTerrainRestriction)) {
				gameTerrainRestrInfo = NULL;
			}
		}
		index++;
	}
	if (gameTerrainRestrInfo == NULL) { return false; }

	int sizeX = global->gameMapInfo->mapArraySizeX;
	int sizeY = global->gameMapInfo->mapArraySizeY;
	assert((sizeX > 0) && (sizeY > 0));
	if ((sizeX <= 0) || (sizeY <= 0)) { return false; }
	int pos = 0;
	char *b = (char*)malloc(sizeX * sizeY);
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			b[pos] = gameTerrainRestrInfo->GetTerrainAccessibilityValue(x, y);
			pos++;
		}
	}
	_BITMAP::BitmapExporter::ExportDataAsBitmapGreyShades("D:\\testtrn.bmp", sizeX, sizeY, b, 0, 255, false);

	free(b);
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


}

#endif
