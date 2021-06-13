#include "../include/researches.h"

using namespace AOE_STRUCTURES;
using namespace AOE_CONST_FUNC;




// Enable or disable a research for a player
void AOE_METHODS::AOE_enableResearch(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id, bool enable) {
	if (!player || !player->IsCheckSumValid()) { return; }
	if (!player->ptrResearchesStruct) { return; }
	if ((research_id < 0) || (research_id >= player->ptrResearchesStruct->researchCount)) { return; }
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	const long int f = 0x4F1870;
	long int argResearch_id = research_id & 0xFF; // It is a byte only
	long int argEnable = enable ? 1 : 0;
	_asm {
		MOV ECX, player;
		PUSH argEnable;
		PUSH argResearch_id;
		CALL f;
	}
}


namespace AOE_STRUCTURES {
namespace RESEARCH {
;

// Returns true if technology enables at least one unit (including upgrade a unit into another one).
bool DoesTechEnableSomeUnit(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect) {
			// Enable unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 1)) {
				return true;
			}
			
			// Upgrade some unit TO THIS unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) && (techEffect->effectUnit > -1) && (techEffect->effectClass > -1)) {
				return true;
			}
		}
	}
	return false;
}


// Returns true if technology disables at least one unit (generally, tech tree effects).
bool DoesTechDisableSomeUnit(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 0)) {
			return true;
		}
	}
	return false;
}


// Returns true if technology disables at least one research (generally, tech tree effects).
bool DoesTechDisableSomeResearch(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH) {
			return true;
		}
	}
	return false;
}


// Returns the first found armor class that is improved by a technology. Returns CST_AC_NONE(-1) if the tech does not improve any armor.
// Generally, even if it has many effects, a technology should improve only 1 armor type, so the result is deterministic as long as this hypothesis is correct.
// Warning: "default armor" is ignored here
AOE_CONST_FUNC::ATTACK_CLASS DoesTechImproveSomeArmor(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return AOE_CONST_FUNC::CST_AC_NONE; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsAttributeModifier() && (techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_ARMOR)) {
			AOE_CONST_FUNC::ATTACK_CLASS armorClass = techEffect->GetAttackOrArmorType();
			return armorClass;
		}
	}
	return AOE_CONST_FUNC::CST_AC_NONE;
}


// Returns the first found attack class that is improved by a technology. Returns CST_AC_NONE(-1) if the tech does not improve any attack.
// Generally, even if it has many effects, a technology should improve only 1 attack type, so the result is deterministic as long as this hypothesis is correct.
AOE_CONST_FUNC::ATTACK_CLASS DoesTechImproveSomeAttack(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return AOE_CONST_FUNC::CST_AC_NONE; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsAttributeModifier() && (techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_ATTACK)) {
			AOE_CONST_FUNC::ATTACK_CLASS attackClass = techEffect->GetAttackOrArmorType();
			return attackClass;
		}
	}
	return AOE_CONST_FUNC::CST_AC_NONE;
}


// Returns true for technologies that affect "Shared exploration" resource (#50).
// Can be use to identify writing tech (*not* hardcoded Id)
bool DoesTechRevealAlly(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsResourceModifier() && (techEffect->effectUnit == AOE_CONST_FUNC::CST_RES_ORDER_SHARED_EXPLORATION)) {
			return true;
		}
	}
	return false;
}


// Returns true if technology improves priest in some way (includes medecine, fanatism or martydom, but not jihad !)
bool DoesTechImpactReligion(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		
		// Classical: modify priest attributes (HP, speed, range, work rate, etc)
		if (techEffect->IsAttributeModifier()) {
			if ((techEffect->effectClass == AOE_CONST_FUNC::TribeAIGroupPriest) ||
				(techEffect->effectUnit == AOE_CONST_FUNC::CST_UNITID_PRIEST)) {
				return true;
			}
		}

		// Martyrdom, fanaticism, monotheism...
		if (techEffect->IsResourceModifier()) {
			if ((techEffect->effectUnit == AOE_CONST_FUNC::CST_RES_ORDER_CAN_CONVERT_BUILDING) ||
				(techEffect->effectUnit == AOE_CONST_FUNC::CST_RES_ORDER_CAN_CONVERT_PRIEST) ||
				(techEffect->effectUnit == AOE_CONST_FUNC::CST_RES_ORDER_FAITH_RECHARGING_RATE) ||
				(techEffect->effectUnit == AOE_CONST_FUNC::CST_RES_ORDER_HEALING) ||
				(techEffect->effectUnit == AOE_CONST_FUNC::CST_RES_ORDER_PRIEST_SACRIFICE))
				return true;
		}
	}
	return false;
}


// Returns true if technology affects villager speed. Wheel and Jihad should do ! (+some tech trees)
bool DoesTechAffectCivilianSpeed(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsAttributeModifier() && (techEffect->effectClass == TribeAIGroupCivilian) &&
			(techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_SPEED)) {
			return true;
		}
	}
	return false;
}


// Returns true if technology appears to be 'architecture' (improve buildings and walls HP, builder work rate)
bool HasArchitectureEffects(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	bool foundBuilderWorkRate = false;
	bool foundBuildingHP = false;
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsAttributeModifier() && (techEffect->effectClass == TribeAIGroupBuilding) &&
			(techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_HP)) {
			foundBuildingHP = true;
		}
		if (techEffect->IsAttributeModifier() && (techEffect->effectClass == TribeAIGroupCivilian) &&
			(techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_WORK_RATE)) {
			foundBuilderWorkRate = true;
		}
	}
	return foundBuilderWorkRate && foundBuildingHP;
}


// Returns true if technology improves lumberjacks AND improves range/LOS for almost all range units (=woodworking-like tech)
bool HasWoodWorkingAndImproveRangeEffects(STRUCT_TECH_DEF *techDef) {
	if (!techDef) { return false; }
	bool foundLumberjackEffect = false;
	int foundLOSEffectCount = 0;
	int foundRangeEffectCount = 0;
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsAttributeModifier() && (techEffect->effectUnit == CST_UNITID_LUMBERJACK) &&
			((techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_RESOURCE_CAPACITY) || (techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_WORK_RATE))
			) {
			foundLumberjackEffect = true;
		}
		if (techEffect->IsAttributeModifier() &&
			(techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_RANGE)) {
			foundRangeEffectCount++;
		}
		if (techEffect->IsAttributeModifier() &&
			(techEffect->effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_LOS)) {
			foundLOSEffectCount++;
		}
	}
	// True if improves lumberjack (at least work rate or capacity) and improves LOS/range for at least 2 units or 2 classes
	return foundLumberjackEffect && (foundLOSEffectCount >= 2) && (foundRangeEffectCount >= 2);
}


}
}



// Get a technology name from languagex.dll or language.dll.
// Technologies don't really have a name, we use matching research to find it. Works in many cases, not all.
std::string GetTechnologyLocalizedName(short int techId) {
	if (techId < 0) {
		return "invalid-tech";
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	if (!settings) { return ""; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->researchDefInfo) {
		return "";
	}
	for (int researchId = 0; researchId < global->researchDefInfo->researchCount; researchId++) {
		STRUCT_RESEARCH_DEF *resDef = global->researchDefInfo->GetResearchDef(researchId);
		assert(resDef != NULL);
		if (resDef && (resDef->technologyId == techId)) {
			char buffer[100];
			buffer[0] = 0;
			GetLanguageDllText(resDef->languageDLLName, buffer, sizeof(buffer), resDef->researchName);
			return std::string(buffer);
		}
	}
	std::string name = "techid-";
	name += std::to_string(techId);
	return name;
}


// Get a research name from languagex.dll or language.dll. Or internal name if no localization was found.
std::string GetResearchLocalizedName(short int researchId) {
	if (researchId < 0) {
		return "invalid-research";
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	if (!settings) { return ""; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->researchDefInfo) {
		return "";
	}
	if (researchId >= global->researchDefInfo->researchCount) {
		return "invalid-research";
	}
	STRUCT_RESEARCH_DEF *resDef = global->researchDefInfo->GetResearchDef(researchId);
	assert(resDef != NULL);
	if (!resDef) { return "invalid-research"; }
	char buffer[100];
	buffer[0] = 0;
	GetLanguageDllText(resDef->languageDLLName, buffer, sizeof(buffer), resDef->researchName);
	return std::string(buffer);
}


// Returns true if the research has more required researches than "minimum required researches count" (all requirements are not mandatory)
bool ResearchHasOptionalRequirements(STRUCT_RESEARCH_DEF *resDef) {
	if (!resDef) { return false; }
	int actualCount = 0;
	for (int i = 0; i < 4; i++) {
		if (resDef->requiredResearchId[i] > -1) {
			actualCount++;
		}
	}
	return (actualCount > resDef->minRequiredResearchesCount);
}

// Returns the researchId of an age (stone/tool/bronze/iron) if found in one of the 4 research's requirements. NOT recursive.
// Returns -1 if not found, a value in [CST_RSID_STONE_AGE, CST_RSID_IRON_AGE] if found.
#pragma message("Remove this, see FindResearchRequiredAge")
short int GetAgeResearchFromDirectRequirement(STRUCT_RESEARCH_DEF *researchDef) {
	if (researchDef == NULL) { return -1; }
	for (int iReq = 0; iReq < 4; iReq++) {
		// Age researches are consecutive. Republic age would be next one (104) if enabled
		if ((researchDef->requiredResearchId[iReq] >= CST_RSID_STONE_AGE) &&
			(researchDef->requiredResearchId[iReq] <= CST_RSID_IRON_AGE)) {
			return researchDef->requiredResearchId[iReq]; // does not support more than 1 age in dependencies (ok, it is stupid)
		}
	}
	return -1;
}


// Returns the age research id of research's required age, from direct dependencies or recursively, if necessary.
// Returns -1 if there is no required age (always available)
short int FindResearchRequiredAge(STRUCT_PLAYER *player, short int researchId) {
	if (!player || !player->IsCheckSumValid()) { return -1; }
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef = player->GetResearchDef(researchId);
	if (!researchDef) { return -1; }
	// First try in direct requirements
	short int currentResult = -1;
	int requirementsActualCount = 0;
	for (int i = 0; i < 4; i++) {
		if ((researchDef->requiredResearchId[i] >= CST_RSID_STONE_AGE) &&
			(researchDef->requiredResearchId[i] <= CST_RSID_IRON_AGE) && // +104 for republic age
			(researchDef->requiredResearchId[i] >= currentResult)) {
			currentResult = researchDef->requiredResearchId[i]; // if a research has dependencies on 2 ages, take most advanced one.
		}
		if (researchDef->requiredResearchId[i] >= 0) {
			requirementsActualCount++;
		}
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bld = FindBuildingDefThatEnablesResearch(player, researchId);
	if (bld && bld->IsCheckSumValidForAUnitClass()) {
		short int bldEnablerResearchId = FindResearchThatEnableUnit(player, bld->DAT_ID1, 0);
		if (bldEnablerResearchId >= 0) {
			// Recursive call on research that enables the building
			short int tmpRes = FindResearchRequiredAge(player, bldEnablerResearchId);
			if (tmpRes > currentResult) {
				currentResult = tmpRes;
			}
		}
	}
	if (currentResult > -1) { return currentResult; }

	if (requirementsActualCount <= 0) { return currentResult; } // No requirements for recursion
	// Here we need to search in requirements, recursively.
	// We explore all requirements, even "optional" ones (min required count < actual requirements)
	for (int i = 0; i < 4; i++) {
		if (researchDef->requiredResearchId[i] >= 0) {
			assert(researchDef->requiredResearchId[i] != researchId);
			if (researchDef->requiredResearchId[i] != researchId) { // Prevents infinite recursion
				// There is still an infinite recursion risk if several researches loop (cyclic requirements)
				short int thisResult = FindResearchRequiredAge(player, researchDef->requiredResearchId[i]);
				if (thisResult > currentResult) {
					currentResult = thisResult; // take most advanced age, if more than one is found
				}
			}
		}
	}
	return currentResult;
}


// Disable all impossible researches for a specific player.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisablePlayerImpossibleResearches(STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) {
		return 0;
	}
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return 0; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray) { return 0; }

	std::vector<short int> availableResearches; // status in AVAILABLE, BEING_RESEARCHED, DONE_OR_INVALID
	std::vector<short int> disabledResearches; // CST_RESEARCH_STATUS_DISABLED
	std::vector<short int> potentialResearches; // CST_RESEARCH_STATUS_WAITING_REQUIREMENT. We don't know if they can REALLy become available.
	std::vector<short int> potentialToRemove; // internal temp buffer for loop

	// Loop 1 : sort researches into 3 categories (disabled, done or available, unknown)
	for (int i = 0; i < resCount; i++) {
		if (statuses[i].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) {
			disabledResearches.push_back(i);
		} else {
			if (statuses[i].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
				potentialResearches.push_back(i);
			} else {
				availableResearches.push_back(i);
			}
		}
	}

	std::string playerString = "player #";
	playerString += std::to_string(player->playerId);
	playerString += " (";
	playerString += player->playerName_length16max;
	playerString += ")";

	// Loop 2 : "recursively" treat elements with "unknown" status (potentialResearches)
	// until it is empty or we don't progress anymore.
	int foundCount = 0;
	int remainingUnknownCount = potentialResearches.size();
	int previousRemainingCount = 9999;
	while ((remainingUnknownCount > 0) && (remainingUnknownCount != previousRemainingCount)) {
		for each (int curResId in potentialResearches) {
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = resInfoArray->GetResearchDef(curResId);
			assert(resDef != NULL);
			if (!resDef) { continue; }
			int minRequiredResearchesCount = resDef->minRequiredResearchesCount;
			int reqResOK = 0;
			int reqResImpossible = 0;

			for (int i = 0; i < 4; i++) {
				if ((resDef->requiredResearchId[i] == -1) ||
					(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId[i]) > 0)) {
					reqResImpossible++;
				}
				if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId[i]) > 0) {
					reqResOK++;
				}
			}
			if (reqResOK >= minRequiredResearchesCount) {
				availableResearches.push_back(curResId);
				potentialToRemove.push_back(curResId);
			}
			if (4 - reqResImpossible < minRequiredResearchesCount) {
				// We found an impossible research (some requirements can't be obtained)
				disabledResearches.push_back(curResId);
				potentialToRemove.push_back(curResId);
				foundCount++;
				assert(statuses[curResId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT);
				statuses[curResId].currentStatus = RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED;
				std::string tmpmsg = playerString;
				tmpmsg += ": this research can never become available: ";
				tmpmsg += std::to_string(curResId);
				tmpmsg += " = ";
				char tmpbuffer[100];
				GetLanguageDllText(resDef->languageDLLName, tmpbuffer, sizeof(tmpbuffer), resDef->researchName);
				tmpmsg += tmpbuffer;
				traceMessageHandler.WriteMessageNoNotification(tmpmsg);
			}
		}
		// Do not do the remove IN the loop ! Do it once iterations are finished
		for each (auto curResId in potentialToRemove)
		{
			auto it = remove_if(potentialResearches.begin(), potentialResearches.end(), [curResId](short int i) {return i == curResId; });
			potentialResearches.erase(it, potentialResearches.end());
		}
		potentialToRemove.clear(); // reset temp loop buffer

		previousRemainingCount = remainingUnknownCount;
		remainingUnknownCount = potentialResearches.size();
	}
	if (remainingUnknownCount > 0) {
		std::string msg = "Could not complete researches tree analysis for ";
		msg += playerString;
		traceMessageHandler.WriteMessage(msg);
	}
	return foundCount;
}

// Finds all non-disabled researches that are impossible to develop for given civilization's technology tree.
// Only writes log, does not fix anything.
int DetectDatImpossibleResearches(STRUCT_GAME_GLOBAL *global, short int civId) {
	if (!global || !global->IsCheckSumValid() || !global->researchDefInfo || !global->technologiesInfo || !global->civilizationDefinitions) {
		return 0;
	}
	if ((civId < 0) || (civId >= global->civCount)) { return 0; }
	STRUCT_CIVILIZATION_DEF *civDef = global->civilizationDefinitions[civId];
	if ((civDef->techTreeId < 0) || (civDef->techTreeId >= global->technologiesInfo->technologyCount)) {
		return 0;
	}

	int resCount = global->researchDefInfo->researchCount;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = global->researchDefInfo;

	std::set<short int> availableResearches; // status in AVAILABLE, BEING_RESEARCHED, DONE_OR_INVALID
	std::set<short int> disabledResearches; // CST_RESEARCH_STATUS_DISABLED
	std::set<short int> potentialResearches; // CST_RESEARCH_STATUS_WAITING_REQUIREMENT. We don't know if they can REALLy become available.
	std::set<short int> potentialToRemove; // internal temp buffer for loop

	// Loop 1 : sort researches into 3 categories (disabled, done or available, undetermined)
	AOE_STRUCTURES::STRUCT_TECH_DEF *techTreeDef = global->technologiesInfo->GetTechDef(civDef->techTreeId);
	for (int researchId = 0; researchId < resCount; researchId++) {
		bool isDisabled = false;
		for (int effectId = 0; effectId < techTreeDef->effectCount; effectId++) {
			if (DoesTechDisableResearch(techTreeDef, researchId)) {
				isDisabled = true;
			}
			// Disable unit (building) and this building has a valid "enable research". Like temple for macedonian, academy for persian.
			if ((techTreeDef->ptrEffects[effectId].effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) &&
				(techTreeDef->ptrEffects[effectId].effectClass == 0) && (techTreeDef->ptrEffects[effectId].effectUnit > -1)) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDefAsBld = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)
					civDef->GetUnitDef(techTreeDef->ptrEffects[effectId].effectUnit);
				if (unitDefAsBld && unitDefAsBld->IsCheckSumValid() && unitDefAsBld->IsTypeValid() && (unitDefAsBld->initiatesResearch > -1)) {
					// We DO have a valid building with a "initiate research" value... And it's not available, making the research unavailable
					potentialResearches.erase(unitDefAsBld->initiatesResearch);
					disabledResearches.insert(unitDefAsBld->initiatesResearch);
				}
			}
		}
		if (isDisabled) {
			disabledResearches.insert(researchId);
		} else {
			potentialResearches.insert(researchId);
		}
	}

	std::string techTreeString = "Civ #";
	techTreeString += std::to_string(civId);
	techTreeString += " (";
	techTreeString += civDef->civilizationName;
	techTreeString += "), tech tree #";
	techTreeString += std::to_string(civDef->techTreeId);

	// Loop 2 : "recursively" treat elements with "unknown" status (potentialResearches)
	// until it is empty or we don't progress anymore.
	int foundCount = 0;
	int remainingUnknownCount = potentialResearches.size();
	int previousRemainingCount = 9999;
	while ((remainingUnknownCount > 0) && (remainingUnknownCount != previousRemainingCount)) {
		for each (int curResId in potentialResearches) {
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = resInfoArray->GetResearchDef(curResId);
			assert(resDef);
			if (!resDef) { continue; }
			int minRequiredResearchesCount = resDef->minRequiredResearchesCount;
			int reqResOK = 0;
			int reqResImpossible = 0;

			for (int i = 0; i < 4; i++) {
				if ((resDef->requiredResearchId[i] == -1) ||
					(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId[i]) > 0)) {
					reqResImpossible++;
				}
				if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId[i]) > 0) {
					reqResOK++;
				}
			}
			if (reqResOK >= minRequiredResearchesCount) {
				availableResearches.insert(curResId);
				potentialToRemove.insert(curResId);
			}
			if (4 - reqResImpossible < minRequiredResearchesCount) {
				// We found an impossible research (some requirements can't be obtained)
				disabledResearches.insert(curResId);
				potentialToRemove.insert(curResId);
				foundCount++;
				std::string tmpmsg = techTreeString;
				tmpmsg += ": this research can never become available: ";
				tmpmsg += std::to_string(curResId);
				tmpmsg += " = ";
				char tmpbuffer[100];
				GetLanguageDllText(resDef->languageDLLName, tmpbuffer, sizeof(tmpbuffer), resDef->researchName);
				tmpmsg += tmpbuffer;
				traceMessageHandler.WriteMessageNoNotification(tmpmsg);
			}
		}
		// Do not do the remove IN the loop ! Do it once iterations are finished
		for each (auto curResId in potentialToRemove)
		{
			potentialResearches.erase(curResId);
		}
		potentialToRemove.clear(); // reset temp loop buffer

		previousRemainingCount = remainingUnknownCount;
		remainingUnknownCount = potentialResearches.size();
	}
	if (remainingUnknownCount > 0) {
		std::string msg = "Could not complete researches tree analysis for ";
		msg += techTreeString;
		traceMessageHandler.WriteMessage(msg);
	}
	return foundCount;
}


// Disable all impossible researches for all players.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisableImpossibleResearches() {
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return 0; }
	STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->ptrPlayerStructPtrTable) {
		return 0;
	}
	int result = 0;
	for (int i = 1; i < global->playerTotalCount; i++) {
		STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
		result += DisablePlayerImpossibleResearches(player);
	}
	return result;
}


// Returns true if technology has at least one effect on provided unit definition.
// Effect can be negative too.
// Use filter argument to exclude techs like martyrdom or negative effects
// /!\ Be careful with villager effects (eg. effects on lumberjack only VS effects on all villagers like jihad)
bool DoesTechAffectUnit(STRUCT_TECH_DEF *techDef, STRUCT_UNITDEF_BASE *unitDef, const AOE_TECHNOLOGIES::TechnologyFilterBase *filter) {
	if (!techDef || !unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (filter && filter->IgnoreWholeTech(techEffect)) {
			return false; // If technology contains 1 effect "to ignore", ignore the whole thing (example: jihad contains both "good" and undesirable effects)
		}
	}

	short int projectileUnitId = -1;
	if (unitDef->DerivesFromAttackable()) {
		STRUCT_UNITDEF_ATTACKABLE *attackableDef = (STRUCT_UNITDEF_ATTACKABLE*)unitDef;
		projectileUnitId = attackableDef->projectileUnitId;
	}

	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		// Does this affect affect units ?
		if (techEffect && (techEffect->HasValidEffect()) && (
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT)
			)) {
			if (techEffect->effectUnit == unitDef->DAT_ID1) { return true; }
			if (techEffect->effectClass == unitDef->unitAIType) { return true; }
			if ((projectileUnitId > -1) && (techEffect->effectUnit == projectileUnitId)) { return true; }
		}
		if (techEffect->IsEnableUnit(unitDef->DAT_ID1)) {
			return true;
		}
		if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT)) {
			// Upgrade unit : effectClass field is "TO" unit
			if ((techEffect->effectUnit == unitDef->DAT_ID1) || (techEffect->effectClass == unitDef->DAT_ID1) ||
				((techEffect->effectUnit == projectileUnitId) && (projectileUnitId >= 0))) {
				return true;
			}
		}
		bool isResourceModifier = ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT));
		// Very special cases: some resources can improve units: priest/building conversion, etc
		if (unitDef->unitAIType == TribeAIGroupPriest) {
			if (isResourceModifier && (
				(techEffect->effectUnit == CST_RES_ORDER_CAN_CONVERT_BUILDING) ||
				(techEffect->effectUnit == CST_RES_ORDER_CAN_CONVERT_PRIEST) ||
				(techEffect->effectUnit == CST_RES_ORDER_FAITH_RECHARGING_RATE) ||
				(techEffect->effectUnit == CST_RES_ORDER_HEALING) || // (medecine tech)
				(techEffect->effectUnit == CST_RES_ORDER_PRIEST_SACRIFICE) // AI will never use it. It can be ignored using filter.
				)) {
				return true; // Priest & building/priest conversion + faith recharging rate
			}
		}
		if (unitDef->unitAIType == TribeAIGroupUnusedHealer) {
			if (isResourceModifier && (
				(techEffect->effectUnit == CST_RES_ORDER_FAITH_RECHARGING_RATE) ||
				(techEffect->effectUnit == CST_RES_ORDER_HEALING) // (medecine tech)
				)) {
				return true; // Healer (not used in standard game)
			}
		}
		if (IsVillager(unitDef->DAT_ID1)) { // excluding boats
			if (isResourceModifier) {
				if (techEffect->effectUnit == CST_RES_ORDER_GOLD_MINING_PODUCTIVITY) { return true; } // Villagers: mining productivity
			}
		}
		if (unitDef->DAT_ID1 == CST_UNITID_FARM) {
			if (isResourceModifier) {
				if (techEffect->effectUnit == CST_RES_ORDER_FARM_FOOD_AMOUNT) { return true; } // Farms : farm amount
			}
		}
	}
	return false;
}

// Returns "destination" unit ID if technology upgrades provided unit definition ID into another unit
// Returns -1 if technology does NOT upgrade provided unit to another unit
short int GetNewUnitIdIfTechUpgradesUnit(STRUCT_TECH_DEF *techDef, short int unitDefId) {
	if (!techDef || (unitDefId < 0)) { // -1 CAN'T be a joker here !
		return -1;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect) {
			// Upgrade THIS unit to another unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) && (techEffect->effectUnit == unitDefId)) {
				return techEffect->effectClass; // "effectClass" represents target unit def id here ;)
			}
		}
	}
	return -1;
}


// Returns true if technology enables provided unit (including upgrade TO provided unit, if provided unit is an upgrade).
bool DoesTechEnableUnit(STRUCT_TECH_DEF *techDef, short int unitDefId) {
	if (!techDef || (unitDefId < 0)) { // -1 CAN'T be a joker here !
		return false;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect) {
			// Enable unit
			if (techEffect->IsEnableUnit(unitDefId)) { return true; }
			// Upgrade some unit TO THIS unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) && (techEffect->effectClass == unitDefId)) {
				return true;
			}
		}
	}
	return false;
}

// Returns true if technology disables provided unit (generally, tech tree effects).
bool DoesTechDisableUnit(STRUCT_TECH_DEF *techDef, short int unitDefId) {
	if (!techDef || (unitDefId < 0)) { // -1 CAN'T be a joker here !
		return false;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsDisableUnit(unitDefId)) { return true; }
	}
	return false;
}


// Returns true if technology disables provided research (generally, tech tree effects).
bool DoesTechDisableResearch(STRUCT_TECH_DEF *techDef, short int researchId) {
	if (!techDef || (researchId < 0)) { // -1 CAN'T be a joker here !
		return false;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->IsDisableResearch(researchId)) { return true; }
	}
	return false;
}


// Returns true if the technology has some negative side effect (slower projectile, less-efficient villagers, etc)
// Evaluates unit attribute changes, upgraded units (slower projectile, lower damage rate, etc)
// Does not take into account "disable unit" or "disable research" effects
// In standard game, this can be trireme/ballista tower (slower projectile), jihad
bool HasTechNegativeSideEffect(STRUCT_TECH_DEF *techDef) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!techDef || !global || !global->IsCheckSumValid()) {
		return false;
	}
	STRUCT_CIVILIZATION_DEF *civdef1 = global->GetCivDef(1);
	if (!civdef1 || !civdef1->IsCheckSumValid()) { return false; }
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect->HasValidEffect()) {
			// Init
			STRUCT_UNITDEF_BASE *baseUnitDef = civdef1->GetUnitDef(techEffect->effectUnit);
			STRUCT_UNITDEF_ATTACKABLE *baseAtk = NULL;
			if (baseUnitDef && baseUnitDef->DerivesFromAttackable()) {
				baseAtk = (STRUCT_UNITDEF_ATTACKABLE*)baseUnitDef;
			}
			short int oldProjId = -1;
			short int newProjId = -1;
			float effectValue = techEffect->GetValue();
			if (baseAtk) {
				oldProjId = baseAtk->projectileUnitId; // might be -1
			}
			bool newProjectileIsMuchSlower = false;
			bool newReloadTimeIsMuchSlower = false;

			// Unit attribute modification (handles most cases)
			if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) {
				if (((effectValue > 0) && (AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) < 0)) ||
					((effectValue < 0) && (AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) > 0))) {
					return true; // "negative" attribute value change
				}
			}
			if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT) {
				if (((effectValue > 1) && (AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) < 0)) ||
					((effectValue < 1) && (AttributeValueHasPositiveEffect((TECH_UNIT_ATTRIBUTES)techEffect->effectAttribute) > 0))) {
					return true; // "negative" attribute value change
				}
			}
			if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
				// ?
				if (techEffect->effectAttribute == TUA_PROJECTILE_UNIT) {
					newProjId = (short int)effectValue;
				}
			}

			// Evaluate unit upgrade
			short int upgradedUnitId = techEffect->UpgradeUnitGetTargetUnit();
			if (upgradedUnitId >= 0) {
				STRUCT_UNITDEF_BASE *upgradedUnitDef = civdef1->GetUnitDef(upgradedUnitId);
				STRUCT_UNITDEF_ATTACKABLE *upAtk = (STRUCT_UNITDEF_ATTACKABLE *)upgradedUnitDef;
				if (upAtk && upAtk->DerivesFromAttackable()) {
					newProjId = upAtk->projectileUnitId; // might be -1
					float oldDamagePerTimeUnit = baseAtk->displayedAttack / baseAtk->reloadTime1;
					float newDamagePerTimeUnit = upAtk->displayedAttack / upAtk->reloadTime1;
					newReloadTimeIsMuchSlower = (newDamagePerTimeUnit < oldDamagePerTimeUnit);
				}
			}

			// Evaluate projectile unit upgrade
			if ((oldProjId >= 0) && (newProjId >= 0)) {
				STRUCT_UNITDEF_PROJECTILE *oldProj = (STRUCT_UNITDEF_PROJECTILE*)civdef1->GetUnitDef(oldProjId);
				STRUCT_UNITDEF_PROJECTILE *newProj = (STRUCT_UNITDEF_PROJECTILE*)civdef1->GetUnitDef(newProjId);
				if (oldProj && oldProj && newProj->IsCheckSumValid() && newProj->IsCheckSumValid()) {
					float speedInitial = oldProj->speed;
					float speedAfter = newProj->speed;
					newProjectileIsMuchSlower = (speedAfter < speedInitial * 0.9);
				}
			}
			if (newProjectileIsMuchSlower || newReloadTimeIsMuchSlower) {
				return true;
			}
		}
	}
	return false;
}


// Returns the first (non disabled) research ID found that enables a unit. Returns -1 if not found.
// startAtResearchId : -1=ignored (joker). If >=0, than the search will start at this index and ignore previous researches.
short int FindResearchThatEnableUnit(STRUCT_PLAYER *player, short int unitDefId, short int startAtResearchId) {
	if (!player || !player->IsCheckSumValid() || !player->ptrGlobalStruct || !player->ptrGlobalStruct->IsCheckSumValid() ||
		!player->ptrGlobalStruct->technologiesInfo || !player->ptrGlobalStruct->technologiesInfo->IsCheckSumValid()) {
		return -1;
	}
	if (startAtResearchId < 0) { startAtResearchId = 0; }
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return -1; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray || !statuses) { return -1; }
	for (int researchId = startAtResearchId; researchId < resCount; researchId++) {
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = resInfoArray->GetResearchDef(researchId);
		assert(resDef);
		if (!resDef) { return -1; }
		if (statuses[researchId].currentStatus > RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) {
			short int techId = resDef->technologyId;
			if ((techId >= 0) && DoesTechEnableUnit(player->ptrGlobalStruct->technologiesInfo->GetTechDef(techId), unitDefId)) {
				return researchId;
			}
		}
	}
	return -1;
}


// Returns an ordered list of research IDs that allow researching input researches (researchesList) with all dependencies
// Returned collection includes all dependencies, including "shadow" researches (no location).
// Warning: dependency with choices (optional requirements, e.g. research74="A01, B01, F01, I01 ->Tool Age" are returned
// ... BUT underlying requirements are NOT analyzed (as we can't choose which requirements should be used / ignored)
// The caller MUST handle such researches (tip: use ResearchHasOptionalRequirements(...) to identify them)
// All research IDs in returned collection are available in tech tree. Impossible research IDs from researchesList are NOT in returned collection.
std::vector<short int> GetValidOrderedResearchesListWithDependencies(STRUCT_PLAYER *player, std::vector<short int> researchesList) {
	std::vector<short int> allValidResearchesToReturn;
	if (!player || !player->IsCheckSumValid()) { return allValidResearchesToReturn; }
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return allValidResearchesToReturn; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray || !statuses) { return allValidResearchesToReturn; }

	std::set<short int> requiredResearches; // Set of research IDs that are required (including input list)
	for each (short int researchId in researchesList) {
		requiredResearches.insert(researchId);
	}

	// First collect all required researches (input list + dependencies), recursively (for dependencies)
	int previousCount = 0;
	int currentCount = requiredResearches.size();
	while (previousCount < currentCount) {
		std::set<short int> elementsToAdd;
		// Collect required reseaches (=>elementsToAdd) for all researches in list (requiredResearches)
		for each (short int researchId in requiredResearches)
		{
			STRUCT_RESEARCH_DEF *resDef = resInfoArray->GetResearchDef(researchId);
			for (int i = 0; i < 4; i++) {
				if (!ResearchHasOptionalRequirements(resDef) && (resDef->requiredResearchId[i] > -1)) {
					// Don't add optional requirements (we can't guess which ones should be actually added)
					elementsToAdd.insert(resDef->requiredResearchId[i]);
				}
			}
		}
		// Do not insert into requiredResearches while looping in it !
		for each (short int curResId in elementsToAdd) {
			requiredResearches.insert(curResId); // will not insert duplicates
		}
		elementsToAdd.clear();
		previousCount = currentCount;
		currentCount = requiredResearches.size();
	}

	// Move available researches to "ok" list until requiredResearches is empty (or we're stuck)

	int remainingRequirementsCount = requiredResearches.size();
	int previousRemainingCount = 9999;
	std::set<short int> requiredElementsToRemove;
	//std::set<short int> impossibleResearches;

	while ((remainingRequirementsCount > 0) && (remainingRequirementsCount != previousRemainingCount)) {
		for each (short int curResId in requiredResearches) {
			STRUCT_RESEARCH_DEF *resDef = resInfoArray->GetResearchDef(curResId);
			switch (statuses[curResId].currentStatus) {
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE:
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED:
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID:
				// This research is OK for tech tree
				requiredElementsToRemove.insert(curResId);
				allValidResearchesToReturn.push_back(curResId);
				break;
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT:
				// If all requirements are OK, then it's ok. Otherwise, wait for a next loop (some dependencies may evolve).
				bool requirementsAreOk;
				// Compiler bug: can't initialize vars in declaration in a case ! (lol)
				requirementsAreOk = true;
				if (ResearchHasOptionalRequirements(resDef)) {
					// When a research with "choices" is met, do not go further and consider it OK
					// Such researches are shadow technologies for buildings/ages. 
					// The caller *MUST* check their requirements, we don't have enough context here (when minReq=2 and we have 4 requirements, which ones should we choose ?)
					requirementsAreOk = true;
				} else {
					for (int i = 0; i < 4; i++) {
						short int requiredResearchId = resDef->requiredResearchId[i];
						if (requiredResearchId > -1) {
							bool requiredResearchIsAvailable = (statuses[requiredResearchId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE) ||
								(statuses[requiredResearchId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED) ||
								(statuses[requiredResearchId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID) ||
								// Shadow intermediate researches (no location) are always considered OK here
								// They can be Market+Bronze Age->Temple, {2 bronze bld}->Iron Age, etc
								(resDef->researchLocation == -1);
							// If requirement is NOT currently available and NOT in our "OK list", then the dependency can't be satisfied (for now)
							if (!requiredResearchIsAvailable &&
								(std::count(allValidResearchesToReturn.begin(), allValidResearchesToReturn.end(), requiredResearchId) == 0)) {
								requirementsAreOk = false;
							}
						}
					}
				}
				if (requirementsAreOk) {
					requiredElementsToRemove.insert(curResId);
					allValidResearchesToReturn.push_back(curResId);
				}
				break;
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED:
				//impossibleResearches.insert(curResId); // Unused / should not happen if tech tree is consistent ?
				break;
			default:
				break;
			}
		}

		// Do not do the remove IN the loop ! Do it once iterations are finished
		for each (short int curResId in requiredElementsToRemove)
		{
			requiredResearches.erase(curResId);
		}
		requiredElementsToRemove.clear(); // reset temp loop buffer
		previousRemainingCount = remainingRequirementsCount;
		remainingRequirementsCount = requiredResearches.size();
	}

	if (remainingRequirementsCount > 0) {
		std::string msg = "[Build research requirements tree] some researches are not available: ";
		bool first = true;
		for each (short int researchId in requiredResearches)
		{
			if (!first) {
				msg += ", ";
			}
			first = false;
			msg += std::to_string(researchId);
		}
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
	return allValidResearchesToReturn;
}


// Returns a unit (building) definition that enables provided research ID.
// For example, for research 17, it will return temple's definition because temple's unitDef.initiatesResearch == 17.
// Returns NULL if not found.
// Warning: result is not unique. Eg. Both barracks (12,132) initiate research 62. This method tries to return the base unit ID (not upgraded one)
AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *FindBuildingDefThatEnablesResearch(STRUCT_PLAYER *player, short int researchId) {
	if (!player || !player->IsCheckSumValid() || researchId < 0) { return NULL; }
	for (int unitDefId = 0; unitDefId < player->structDefUnitArraySize; unitDefId++) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDefBuilding = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)player->ptrStructDefUnitTable[unitDefId];
		if (unitDefBuilding && unitDefBuilding->IsCheckSumValid() && unitDefBuilding->IsTypeValid()) {
			if (unitDefBuilding->initiatesResearch == researchId) {
				short int baseBldId = GetBaseBuildingUnitId(unitDefBuilding->DAT_ID1);
				if (baseBldId != unitDefBuilding->DAT_ID1) {
					// It seems current unit is an "upgraded" unit. Before returning, try "base" unit because this result would be more relevant
					if ((baseBldId >= 0) && (baseBldId < player->structDefUnitArraySize)) {
						AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING* baseBld = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)player->ptrStructDefUnitTable[baseBldId];
						if (baseBld && baseBld->IsCheckSumValid() && baseBld->IsTypeValid() && (baseBld->initiatesResearch == researchId)) {
							return baseBld;
						}
					}
				}
				return unitDefBuilding;
			}
		}
	}
	return NULL;
}


// [private] Writes a string representing tech tree information for a given research
// defaultResearchName = name to write for research, if none can be found automatically
std::string GetResearchTechTreeLine(AOE_STRUCTURES::STRUCT_PLAYER *player, short int researchId, 
	const char *defaultResearchName) {
	std::string result = "";
	char nameBuffer[50];
	nameBuffer[0] = 0;
	if (researchId < 0) {
		traceMessageHandler.WriteMessageNoNotification("Tried to get tech tree line for invalid research ID");
		return result;
	}
	STRUCT_RESEARCH_DEF *resDef = player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(researchId);
	assert(resDef != NULL);
	if (resDef == NULL) { return result; }
	bool isAutomaticTech = (resDef->researchTime <= 0);
	result = "techId ";
	result += std::to_string(resDef->technologyId);
	result += " = ";
	GetLanguageDllText(resDef->languageDLLName, nameBuffer, sizeof(nameBuffer) - 1, resDef->researchName);
	if (isAutomaticTech) { result += "("; }
	result += ((nameBuffer[0] == 0) ? defaultResearchName : nameBuffer);
	if (isAutomaticTech) { result += ")"; }

	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *rs = player->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
	if (rs[researchId].currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
		result += " (";
		result += localizationHandler.GetTranslation(CRLANG_ID_NOT_AVAILABLE_YET, "not available yet");
		result += " )";
	}
	return result;
}

// [private] Get a string representing all (future) available tech tree for building
// (researches and units that can become available in given building)
std::string GetRemainingTechTreeText(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bldDef) {
	// Invalid (technical reasons)
	if (!bldDef || !bldDef->IsCheckSumValidForAUnitClass() || !bldDef->IsTypeValid() ||
		!player || !player->IsCheckSumValid()) { return ""; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) { return ""; }
	// Not relevant (functional reasons)
	if ((bldDef->unitType != AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) ||
		(bldDef->hideInEditor != 0 /*exclude duplicates*/) || (bldDef->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding/*excludes walls*/) ||
		(bldDef->interactionMode != 3/*exclude towers*/) || (bldDef->combatLevel != UNITDEF_COMBAT_LEVEL::CST_UCL_BUILDING /* this criterion might no be relevant*/)
		) { return ""; }
	std::string result = "";
	char nameBuffer[50];
	char nameBufferBackup[50];
	GetLanguageDllText(bldDef->languageDLLID_Name, nameBuffer, sizeof(nameBuffer), bldDef->ptrUnitName);
	result += std::string(nameBuffer) + std::string(": "); // Write building name as a "header" (title)

	bool foundItems = false;
	assert(player->ptrResearchesStruct);
	if (!player->ptrResearchesStruct) { return result; }
	short int researchCount = player->ptrResearchesStruct->researchCount;
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *rs = player->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
	// Search all researches that are developed in this building (and write text for each).
	for (int rid = 0; rid < researchCount; rid++) {
		*nameBuffer = 0; // Reset string for next usage
		*nameBufferBackup = 0;
		if ((rs[rid].currentStatus >= AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) &&
			(rs[rid].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID)) {

			STRUCT_RESEARCH_DEF *resDef = player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(rid);
			assert(resDef != NULL);
			//short int researchLocation = resDef->researchLocation;
			if (resDef && (resDef->researchLocation < 0)) {
				// Some research, like slinger, camel, are missing researchLocation (-1). We can try to retrieve it from technology/enabledUnit=>trainlocation
				short int techId = resDef->technologyId;
				if ((techId >= 0) && (techId < global->technologiesInfo->technologyCount)) {
					AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techId);
					short int enabledUnitDefID = -1;
					if (techDef) {
						// Find if this tech enables some unit
						for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
							STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
							if (techEffect && techEffect->HasValidEffect()) {
								// Enable unit
								if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 1)) {
									enabledUnitDefID = techEffect->effectUnit;
								}
								// Upgrade *to* some unit
								if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) {
									enabledUnitDefID = techEffect->UpgradeUnitGetTargetUnit();
								}
							}
						}
					}
					if (enabledUnitDefID >= 0) {
						AOE_STRUCTURES::STRUCT_UNITDEF_BASE *enabledUnitDefBase = player->GetUnitDefBase(enabledUnitDefID);
						if (enabledUnitDefBase && enabledUnitDefBase->IsCheckSumValidForAUnitClass() && enabledUnitDefBase->DerivesFromTrainable()) {
							AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *t = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)enabledUnitDefBase;
							if (t->trainLocation == bldDef->DAT_ID1) {
								// Get unit name. Might be used if research does not have a name
								GetLanguageDllText(t->languageDLLID_Name, nameBufferBackup, sizeof(nameBufferBackup), t->ptrUnitName);
								result += "\r\n";
								result += GetResearchTechTreeLine(player, rid, nameBufferBackup);
								foundItems = true;
							}
						}
					}
				}
			} else {
				if (resDef && (resDef->researchLocation == bldDef->DAT_ID1)) {
					result += "\r\n";
					result += GetResearchTechTreeLine(player, rid, "");
					foundItems = true;
				}
			}
		}
	}
	if (foundItems) {
		result += std::string("\r\n");
	}
	return result;
}


// Writes text representing available tech tree (available technologies that have not been researched yet)
std::string GetRemainingTechTreeText(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return ""; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) {
		return "";
	}
	std::string result;
	int unitDefCount = player->structDefUnitArraySize;
	// Loop on all buildings that can potentially "host" researches
	for (int i = 0; i < unitDefCount; i++) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = player->ptrStructDefUnitTable[i];
		std::string s = GetRemainingTechTreeText(player, (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)unitDef);
		if (!s.empty()) {
			result += s;
			result += "\r\n"; // Leave an line break
		}
	}
	return result;
}


// Apply a technology for a player. To be used only when there is no research for given technology (like cheat techs).
// In most cases, please use ApplyResearchForPlayer instead.
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int tech_id) {
	if (!player || !player->IsCheckSumValid() || (!player->ptrGlobalStruct) || (!player->ptrGlobalStruct->IsCheckSumValid())) { return false; }
	if (tech_id < 0) { return false; } // The call we do will check that tech_id is valid.
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techDefInfo = player->ptrGlobalStruct->technologiesInfo;
	if (!techDefInfo || !techDefInfo->IsCheckSumValid()) { return false; }
	_asm {
		MOV ECX, techDefInfo;
		XOR EAX, EAX;
		MOV AX, tech_id;
		PUSH player;
		PUSH EAX;
		MOV EAX, 0x004EBB10;
		CALL EAX;
		// No return value
	}
	return true;
}
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int tech_id) {
	return ApplyTechnologyForPlayer(player, (short int)tech_id);
}


// Apply a research for a player (if possible).
// Not allowed if research status is CST_RESEARCH_STATUS_DISABLED or CST_RESEARCH_STATUS_DONE_OR_INVALID
// Works as expected (= like if player actually clicked the corresponding button...) if research status is CST_RESEARCH_STATUS_AVAILABLE
// Works with side effects if research status is : CST_RESEARCH_STATUS_WAITING_REQUIREMENT : other researches might not become available,
// ... tech tree might be somewhat stuck
// Return true if successful
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id) {
	if (!player || !player->IsCheckSumValid() || (!player->ptrResearchesStruct)) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *researchInfo = player->ptrResearchesStruct;
	if (!researchInfo) { return false; }
	if ((research_id < 0) || (research_id >= researchInfo->researchCount)) { return false; }
	short int currentStatus = researchInfo->researchStatusesArray[research_id].currentStatus;
	if ((currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) ||
		(currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID)) {
		return false;
	}
	long int result;
	// Apply research if its status is not done and not disabled.
	_asm {
		MOV ECX, researchInfo;
		XOR EAX, EAX;
		MOV AX, research_id;
		PUSH EAX;
		MOV EAX, 0x004EAA3D;
		CALL EAX;
		MOV result, EAX;
	}
	return (result != 0);
}
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id) {
	return ApplyResearchForPlayer(player, (short int)research_id);
}


// Returns true if a research has been researched for a given player
bool IsResearchResearched(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id) {
	assert(player != NULL);
	assert(player->ptrResearchesStruct != NULL);
	if (player->ptrResearchesStruct == NULL) { return false; }
	if (player->ptrResearchesStruct->researchCount <= research_id) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS status = player->ptrResearchesStruct->researchStatusesArray[research_id];
	return (status.currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID);
}


// Returns current status of a research for given player.
// Returns CST_RESEARCH_STATUS_DONE_OR_INVALID if research_id is invalid (not in bounds)
AOE_CONST_FUNC::RESEARCH_STATUSES GetResearchStatus(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id) {
	assert(player != NULL);
	assert(player->ptrResearchesStruct != NULL);
	if (player->ptrResearchesStruct == NULL) { return AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID; }
	if ((research_id < 0) || (player->ptrResearchesStruct->researchCount <= research_id)) { return AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID; }
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS status = player->ptrResearchesStruct->researchStatusesArray[research_id];
	return status.currentStatus;
}
