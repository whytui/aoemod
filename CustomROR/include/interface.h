#pragma once

#include <stdio.h>
#include <yvals.h> // _STRINGIZE
#include <list>
#include <Windows.h>
#include <AOE_offsets_10c.h>
#include <customResolution.h>


static const char *CST_CUSTOMROR_DRS_FILENAME = "customROR.drs";
static const char *CST_CUSTOMROR_FOLDER = "customROR\\";

/* DRS object IDs usage in ROR:
- 0 -> 810 ? : graphics.drs (data and data2)
- 5000 -> 5285 : sounds.drs (data and data2)
- 15000 -> 15003 : terrain.drs
- 20000 -> 20006 : border.drs
- 50000 -> 53012 : interfac.drs (data and data2)

Custom ROR uses 10000-14999.
*/

// ***************** These numbers must match DRS file object IDs !!! ************************
// Identifies customROR icons SLP ID (id in DRS file)
static const long int CST_CUSTOMROR_CMD_ICONS_SLP_ID = 10000;

static const long int CST_CUSTOMROR_ICON_ID_DONT_ATTACK_VILLAGERS = 0;
static const long int CST_CUSTOMROR_ICON_ID_DONT_ATTACK_BUILDINGS = 1;
static const long int CST_CUSTOMROR_ICON_ID_DONT_AUTO_ATTACK = 3;
static const long int CST_CUSTOMROR_ICON_ID_RESTORE_AUTO_ATTACK = 2;


// Manage Resolution
void ChangeItfDRS_file(); // Change interfac.drs filename to manage custom resolutions
