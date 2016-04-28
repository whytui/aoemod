
#pragma once

#include <string>
#include <assert.h>
#include <AOE_offsets_10c.h>
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include "EXESelfEditor.h"


static AOE_binData x;




static int WriteFromSequence(BinarySeqDefinition *seq, int seqIndex) {
	if (!seq->IsValid()) {
		printf("The sequence is invalid");
		return 0;
	}
	try {
		//return this->WriteBuffer(seq->GetSeqOffset(), seq->GetSequence(seqIndex), seq->GetSeqSize());
		unsigned long int addr = AOE_FileOffsetToExeAddr(seq->GetSeqOffset());
		if (addr > 0) {
			WriteInMyMemory(addr, seq->GetSequence(seqIndex), seq->GetSeqSize());
			return seq->GetSeqSize();
		}
		return 0;
	}
	catch (...) { // TO DO : replace with something like InvalidSequenceIndexException ?
		printf("The sequence or its index is invalid");
		return 0;
	}
}

// Write a buffer from a sequence definition (given its index in seq) to file, according to the offset, size, etc.
// Using the provided variable value ("var" argument)
// Returns the number of written bytes (0 is not OK !)
// May throw an exception if writing the variable fails (unlikely case because the rest of the sequence is written first).
template<typename varType> int WriteFromSequenceUsingValue(BinarySeqDefinition *seq, int seqIndex, varType *var) {
	// First write the global sequence (including default variable value)
	int res = WriteFromSequence(seq, seqIndex);
	if (res == 0) { return 0; }; // Did not work, do not continue

	// Now write provided variable value (overwrite prevously-written default value)
	SetVarValue(seq, seqIndex, var);
	return res;
}


static void toto() {
	x.SetCurrentVersion(AOE_FILE_VERSION::AOE_VERSION_1_0C);
	BinarySeqDefSet *sds = x.GetSeqDefSet(AOE_FILE_VERSION::AOE_VERSION_1_0C, BINSEQ_CATEGORIES::BC_OPTIONS);
	//x.options_10c.SetUserVarValue(21, (char)25);
	int index = sds->FindSeqDefinitionIndex(_T("PointsPerTemple"));
	sds->SetUserSelection(index, 0);
	char v = 25;
	sds->SetUserVarValue(index, &v);
	int res = WriteFromSequence(sds->GetBinSeqDefinition(index), 0); // issue: will use default var value
	//int res = WriteFromSequenceUsingValue(sds->GetBinSeqDefinition(index), 0, &v);
	if (res == 0) {
		traceMessageHandler.WriteMessageNoNotification("OK");
	} else {
		traceMessageHandler.WriteMessageNoNotification("failed");
	}
}

