#include "../include/civilizationInfo.h"



CivilizationInfo::CivilizationInfo() {
	this->ConstructorInit();
}


CivilizationInfo::CivilizationInfo(int civId) {
	this->ConstructorInit();
	this->civId = civId;
}


void CivilizationInfo::ConstructorInit() {
	this->civId = -1;
	this->SetCivName("");
	this->isDynamicCiv = false;
	for (int i = 1; i <= 9; i++) {
		this->SetPlayerName(i, "");
	}
}


bool CivilizationInfo::IsStandard() const {
	return (this->civId >= 0) && (this->civId <= 16);
}


std::string CivilizationInfo::GetCivName() {
	return this->civName;
}

void CivilizationInfo::SetCivName(std::string name) {
	this->civName = name.substr(0, CST_MAX_CIV_NAME_SIZE);
}
void CivilizationInfo::SetCivName(char *name) {
	this->civName = name;
	this->civName = this->civName.substr(0, CST_MAX_CIV_NAME_SIZE);
}

// Access player names with indexes from 1 to 9. Returns "" if index is invalid.
std::string CivilizationInfo::GetPlayerName(int i) const {
	if ((i < 1) || (i > CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV)) {
		return "";
	}
	return this->playerNames[i - 1];
}

// Index 1-9. Returns true if successful (correct index)
bool CivilizationInfo::SetPlayerName(int i, std::string name) {
	if ((i < 1) || (i > CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV)) {
		return false;
	}
	this->playerNames[i - 1] = name;
	this->playerNames[i - 1] = this->playerNames[i - 1].substr(0, CST_MAX_PLAYER_NAME_SIZE);
	return true;
}

// Index 1-9. Returns true if successful (correct index)
bool CivilizationInfo::SetPlayerName(int i, char *name) {
	if ((i < 1) || (i > CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV)) {
		return false;
	}
	this->playerNames[i - 1] = name;
	this->playerNames[i - 1] = this->playerNames[i - 1].substr(0, CST_MAX_PLAYER_NAME_SIZE);
	return true;
}


// Warning: alreadyUsedNamesArray indexes start at 0, not 1 (player name indexes are 1-9)
std::string CivilizationInfo::ChooseRandomPlayerName(char alreadyUsedNamesArray[], char playerId) const {
	if (playerId < 0) { return ""; } // Would be stupid.
	assert(CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV > 8); // Or we will get infinite loops !!!
	// Create a list of random values to pick from. Each number from 1 to CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV will appear ONCE.
	// In all this parts, indexes go from 0 to CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV-1
	bool available[CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV];
	int randoms[CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV]; // contains numbers from 1 to CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV
	for (int index = 0; index < CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV; index++) {
		available[index] = true; // initially, all are available.
		randoms[index] = -1; // initially, all "randoms" are unset.
	}
	for (int index = 0; index < CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV; index++) {
		// TODO: use our randomizer
		int r = (rand() % CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV); // random number from 0 to CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV-1
		while (!available[r]) { // already picked, go to next...
			r++;
			if (r >= CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV) { r = 0; }
		}
		randoms[index] = r + 1; // found a random value for this index.
		available[r] = false; // do not pick that number again.
	}

	// Now use our random values list...
	int currentIndex = 0;
	int currentRandom = randoms[currentIndex];
	while (( // Empty name OR already used: try next random name index...
		(this->GetPlayerName(currentRandom).empty()) ||
		(alreadyUsedNamesArray[currentRandom - 1] >= 0) // BE CAREFUL with indexes here
		) &&
		currentIndex < CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV) { // BUT stop loop as soon as all numbers have ben picked
		currentIndex++;
		currentRandom = randoms[currentIndex];
	}
	if (!this->GetPlayerName(currentRandom).empty()) {
		// If found, store player id so that the name won't be used again in this game.
		alreadyUsedNamesArray[currentRandom - 1] = playerId; // BE CAREFUL with indexes here
	}
	return this->GetPlayerName(currentRandom); // empty string if not found
}

