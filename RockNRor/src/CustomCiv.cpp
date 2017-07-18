#include "../include/CustomCiv.h"


namespace ROCKNROR {
	namespace CUSTOMCIV {
;

bool CustomCivHandler::CreateFakeRandomCivsForAllPlayers() {
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!settings || !settings->IsCheckSumValid() || !global || !global->IsCheckSumValid()) {
		return false;
	}
	if (!global->technologiesInfo) { return false; }
	int playerCount = global->playerTotalCount - 1; // do not include gaia here.

	long int initialTechDefCount = ROCKNROR::crInfo.empiresDatTechDefCount;
	if (initialTechDefCount <= 0) { return false; } // maybe empires.dat has not been read yet.
	AOE_STRUCTURES::ReallocTechDefArray(initialTechDefCount + 8);

	for (int playerId = 1; playerId <= playerCount; playerId++) {
		int techTreeIndex = initialTechDefCount + playerId - 1;
		// Clean previous effects
		CleanEffectsForTechDef(global->technologiesInfo->GetTechDef(techTreeIndex));
		STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
		if (!player || !player->IsCheckSumValid()) { continue; }
		player->techTreeId = techTreeIndex; // used at game init to apply civilization bonus (malus), then not really used.

		//TEST
		STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techTreeIndex);
		techDef->effectCount = 3;
		techDef->ptrEffects = (STRUCT_TECH_DEF_EFFECT*)AOEAllocZeroMemory(3, sizeof(STRUCT_TECH_DEF_EFFECT));
		techDef->ptrEffects[0].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT;
		techDef->ptrEffects[0].effectClass = AOE_CONST_FUNC::TribeAIGroupFootSoldier;
		techDef->ptrEffects[0].effectUnit = -1;
		techDef->ptrEffects[0].effectAttribute = TECH_UNIT_ATTRIBUTES::TUA_HP;
		techDef->ptrEffects[0].effectValue = ((float)playerId) / 10 + 1;

		techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD;
		//techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH;
		//techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT;
		//techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT;
		techDef->ptrEffects[1].effectClass = AOE_CONST_FUNC::TribeAIGroupCivilian;
		techDef->ptrEffects[1].effectUnit = -1;
		techDef->ptrEffects[1].effectAttribute = TECH_UNIT_ATTRIBUTES::TUA_LOS;
		techDef->ptrEffects[1].effectValue = (float)playerId;

		techDef->ptrEffects[2].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET;
		techDef->ptrEffects[2].effectUnit = CST_RES_ORDER_TRIBUTE_SENT; // resource
		techDef->ptrEffects[2].effectClass = 0; // set ?
		techDef->ptrEffects[2].effectAttribute = -1; // unused?
		techDef->ptrEffects[2].effectValue = (float)playerId;
	}


	return true;
}


}
}
