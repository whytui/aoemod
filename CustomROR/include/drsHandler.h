
#pragma once

#include <list>
#include <assert.h>
#include <ROR_structures_drs.h>
#include <ROR_global_variables.h>


namespace ROR_STRUCTURES_10C {

	// Returns a list of ALL object IDs (slpID, wav Id, shp Id, etc) from all DRS files used (loaded at this point) by the game
	std::list<long int> GetListOfUsedObjectIDsInDrsFiles();

	// Finds a DRS link object for a specific DRS filename.
	// Warning: unsure when 2 DRS files have same name (data\ vs data2\ files)
	ROR_STRUCTURES_10C::STRUCT_DRS_FILE_LINK *FindDrsLinkForFile(const char *drsFilename);

	// Runs constructor on a STRUCT_SLP_INFO object so it refers to a specific SLP.
	// This makes SLP bitmaps available for further treatments.
	void InitSlpInfoFromDrs(ROR_STRUCTURES_10C::STRUCT_SLP_INFO *slpInfo, long int slpId, char *shpName = "");


	// Loads / add a DRS file to internal data so its content (slp, wav, etc) is available like any other resources.
	// Filename is the DRS file name with extension. Example "graphics.drs"
	// folder is relative path from AOE root directory. Example "data2\"
	// WARNING: if DRS file does not exist, an empty one is created !
	void AOE_AddDrsFile(const char *filename, const char *folder);

	// A very technical primitive to retrieve objects from DRS files.
	void *AOE_GetDrsObject(char objTypeName[4], long int objectId, long int *objectSize);

	// unused, too technical, please see AOE_GetDrsObject
	bool AOE_GetDrsObjectInfos(long int objectId, ROR_STRUCTURES_10C::STRUCT_DRS_FILE **outDrsFile,
		long int *offsetInDrsFile, long int *outSize);

	// Returns a SLP file header object that corresponds to a slpID (object ID in one of the used DRS files)
	ROR_STRUCTURES_10C::STRUCT_SLP_FILE_HEADER *GetSlpFromDrsFiles(long int slpId);

	// Returns true if an object index (iconId, for example) is valid (exists) in a SLP file (SLP info object)
	bool IsObjectIndexValidInSlp(ROR_STRUCTURES_10C::STRUCT_SLP_INFO *slpInfo, long int objectIndex);
}
