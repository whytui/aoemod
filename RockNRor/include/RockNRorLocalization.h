
#pragma once
#include <string>
#include <map>
//#include <iostream>
#include <fstream>

class LocalizationHandler {
public:
	// Returns true if a string ID exists in "custom" translations.
	inline bool StringExists(unsigned short int stringId) const;
	// Get localized text from internal map and writes it in buffer.
	// Returns true if successful, false if string does not exist in internal map.
	bool ReadTranslation(unsigned short int stringId, char *buffer, long int bufferSize) const;
	// Get localized text from internal map and writes it in buffer.
	// Returns true if string was found. Otherwise, defaultText is written into buffer.
	bool ReadTranslation(unsigned short int stringId, char *buffer, long int bufferSize, const char *defaultText) const;
	// Returns translated text (custom strings only), or defaultText if not found.
	const char *GetTranslation(unsigned short int stringId, const char *defaultText) const;
	// Read a localization text file and imports all strings from it
	// Returns true if successful
	bool LoadTranslationsFromFile(std::string filename);
private:
	std::map<unsigned short int, const char*> localizedStrings;

	// Get a string from map, if found. Returns NULL if not found.
	const char *GetString(unsigned short int stringId) const;
	// Set a string in map. Overwrite previous string, if any.
	void SetString(unsigned short int stringId, const char *text);
	// Removes a string from map, if found.
	void RemoveString(unsigned short int stringId);
};

// Define it only once (in .cpp file) so all files will use the same instance.
extern LocalizationHandler localizationHandler;
