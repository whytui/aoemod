
#pragma once



/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Credit: First debugging of sound objects was initially done by LucTieuPhung

	class STRUCT_SOUND_DRIVER;


	// "TDigital". Size = 0x3C. Constructor=0x4A2BE0.
	class STRUCT_SOUND_TDIGITAL {
	public:
		STRUCT_SOUND_DRIVER *pSoundStruct;
		unsigned long int unknown_04;
		unsigned short int unknown_08;
		char soundName[0x0D]; // +0A. e.g. Size=0xC + 1 for ending "\0" (0x4A2C0C)
		char unknown_17; // unused ?
		long int unknown_18;
		long int lVolume; // +1C
		// 0x20
		long int dwFrequency; // +20. pitch?
		long int lPan;
		unsigned long int loop; // +28
		unsigned long int unknown_2C;
		// 0x30
		unsigned long int unknown_30;
		unsigned long int pDirectSoundBuffer; // IDirectSoundBuffer*
		unsigned long int unknown_38;
	};
	static_assert(sizeof(STRUCT_SOUND_TDIGITAL) == 0x3C, "STRUCT_SOUND_TDIGITAL Size");


	// "TSoundDriver". Size 0x69C. Constructor=0x4A2560.
	// Init: see 0x418920
	class STRUCT_SOUND_DRIVER {
	public:
		char isSoundActive;
		char unknown_001; // Related to soundDataArray
		short int unknown_002;
		long int hWndToNotify;
		unsigned long int lpDirectSound; // +08. Real type=IDirectSound* (See DirectSoundCreate)
		unsigned long int lpPrimaryDsb; // +0C. Real type=IDirectSoundBuffer*
		// 0x10
		long int lastErrorCode; // Return value of mixerSetControlDetails, etc.
		long int tempSoundVolume; // -10000 to 0
		long int soundVolume; // -10000 to 0 (Registry)
		char soundFolder[0x104];
		// 0x120
		long int hasSoundVolumeCtrl; // Sound Volume Control Found Successfully
		unsigned long int hMixer; // Real type=HMIXER ?
		long int mixerLineSize; // +128. =sizeof(MIXERLINE) = 0xA8. 0x4A2A17
		unsigned long int unknown_12C;
		unsigned long int unknown_130;
		long int ml_dwLineID;
		unsigned long int unknown_138;
		unsigned long int unknown_13C;
		// 0x140
		long int ml_dwComponentType; // = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT. =0x1008
		char unknown_144[0x1D0 - 0x144];
		// 0x1D0 - MIXERLINECONTROLS: 0x18 bytes
		long int mlc_cbStruct; // = sizeof(MIXERLINECONTROLS)
		long int mlc_dwLineID; // = ml.dwLineID
		long int mlc_dwControlType; // = MIXERCONTROL_CONTROLTYPE_VOLUME;
		long int mlc_cControls; // = 1 ?
		// 0x1E0
		long int mlc_cbmxctrl; // = sizeof(MIXERCONTROL);
		long int mlc_pamxctrl; // = &mc
		// 0x1E8 - MIXERCONTROL: 0x94 bytes
		long int mc_cbStruct;
		long int mc_dwControlID;
		// 0x1F0
		long int mc_dwControlType;
		long int mc_fdwControl;
		long int mc_cMultipleItems;
		long int mc_szShortName[4]; // +1FC
		long int szName[0x10]; // +20C
		long int mc_dwMinimum; // +24C. =0 ?
		// 0x250
		long int mc_dwMaximum; // =65535 ?
		char unknown_254[0x27C - 0x254];
		// 0x27C - MIXERCONTROLDETAILS: 0x18 bytes
		long int mcd_cbStruct; // = sizeof(MIXERCONTROLDETAILS);
		// 0x280
		long int mcd_dwControlID; // = mc.dwControlID;
		long int mcd_cChannels; // +284. =1?
		unsigned long int unknown_288;
		long int mcd_cbDetails; // +28C. = sizeof(MIXERCONTROLDETAILS_UNSIGNED) = 4?
		// 0x290
		long int mcd_paDetails; // = &mcdu;
		// 0x294 - MIXERCONTROLDETAILS_UNSIGNED:
		long int mcdu_dwValue;
		STRUCT_SOUND_TDIGITAL *stackedSoundsToPlay[0x100]; // +298. Array of (buffered?) soundDataElements. Only values from 0 to (stackedSoundsCount-1) are valid. Add in 0x4A2950
		long int stackedSoundsCount; // +698. Number of used elements in stackedSoundsToPlay
	};
	static_assert(sizeof(STRUCT_SOUND_DRIVER) == 0x69C, "STRUCT_SOUND_DRIVER Size");


	// Size = 0x3F8. Constructor=0x44FAD0. "TMusicSystem".
	class STRUCT_MAIN_MUSIC {
	public:
		char soundPath[0x104];
		char musicFormat; // AudioCD=1, MIDI=2 (WAV=3)
		char unknown_105;
		char unknown_106;
		char unknown_107;
		STRUCT_SOUND_DRIVER *pSound; // +108
		long int hInstance; // = GetModuleHandle(NULL): Module handle to the file
		// 0x110
		long int hWnd; // The window to send MM_MCINOTIFY messages to.
		long int isTracksInitialized; // //Music Tracks Initialized? Music is playing?
		long int wDeviceID; // deviceID for mciSendCommand (Play, Stop, Close, etc.)
		// 0x11C - MCI_PLAY_PARMS: 0x0C bytes
		long int play_dwCallback;
		// 0x120
		long int play_dwFrom;
		long int play_dwTo;
		char isPlaying; // is music playback in progress?
		char musicFileName[0x230 - 0x129]; // "xopen.mp3", "xwon.mp3", "xlost.mp3", ""...
		// 0x230
		long int minMusicTrack;
		long int maxMusicTrack;
		unsigned long int unknown_238;
		long int seekPos; // to resume playback
		// 0x240
		long int isLooping; // to enable/disable playback loop. 0 for xopen, xwon, xlost.
		long int currentMusicTrack; // Current music number being played.
		long int nMusicTracks; // The number of xmusic#.mid or AudioCD tracks found.
		long int nMusicPlaying; // a counter. +1 for succesful MCI_PLAY. -1 after completing a playback.
		// 0x250 - music volume
		long int tempMusicVolume; // To indicate if the volume need to be set. 1 = Music Volume does not need to be set. -10000 to 0 = Music volume to be set.
		long int musicVolume; // registry value = -musicVolume
		long int isPaused; // Pause Music related
		unsigned long int unknown_25C; // related to Pause Music
		// 0x260
		unsigned long int unknown_260; // related to Pause Music
		long int fadeOutValue; // +264.
		long int fadeOutTime; // +268.
		long int isMusicVolumeStored; // indicates if music volume is stored
		// 0x270
		long int currentMusicVolume; // ?
		long int lastMusicVolumeStorageTime; // a timeGetTime value when fade out started ?
		long int hasMusicVolumeCtrl; // 0/1 flag
		long int hMixer; // to change volume of an audio line.
		// 0x280 - MIXERLINE: 0xA8 bytes
		long int ml_cbStruct; // = sizeof(MIXERLINE);
		long int ml_dwDestination;
		long int ml_dwSource;
		long int ml_dwLineID;
		// 0x290
		unsigned long int unknown_290;
		unsigned long int unknown_294;
		unsigned long int unknown_29ml_dwComponentType;
		unsigned long int unknown_29C;
		// 0x2A0
		char unknown_2A0[0x328 - 0x2A0];
		// 0x328 - MIXERLINECONTROLS: 0x18 bytes
		long int mlc_cbStruct; // = sizeof(MIXERLINECONTROLS);
		long int mlc_dwLineID; // = ml.dwLineID;
		// 0x330
		long int mlc_dwControlType; // = MIXERCONTROL_CONTROLTYPE_VOLUME;
		long int mlc_cControls;
		long int mlc_cbmxctrl; // = sizeof(MIXERCONTROL);
		long int mlc_pamxctrl; // = &mc;
		// 0x340 - MIXERCONTROL:0x94 bytes
		//long int mc_cbStruct
		long int mc_cbStruct;
		long int mc_dwControlID;
		long int mc_dwControlType;
		long int mc_fdwControl;
		// 0x350
		long int mc_cMultipleItems;
		long int mc_szShortName[4]; // +354
		long int szName[0x10]; // +364
		long int mc_dwMinimum; // +3A4. =0 ?
		long int mc_dwMaximum; // +3A8. =65535 ?
		char unknown_3AC[0x3D4 - 0x3AC];
		// 0x3D4 - MIXERCONTROLDETAILS: 0x18 bytes
		long int mcd_cbStruct; // = sizeof(MIXERCONTROLDETAILS);
		long int mcd_dwControlID; // = mc.dwControlID;
		long int mcd_cChannels;
		// 0x3E0
		unsigned long int unknown_3E0;
		long int mcd_cbDetails; // +3E4. = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		long int mcd_paDetails; // = &mcdu;
		// 0x3EC - MIXERCONTROLDETAILS_UNSIGNED:
		long int mcdu_dwValue;
		unsigned long int unknown_3F0;
		unsigned long int unknown_3F4;
	};
	static_assert(sizeof(STRUCT_MAIN_MUSIC) == 0x3F8, "STRUCT_MAIN_MUSIC Size");

}
