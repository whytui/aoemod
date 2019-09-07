
#pragma once

#include <assert.h>
#include <set>
#include <string>
#include <mystrings.h>
#include <AOE_offsets.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_player.h>
#include <AOE_struct_main_ai.h>
#include <AOE_struct_export.h>
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "basicFilesHandling.h"
#include "BitmapExporter.h"
#include "palette.h"
#include "mainCustomAI.h"
#include "AOEPrimitives_UI_gameMain.h"


/* This file contains various primitives for debugging */


using namespace AOE_STRUCTURES;

namespace CR_DEBUG {

// Writes text to RockNRor log file (appends).
// Returns true if successful
bool AppendTextToLogFile(const char *text, bool addNewLineAfterwards);

#ifdef _DEBUG

// Dumps useful information for debugging.
void DumpDebugInfoToFile();

// Export InfAI's exploration status map data to a bitmap using color codes
// Grey=unexplored, Green=explored, Red=failed, Blue=to reexplore
bool exportInfAIExplorationToBitmap(STRUCT_PLAYER *player);

bool exportGameTerrainRestrictionValuesToBitmap();

// Export map visibility values to a bitmap.
// fogVisibility : true=export fog visibility, false=export exploration
// playerIdFilter : if -1, export for all players (value=mask), otherwise, export for supplied player only
bool exportVisibilityToBitmap(bool fogVisibility, long int playerIdFilter);

// Export obstruction map (from pathing system) to a bitmap
bool exportPathingSystemObstruction(bool obstructionMapCode);

// To customize for debug purpose...
void DebugDumpAllUnits();

// Writes in a text buffer the content of a "temp map like info" zone.
char *DumpPosToTextBuffer(AOE_STRUCTURES::STRUCT_INFLUENCE_MAP *mapInfosStruct, long int posX, long int posY, int radius);


static const char serializationLogFilename[] = "D:\\AOESrlz.txt";
extern bool debugSerialization;
// Write deserialization data into a buffer, then to a log file.
// This affects greatly performance !!! Debug only.
// This uses a buffer system, which is flushed when exiting game.
void WriteDebugLogForDeserializedData(unsigned long int callAddr, unsigned char *buffer, long int bufferSize);


// Displays debug info in F5 debug zone and/or in in-game bottom text lines.
bool HandleRockNRorInGameF5DebugInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings);


#endif


// Automatically detects issues in empires.dat (requires to have been loaded already) and writes logs about errors in trace message handler.
bool AnalyzeEmpiresDatQuality();


bool AnalyzeMilitaryUnitsEfficency();


// A method to do various dirty testing. Does nothing in RELEASE mode.
void CurrentTestMethod();

}
