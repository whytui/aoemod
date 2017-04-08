
#pragma once


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	class STRUCT_DRS_TABLE;
	class STRUCT_DRS_TABLE_DATA;
	class STRUCT_DRS_FILE;
	class STRUCT_DRS_FILE_LINK;
	class STRUCT_SLP_FILE_HEADER;
	class STRUCT_SLP_INFO;


	static const char DrsTableTribeType[] = "tribe";
	static unsigned long int SlpTypeNameAsDword = 0x736C7020; // "slp " as a DWORD.

	// Size 0x0C
	class STRUCT_DRS_TABLE {
	public:
		char typeName[4]; // " wav", " pls"... Chars 1-3=reversed extension. First char='a' if extension is "bin" (to indicate when it is Text?), ' ' otherwise.
		long int fileInfoOffsetInDrsFile; // +04. Offset to be added to STRUCT_DRS_FILE object, to retrieve table data (STRUCT_DRS_TABLE_DATA)
		long int filesCount; // +08: number of included files for this type (wav, slp, shp, bina...)

		bool IsSlp() const {
			return *((unsigned long int*)&this->typeName) == SlpTypeNameAsDword; // optimized comparison (DWORD)
		}
		bool IsShp() const {
			return (typeName[0] == ' ') && (typeName[1] == 'p') && (typeName[2] == 'h') && (typeName[3] == 's');
		}
		bool IsBina() const {
			return (typeName[0] == 'a') && (typeName[1] == 'n') && (typeName[2] == 'i') && (typeName[3] == 'b');
		}
		bool IsWav() const {
			return (typeName[0] == ' ') && (typeName[1] == 'v') && (typeName[2] == 'a') && (typeName[3] == 'w');
		}
	};
	static_assert(sizeof(STRUCT_DRS_TABLE) == 0xC, "STRUCT_DRS_TABLE Size");

	// Size 0x0C
	// Represents a table of DRS-included objects ("file") info (bina, slp, wav...) => id + offset/size in DRS file.
	class STRUCT_DRS_TABLE_DATA {
	public:
		long int objectId; // Corresponds to id visible in DRS editor (slpId, etc)
		long int offsetInDrsFile; // +04. Offset to be added to STRUCT_DRS_FILE object, to retrieve file data (binary)
		long int objectSize; // +08. File binary data size in bytes.
	};
	static_assert(sizeof(STRUCT_DRS_TABLE_DATA) == 0xC, "STRUCT_DRS_TABLE_DATA Size");

	// Size 0x40 + tableCount*0x0C.
	// Note: This is the merged result of data/xxx.drs and data2/xxx.drs (when both exist)
	// Useful method: 0x46B280=getObjFromInterfacDrs(objTypeName, objId, pvar, pOutOffsetInDrsFile, pOutDrsFileContent, pOutObjSize)
	// Useful method: 0x46AFC0=addDrsFile(fileName, "tribe", folder)
	class STRUCT_DRS_FILE {
	public:
		char copyrightHeader[0x28]; // text about copyright.
		char version[4]; // +28. Like 1.00
		char fileType[0x0C]; // +2C. // "tribe"...
		long int includedTableCount; // +38. Number of element in +40 (includedTables)
		long int firstFileDataOffset; // +3C. unsure
		// 0x40
		STRUCT_DRS_TABLE includedTables[1]; // number of element is tableCount
		// STRUCT_DRS_TABLE_DATA includedTableData[]? total offset: cf includedTables[i].offsetInDrsFile
		// This corresponds to file info object, with actual file content offset and size.
		STRUCT_DRS_TABLE_DATA *getDrsTableData(unsigned long int offsetInDrsFile, long int fileIndex) const {
			STRUCT_DRS_TABLE_DATA *arrayBase = (STRUCT_DRS_TABLE_DATA *)(((unsigned long int)this) + offsetInDrsFile);
			return &arrayBase[fileIndex];
		}
	};


	// 0x7BFAC4 = first DRS link element (STRUCT_DRS_FILE_LINK*)
	// Size=0x114 (cf 46B090) - no constructor ?
	class STRUCT_DRS_FILE_LINK {
	public:
		STRUCT_DRS_FILE *drsFileContent_00; // +00. Why is there 2 pointers ??? This one is nullable.
		long int unknown_04; // +04. File open call result ?
		STRUCT_DRS_FILE *drsFileContent; // +08. It seems better to use this pointer.
		STRUCT_DRS_FILE_LINK *nextDrsFile; // +0C.
		char drsFileName[0x104]; // +10. Unknown size. Includes file extension (.drs)
	};
	static_assert(sizeof(STRUCT_DRS_FILE_LINK) == 0x114, "STRUCT_DRS_FILE_LINK size");

	// Header size=0x20
	class STRUCT_SLP_FILE_HEADER {
	public:
		char version[4]; // +0. Generally "2.0N"
		long int shapeCount; // +4. The number of "SLP_FRAME_HEADER" following this header.
		char description[0x18]; // +08.
		// Does it always continues with SLP file data ?
		// char fileData[1] // Unknown size
	};
	static_assert(sizeof(STRUCT_SLP_FILE_HEADER) == 0x20, "STRUCT_SLP_FILE_HEADER size"); // to remove if fileData is uncommented


	// Slp frame header, Size=0x20. An array of this is located just after STRUCT_SLP_FILE_HEADER in file.
	// 1 frame = 1 shape in the SLP file.
	class STRUCT_SLP_FRAME_HEADER {
	public:
		long int commandTableOffset;
		long int outlineTableOffset; // +04. Refers to an array of STRUCT_SLP_FRAME_ROW_EDGE ?
		long int paletteOffset; // +08.
		long int properties; // +0C. Unsure. 0x10=game default palette ?
		long int xSize; // +10.
		long int ySize; // +14.
		long int xCenterPos; // +18. Relative position of sprite's center
		long int yCenterPos; // +1C. Relative position of sprite's center
	};
	static_assert(sizeof(STRUCT_SLP_FRAME_HEADER) == 0x20, "STRUCT_SLP_FRAME_HEADER size");


	// Represents the definition of side transparent pixels for a shape.
	class STRUCT_SLP_FRAME_ROW_EDGE {
	public:
		short int leftSpace; // +00. Number of transparent pixels on left side for current row ?
		short int rightSpace; // +04. Number of transparent pixels on right side for current row ?
	};
	static_assert(sizeof(STRUCT_SLP_FRAME_ROW_EDGE) == 0x04, "STRUCT_SLP_FRAME_ROW_EDGE size");


	// Size = 0x20. Constructor = 0x49F5F0. "TShape".
	// Destructor? = 0x49F840 (does NOT free).
	// 0x49FA90=slpInfo.GetShapeCount() : info taken from slpFileHeader
	class STRUCT_SLP_INFO {
	public:
		STRUCT_SLP_FILE_HEADER *slpData_00; // +0. Set to NULL after init ? A temp value ?
		unsigned long int unknown_04; // +4. Same type as STRUCT_DRS_FILE_LINK+4
		long int slpSize; // +08.
		unsigned long int unknown_0C;
		// 0x10
		unsigned long int unknown_10;
		unsigned long int unknown_14;
		STRUCT_SLP_FILE_HEADER *slpFileHeader; // +18. Use this one when +00 is NULL.
		STRUCT_SLP_FRAME_HEADER *slpFileData; // +1C. Pointer to an array of SLP frame header.
	};
	static_assert(sizeof(STRUCT_SLP_INFO) == 0x20, "STRUCT_SLP_INFO size");

}
