
#pragma once

#include <list>
#include <assert.h>
#include <AOE_structures_drs.h>
#include <ROR_global_variables.h>


namespace AOE_METHODS {
	// Frees all underlying info from a SLP INFO object, but does NOT free slpInfo itself.
	void FreeSlpInfoUnderlyingObjects(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo);


	// Loads / add a DRS file to internal data so its content (slp, wav, etc) is available like any other resources.
	// Filename is the DRS file name with extension. Example "graphics.drs"
	// folder is relative path from AOE root directory. Example "data2\"
	// WARNING: if DRS file does not exist, an empty one is created !
	void AddDrsFile(const char *filename, const char *folder);

	// A very technical primitive to retrieve objects from DRS files.
	void *GetDrsObject(char objTypeName[4], long int objectId, long int *objectSize);

	// unused, too technical, please see AOE_GetDrsObject
	bool GetDrsObjectInfos(long int objectId, AOE_STRUCTURES::STRUCT_DRS_FILE **outDrsFile,
		long int *offsetInDrsFile, long int *outSize);

	// Runs constructor on a STRUCT_SLP_INFO object so it refers to a specific SLP.
	// This makes SLP bitmaps available for further treatments.
	// You need to alloc slpInfo first !
	void InitSlpInfoFromDrs(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo, long int slpId, const char *shpName = "");

	// Completely (properly) frees a slp info object
	void FreeSlpInfo(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo);

}


namespace AOE_STRUCTURES {

	// Returns a list of ALL object IDs (slpID, wav Id, shp Id, etc) from all DRS files used (loaded at this point) by the game
	std::list<long int> GetListOfUsedObjectIDsInDrsFiles();

	// Finds a DRS link object for a specific DRS filename.
	// Warning: unsure when 2 DRS files have same name (data\ vs data2\ files)
	AOE_STRUCTURES::STRUCT_DRS_FILE_LINK *FindDrsLinkForFile(const char *drsFilename);

	// Returns a SLP file header object that corresponds to a slpID (object ID in one of the used DRS files)
	AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *GetSlpFromDrsFiles(long int slpId);

	// Returns true if an object index (iconId, for example) is valid (exists) in a SLP file (SLP info object)
	bool IsObjectIndexValidInSlp(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo, long int objectIndex);
}
