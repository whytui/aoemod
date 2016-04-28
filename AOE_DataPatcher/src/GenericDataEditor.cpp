#include "../include/GenericDataEditor.h"

GenericDataEditor::GenericDataEditor() {

}

GenericDataEditor::~GenericDataEditor() {

}


// If one of the sequences from the definition matches the file content, returns its index.
// Returns -1 if no sequence matches.
// If a sequence contains a variable, its value is allowed to differ from file content's variable value
int GenericDataEditor::CheckSeqDefConsistency(BinarySeqDefinition *seq) {
	if (!seq->IsValid()) { return -1; }

	for (int i = 0; i < seq->GetTotalSeqCount(); i++) {
		if (CheckSeqConsistency(seq, i)) { return i; }
	}
	return -1;
}


// See also WriteFromSequenceUsingValue
int GenericDataEditor::WriteFromSequence(BinarySeqDefinition *seq, int seqIndex) {
	if (!seq->IsValid()) {
		printf("The sequence is invalid");
		return 0;
	}
	try {
		return this->WriteFromSequenceNoCheck(seq, seqIndex);
	}
	catch (...) { // TO DO : replace with something like InvalidSequenceIndexException ?
		printf("The sequence or its index is invalid");
		return 0;
	}
}
