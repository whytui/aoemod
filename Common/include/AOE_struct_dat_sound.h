
#pragma once



/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Size = 0x24
	class STRUCT_DAT_SOUND_FILE_INFO {
	public:
		char fileName[0x10]; // Actual size is 0xD (3 unused).
		long int resourceId; // +10. DRS resource id
		long int fileProbability; // +14
		unsigned long int *unknown_18; // Some pointer ? "TDigital"
		long int unknown_1C;
		unsigned long int unknown_20;
	};
	static_assert(sizeof(STRUCT_DAT_SOUND_FILE_INFO) == 0x24, "STRUCT_DAT_SOUND_FILE_INFO size");


	// Size 0x10. Constructor = 0x4A2000(fromFile) = datSound.read(internalFileId, pSounds, pMPInfo). "RGE_Sound" ?
	// 0x4A2370=play(arg1)
	// 0x4A2510=stop()
	class STRUCT_DAT_SOUND {
	public:
		short int unknown_00; // status ?
		short int filesCount; // +2
		STRUCT_DAT_SOUND_FILE_INFO *fileInfoArray; // +04. Array of sound file info, count=filesCount
		long int cacheTime; // +08. (300000, etc)
		short int soundId; // +0C
		short int unknown_0E;
	};
	static_assert(sizeof(STRUCT_DAT_SOUND) == 0x10, "STRUCT_DAT_SOUND Size");

}
