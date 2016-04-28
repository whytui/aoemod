#include <Windows.h>
#include <stdio.h>
//#include <string>
//#include <tchar.h>
#include "../../AOE_DataPatcher/include/GenericDataEditor.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"

#pragma once


/* Provides methods to manage files.
 * Takes an open file (FILE*) as entry parameter (constructor)
 * Caller must manage the file open mode (read/write) if he uses methods that write to the file
*/
class FileEditor : public GenericDataEditor
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


	// Reads "size" bytes from file to buffer
	// Buffer must be large enough and offset must be correct. Please check your buffer size first !
	int ReadBuffer(long fileOffset, void *destBuf, long size);


	// Writes "size" bytes from buffer to file
	// Buffer must be large enough and offset must be correct
	// The file must have been opened in write mode
	// Returns the number of written bytes (0 is not OK !)
	int WriteBuffer(long fileOffset, void *srcBuf, long size);


	int ReadSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf);

	int WriteSeqVariable(BinarySeqDefinition *seq, int seqIndex, void *destBuf);

protected:
	int WriteFromSequenceNoCheck(BinarySeqDefinition *seq, int seqIndex);


private:
	FILE *workingFile;
};
