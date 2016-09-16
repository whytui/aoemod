
#ifdef _DEBUG
#include "../include/crDebugOutputPrimitives.h"

/* This file contains various primitives for debugging */


using namespace AOE_STRUCTURES;

namespace CR_DEBUG {

bool exportInfAIExplorationToBitmap(STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	if (player->ptrAIStruct && (player->ptrAIStruct->structInfAI.XMapSize > 0) && (player->ptrAIStruct->structInfAI.YMapSize > 0)) {
		int pos = 0;
		char *b = (char*)malloc(player->ptrAIStruct->structInfAI.XMapSize * player->ptrAIStruct->structInfAI.YMapSize);
		for (int y = 0; y < player->ptrAIStruct->structInfAI.YMapSize; y++) {
			for (int x = 0; x < player->ptrAIStruct->structInfAI.XMapSize; x++) {
				b[pos] = player->ptrAIStruct->structInfAI.mapExplorationInfo.GetTileValue(x, y);
				pos++;
			}
		}
		_BITMAP::ExportDataAsBitmap("D:\\test.bmp",
			player->ptrAIStruct->structInfAI.XMapSize, player->ptrAIStruct->structInfAI.YMapSize,
			b, -1, 3);
		free(b);
	}
	return true;
}


// To customize for debug purpose...
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
			unsigned char value = (unsigned char)mapInfosStruct->GetTileValue(posX + i, posY + j);
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
