
#include "../include/drsHandler.h"

namespace ROR_STRUCTURES_10C {

	// Returns a list of ALL object IDs (slpID, wav Id, shp Id, etc) from all DRS files used (loaded at this point) by the game
	std::list<long int> GetListOfUsedObjectIDsInDrsFiles() {
		std::list<long int> allIds;
		ROR_STRUCTURES_10C::STRUCT_DRS_FILE_LINK *lnk = *ROR_STRUCTURES_10C::ROR_firstDrsFileLink;
		// Loop on DRS file links
		while (lnk) {
			ROR_STRUCTURES_10C::STRUCT_DRS_FILE *drsFile = lnk->drsFileContent;
			if (drsFile) {
				// In a DRS file: loop on "forms" ("tables") : slp, bina, wav...
				for (int itable = 0; itable < lnk->drsFileContent->includedTableCount; itable++) {
					//char *typeName = lnk->drsFileContent->includedTables[itable].typeName;
					ROR_STRUCTURES_10C::STRUCT_DRS_TABLE *dt = &lnk->drsFileContent->includedTables[itable];
					for (long int curFile = 0; curFile < dt->filesCount; curFile++) {
						ROR_STRUCTURES_10C::STRUCT_DRS_TABLE_DATA *dtData =
							drsFile->getDrsTableData(dt->offsetInDrsFile, curFile);
						allIds.push_back(dtData->objectId);
					}
				}
			}
			lnk = lnk->nextDrsFile;
		}
		return allIds;
	}


	// Finds a DRS link object for a specific DRS filename.
	// Warning: unsure when 2 DRS files have same name (data\ vs data2\ files)
	ROR_STRUCTURES_10C::STRUCT_DRS_FILE_LINK *FindDrsLinkForFile(const char *drsFilename) {
		ROR_STRUCTURES_10C::STRUCT_DRS_FILE_LINK *lnk = *ROR_STRUCTURES_10C::ROR_firstDrsFileLink;
		while (lnk && (strcmp(lnk->drsFileName, drsFilename) != 0)) {
			lnk = lnk->nextDrsFile;
		}
		return lnk; // NULL if not found
	}

	// Runs constructor on a STRUCT_SLP_INFO object so it refers to a specific SLP.
	// This makes SLP bitmaps available for further treatments.
	void InitSlpInfoFromDrs(ROR_STRUCTURES_10C::STRUCT_SLP_INFO *slpInfo, long int slpId, char *shpName) {
		if (!EXEADDR_SlpInfo_ctor) { return; }
		//char *shpName = ""; // unused when slpId is supplied
		_asm {
			PUSH slpId;
			PUSH shpName;
			MOV ECX, slpInfo;
			CALL EXEADDR_SlpInfo_ctor;
		}
	}


	// Loads / add a DRS file to internal data so its content (slp, wav, etc) is available like any other resources.
	// Filename is the DRS file name with extension. Example "graphics.drs"
	// folder is relative path from AOE root directory. Example "data2\". If non-empty, it must end with "\" !!!
	void AOE_AddDrsFile(const char *filename, const char *folder) {
		const unsigned long int callAddress = EXEADDR_AddDrsFile;
		if (!filename) { return; }
		const char *tribe = "tribe";
		_asm {
			PUSH 0; // Not sure what exactly it means, 1 for sounds.drs. If 1, drsLink+0 is no valued and AOE_GetDrsObjectInfos won't work.
			PUSH folder;
			PUSH tribe;
			PUSH filename;
			CALL callAddress;
			// Don't know if there is a return value ?
			ADD ESP, 0x10;
		}
	}


	// A very technical primitive to retrieve objects from DRS files.
	void *AOE_GetDrsObject(char objTypeName[4], long int objectId, long int *objectSize) {
		if (!objectSize) {
			return NULL;
		}
		long int pFileOpenValue = 0;
		unsigned long int callAddr = 0x46B360;
		unsigned long int nameAsDword = *((unsigned long int*)objTypeName);
		void *res = NULL;
		_asm {
			PUSH objectSize;
			LEA EAX, pFileOpenValue;
			PUSH EAX;
			PUSH objectId;
			PUSH nameAsDword;
			CALL callAddr;
			MOV res, EAX;
			ADD ESP, 0x10
		}
		return res;
	}

	// unused, too technical, please see AOE_GetDrsObject
	bool AOE_GetDrsObjectInfos(long int objectId, ROR_STRUCTURES_10C::STRUCT_DRS_FILE **outDrsFile,
		long int *offsetInDrsFile, long int *outSize) {
		unsigned long int arg6 = (unsigned long int)outSize;
		unsigned long int arg5 = (unsigned long int)outDrsFile;
		unsigned long int arg4 = (unsigned long int)offsetInDrsFile;
		long int arg3 = 0;
		unsigned long int _pls = 0x736C7020; // "slp " as an DWORD.
		unsigned long int callAddr = 0x46B280;
		long int res = 0;
		_asm {
			PUSH arg6;
			PUSH arg5;
			PUSH arg4;
			LEA EAX, arg3;
			PUSH EAX;
			PUSH objectId;
			PUSH _pls;
			CALL callAddr;
			MOV res, EAX;
			ADD ESP, 0x18;
		}
		return (res != 0);
	}

	// Returns a SLP file header object that corresponds to a slpID (object ID in one of the used DRS files)
	ROR_STRUCTURES_10C::STRUCT_SLP_FILE_HEADER *GetSlpFromDrsFiles(long int slpId) {
		long int size;
		ROR_STRUCTURES_10C::STRUCT_SLP_FILE_HEADER *res = (ROR_STRUCTURES_10C::STRUCT_SLP_FILE_HEADER *)
			AOE_GetDrsObject(/*0x736C7020*/" pls", slpId, &size);
		return res;
	}

	// Returns true if an object index (iconId, for example) is valid (exists) in a SLP file (SLP info object)
	bool IsObjectIndexValidInSlp(ROR_STRUCTURES_10C::STRUCT_SLP_INFO *slpInfo, long int objectIndex) {
		if (!slpInfo || (objectIndex < 0) || !slpInfo->slpFileHeader) {
			return false;
		}
		return (objectIndex < slpInfo->slpFileHeader->shapeCount);
	}

}
