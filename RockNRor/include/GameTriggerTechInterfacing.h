#pragma once

#include <assert.h>
#include <AOE_struct_scenario_info.h>
#include <triggerDefinition.h>


#define UNUSED_PLID_FOR_TRIGGERS 9
static_assert((UNUSED_PLID_FOR_TRIGGERS >= 0) && (UNUSED_PLID_FOR_TRIGGERS < 16), "Bad player ID for UNUSED_PLID_FOR_TRIGGERS");


namespace ROCKNROR {
namespace TRIGGER {
;

// Returns a pointer to trigger data
char *GetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo);
// Set trigger data pointer. Returns false if failed.
bool SetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, char *newDataPtr);
// Returns trigger data size
long int GetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo);
// Set trigger data size. Returns false if failed.
bool SetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, long int sizeToSet);

// Returns true if provided trigger text contains the END tag before any trigger information.
bool TriggerTextContainsENDTagAtBeginning(char *triggerText);


}
}
