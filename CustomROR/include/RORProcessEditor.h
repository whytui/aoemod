#pragma once

#include "../../AOE_DataPatcher/include/GenericDataEditor.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include <AOE_offsets_10c.h>
#include "EXESelfEditor.h"


class RORProcessEditor : public GenericDataEditor
{
public:
	RORProcessEditor();
	~RORProcessEditor();

	bool CheckMemoryConsistency(BinarySeqDefinition *seq, int seqIndex, long relativeOffset, long size);

	bool CheckSeqConsistency(BinarySeqDefinition *seq, int seqIndex) override;

	int ReadSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf) override;

	int WriteSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf) override;

protected:
	int WriteFromSequenceNoCheck(BinarySeqDefinition *seq, int seqIndex) override;
};
