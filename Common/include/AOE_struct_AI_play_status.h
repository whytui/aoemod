
#pragma once



/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment). Very important here as there are some weird structures.

	// TODO
	// Constructor=0x40C2F0
	class STRUCT_AI_PLAY_PHASE_COMMAND {
	public:
		char unknown_00; // "g"
		char phaseType; // +01. Type : move=1, attack=2, attackSavedTarget=3, Retreat=4, Heal=5, ResetHitPoints=6, ResetAliveCount=7, Wait=8. Others=invalid
		char unused[2];
		long int unknownn_04; // "v1"
		long int unknownn_08; // "v2"
		long int unknownn_0C; // "v3"
	};
	static_assert(sizeof(STRUCT_AI_PLAY_PHASE_COMMAND) == 0x10, "STRUCT_AI_PLAY_PHASE_COMMAND size");

	// TODO
	// Constructor=0x40C5A0
	class STRUCT_AI_PLAY_PHASE_TRIGGER {
	public:
		char phaseTriggerType; // +00. gather=1, death=2, healthLost=3, DamageToGroup=4, DamageToAnyGroup=5, Level1EnemySighted=6, Level2EnemySighted=7, DeathOfGroup=8, HealOfGroup=9, Immediate=10, Time=11
		char unused_01[3];
		long int unknown_04; // v1
		long int unknown_08; // v2
		char unknown_0C;
		char unknown_0D;
		char unused_0E[2];
		long int unknown_10;
	};
	static_assert(sizeof(STRUCT_AI_PLAY_PHASE_TRIGGER) == 0x14, "STRUCT_AI_PLAY_PHASE_TRIGGER size");

	// TODO
	// Constructor 0x40C8E0
	class STRUCT_AI_PLAY_PHASE {
	public:
		STRUCT_AI_PLAY_PHASE_COMMAND playPhaseCommands[5];
		STRUCT_AI_PLAY_PHASE_TRIGGER playPhaseTriggers[3]; // +50.
	};
	static_assert(sizeof(STRUCT_AI_PLAY_PHASE) == 0x8C, "STRUCT_AI_PLAY_PHASE size");


	// TODO
	// Constructor 0x40CA40
	class STRUCT_AI_PLAY_GROUP {
	public:
		char unknown_00_minimum[6];
		char unknown_06_maximum[6];
	};
	static_assert(sizeof(STRUCT_AI_PLAY_GROUP) == 0x0C, "STRUCT_AI_PLAY_GROUP size");


	// TODO
	// Constructor=0x40CAF0
	// types attack=1, annoy=2, Flank=3, recon=4, 
	class STRUCT_AI_PLAY {
		char aiPlayName[0x40]; // if "HP"=>Human Play
		char unknown_40;
		char unknown_41; // default 1
		char unknown_42; // default -1
		char unknown_43; // default 1
		char targetType[5]; // +44.
		char targetCharacteristics[5]; // +49.
		char unknown_4E; // default 0
		char unknown_4F; //
		char unknown_50;
		char unknown_51; // default 1
		char unknown_52;
		STRUCT_AI_PLAY_GROUP playGroups[5]; // +53.
		char unknown_8F;
		STRUCT_AI_PLAY_PHASE playPhases[0x0A]; // +90
		// end +608?
	};
	static_assert(sizeof(STRUCT_AI_PLAY) == 0x608, "STRUCT_AI_PLAY size");


	// Size=0x8. Is it STRUCT_NEARBY_UNIT_INFO ??? Maybe not as +6/+7 fields seem to be unused here ?
	// Represents units, but what exactly ?
	class STRUCT_AI_PLAY_STATUS_GROUPING_ELEM {
	public:
		long int unitId;
		char unknown_04_gid; // +04. -1=invalid or N/A. Group id ? (0-4?)
		char unknown_05_type; // +05. -1=invalid or N/A
		char unknown_06; // Unused ?
		char unknown_07; // Unused ?
	};
	static_assert(sizeof(STRUCT_AI_PLAY_STATUS_GROUPING_ELEM) == 0x8, "STRUCT_AI_PLAY_STATUS_GROUPING_ELEM size");

	// Constructor=0x40D2F0
	// Related to task 0x2D6 ?
	class STRUCT_AI_PLAY_STATUS {
	public:
		STRUCT_AI_PLAY_STATUS_GROUPING_ELEM unitsInGroup[50]; // +00.
		unsigned long int unknown_190;
		unsigned long int unknown_194;
		unsigned long int unknown_198_Y; // A struct size=0x0C, ccor(init?)=0x46A7E0
		unsigned long int unknown_19C_X;
		unsigned long int unknown_1A0_Z; // end of struct...
		unsigned long int originalHitPoints[5]; // +1A4. Cf 0x40D390. (index can be 0-4 = "gid").
		char unknown_1B8; // +1B8. Default=0xFD=253
		char unused_1B9[3];
		unsigned long int unknown_1BC;
		unsigned long int unknown_1C0;
		unsigned long int unknown_1C4;
	};
	static_assert(sizeof(STRUCT_AI_PLAY_STATUS) == 0x1C8, "STRUCT_AI_PLAY_STATUS size");


#define CHECKSUM_AI_PLAYBOOK 0x00542C40
#define CHECKSUM_AI_PLAYBOOK_CHILD 0x00548C48
	// Constructor=0x40D7D0(parent), 0x4C88C0("tribe"=child). Size=0x50 for both classes.
	// Load from file=0x40D840 (aoe.ply !)
	// "types": 1=Any (only type)
	// target characteristics: Stationary=1, Moving=2, Slow=3, Fast=4, Any=5
	class STRUCT_AI_PLAYBOOK {
	public:
		unsigned long int checksum;
		unsigned long int unknown_04;
		char unknown_08[0x4C - 0x08];
		unsigned long int *unknown_4C;

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_AI_PLAYBOOK) || (this->checksum == CHECKSUM_AI_PLAYBOOK_CHILD); }
	};
	static_assert(sizeof(STRUCT_AI_PLAYBOOK) == 0x50, "STRUCT_AI_PLAYBOOK size");


#pragma pack(pop)

}

