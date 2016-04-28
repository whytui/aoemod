#include <tchar.h>
#include <string>
#include "../include/BinarySeqDefinition.h"

#pragma once

// This class can be inherited to patch ROR file or ROR process.

class GenericDataEditor {
public:
	GenericDataEditor();
	~GenericDataEditor();
	
	// Returns true if the sequence matches the file content.
	// If the sequence contains a variable, its value is allowed to differ from file content's variable value
	virtual bool CheckSeqConsistency(BinarySeqDefinition *seq, int seqIndex) {
		return false; // To be overriden by child classes
	}


	// If one of the sequences from the definition matches the file content, returns its index.
	// Returns -1 if no sequence matches.
	// If a sequence contains a variable, its value is allowed to differ from file content's variable value
	int CheckSeqDefConsistency(BinarySeqDefinition *seq);


	virtual int ReadSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf) = 0;

	virtual int WriteSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf) = 0;

protected:
	virtual int WriteFromSequenceNoCheck(BinarySeqDefinition *seq, int seqIndex) = 0; // To be overriden by child classes


public:
	// Write a buffer from a sequence definition (given its index in seq) to file, according to the offset, size, etc.
	// All the sequence is written (including the variable, if any : if so, the default var value is written)
	// Returns the number of written bytes (0 is not OK !)
	int WriteFromSequence(BinarySeqDefinition *seq, int seqIndex);

	// Write a buffer from a sequence definition (given its index in seq) to file, according to the offset, size, etc.
	// Using the provided variable value ("var" argument)
	// Returns the number of written bytes (0 is not OK !)
	// May throw an exception if writing the variable fails (unlikely case because the rest of the sequence is written first).
	template<typename varType> int WriteFromSequenceUsingValue(BinarySeqDefinition *seq, int seqIndex, varType *var) {
		// First write the global sequence (including default variable value)
		int res = this->WriteFromSequence(seq, seqIndex);
		if (res == 0) { return 0; }; // Did not work, do not continue

		// Now write provided variable value (overwrite prevously-written default value)
		this->SetVarValue(seq, seqIndex, var);
		return res;
	}


	// Template functions MUST be entirely defined in header file (to avoid link error)
	// GetVarValue: get the variable value from "default" sequence (from sequence definition)
	template<typename varType> void GetVarValue(BinarySeqDefinition *seq, int seqIndex, varType *var) {
		if (sizeof(varType) != seq->GetVarSize(seqIndex)) {
			throw std::exception("Variable type does not correspond to the size");
		}
		size_t readBytes = this->ReadSeqVariable(seq, seqIndex, (void *)var);
		if (readBytes != seq->GetVarSize(seqIndex)) {
			throw std::exception("Could not read file sequence");
		}
	}


	// Template functions MUST be entirely defined in header file (to avoid link error)
	// SetVarValue: writes the provided variable value in the right location in target file (using BinarySeqDefinition information)
	template<typename varType> void SetVarValue(BinarySeqDefinition *seq, int seqIndex, varType *var) {
		if (sizeof(varType) != seq->GetVarSize(seqIndex)) {
			throw std::exception("Variable type does not correspond to the size");
		}

		if (!this->CheckSeqConsistency(seq, seqIndex)) {
			throw std::exception("Sequence context does not match, cannot edit variable value in file");
		};

		size_t writtenBytes = this->WriteSeqVariable(seq, seqIndex, (void*)var);
		if (writtenBytes != seq->GetVarSize(seqIndex)) {
			throw std::exception("Could not write file sequence. Make sure the file was open with \"w\" attribute");
		}
	}
};
