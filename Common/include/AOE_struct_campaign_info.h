
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_struct_command_line_info.h>
#include <AOE_struct_sound.h>
#include <AOE_struct_mp_communication.h>
#include <AOE_structures_drs.h>
#include <AOE_struct_game_global.h>
#include <gameVersion.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Size 0x208.
	class SCENARIO_FILE_INFO {
		unsigned long int unknown_000;
		unsigned long int unknown_004;
		char scenarioName[0xFF]; // +8
		char filename[0xFF]; // +107
		char unknown_206[2]; // +206. unused ?
	};
	static_assert(sizeof(SCENARIO_FILE_INFO) == 0x208, "SCENARIO_FILE_INFO size");

	// RGE_Campaign (to confirm). Size0x20C, ccor=0x41C2D0,0x41C3D0
	class CAMPAIGN_FILE_INFO {
	public:
		unsigned long int unknown_004;
		char campaignName[0x100]; // Get in 0x41C780
		unsigned long int scenarioNumber; // +104.
		SCENARIO_FILE_INFO *scenarioInfo; // +108. Array of (available?) scenarios
		char filename[0x100]; // +10C. Unknown size
	};

	// Information about a campaign "user" (account) = "RGE_CampaignInfo"
	// Constructor=0x43BB40, 0x43BD10
	class CAMPAIGN_USER_STATUS {
	public:
		char campaignName[0x100];
		long int selectedScenario; // +100. unsure
		long int scenarioCount; // +104. unsure
		long int *unknown_108;
		long int completedScenarioCount; // +10C. unsure
		CAMPAIGN_FILE_INFO *campaignInfo; // +110
	};

	// Constructor=0x43C190. "RGE_PersonInfo"
	class CAMPAIGN_USER_INFO {
	public:
		char userName[0x100];
		long int selectedCampaignIndex; // +100. Unsure
		long int numberOfXxx; // +104
		CAMPAIGN_USER_STATUS **userCampaignStatus; // +108. Array of campaign statuses for user ?
		long int campaignsCount; // +10C. Array size for +110
		CAMPAIGN_FILE_INFO *campaignInfo; // +110.
	};


	// TO move somewhere
	// Size=0x118. Constructor=0x43C6B0. "RGE_Game_Info" ?
	class GAME_NFO_DATA {
	public:
		long int currentUserIndex; // Currently selected "user". starts at 0.
		long int userCount; // +4
		CAMPAIGN_USER_INFO **userInfoArray; // +08. The users you create in campaign menu.
		unsigned long int unknown_0C; // unsure
		unsigned long int unknown_10; // +10. unsure
		char filename[0x104]; // +14. unsure
	};
	static_assert(sizeof(GAME_NFO_DATA) == 0x118, "GAME_NFO_DATA size");


	// Size=0x10C. Constructor=0x476CD0.
	// Scenario.inf path + other data ?
	class STRUCT_SCENARIO_INF_DATA {
	public:
		char unknown_00[0x104]; // unknown size
		unsigned long int unknown_104; // +104. A DWORD.
		unsigned long int *unknown_108; // +108. Array of elems, size=0x10C
	};
	static_assert(sizeof(STRUCT_SCENARIO_INF_DATA) == 0x10C, "STRUCT_SCENARIO_INF_DATA size");


}
