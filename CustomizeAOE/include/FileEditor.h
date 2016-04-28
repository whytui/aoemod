#include <Windows.h>
#include <stdio.h>
//#include <string>
//#include <tchar.h>
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"

#pragma once


/* Provides methods to manage files.
 * Takes an open file (FILE*) as entry parameter (constructor)
 * Caller must manage the file open mode (read/write) if he uses methods that write to the file
*/
class FileEditor
{
public:
	// If you use this constructor, then call SetFile(...)
	FileEditor();
	FileEditor(FILE *file);
	~FileEditor();

	// Returns the opened file size
	long GetFileSize();

	// Can be used if native constructor (without argument) has been called
	// Call this with a NULL argument to unset the file.
	void SetFile(FILE *file);


	// Returns true if the provided buffer matches file content at offsetBegin
	bool CheckBufferConsistency(const unsigned char *buf, long offsetBegin, long size);
	

	// Returns true if the sequence matches the file content.
	// If the sequence contains a variable, its value is allowed to differ from file content's variable value
	bool CheckSeqConsistency(BinarySeqDefinition *seq, int seqIndex);


	// If one of the sequences from the definition matches the file content, returns its index.
	// Returns -1 if no sequence matches.
	// If a sequence contains a variable, its value is allowed to differ from file content's variable value
	int CheckSeqConsistency(BinarySeqDefinition *seq);


	// Reads "size" bytes from file to buffer
	// Buffer must be large enough and offset must be correct. Please check your buffer size first !
	int ReadBuffer(long fileOffset, void *destBuf, long size);


	// Writes "size" bytes from buffer to file
	// Buffer must be large enough and offset must be correct
	// The file must have been opened in write mode
	// Returns the number of written bytes (0 is not OK !)
	int WriteBuffer(long fileOffset, void *srcBuf, long size);


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
		size_t readBytes = this->ReadBuffer(seq->GetSeqOffset() + seq->GetVarRelativeOffset(seqIndex), (void *)var, seq->GetVarSize(seqIndex));
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
		
		size_t writtenBytes = this->WriteBuffer(seq->GetSeqOffset() + seq->GetVarRelativeOffset(seqIndex), (void *)var, seq->GetVarSize(seqIndex));
		if (writtenBytes != seq->GetVarSize(seqIndex)) {
			throw std::exception("Could not write file sequence. Make sure the file was open with \"w\" attribute");
		}
	}



private:
	FILE *workingFile;
};
