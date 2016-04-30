#include "../include/RORProcessEditor.h"

RORProcessEditor::RORProcessEditor() {}
RORProcessEditor::~RORProcessEditor() {}

bool RORProcessEditor::CheckMemoryConsistency(BinarySeqDefinition *seq, int seqIndex, long relativeOffset, long size) {
	if (size < 1) { return true; } // Empty sequence: can not differ !

	if (relativeOffset < 0) { return false; }
	if (relativeOffset + size > seq->GetSeqSize()) { return false; }

	unsigned long int currentOffset = relativeOffset;
	unsigned char *sequenceBeginAddr = (unsigned char *)AOE_FileOffsetToExeAddr(seq->GetSeqOffset());
	unsigned char *sequenceDefBuffer = seq->GetSequence(seqIndex);
	while (currentOffset < size) {
		if (sequenceDefBuffer[currentOffset] != sequenceBeginAddr[currentOffset]) {
			return false;
		}
		currentOffset++;
	}
	return true;
}

// Returns true if the sequence matches the file content.
// If the sequence contains a variable, its value is allowed to differ from file content's variable value
bool RORProcessEditor::CheckSeqConsistency(BinarySeqDefinition *seq, int seqIndex) {
	if (!seq->IsValid()) { return false; }
	if (seq->GetVarType(seqIndex) == SVT_NO_VARIABLE) {
		// No variable - simple case
		return (this->CheckMemoryConsistency(seq, seqIndex, 0, seq->GetSeqSize()));
	} else {
		// sequence owns a variable whose value is free (allowed to differ between definition and file content)
		// Before the variable
		bool tmpRes = this->CheckMemoryConsistency(seq, seqIndex, 0, seq->GetVarRelativeOffset(seqIndex));
		// After the variable
		tmpRes = tmpRes && this->CheckMemoryConsistency(seq, seqIndex, 
			seq->GetVarRelativeOffset(seqIndex) + seq->GetVarSize(seqIndex),
			seq->GetSeqSize() - seq->GetVarRelativeOffset(seqIndex) - seq->GetVarSize(seqIndex));
		return tmpRes;
	}
}


int RORProcessEditor::ReadSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf) {
	if (!seq->IsValid()) {
		printf("The sequence is invalid");
		return 0;
	}
	unsigned char *addr = (unsigned char *)AOE_FileOffsetToExeAddr(seq->GetSeqOffset());
	// For read operations, we can access memory normally (even TEXT memory zone).
	errno_t err = memcpy_s(destBuf, seq->GetVarSize(seqIndex), addr + seq->GetVarRelativeOffset(seqIndex), 1);
	if (!err) {
		return seq->GetVarSize(seqIndex);
	}
	return 0;
}

int RORProcessEditor::WriteSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf) {
	if (!seq->IsValid()) {
		printf("The sequence is invalid");
		return 0;
	}
	unsigned long int addr = AOE_FileOffsetToExeAddr(seq->GetSeqOffset());
	if (WriteInMyMemory(addr + seq->GetVarRelativeOffset(seqIndex), (char*)destBuf, seq->GetVarSize(seqIndex))) {
		return seq->GetVarSize(seqIndex);
	}
	return 0;
}

int RORProcessEditor::WriteFromSequenceNoCheck(BinarySeqDefinition *seq, int seqIndex) {
	if (!seq->IsValid()) {
		printf("The sequence is invalid");
		return 0;
	}
	try {
		unsigned long int addr = AOE_FileOffsetToExeAddr(seq->GetSeqOffset());
		if (addr > 0) {
			if (WriteInMyMemory(addr, seq->GetSequence(seqIndex), seq->GetSeqSize())) {
				return seq->GetSeqSize();
			}
		}
		return 0;
	}
	catch (...) { // TO DO : replace with something like InvalidSequenceIndexException ?
		printf("The sequence or its index is invalid");
		return 0;
	}

}

