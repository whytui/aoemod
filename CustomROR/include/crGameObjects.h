#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

// ROR buildings always have "round" positions with a 0.5 precision (example 15.5 but never 15.45)
// So this threshold system should not be necessary.
#define REBUILD_FARMS_POSITION_THRESHOLD 0.25


class UnitCustomInfo {
public:
	UnitCustomInfo();

	long int unitId; // the unit custom information are about
	float spawnUnitMoveToPosX;
	float spawnUnitMoveToPosY;
	long int spawnTargetUnitId;
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

	// Returns a FarmRebuildInfo pointer to matching element for given position
	// Returns NULL if not found.
	FarmRebuildInfo *FindFarmRebuildInfo(float posX, float posY);
	// Returns (and adds if not existing) a FarmRebuildInfo pointer for given position.
	FarmRebuildInfo *FindOrAddFarmRebuildInfo(float posX, float posY);
	// Remove a FarmRebuildInfo element from list
	// Returns true if an element was found (and removed)
	bool RemoveFarmRebuildInfo(float posX, float posY);

private:
	vector<UnitCustomInfo*> unitCustomInfoList;
	vector<FarmRebuildInfo*> farmRebuildInfoList;

};

