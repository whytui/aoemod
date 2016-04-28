
#pragma once

#include <string>
#include <assert.h>
#include <AOE_offsets_10c.h>
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include "EXESelfEditor.h"

/*
AOE_binData x;
void toto() {
	x.SetCurrentVersion(AOE_FILE_VERSION::AOE_VERSION_1_0C);
	BinarySeqDefSet *sds = x.GetSeqDefSet(AOE_FILE_VERSION::AOE_VERSION_1_0C, BINSEQ_CATEGORIES::BC_OPTIONS);
	//x.options_10c.SetUserVarValue(21, (char)25);
	int index = sds->FindSeqDefinitionIndex(_T("PointsPerTemple"));
	sds->SetUserSelection(index, 0);
	char v = 25;
	sds->SetUserVarValue(index, &v);
}


int WriteFromSequence(BinarySeqDefinition *seq, int seqIndex) {
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
*/

