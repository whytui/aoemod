
#ifdef _DEBUG
#pragma once

#include <assert.h>
#include <string>
#include <mystrings.h>
#include <AOE_offsets.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_player.h>
#include <AOE_struct_main_ai.h>
#include <AOE_struct_export.h>
#include "mainStructuresHandling.h"
#include "basicFilesHandling.h"
#include "BitmapExporter.h"
#include "palette.h"

/* This file contains various primitives for debugging */


using namespace AOE_STRUCTURES;

namespace CR_DEBUG {


// Dumps useful information for debugging.
void DumpDebugInfoToFile();

// Export InfAI's exploration status map data to a bitmap using color codes
bool exportInfAIExplorationToBitmap(STRUCT_PLAYER *player);

bool exportGameTerrainRestrictionValuesToBitmap();

// Export map visibility values to a bitmap.
// fogVisibility : true=export fog visibility, false=export exploration
// playerIdFilter : if -1, export for all players (value=mask), otherwise, export for supplied player only
bool exportVisibilityToBitmap(bool fogVisibility, long int playerIdFilter);

// To customize for debug purpose...
void DebugDumpAllUnits();

// Writes in a text buffer the content of a "temp map like info" zone.
char *DumpPosToTextBuffer(AOE_STRUCTURES::STRUCT_MAP_TILE_VALUES *mapInfosStruct, long int posX, long int posY, int radius);


static const char serializationLogFilename[] = "D:\\AOESrlz.txt";
extern bool debugSerialization;
// Write deserialization data into a buffer, then to a log file.
// This affects greatly performance !!! Debug only.
// This uses a buffer system, which is flushed when exiting game.
void WriteDebugLogForDeserializedData(unsigned long int callAddr, unsigned char *buffer, long int bufferSize);


}

#endif
