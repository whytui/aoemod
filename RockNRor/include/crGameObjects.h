#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>
#include "autoAttackPolicy.h"

using namespace std;

// ROR buildings always have "round" positions with a 0.5 precision (example 15.5 but never 15.45)
// So this threshold system should not be necessary.
#define REBUILD_FARMS_POSITION_THRESHOLD 0.25


class UnitCustomInfo {
public:
	UnitCustomInfo();

	long int unitId; // the unit custom informations are about
	float spawnUnitMoveToPosX;
	float spawnUnitMoveToPosY;
	long int spawnTargetUnitId;
	AutoAttackPolicy autoAttackPolicy;
	bool autoAttackPolicyIsSet;
	float protectPosX;
	float protectPosY;
	long int protectUnitId;

	// Returns true if object contains no relevant information and can be removed. (all values are set to "none" or default)
	bool CanBeRemoved() const;
	// Returns true if one of (protect unit, protect location) is defined.
	bool HasValidProtectInfo() const;
	void ResetSpawnAutoTargetInfo();
	void ResetProtectInfo();
};



class FarmRebuildInfo {
public:
	FarmRebuildInfo();
	long int playerId;
	long int villagerUnitId; // The villager we will use to trigger rebuild operation.
	float posX;
	float posY;
	long int gameTime; // Last update game time in ms
	bool forceRebuild;
	bool forceNotRebuild;
};


class CrGameObjects {
public:
	CrGameObjects();
	~CrGameObjects();

	void ResetObjects();
	void FreeAllUnitCustomInfoList();
	void FreeAllFarmRebuildInfoList();

	// Remove all information concerning a specific unit
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

private:
	vector<UnitCustomInfo*> unitCustomInfoList;
	vector<FarmRebuildInfo*> farmRebuildInfoList;

};

