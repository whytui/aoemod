#include "FileEditor.h"


FileEditor::FileEditor()
{
	this->workingFile = NULL;
}

FileEditor::FileEditor(FILE *file)
{
	this->workingFile = file;

}

FileEditor::~FileEditor()
{
}

long FileEditor::GetFileSize() {
	if (!this->workingFile) { throw std::exception("No file selected"); }
	fseek(this->workingFile, 0, SEEK_END);
	long res = ftell(this->workingFile);
	rewind(this->workingFile);
	return res;
}

void FileEditor::SetFile(FILE *file) {
	if (this->workingFile && file) { // warning if a file was open EXCEPT if this is an "unset" (file == NULL)
		printf("Warning: a file was already open\n");
	}
	this->workingFile = file;
}


bool FileEditor::CheckBufferConsistency(const unsigned char *buf, long offsetBegin, long size) {
	if (size < 1) { return true; } // Empty sequence: can not differ !
	if ((offsetBegin < 0) || (offsetBegin + size - 1 > this->GetFileSize())) {
		throw std::exception("Offsets are out of file bounds");
	}
	// Read file sequence
	unsigned char * fileBuf = (unsigned char *) malloc(size);

	fseek(this->workingFile, offsetBegin, 0);
	size_t readBytes = fread_s((void *)fileBuf, size, 1, size, this->workingFile);
	if (readBytes != size) { 
		free(fileBuf);
		fileBuf = NULL;
		throw std::exception("Could not read file sequence");
	}

	long relativePos = 0;
	while (relativePos < size) {
		if (buf[relativePos] != fileBuf[relativePos]) { 
			free(fileBuf);
			fileBuf = NULL;
			return false;
		}
		relativePos++;
	}

	free(fileBuf);
	fileBuf = NULL;
	return true;
}


bool FileEditor::CheckSeqConsistency(BinarySeqDefinition *seq, int seqIndex) {
	if (!seq->IsValid()) { return false; }
	if (seq->GetVarType(seqIndex) == SVT_NO_VARIABLE) {
		// No variable - simple case
		return (this->CheckBufferConsistency(seq->GetSequence(seqIndex), seq->GetSeqOffset(), seq->GetSeqSize()));
	} else {
		// sequence owns a variable whose value is free (allowed to differ between definition and file content)
		// Before the variable
		bool tmpRes = this->CheckBufferConsistency(seq->GetSequence(seqIndex), seq->GetSeqOffset(), seq->GetVarRelativeOffset(seqIndex));
		// After the variable
		tmpRes = tmpRes && this->CheckBufferConsistency(seq->GetSequenceShifted(seqIndex, seq->GetVarRelativeOffset(seqIndex) + seq->GetVarSize(seqIndex)),
			seq->GetSeqOffset() + seq->GetVarRelativeOffset(seqIndex) + seq->GetVarSize(seqIndex),
			seq->GetSeqSize() - seq->GetVarRelativeOffset(seqIndex) - seq->GetVarSize(seqIndex));
		return tmpRes;
	}
}


int FileEditor::CheckSeqConsistency(BinarySeqDefinition *seq) {
	if (!seq->IsValid()) { return -1; }

	for (int i = 0; i < seq->GetTotalSeqCount(); i++) {
		if (CheckSeqConsistency(seq, i)) { return i; }
	}
	return -1;
}


int FileEditor::ReadBuffer(long fileOffset, void *destBuf, long size) {
	fseek(this->workingFile, fileOffset, 0);
	size_t readBytes = fread_s((void *)destBuf, size, 1, size, this->workingFile);
	return readBytes;
}

int FileEditor::WriteBuffer(long fileOffset, void *srcBuf, long size) {
	fseek(this->workingFile, fileOffset, 0);
	size_t writtenBytes = fwrite(srcBuf, 1, size, this->workingFile);
	return writtenBytes;
}

// See also WriteFromSequenceUsingValue
int FileEditor::WriteFromSequence(BinarySeqDefinition *seq, int seqIndex) {
	if (!seq->IsValid()) {
		printf("The sequence is invalid");
		return 0;
	}
	try {
		return this->WriteBuffer(seq->GetSeqOffset(), seq->GetSequence(seqIndex), seq->GetSeqSize());
	}
	catch (...) { // TO DO : replace with something like InvalidSequenceIndexException ?
		printf("The sequence or its index is invalid");
		return 0;
	}
}

