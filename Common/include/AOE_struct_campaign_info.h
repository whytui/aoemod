
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

	// Size 0x208
	class SCENARIO_FILE_INFO {
		unsigned long int unknown_000;
		unsigned long int unknown_004;
		char scenarioName[0xFF]; // +8
		char filename[0xFF]; // +107
		char unknown_206[2]; // +206. unused ?
	};
	static_assert(sizeof(SCENARIO_FILE_INFO) == 0x208, "SCENARIO_FILE_INFO size");

	class CAMPAIGN_FILE_INFO {
	public:
		char campaignName[0x100];
		unsigned long int unknown_100;
		unsigned long int unknown_104;
		SCENARIO_FILE_INFO *scenarioInfo; // +108. Array of (available?) scenarios
		char filename[0x100]; // Unknown size
	};

	class CAMPAIGN_USER_STATUS {
	public:
		char campaignName[0x100];
		long int selectedScenario; // +100. unsure
		long int scenarioCount; // +104. unsure
		long int *unknown_108;
		long int completedScenarioCount; // +10C. unsure
		CAMPAIGN_FILE_INFO *campaignInfo; // +110
	};

	// Information about a campaign "user" (account)
	class CAMPAIGN_USER_INFO {
	public:
		char userName[0x100];
		long int selectedCampaignIndex; // +100. Unsure
		long int numberOfxxx_104; // +104
		CAMPAIGN_USER_STATUS **userCampaignStatus; // +108. Array of campaign statuses for user ?
		long int campaignsCount; // +10C. Array size for +110
		CAMPAIGN_FILE_INFO *campaignInfo; // +110.
	};

	// TO move somewhere
	class GAME_NFO_DATA {
	public:
		long int currentUserIndex; // Currently selected "user". starts at 0.
		long int userCount; // +4
		CAMPAIGN_USER_INFO **userInfoArray; // +08. The users you create in campaign menu.
		unsigned long int unknown_0C; // unsure
		unsigned long int unknown_10; // unsure
		char filename[0x100]; // unsure
	};


}
