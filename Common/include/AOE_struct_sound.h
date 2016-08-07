
#pragma once



/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {


	// Size: seems to be 0x40
	class STRUCT_SOUND_DATA_ELEMENT {
		// Object info initially provided by LucTieuPhung
	public:
		long int pSound;
		unsigned long int unknown_04;
		unsigned short int unknown_08;
		char filename[0x0C];
		unsigned long int unknown_18;
		long int lVolume; // +1C
		// 0x20
		long int dwFrequency;
		long int lPan;
		unsigned long int unknown_28;
		unsigned long int unknown_2C;
		// 0x30
		unsigned long int unknown_30;
		unsigned long int pDirectSoundBuffer; // IDirectSoundBuffer*
		unsigned long int unknown_38;
		unsigned long int unknown_3C;
	};
	static_assert(sizeof(STRUCT_SOUND_DATA_ELEMENT) == 0x40, "STRUCT_SOUND_DATA_ELEMENT Size");

	// Size 0x69C
	class STRUCT_MAIN_SOUND {
		// Object info initially provided by LucTieuPhung
	public:
		char isDSoundInitialized;
		char unknown_001; // Related to ListSoundData
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
		// 0x128 - MIXERLINE: 0xA8 bytes
		long int ml_cbStruct; // =sizeof(MIXERLINE)
		unsigned long int unknown_12C;
		unsigned long int unknown_130;
		long int ml_dwLineID;
		unsigned long int unknown_138;
		unsigned long int unknown_13C;
		// 0x140
		long int ml_dwComponentType; // = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
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
		long int mcd_cChannels;
		unsigned long int unknown_288;
		long int mcd_cbDetails; // +28C. = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		// 0x290
		long int mcd_paDetails; // = &mcdu;
		// 0x294 - MIXERCONTROLDETAILS_UNSIGNED:
		long int mcdu_dwValue;
		STRUCT_SOUND_DATA_ELEMENT soundDataArray[0x10];
		long int soundDataCount; // Number of used elements in soundDataArray ?
	};
	static_assert(sizeof(STRUCT_MAIN_SOUND) == 0x69C, "STRUCT_MAIN_SOUND Size");

	// Size = 0x3F8
	class STRUCT_MAIN_MUSIC {
		// Object info initially provided by LucTieuPhung
	public:
		char soundPath[0x104];
		char musicFormat; // AudioCD=1, MIDI=2 (WAV=3)
		char unknown_105;
		char unknown_106;
		char unknown_107;
		STRUCT_MAIN_SOUND *pSound; // +108
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
		unsigned long int unknown_264;
		unsigned long int unknown_268;
		long int isMusicVolumeStored; // indicates if music volume is stored
		// 0x270
		long int currentMusicVolume; // ?
		long int lastMusicVolumeStorageTime; // a timeGetTime value
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
