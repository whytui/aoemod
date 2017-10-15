
#pragma once

#include <stdio.h>
#include <string>
#include <assert.h>
#include <AOE_offsets.h>
#include <mystrings.h>
#include <AOE_const_internal.h>
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include "EXESelfEditor.h"
#include "RORProcessEditor.h"
#include "traceMessage.h"

static AOE_binData aoeBinData;

// Returns false if sequences are not installed or if an error occurs.
bool CheckRorApiSequencesAreInstalled(FILE *logFile, bool autoFix);

// Returns true if successful
// This only modifies the getter function.
// If you wish to change in-game maximum population, you'll probably have to modify all player "max pop" resource.
bool SetMaxPopulationGetterInSPGames(long int newMaxPopulationValue);

bool IsBinaryChangeOn(BINSEQ_CATEGORIES binCategory, std::string sequenceName);

long int GetBinaryChangeVarValue(BINSEQ_CATEGORIES binCategory, std::string sequenceName, int seqIndex);

bool SetBinaryChangeVarValue(BINSEQ_CATEGORIES binCategory, std::string sequenceName, int seqIndex, long int value);

#ifdef _DEBUG
bool SetDeserializationDebugChange(bool enable);
#endif
