#include "../include/crPatcher.h"

namespace ROCKNROR {
namespace PATCHER {

bool CheckRorApiSequencesAreInstalled(FILE *logFile, bool autoFix) {
	try {
		if (autoFix) {
			// Useful if some binary change modifies .rdata section (by default, it is read-only and would fail)
			EnableWritingInRData(true);
		}

		RORProcessEditor pe;
		aoeBinData.SetCurrentVersion(GetBuildVersion());
		BinarySeqDefSet *seqDefSet = aoeBinData.GetSeqDefSet(GetBuildVersion(), BINSEQ_CATEGORIES::BC_ROR_API);
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
				if (addr >= AOE_OFFSETS::ADDR_DATA_MIN) {
					// Do not check/try to fix sequences in DATA section (may be modified during normal execution)
					fprintf_s(logFile, "This sequence will not be checked: %s\n", seqDefName.c_str());
				} else {
					// Some sequences are not mandatory, do not raise an error for them.
					bool isOptionalSequence = /*(seqDefName == "Optional_ManageGameTimerSkip") ||
						(seqDefName == "Optional_GameTimerSkipStats") ||*/
						(seqDefName == "GetAOEDebugLogs") ||
						(seqDefName == "ROR_writeDebugLog");

					//int seqIndexCount = seqDef->GetTotalSeqCount();
					int seqIndexON = seqDef->GetSeqIndexFromFuncMeaning(FUNC_MEANING::FM_ON);
					if (seqIndexON >= 0) {
						int matchingSeqIndex = pe.CheckSeqDefConsistency(seqDef);
						//if (!pe.CheckSeqConsistency(seqDef, seqIndexON)) {
						if (matchingSeqIndex == seqIndexON) {
							fprintf_s(logFile, "...%s... Installed !\n", seqDefName.c_str());
						}
						if ((matchingSeqIndex != seqIndexON) && (matchingSeqIndex >= 0)) {
							// Note: this can be provoked by breakpoints set by debugging programs.
							fprintf_s(logFile, "%sFeature is not installed: %s\n",
								isOptionalSequence ? "[INFO] " : "[WARNING] ", seqDefName.c_str());
							int writtenBytes = 0;
							if (!isOptionalSequence && autoFix) {
								fprintf_s(logFile, "Force installation of %s\n", seqDefName.c_str());
								writtenBytes = pe.WriteFromSequence(seqDef, seqIndexON); // Force install missing sequence
							}
							if (!isOptionalSequence && (writtenBytes <= 0)) {
								hasMissingSequences = true; // Set to true (unless it is optional or it has been fixed on the fly).
							}
						}
						if (matchingSeqIndex < 0) {
							fprintf_s(logFile, "[ERROR] %s is inconsistent or an error occurred while reading memory.\n", seqDefName.c_str());
							hasMissingSequences = true;
						}
					} else {
						if (logFile) {
							fprintf_s(logFile, "Did not find functional meaning \"ON\" for sequence %s\n", seqDefName.c_str());
						}
					}
				}
			}
		}
		if (autoFix) {
			EnableWritingInRData(false);
		}
		return !hasMissingSequences;
	}
	catch (std::exception e) {
		fprintf_s(logFile, "An error occurred: %s\n", e.what());
		traceMessageHandler.WriteMessage(e.what());
		if (autoFix) {
			EnableWritingInRData(false);
		}
		return false;
	}
}


// Returns true if successful
// This only modifies the getter function.
// If you wish to change in-game maximum population, you'll probably have to modify all player "max pop" resource.
bool SetMaxPopulationGetterInSPGames(long int newMaxPopulationValue) {
	try {
		if ((newMaxPopulationValue < 0) || (newMaxPopulationValue > AOE_CONST_INTERNAL::TECH_LIMIT_MAXIMUM_POPULATION)) { return false; }
		bool success;
		success = SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_OPTIONS, "SetSinglePlayerMaxPop", 0, newMaxPopulationValue);
		success = success && SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_OPTIONS, "SetAIHousesMaxPop", 0, newMaxPopulationValue);
		if (!success) {
			traceMessageHandler.WriteMessage("An error occurred while setting up custom population limit");
		}
		return success;
	}
	catch (std::exception e) {
		traceMessageHandler.WriteMessage(e.what());
		return false;
	}
}


bool IsBinaryChangeOn(BINSEQ_CATEGORIES binCategory, std::string sequenceName) {
	try {
		RORProcessEditor pe;
		aoeBinData.SetCurrentVersion(GetBuildVersion());
		BinarySeqDefSet *seqDefSet = aoeBinData.GetSeqDefSet(GetBuildVersion(), binCategory);
		if (!seqDefSet) {
			return false;
		}
		int seqDefIndex = seqDefSet->FindSeqDefinitionIndex(widen(sequenceName)); // throws if not found
		BinarySeqDefinition *binSeq = seqDefSet->GetBinSeqDefinition(seqDefIndex);
		if (!binSeq) { return false; }
		int searchedSelectionIndex = binSeq->GetSeqIndexFromFuncMeaning(FUNC_MEANING::FM_ON);
		int foundSelectionIndex = pe.CheckSeqDefConsistency(binSeq);
		return (foundSelectionIndex == searchedSelectionIndex);

	}
	catch (std::exception e) {
		traceMessageHandler.WriteMessage(e.what());
		return false;
	}
}

long int GetBinaryChangeVarValue(BINSEQ_CATEGORIES binCategory, std::string sequenceName, int seqIndex) {
	try {
		RORProcessEditor pe;
		aoeBinData.SetCurrentVersion(GetBuildVersion());
		BinarySeqDefSet *seqDefSet = aoeBinData.GetSeqDefSet(GetBuildVersion(), binCategory);
		if (!seqDefSet) {
			return 0;
		}
		int seqDefIndex = seqDefSet->FindSeqDefinitionIndex(widen(sequenceName)); // throws if not found
		BinarySeqDefinition *binSeq = seqDefSet->GetBinSeqDefinition(seqDefIndex);
		if (!binSeq) { return 0; }

		switch (binSeq->GetVarSize(seqIndex)) {
		case 1:
			char c;
			pe.GetVarValue(binSeq, seqIndex, &c);
			return (long int)c;
		case 2:
			short int si;
			pe.GetVarValue(binSeq, seqIndex, &si);
			return (long int)si;
		case 4:
			long int li;
			pe.GetVarValue(binSeq, seqIndex, &li);
			return li;
		default:
			return 0;
		}
	}
	catch (std::exception e) {
		traceMessageHandler.WriteMessage(e.what());
		return 0;
	}
}


bool SetBinaryChangeVarValue(BINSEQ_CATEGORIES binCategory, std::string sequenceName, int seqIndex, long int value) {
	try {
		RORProcessEditor pe;
		aoeBinData.SetCurrentVersion(GetBuildVersion());
		BinarySeqDefSet *seqDefSet = aoeBinData.GetSeqDefSet(GetBuildVersion(), binCategory);
		if (!seqDefSet) {
			return false;
		}
		int seqDefIndex = seqDefSet->FindSeqDefinitionIndex(widen(sequenceName)); // throws if not found
		BinarySeqDefinition *binSeq = seqDefSet->GetBinSeqDefinition(seqDefIndex);
		if (!binSeq) { return false; }

		switch (binSeq->GetVarSize(seqIndex)) {
		case 1:
			char c;
			c = (char)value;
			pe.SetVarValue(binSeq, seqIndex, &c);
			return true;
		case 2:
			short int si;
			si = (short int)value;
			pe.SetVarValue(binSeq, seqIndex, &si);
			return true;
		case 4:
			//long int li;
			pe.SetVarValue(binSeq, seqIndex, &value);
			return true;
		default:
			return false;
		}
	}
	catch (std::exception e) {
		traceMessageHandler.WriteMessage(e.what());
		return false;
	}
}

#ifdef _DEBUG
bool SetDeserializationDebugChange(bool enable) {
	try {
		RORProcessEditor pe;
		unsigned char bufferSeqOFF[] = { 0x89, 0x0D, 0xD8, 0xFA, 0x7B, 0x00, 0xA3, 0xE8, 0xFA, 0x7B, 0x00, 0x5F };
		unsigned char bufferSeqON[] = { 0x89, 0x0D, 0xD8, 0xFA, 0x7B, 0x00, 0xE8, 0x70, 0xDB, 0xFA, 0xFF, 0x5F };
		BinarySeqDefinition b = BinarySeqDefinition(sizeof(bufferSeqOFF), 2, 0x6B9F9);
		b.SetFuncMeaning(0, FUNC_MEANING::FM_OFF);
		b.SetFuncMeaning(1, FUNC_MEANING::FM_ON);
		b.SetSeqName(_T("[DEBUG] deserialization output handler"));
		b.WriteSequence(0, bufferSeqOFF);
		b.WriteSequence(1, bufferSeqON);
		pe.WriteFromSequence(&b, enable ? 1 : 0);
		return true;
	}
	catch (std::exception e) {
		traceMessageHandler.WriteMessage(e.what());
		return false;
	}
}
#endif

}
}
