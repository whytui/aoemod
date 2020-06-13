#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>
#include "autoAttackPolicy.h"
#include "Serializable.h"

using namespace std;

// ROR buildings always have "round" positions with a 0.5 precision (example 15.5 but never 15.45)
// So this threshold system should not be necessary.
#define REBUILD_FARMS_POSITION_THRESHOLD 0.25


// Stores specific information about a game unit, for RockNRor features
class UnitCustomInfo : public ROCKNROR::SYSTEM::Serializable {
public:
	UnitCustomInfo();

	long int unitId; // the unit custom informations are about
	float spawnUnitMoveToPosX; // "rally point" X location
	float spawnUnitMoveToPosY; // "rally point" X location
	long int spawnTargetUnitId; // "rally point" target unit
	AutoAttackPolicy autoAttackPolicy;
	bool autoAttackPolicyIsSet;
	float protectPosX;
	float protectPosY;
	long int protectUnitId;
	long int myMainBuilderId; // unitId of "main" villager builder, that will have priority on others for next action (farming when farm is built...). -1=N/A (most cases).

	// Returns true if object contains no relevant information and can be removed. (all values are set to "none" or default)
	bool CanBeRemoved() const;
	// Returns true if one of (protect unit, protect location) is defined.
	bool HasValidProtectInfo() const;
	void ResetSpawnAutoTargetInfo();
	void ResetProtectInfo();

	// Serialize object to output stream (file). May throw SerializeException
	long int Serialize(FILE *outputFile) const override;
	// Deserialize object from input stream (file). May throw SerializeException
	bool Deserialize(FILE *inputFile) override;
};


// Stores specific information about a farm, for RockNRor "auto-rebuild farms" feature
class FarmRebuildInfo : public ROCKNROR::SYSTEM::Serializable {
public:
	FarmRebuildInfo();
	long int playerId;
	long int villagerUnitId; // The villager we will use to trigger rebuild operation.
	float posX;
	float posY;
	long int gameTime; // Last update game time in ms
	bool forceRebuild;
	bool forceNotRebuild;

	// Serialize object to output stream (file). May throw SerializeException
	long int Serialize(FILE *outputFile) const override;
	// Deserialize object from input stream (file). May throw SerializeException
	bool Deserialize(FILE *inputFile) override;
};


// Handler class for RockNRor-specific objects dedicated to current game, such as auto-build farms info, rally points, etc.
class RockNRorGameObjects : public ROCKNROR::SYSTEM::Serializable {
public:
	RockNRorGameObjects();
	~RockNRorGameObjects();

	bool currentGameHasAllTechs; // Indicates if current game has "all techs" option. Always set, even for saved games.
	bool doNotApplyHardcodedCivBonus; // Indicates if "hardcoded civilization bonuses" should NOT be applied in current game, eg Macedonian conversion resistance. True if all techs, or if using "generated tech trees".

	void ResetObjects();
	void FreeAllUnitCustomInfoList();
	void FreeAllFarmRebuildInfoList();

	// Remove all information concerning a specific unit
	// Does not impact unitExtensions though
	bool RemoveAllInfoForUnit(long int unitId, float posX, float posY);

	// Returns a UnitCustomInfo pointer to matching element for given unitId.
	// Returns NULL if not found.
	// Asserts unitId > 0
	UnitCustomInfo *FindUnitCustomInfo(long int unitId);
	// Returns (and adds if not existing) a UnitCustomInfo pointer to matching element for given unitId.
	// Asserts unitId > 0
	UnitCustomInfo *FindOrAddUnitCustomInfo(long int unitId);
	// Remove custom information for a specific unit
	// Returns true if an element was found (and removed)
	bool RemoveUnitCustomInfo(long int unitId);
	// Remove custom information for a specific unit, ONLY if it contains no relevant value (all parameters are not applicable/default)
	// Returns true if an element was removed
	bool RemoveUnitCustomInfoIfEmpty(long int unitId);

	// Returns a FarmRebuildInfo pointer to matching element for given position
	// Returns NULL if not found.
	FarmRebuildInfo *FindFarmRebuildInfo(float posX, float posY);
	// Returns (and adds if not existing) a FarmRebuildInfo pointer for given position.
	FarmRebuildInfo *FindOrAddFarmRebuildInfo(float posX, float posY);
	// Remove a FarmRebuildInfo element from list
	// Returns true if an element was found (and removed)
	bool RemoveFarmRebuildInfo(float posX, float posY);

	// Remove "protect" info from all "unit info" objects that tell to protect a specific unit id
	bool RemoveProtectedUnit(long int protectedUnitId);

	// Serialize object to output stream (file). May throw SerializeException
	long int Serialize(FILE *outputFile) const override;
	// Deserialize object from input stream (file). May throw SerializeException
	bool Deserialize(FILE *inputFile) override;
private:
	vector<UnitCustomInfo*> unitCustomInfoList;
	vector<FarmRebuildInfo*> farmRebuildInfoList;

};

