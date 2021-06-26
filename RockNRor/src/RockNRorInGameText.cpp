#include "../include/RockNRorInGameText.h"
#include "../include/interruption.h"


namespace ROCKNROR {
	unsigned char integrityCheck = 0x9A;


	InGameTextHandler::InGameTextHandler() {
		
	}


	string InGameTextHandler::GetText(unsigned long int textId) const {
		if (textId >= this->GetCurrentTextArraySize()) {
			return string();
		}
		return this->textArray.at(textId);
	}


	// Update a text for a specific ID
	// Returns true if successful. Returns false (fails) if textId is not in current valid range.
	bool InGameTextHandler::UpdateText(unsigned long int textId, string value) {
		if (textId >= this->GetCurrentTextArraySize()) {
			return false;
		}
		this->textArray[textId] = value;
		return true;
	}


	// Add a text for a specific ID, and reallocates the array size if necessary
	// Returns true if successful
	// Returns false otherwise (textId < 0 or too big)
	bool InGameTextHandler::AddText(unsigned long int textId, string value) {
		if (textId < 0) {
			return false;
		}
		if (textId > IN_GAME_TEXT_MAX_ARRAY_SIZE) {
			ROCKNROR::SYSTEM::StopExecution(_T("Tried to allocate a too big array for InGameText"), true, true);
			return false;
		}
		if (textId >= this->GetCurrentTextArraySize()) {
			this->textArray.resize(textId + 1);
		}
		this->textArray[textId] = value;
		return true;
	}


	// Add a text with a new ID, and returns this textId
	unsigned long int InGameTextHandler::AppendNewText(string value) {
		unsigned long int previousSize = this->GetCurrentTextArraySize();
		if (previousSize + 1 > IN_GAME_TEXT_MAX_ARRAY_SIZE) {
			ROCKNROR::SYSTEM::StopExecution(_T("Reached max array size in InGameTextHandler"), true, true);
			this->UpdateText(0, "***Error, reached max array size in InGameTextHandler***");
			return 0;
		}
		this->textArray.push_back(value); // add after last
		return previousSize; // previous size = new index (starts at 0)
	}


	// Serialize object to output stream (file). May throw SerializeException
	long int InGameTextHandler::Serialize(FILE *outputFile) const {
		long int result = 0;
		result += this->WriteBytes(outputFile, &integrityCheck, sizeof(integrityCheck));

		unsigned long int size = this->GetCurrentTextArraySize();
		result += this->WriteBytes(outputFile, &size, sizeof(size));
		for (unsigned long int i = 0; i < size; i++) {
			string text = this->GetText(i);
			unsigned long int txtSize = text.size();
			if (txtSize > IN_GAME_TEXT_MAX_TEXT_SIZE) {
				txtSize = IN_GAME_TEXT_MAX_TEXT_SIZE; // Truncate
			}
			result += this->WriteBytes(outputFile, &txtSize, sizeof(txtSize));
			result += this->WriteBytes(outputFile, text.c_str(), txtSize);
		}
		return result;
	}


	// Deserialize object from input stream (file). May throw SerializeException
	bool InGameTextHandler::Deserialize(FILE *inputFile) {
		this->ClearAllText();

		unsigned char tmpCheck = 0;
		this->ReadBytes(inputFile, &tmpCheck, sizeof(tmpCheck));
		if (tmpCheck != integrityCheck) {
			throw ROCKNROR::SYSTEM::SerializeException("Integrity check failed in deserialize (InGameText)");
		}

		unsigned long int size = 0;
		this->ReadBytes(inputFile, &size, sizeof(size));
		assert(size <= IN_GAME_TEXT_MAX_ARRAY_SIZE);

		this->textArray.resize(size);

		char buffer[IN_GAME_TEXT_MAX_TEXT_SIZE + 1];
		for (unsigned long int i = 0; i < size; i++) {
			unsigned long int txtSize = 0;
			this->ReadBytes(inputFile, &txtSize, sizeof(txtSize));

			assert(txtSize <= IN_GAME_TEXT_MAX_TEXT_SIZE);
			if (txtSize > IN_GAME_TEXT_MAX_TEXT_SIZE) {
				throw ROCKNROR::SYSTEM::SerializeException("String too long in InGameText.Deserialize");
			}
			if (txtSize > 0) {
				this->ReadBytes(inputFile, buffer, txtSize);
				buffer[txtSize] = 0;
				this->textArray[i] = string(buffer);
			}
			else {
				this->textArray[i] = string();
			}
		}
		return true;
	}

}

