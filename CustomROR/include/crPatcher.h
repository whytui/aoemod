
#pragma once

#include <stdio.h>
#include <string>
#include <assert.h>
#include <AOE_offsets_10c.h>
#include <mystrings.h>
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include "EXESelfEditor.h"
#include "RORProcessEditor.h"
#include "traceMessage.h"

static AOE_binData aoeBinData;

// Returns false if sequences are not installed or if an error occurs.
bool CheckRorApiSequencesAreInstalled(FILE *logFile, bool autoFix);

