#include "../include/crPatcher.h"

bool CheckRorApiSequencesAreInstalled(FILE *logFile, bool autoFix) {
	try {
		RORProcessEditor e;
		aoeBinData.SetCurrentVersion(AOE_FILE_VERSION::AOE_VERSION_1_0C);
		BinarySeqDefSet *seqDefSet = aoeBinData.GetSeqDefSet(AOE_FILE_VERSION::AOE_VERSION_1_0C, BINSEQ_CATEGORIES::BC_ROR_API);
		if (!seqDefSet) {
			return true;
		}

		bool hasMissingSequences = false;
		int seqDefCount = seqDefSet->GetCount();
		for (int seqDefIndex = 0; seqDefIndex < seqDefCount; seqDefIndex++) {
			BinarySeqDefinition *seqDef = seqDefSet->GetBinSeqDefinition(seqDefIndex);
			if (seqDef) {
				std::string seqDefName = narrow(seqDef->GetSeqName());
				unsigned long int addr = AOE_FileOffsetToExeAddr((unsigned long int)seqDef->GetSeqOffset());
				if (addr >= AOE_OFFSETS_10C::ADDR_DATA_MIN) {
					// Do not check/try to fix sequences in DATA section (may be modified during normal execution)
					fprintf_s(logFile, "This sequence will not be checked: %s\n", seqDefName.c_str());
				} else {
					// Some sequences are not mandatory, do not raise an error for them.
					bool isOptionalSequence = (seqDefName == "Optional_ManageGameTimerSkip") ||
						(seqDefName == "Optional_GameTimerSkipStats") ||
						(seqDefName == "GetAOEDebugLogs") ||
						(seqDefName == "ROR_writeDebugLog");

					//int seqIndexCount = seqDef->GetTotalSeqCount();
					int seqIndexON = seqDef->GetSeqIndexFromFuncMeaning(FUNC_MEANING::FM_ON);
					if (seqIndexON >= 0) {
						if (!e.CheckSeqConsistency(seqDef, seqIndexON)) {
							fprintf_s(logFile, "%sFeature is not installed: %s\n",
								isOptionalSequence ? "[INFO] " : "[WARNING] ", seqDefName.c_str());
							int writtenBytes = 0;
							if (!isOptionalSequence) {
								fprintf_s(logFile, "Force installation of %s\n", seqDefName.c_str());
								writtenBytes = e.WriteFromSequence(seqDef, seqIndexON); // Force install missing sequence
							}
							if (!isOptionalSequence && (writtenBytes <= 0)) {
								hasMissingSequences = true; // Set to true (unless it is optional or it has been fixed on the fly).
							}
						} else {
							fprintf_s(logFile, "...%s... Installed !\n", seqDefName.c_str());
						}
					} else {
						if (logFile) {
							fprintf_s(logFile, "Did not find functional meaning \"ON\" for sequence %s\n", seqDefName.c_str());
						}
					}
				}
			}
		}
		return !hasMissingSequences;
	}
	catch (std::exception e) {
		fprintf_s(logFile, "An error occurred: %s\n", e.what());
		traceMessageHandler.WriteMessage(e.what());
		return false;
	}
}

