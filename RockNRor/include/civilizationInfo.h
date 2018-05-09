
#pragma once

#include "../../tinyxml/tinyxml.h"
#include <string>
#include <stdlib.h> // rand
#include <vector>

using namespace std;

#define CST_MAX_CIV_NAME_SIZE 20
#define CST_MAX_PLAYER_NAME_SIZE 19
#define CST_MAX_TOTAL_CIV_COUNT 256
#define CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV 9

class CivilizationInfo {
public:
	CivilizationInfo();
	CivilizationInfo(int civId);
	// Variables
	int civId;
	bool isDynamicCiv; // True for dynamically-generated civs
	vector<string> deathmatch_AI_file;
	vector<string> deathmatch_water_AI_file;
	vector<string> RM_AI_file_no_water;
	vector<string> RM_AI_file_some_water;
	vector<string> RM_AI_file_much_water;

	// Methods
	std::string GetCivName();
	void SetCivName(std::string name);
	void SetCivName(char *name);
	std::string GetPlayerName(int i) const;
	// Index 1-9. Returns true if successful (correct index)
	bool SetPlayerName(int i, std::string name);
	// Index 1-9. Returns true if successful (correct index)
	bool SetPlayerName(int i, char *name);
	bool IsStandard() const;
	// Returns a randomly-picked player name, empty string if none was found.
	// playerId is the player Id we are searching a name for (we store it in alreadyUsedNamesArray if found). Use 0 if you don't need to remember which player uses what name.
	std::string ChooseRandomPlayerName(char alreadyUsedNamesArray[], char playerId = 0) const;

private:
	std::string civName; // Use getter and setter to avoid size issues
	std::string playerNames[CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV]; // Use getter and setter to avoid index issues

	void ConstructorInit(); // Common inits for constructors
};

