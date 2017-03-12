#pragma once

#include <stdio.h>
#include <yvals.h> // _STRINGIZE
#include <list>
#include <Windows.h>
#include <AOE_offsets.h>
#include <customResolution.h>
#include "Version.h"
#include "crPatcher.h"

// Warning: also set in XML config
static const char *CST_ROCKNROR_DRS_FILENAME = "RockNRor.drs";
static const char *CST_ROCKNROR_FOLDER = "RockNRor\\";

/* DRS object IDs usage in ROR:
- 0 -> 810 ? : graphics.drs (data and data2)
- 5000 -> 5285 : sounds.drs (data and data2)
- 15000 -> 15003 : terrain.drs
- 20000 -> 20006 : border.drs
- 50000 -> 53012 : interfac.drs (data and data2)

Custom ROR uses 10000-14999.
*/

// ***************** These numbers must match DRS file object IDs !!! ************************
// See also AOE_const_drs.h
// Identifies RockNRor SLP IDs (id in DRS file)

namespace AOE_CONST_DRS {
;

static const long int CST_ROCKNROR_CMD_ICONS_SLP_ID = 10000; // RockNRor command icons
static const long int CST_ROCKNROR_UNIT_SHORTCUTS_SLP_ID = 10001; // Custom shortcuts (10-20)


static const long int CST_ROCKNROR_MAX_SLP_INDEX_IN_UNIT_SHORTCUTS_FILE = 12; // "10-20" + "G" bitmaps.
static const long int CST_ROCKNROR_MAX_SLP_INDEX_FOR_UNIT_SHORTCUTS = 11; // Max itemindex that corresponds to a valid shortcut, in "shortcuts SLP"
static const long int CST_ROCKNROR_SLP_INDEX_FOR_UNIT_SHORTCUT_10 = 1; // Other itemIndexes are (10+i) => (CST_ROCKNROR_SLP_ID_FOR_UNIT_SHORTCUT_10+i)
static const long int CST_ROCKNROR_SLP_INDEX_FOR_GROUPED_UNIT = 12; // SLP.itemindex for "G" (group) icon


// Button bar command icon IDs for RockNRor (cf RockNRor.drs). Must correspond to actual SLPIDs in DRS file !
enum RockNRorIconId : long int {
	CR_ICON_DONT_ATTACK_VILLAGERS = 0,
	CR_ICON_DONT_ATTACK_BUILDINGS = 1,
	CR_ICON_RESTORE_AUTO_ATTACK = 2,
	CR_ICON_DONT_AUTO_ATTACK = 3,
	CR_ICON_DEFEND_UNIT_OR_POSITION = 4
};


// "DAT" button IDs (1-5 first row, 6-10 second row, etc)
#define CST_FIRE_GALLEY_ORIGINAL_BUTTONDATID 5
#define CST_FIRE_GALLEY_CUSTOM_BUTTONDATID 9
// Internal button IDs (0-5 = first row, 6-11 = second row)
#define CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS 3
#define CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS 4
#define CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED 9
#define CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT 10
#define CST_CUSTOM_BUTTONID_DEFEND_ZONE_OR_UNIT 8


// Manage Resolution
void ChangeItfDRS_file(); // Change interfac.drs filename to manage custom resolutions

}
