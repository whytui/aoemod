#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>
#include "Serializable.h"

using namespace std;

#define IN_GAME_TEXT_MAX_ARRAY_SIZE 50000
#define IN_GAME_TEXT_MAX_TEXT_SIZE 2000

namespace ROCKNROR {

// Stores specific information about a game unit, for RockNRor features
class InGameTextHandler : public SYSTEM::Serializable {
public:
	InGameTextHandler();

	unsigned long int GetCurrentTextArraySize() const {
		return this->textArray.size();
	}

	// Get a text from its ID
	string GetText(unsigned long int textId) const;
	/*string GetText(int textId) const {
		if (textId < 0) { return string(); }
		return this->GetText((unsigned int)textId);
	}*/

	// Update a text for a specific ID.
	// Returns true if successful. Returns false (fails) if textId is not in current valid range.
	bool UpdateText(unsigned long int textId, string value);
	/*bool UpdateText(int textId, string value) {
		if (textId < 0) { return false; }
		return this->UpdateText((unsigned int)textId, value);
	}*/

	// Add a text for a specific ID, and reallocates the array size if necessary
	bool AddText(unsigned long int textId, string value);
	/*bool AddText(unsigned int textId, string value) {
		if (textId < 0) { return false; }
		return this->AddText((unsigned int)textId, value);
	}*/

	// Add a text with a new ID, and returns this textId
	unsigned long int AppendNewText(string value);

	// Reset the custom text collection (completely clear the array)
	void ClearAllText() {
		this->textArray.clear();
	}

	// Serialize object to output stream (file). May throw SerializeException
	long int Serialize(FILE *outputFile) const override;
	// Deserialize object from input stream (file). May throw SerializeException
	bool Deserialize(FILE *inputFile) override;

private:
	vector<string> textArray;
};

}
