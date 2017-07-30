
#include "../include/RockNRorLocalization.h"


// Define it only once (in .cpp file) so all files will use the same instance.
LocalizationHandler localizationHandler;


inline bool LocalizationHandler::StringExists(unsigned short int stringId) const {
	return (this->localizedStrings.find(stringId) != this->localizedStrings.end());
}

// Get localized text from internal map and writes it in buffer.
// Returns true if successful, false if string does not exist in internal map.
bool LocalizationHandler::ReadTranslation(unsigned short int stringId, char *buffer, long int bufferSize) const {
	if (!this->StringExists(stringId)) {
		return false;
	}
	strncpy_s(buffer, bufferSize, this->localizedStrings.at(stringId), bufferSize);
	buffer[bufferSize - 1] = 0;
	return true;
}

// Get localized text from internal map and writes it in buffer.
// Returns true if string was found. Otherwise, defaultText is written into buffer.
bool LocalizationHandler::ReadTranslation(unsigned short int stringId, char *buffer, long int bufferSize, const char *defaultText) const {
	if (this->ReadTranslation(stringId, buffer, bufferSize)) {
		return true;
	}
	int length = strlen(defaultText);
	if (length >= bufferSize) {
		length = bufferSize - 1;
	}
	strncpy_s(buffer, bufferSize, defaultText, length);
	buffer[bufferSize - 1] = 0;
	return false;
}


// Returns translated text (**custom strings only**), or defaultText if not found.
// For standard strings, please refer to "GetLanguageDllText"
const char *LocalizationHandler::GetTranslation(unsigned short int stringId, const char *defaultText) const {
	if (this->StringExists(stringId)) {
		return this->GetString(stringId);
	} else {
		return defaultText;
	}
}


// Read a localization text file and imports all strings from it
// Returns true if successful
bool LocalizationHandler::LoadTranslationsFromFile(std::string filename) {
	std::ifstream inputFile(filename);
	std::string currentLine;
	unsigned short int currentStringId = 0;
	if (inputFile.is_open()) {
		while (std::getline(inputFile, currentLine)) {
			bool ignoreLine;
			//std::string currentLine = std::string(bufferRead);
			ignoreLine = (currentLine.empty()) || (currentLine.substr(0, 2) == "//");
			if (!ignoreLine) {
				int posBegin = currentLine.find_first_of('[');
				int posEnd = currentLine.find_first_of(']');
				if ((posBegin == 0) && (posEnd > 1)) {
					std::string value = currentLine.substr(1, posEnd - 1);
					currentStringId = std::atoi(value.c_str()); // 0 if invalid
				} else {
					if ((currentStringId > 0) && (currentStringId <= 0xFFFF)) {
						this->SetString(currentStringId, currentLine.c_str());
						currentStringId = 0;
					}
				}
			}
		}
		inputFile.close();
	}
	return true;
}

// Get a string from map, if found. Returns NULL if not found.
const char *LocalizationHandler::GetString(unsigned short int stringId) const {
	if (this->StringExists(stringId)) {
		return this->localizedStrings.at(stringId);
	}
	return NULL;
}

// Removes a string from map, if found.
void LocalizationHandler::RemoveString(unsigned short int stringId) {
	if (this->StringExists(stringId)) {
		free((void*)this->GetString(stringId));
		this->localizedStrings[stringId] = NULL;
	}
}

// Set a string in map. Overwrite previous string, if any.
void LocalizationHandler::SetString(unsigned short int stringId, const char *text) {
	this->RemoveString(stringId);
	int length = strlen(text);
	char *s = (char*)malloc(length + 1);
	strcpy_s(s, length + 1, text);
	for (int i = 0; i < length - 1; i++) {
		if ((s[i] == '\\') && (s[i + 1] == 'n')) {
			s[i] = '\r';
			s[i+1] = '\n';
		}
	}
	this->localizedStrings[stringId] = s;
}

