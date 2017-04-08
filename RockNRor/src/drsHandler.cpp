
#include "../include/drsHandler.h"


namespace AOE_METHODS {

	// Frees all underlying info from a SLP INFO object, but does NOT free slpInfo itself.
	void FreeSlpInfoUnderlyingObjects(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo) {
		assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
		const unsigned long int callAddress = 0x49F840; // TODO : constant & other versions
		assert(slpInfo != NULL);
		_asm {
			MOV ECX, slpInfo;
			CALL callAddress;
		}
	}


	// Loads / add a DRS file to internal data so its content (slp, wav, etc) is available like any other resources.
	// Filename is the DRS file name with extension. Example "graphics.drs"
	// folder is relative path from AOE root directory. Example "data2\". If non-empty, it must end with "\" !!!
	void AddDrsFile(const char *filename, const char *folder) {
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
	void *GetDrsObject(char objTypeName[4], long int objectId, long int *objectSize) {
		if (!objectSize) {
			return NULL;
		}
		long int pFileOpenValue = 0;
		assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
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
			ADD ESP, 0x10;
		}
		return res;
	}

	// unused, too technical, please see GetDrsObject
	bool GetDrsObjectInfos(long int objectId, AOE_STRUCTURES::STRUCT_DRS_FILE **outDrsFile,
		long int *offsetInDrsFile, long int *outSize) {
		unsigned long int arg6 = (unsigned long int)outSize;
		unsigned long int arg5 = (unsigned long int)outDrsFile;
		unsigned long int arg4 = (unsigned long int)offsetInDrsFile;
		long int arg3 = 0;
		unsigned long int _pls = AOE_STRUCTURES::SlpTypeNameAsDword; // "slp " as a DWORD.
		assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
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

	// Runs constructor on a STRUCT_SLP_INFO object so it refers to a specific SLP.
	// This makes SLP bitmaps available for further treatments.
	// You need to alloc slpInfo first !
	void InitSlpInfoFromDrs(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo, long int slpId, const char *shpName) {
		if (!EXEADDR_SlpInfo_ctor) { return; }
		//char *shpName = ""; // unused when slpId is supplied
		_asm {
			PUSH slpId;
			PUSH shpName;
			MOV ECX, slpInfo;
			CALL EXEADDR_SlpInfo_ctor;
		}
	}


	// Completely (properly) frees a slp info object
	void FreeSlpInfo(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo) {
		assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
		AOE_METHODS::FreeSlpInfoUnderlyingObjects(slpInfo);
		const unsigned long int callHeapFree = 0x525FC2; // TODO : constant & other versions
		_asm {
			MOV ECX, slpInfo;
			PUSH ECX;
			CALL callHeapFree;
			ADD ESP, 4;
		}
	}

}


namespace AOE_STRUCTURES {

	// Returns a list of ALL object IDs (slpID, wav Id, shp Id, etc) from all DRS files used (loaded at this point) by the game
	std::list<long int> GetListOfUsedObjectIDsInDrsFiles() {
		std::list<long int> allIds;
		AOE_STRUCTURES::STRUCT_DRS_FILE_LINK *lnk = *AOE_STRUCTURES::ROR_firstDrsFileLink;
		// Loop on DRS file links
		while (lnk) {
			AOE_STRUCTURES::STRUCT_DRS_FILE *drsFile = lnk->drsFileContent;
			if (drsFile) {
				// In a DRS file: loop on "forms" ("tables") : slp, bina, wav...
				for (int itable = 0; itable < lnk->drsFileContent->includedTableCount; itable++) {
					//char *typeName = lnk->drsFileContent->includedTables[itable].typeName;
					AOE_STRUCTURES::STRUCT_DRS_TABLE *dt = &lnk->drsFileContent->includedTables[itable];
					for (long int curFile = 0; curFile < dt->filesCount; curFile++) {
						AOE_STRUCTURES::STRUCT_DRS_TABLE_DATA *dtData =
							drsFile->getDrsTableData(dt->fileInfoOffsetInDrsFile, curFile);
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
	AOE_STRUCTURES::STRUCT_DRS_FILE_LINK *FindDrsLinkForFile(const char *drsFilename) {
		AOE_STRUCTURES::STRUCT_DRS_FILE_LINK *lnk = *AOE_STRUCTURES::ROR_firstDrsFileLink;
		while (lnk && (strcmp(lnk->drsFileName, drsFilename) != 0)) {
			lnk = lnk->nextDrsFile;
		}
		return lnk; // NULL if not found
	}


	// Returns a SLP file header object that corresponds to a slpID (object ID in one of the used DRS files)
	AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *GetSlpFromDrsFiles(long int slpId) {
		long int size;
		AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *res = (AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *)
			AOE_METHODS::GetDrsObject(/*0x736C7020*/" pls", slpId, &size);
		return res;
	}

	// Returns true if an object index (iconId, for example) is valid (exists) in a SLP file (SLP info object)
	bool IsObjectIndexValidInSlp(AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo, long int objectIndex) {
		if (!slpInfo || (objectIndex < 0) || !slpInfo->slpFileHeader) {
			return false;
		}
		return (objectIndex < slpInfo->slpFileHeader->shapeCount);
	}

}
