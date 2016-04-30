#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;



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
	long int farmUnitId;
	long int villagerUnitId;
	float posX;
	float posY;
	long int gameTime; // Insertion game time in ms, used to remove obsolete entries
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

	// Removes obsolete farm info elements (insertion game time is old)
	void FlushObsoleteFarmInfo(long int currentGameTime);
	// Returns a FarmRebuildInfo pointer to matching element for given unitId.
	// Returns NULL if not found.
	// Asserts unitId > 0
	FarmRebuildInfo *FindFarmRebuildInfo(long int farmUnitId);
	// Returns (and adds if not existing) a FarmRebuildInfo pointer to matching element for given unitId.
	// Asserts unitId > 0
	FarmRebuildInfo *FindOrAddFarmRebuildInfo(long int farmUnitId);
	// Remove a FarmRebuildInfo element from list
	// Returns true if an element was found (and removed)
	bool RemoveFarmRebuildInfo(long int farmUnitId);

private:
	vector<UnitCustomInfo*> unitCustomInfoList;
	vector<FarmRebuildInfo*> farmRebuildInfoList;

};

