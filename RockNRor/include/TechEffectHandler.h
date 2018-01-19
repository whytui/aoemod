#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_tech_def.h>
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "unitDefHandling.h"

#pragma once

/*
This file contains methods to handle technology effects and their applicatin to game objects
*/

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace TECHEFFECT {
;

// *** IMPORTANT REMARK ***
// To overload/add new stuff in technology effects, don't forget to handle all necessary procedures
// Some may not require any change, depending of the effect. For example, a change on TUA_SPEED attribute has no impact on UNITs
// because speed is only stored in unit definition structure and each time this information is used, it is retrieved from unitDef.
// - unit.apply{add/set/mult}Effect methods (for effects on attributes) : to update existing units !
// - unitDef.apply{add/set/mult}Effect methods (for effects on attributes) : to update unit definitions : can influence future units, but also existing ones (e.g. total HP)
// - 0x45D010 = player.setUnitAvailable(DATID, enable) for "enable unit" effects.
// - 0045CFC0 = player.setResource(resourceId, f_value) for "resource modifier/set"
// - [player+0x78] player.AddValueToResource(resourceId, float_value, countInGatheredGoldResource) for resource modifier +/-

// Refer to the PARENT METHOD that handles technology effects
// - 0x4EBB10 = techDefInfo.applyTech((word)technologyId, playerStruct)
// - 0x4F1870 = player.enableResearch(byte_researchId, enable) for "disable tech" effects (from tech trees)
// - 0x43B940 = techDefInfo.ApplyTechOtherThanEnableDisable?(technoId, playerStruct) --- check that type6=res_mult is supported ?


// Remark: see also entry point "FixPlayerNoTechTree_applyTech" at 0x4EBB16


// Overload unitDef.applyAddEffect(...)
// Returns true if the original ROR code must NOT be executed
// Returns false (default) if original ROR code must be executed after this method.
bool UnitDefApplyAddEffect(STRUCT_UNITDEF_BASE *unitDef, float value, AOE_CONST_FUNC::TECH_UNIT_ATTRIBUTES attribute);


// Overload unitDef.applySetEffect(...)
// Returns true if the original ROR code must NOT be executed
// Returns false (default) if original ROR code must be executed after this method.
bool UnitDefApplySetEffect(STRUCT_UNITDEF_BASE *unitDef, float value, AOE_CONST_FUNC::TECH_UNIT_ATTRIBUTES attribute);


// Overload unitDef.applyMultEffect(...)
// Returns true if the original ROR code must NOT be executed
// Returns false (default) if original ROR code must be executed after this method.
bool UnitDefApplyMultiplyEffect(STRUCT_UNITDEF_BASE *unitDef, float value, AOE_CONST_FUNC::TECH_UNIT_ATTRIBUTES attribute);


// Handles "Resource Modifier (Multiply)" effect, that is natively UNSUPPORTED by ROR
// Returns true if successful
bool PlayerApplyMultiplyResourceEffect(STRUCT_PLAYER *player, float value, AOE_CONST_FUNC::RESOURCE_TYPES resourceType);


// Apply unsupported technology effects to provided player.
// Standard/supported effects are ignored here. Use other overloads if you need to customize/fix something in "standard" supported effects.
void ApplyUnsupportedEffects(STRUCT_PLAYER *player, STRUCT_TECH_DEF_INFO *techDefInfo, short int techId);


}
}
