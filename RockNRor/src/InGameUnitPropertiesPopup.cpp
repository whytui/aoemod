#include "../include/InGameUnitPropertiesPopup.h"


namespace ROCKNROR {
namespace UI {
;



// Reset various pointers for this class level (to override)
void InGameUnitPropertiesPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->unitId = -1;
	this->btnOK = NULL;
	this->lblMainInfos = NULL;
	this->lblTitle = NULL;
	this->lblChildUnitsAutoMove = NULL;
	this->btnResetAutoMove = NULL;
	this->lblFarmAutoRebuild = NULL;
	this->edtBuildingTechs = NULL;
	this->lblBuildingTechsMessage = NULL;
	this->chkForceRebuildFarm = NULL;
	this->chkForceNotRebuildFarm = NULL;
	this->chkRebuildFarmNone = NULL;
	this->lblAutoAttackUnits = NULL;
	this->chkAutoAttackBuildings = NULL;
	this->chkAutoAttackMilitary = NULL;
	this->chkAutoAttackTowers = NULL;
	this->chkAutoAttackVillagers = NULL;
	this->chkAutoAttackWalls = NULL;
	this->edtStrengthWeakness = NULL;
	this->lblConversionResistance = NULL;
	this->btnMakeMainUnitForShortcutSelection = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void InGameUnitPropertiesPopup::CreateScreenComponents() {
	STRUCT_UI_EASY_PANEL *popup = this->GetAoeScreenObject();
	if ((this->GetScreenStatus() == CLOSED) || (popup == NULL) || (this->unitId < 0)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) GetUnitStruct(this->unitId);
	AOE_STRUCTURES::STRUCT_PLAYER *unitPlayer = unitBase->ptrStructPlayer;
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unitBase->unitDefinition;
	if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass() || !unitPlayer || !unitPlayer->IsCheckSumValid() ||
		!controlledPlayer || !controlledPlayer->IsCheckSumValid() ||
		!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) {
		return;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDefBase;
	bool isValidType50 = unitDef50->DerivesFromAttackable();
	bool isMyUnit = (controlledPlayer == unitPlayer);
	bool isTower = isValidType50 && IsTower(unitDefBase);
	bool isMilitary = isValidType50 && (isTower || IsNonTowerMilitaryUnit(unitDefBase->unitAIType));
	bool isRangedUnit = (isValidType50 && (unitDef50->maxRange > 0));

	long int currentYPos = 10;
	const long int btnSize = 0xAC;
	const long int lblTitleHSize = 250;
	const long int lblMainInfoHSize = 400;
	std::string mainInfos = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_UNIT_TERM, "Unit");
	mainInfos += " ";
#ifdef _DEBUG
	mainInfos += "id=";
	mainInfos += std::to_string(this->unitId);
	mainInfos += " ";
#endif
	mainInfos += "\"";
	mainInfos += unitDefBase->ptrUnitName;
	mainInfos += "\" (";
	mainInfos += unitPlayer->playerName_length16max;
	mainInfos += "). ";
	mainInfos += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_SHORTCUT_TERM, "Shortcut");
	mainInfos += "=";
	// Shortcut / group info
	if ((unitBase->shortcutNumber == 0) || (unitBase->shortcutNumber > 10)) {
		mainInfos += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_NONE_TERM, "none");
	} else {
		char displayNumber = unitBase->shortcutNumber;
		if (displayNumber == -11) {
			displayNumber = 20;
		}
		if ((displayNumber < 0) && (displayNumber >= -11)) {
			displayNumber = displayNumber + 21; // for custom shortcut numbers (numpad, stored values are -11 to -2)
		}
		if (displayNumber > 0) {
			mainInfos += std::to_string(displayNumber);
		} else {
			mainInfos += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_INVALID_TERM, "invalid");
		}
	}
	if (unitBase->shortcutNumber > 10) {
		mainInfos += " [";
		mainInfos += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_PART_OF_GROUP, "part of a group");
		mainInfos += "]";
	}

	// Automove infos
	std::string autoMoveInfo = "";
	UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(this->unitId);
	if (isMyUnit && unitInfo && (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		if (unitInfo->spawnTargetUnitId >= 0) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(unitInfo->spawnTargetUnitId);
			char *targetName = NULL;
			if (targetUnitBase && targetUnitBase->IsCheckSumValidForAUnitClass() &&
				targetUnitBase->unitDefinition && targetUnitBase->unitDefinition->IsCheckSumValidForAUnitClass()) {
				targetName = targetUnitBase->unitDefinition->ptrUnitName;
			}
			autoMoveInfo += "[";
			autoMoveInfo += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_BUILDING, "Building");
			autoMoveInfo += "] Child units auto-target=";
			autoMoveInfo += std::to_string(unitInfo->spawnTargetUnitId);
			if (targetName) {
				autoMoveInfo += " (";
				autoMoveInfo += targetName;
				autoMoveInfo += ")";
			}
		}
		if ((unitInfo->spawnUnitMoveToPosX >= 0) && (unitInfo->spawnUnitMoveToPosY >= 0)) {
			char buf[10];
			autoMoveInfo += "[";
			autoMoveInfo += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_BUILDING, "Building");
			autoMoveInfo += "] ";
			autoMoveInfo += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_CHILDREN_UNITS_AUTOMOVE_TO, "Child units auto-move to");
			autoMoveInfo += " (";
			sprintf_s(buf, "%.2f", unitInfo->spawnUnitMoveToPosX);
			autoMoveInfo += buf;
			autoMoveInfo += ", ";
			sprintf_s(buf, "%.2f", unitInfo->spawnUnitMoveToPosY);
			autoMoveInfo += buf;
			autoMoveInfo += ")";
		}
	}
	// Farm auto-rebuild
	std::string farmInfo = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_AUTO_REBUILD_THIS_FARM_WHEN_DEPLETED, "Auto rebuild this farm when depleted");
	farmInfo += ":";
	FarmRebuildInfo *farmBldInfo = ROCKNROR::crInfo.myGameObjects.FindFarmRebuildInfo(unitBase->positionX, unitBase->positionY);

	// Building : future potential techs/units
	std::string buildingTechAndUnitInfo = "";
	long int techToShowCount = 0;
	if (unitPlayer && isMyUnit && (unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		char nameBuffer[50];
		assert(ROCKNROR::crInfo.techTreeAnalyzer.IsReady());
		if (ROCKNROR::crInfo.techTreeAnalyzer.IsReady()) {
			ROCKNROR::STRATEGY::TTDetailedBuildingDef *bldDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(unitDefBase->DAT_ID1);
			if (bldDetail && bldDetail->IsValid()) {
				short int researchCount = unitPlayer->ptrResearchesStruct->researchCount;
				AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *resStatusArray = unitPlayer->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
				list<long int> orderedResearchIds;
				for each (long int oneResearchId in bldDetail->researchesDevelopedHere) {
					orderedResearchIds.push_back(oneResearchId);
				}
				orderedResearchIds.sort([](long int left, long int right){
					auto l = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(left);
					auto r = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(right);
					if (!l) { return true; }
					if (!r) { return false; }
					return *l < *r;
				});

				for each (long int oneResearchId in orderedResearchIds)
				{
					if (resStatusArray && (oneResearchId >= 0) && (oneResearchId < researchCount)) {
						if (resStatusArray[oneResearchId].currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
							techToShowCount++;
							if (techToShowCount > 0) { buildingTechAndUnitInfo += "\n"; }
							buildingTechAndUnitInfo += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_TECHID_ABBREV, "techId");
							buildingTechAndUnitInfo += " ";
							buildingTechAndUnitInfo += std::to_string(unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef((short int)oneResearchId)->technologyId);
							buildingTechAndUnitInfo += " = ";
							*nameBuffer = 0; // Reset string
							GetLanguageDllText(unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef((short int)oneResearchId)->languageDLLName, nameBuffer, sizeof(nameBuffer) - 1,
								unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef((short int)oneResearchId)->researchName);
							buildingTechAndUnitInfo += nameBuffer;
						}
					}
				}
			}
		}
	}

	this->AddLabel(&this->lblTitle, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_INGAME_TITLE, "Unit properties"),
		(this->GetScreenSizeX() - lblTitleHSize) / 2, currentYPos, lblTitleHSize, 30, AOE_FONTS::AOE_FONT_BIG_LABEL);
	currentYPos += 50;
	this->AddLabel(&this->lblMainInfos, (char*)mainInfos.c_str(), (this->GetScreenSizeX() - lblMainInfoHSize) / 2, currentYPos, lblMainInfoHSize, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	currentYPos += 40;
	// Note: auto-move and farm info have same Y position because they can't be displayed simultaneously.
	if (!autoMoveInfo.empty()) {
		this->AddLabel(&this->lblChildUnitsAutoMove, (char*)autoMoveInfo.c_str(), 30, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddButton(&this->btnResetAutoMove, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_DISABLE_AUTO_MOVE, "Disable auto-move"), 340, currentYPos, btnSize, 22, 0);
		currentYPos += 30;
	}
	// Auto rebuild farm
	if (isMyUnit && (unitDefBase->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_FARM)) {
		this->AddLabel(&this->lblFarmAutoRebuild, (char*)farmInfo.c_str(), 30, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);

		AOE_STRUCTURES::STRUCT_UI_LABEL *unusedLabel;
		this->AddCheckBox(&this->chkRebuildFarmNone, 330, currentYPos - 4, 24, 24);
		this->AddCheckBox(&this->chkForceRebuildFarm, 430, currentYPos - 4, 24, 24);
		this->AddCheckBox(&this->chkForceNotRebuildFarm, 530, currentYPos - 4, 24, 24);
		this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_DEFAULT, "Default"), 270, currentYPos, 50, 20);
		this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_ALWAYS, "Always"), 370, currentYPos, 50, 20);
		this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_NEVER, "Never"), 470, currentYPos, 50, 20);
		currentYPos += 20;

		FarmRebuildInfo *fri = ROCKNROR::crInfo.myGameObjects.FindFarmRebuildInfo(unitBase->positionX, unitBase->positionY);
		if (!fri) {
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRebuildFarmNone, true);
		} else {
			if (fri->forceNotRebuild) {
				AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkForceNotRebuildFarm, true);
			} else {
				if (fri->forceRebuild) {
					AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkForceRebuildFarm, true);
				} else {
					AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRebuildFarmNone, true);
				}
			}
		}
	}

	// Military : guard location ?
	if (isMilitary && isMyUnit) {
		bool canHurtOtherUnits = (unitDef50->blastLevel != CST_BL_DAMAGE_TARGET_ONLY) && (unitDef50->blastRadius > 0);
		this->AddLabel(&this->lblAutoAttackUnits, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_AUTO_ATTACK_UNITS, "Auto attack units:"), 30, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddLabel(&this->lblAutoAttackTowers, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_TOWERS, "Towers"), 180, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddLabel(&this->lblAutoAttackMilitary, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_MILITARY, "Military"), 250, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddLabel(&this->lblAutoAttackBuildings, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_BUILDINGS, "Buildings"), 320, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddLabel(&this->lblAutoAttackVillagers, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_VILLAGERS, "Villagers"), 390, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddLabel(&this->lblAutoAttackWalls, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_WALLS, "Walls"), 460, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		currentYPos += 30;
		this->AddCheckBox(&this->chkAutoAttackTowers, 195, currentYPos - 4, 24, 24);
		this->AddCheckBox(&this->chkAutoAttackMilitary, 265, currentYPos - 4, 24, 24);
		this->AddCheckBox(&this->chkAutoAttackBuildings, 335, currentYPos - 4, 24, 24);
		this->AddCheckBox(&this->chkAutoAttackVillagers, 405, currentYPos - 4, 24, 24);
		this->AddCheckBox(&this->chkAutoAttackWalls, 475, currentYPos - 4, 24, 24);
		currentYPos += 30;
		const AutoAttackPolicy *aap = NULL;
		if (canHurtOtherUnits) {
			if (isRangedUnit) {
				aap = &ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits;
			} else {
				aap = &ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits;
			}
		}
		// If there is a config at unit level, take it instead of global parameter.
		if (unitInfo && (unitInfo->autoAttackPolicyIsSet)) {
			aap = &unitInfo->autoAttackPolicy;
		}
		if (!aap) {
			// No unit-specific config, and unit does not fit user config (not a blast attack unit)
			aap = &ROCKNROR::crInfo.configInfo.autoAttackOptionDefaultValues;
		}
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAutoAttackTowers, aap->attackTowers);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAutoAttackMilitary, aap->attackMilitary);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAutoAttackBuildings, aap->attackNonTowerBuildings);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAutoAttackVillagers, aap->attackVillagers);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAutoAttackWalls, aap->attackWalls);
	}

	// Special attacks and armors (unit bonus/malus)
	std::string attackStrengths = "";
	std::string armorWeaknesses = "";
	if (isValidType50) {
		int linesCount = 0;
		for (int i = 0; i < unitDef50->attacksCount; i++) {
			if (!IsStandardAttack(unitDef50->ptrAttacksList[i].classId) && (unitDef50->ptrAttacksList[i].amount >= 0)) {
				const std::string attackName = GetAttackOrArmorClassNameString(unitDef50->ptrAttacksList[i].classId);
				attackStrengths += attackName;
				// Do not display "bonus" attack amounts, they generally are 0 (a malus is set on armors instead)
				//attackStrengths += "=";
				//attackStrengths += std::to_string(unitDef50->ptrAttacksList[i].amount);
				attackStrengths += " ; ";
			}
		}
		for (int i = 0; i < unitDef50->armorsCount; i++) {
			if (!IsStandardAttack(unitDef50->ptrArmorsList[i].classId)) {
				const std::string attackName = GetAttackOrArmorClassNameString(unitDef50->ptrArmorsList[i].classId);
				armorWeaknesses += attackName;
				armorWeaknesses += "=";
				armorWeaknesses += std::to_string(unitDef50->ptrArmorsList[i].amount);
				armorWeaknesses += " ; ";
			}
		}
		linesCount += 2; // attacks/armors
		std::string wholeText = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_ATTACK_BONUSES_FOR, "Attack bonuses for");
		wholeText += std::string(": ");
		if (attackStrengths.empty()) {
			wholeText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_NONE_TERM, "none");
		} else {
			wholeText += attackStrengths;
		}
		wholeText += std::string("\r\n");
		wholeText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_ARMOR_WEAKNESSES, "Armor weaknesses");
		wholeText += std::string(": "); std::string(": ");
		if (armorWeaknesses.empty()) {
			wholeText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_NONE_TERM, "none");
		} else {
			wholeText += armorWeaknesses;
		}

		// Priest characteristics
		if (unitDefBase->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
			bool hasMonotheism = (unitPlayer->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_CAN_CONVERT_PRIEST) != 0);
			float rechargingRate = unitPlayer->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_FAITH_RECHARGING_RATE);
			float healing = unitPlayer->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_HEALING);
			bool hasMartyrdom = (unitPlayer->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_PRIEST_SACRIFICE) != 0);
			float conversionEfficiency = unitDef50->workRate;
			std::string priestText = "\r\n";
			priestText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_CONVERSION_EFFICIENCY, "Conversion efficiency");
			priestText += "=";
			priestText += std::to_string(conversionEfficiency);
			priestText += "\r\n";
			priestText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_CONVERSION_RECHARGING_RATE, "Recharging rate");
			priestText += "=";
			priestText += std::to_string(rechargingRate);
			priestText += "\r\n";
			priestText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_HEALING_RATE_BONUS, "Healing rate bonus");
			priestText += "=";
			priestText += (healing > 0) ? localizationHandler.GetTranslation(CRLANG_ID_YES, "Yes") : localizationHandler.GetTranslation(CRLANG_ID_NO, "No");
			priestText += "\r\n";
			std::string tmpStr = GetLanguageDllText(LANG_ID_RESEARCH_MONOTHEISM);
			if (tmpStr.empty()) {
				tmpStr = "Monotheism";
			}
			priestText += tmpStr;
			priestText += "=";
			priestText += hasMonotheism ? localizationHandler.GetTranslation(CRLANG_ID_YES, "Yes") : localizationHandler.GetTranslation(CRLANG_ID_NO, "No");
			priestText += "\r\n";
			tmpStr = GetLanguageDllText(LANG_ID_RESEARCH_MARTYRDOM);
			if (tmpStr.empty()) {
				tmpStr = "Martyrdom";
			}
			priestText += tmpStr;
			priestText += "=";
			priestText += hasMartyrdom ? localizationHandler.GetTranslation(CRLANG_ID_YES, "Yes") : localizationHandler.GetTranslation(CRLANG_ID_NO, "No");
			wholeText += priestText;
			linesCount += 5;
		}

		// Write civ bonus that applies to this unit, if any
		if (unitBase->DerivesFromTrainable()) {
			STRUCT_UNIT_TRAINABLE *unitTrainable = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)unitBase;
			long int bonusTextIndex = -1;
			if (unitTrainable->hasDedicatedUnitDef) {
				UnitCustomInfo *info = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unitTrainable->unitInstanceId);
				if (info != NULL) { bonusTextIndex = info->bonusTextIndex; }
			}
			else {
				if (ROCKNROR::crInfo.myGameObjects.HasUnitDefBonusTextInfo(unitPlayer->playerId, unitBase->unitDefinition->DAT_ID1)) {
					// For a "native" unit (directly attached to the player's "standard" unitDefinition), get text from the player's unit definition !
					bonusTextIndex = (long int)ROCKNROR::crInfo.myGameObjects.bonusInGameTextByPlayerAndUnitDefId[unitPlayer->playerId]
						[unitBase->unitDefinition->DAT_ID1];
				}
			}
			if (bonusTextIndex >= 0) {
				std::string str = ROCKNROR::crInfo.myGameObjects.inGameTextHandler.GetText(bonusTextIndex);
				if (!str.empty()) {
					wholeText += "\r\n";
					wholeText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_BONUS, "Bonus");
					wholeText += ": ";
					wholeText += str;
					linesCount++;
				}
			}
			if (ROCKNROR::crInfo.myGameObjects.HasNonTransmissibleUnitDefBonusTextInfo(unitPlayer->playerId, unitBase->unitDefinition->DAT_ID1)) {
				// For ANY unit, (non transmissible) player-level bonuses always apply : conversion efficiency, etc
				// With player-level bonuses, the unit *might* have 2 bonuses (one from native civ, + one for current player)
				bonusTextIndex = (long int)ROCKNROR::crInfo.myGameObjects.nonTransmissibleBonusInGameTextByPlayerAndUnitDefId[unitPlayer->playerId]
					[unitBase->unitDefinition->DAT_ID1];
				if (bonusTextIndex >= 0) {
					std::string str = ROCKNROR::crInfo.myGameObjects.inGameTextHandler.GetText(bonusTextIndex);
					if (!str.empty()) {
						wholeText += "\r\n";
						wholeText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_BONUS, "Bonus");
						wholeText += ": ";
						wholeText += str;
						linesCount++;
					}
				}
			}
		}

		if (unitInfo && (unitInfo->creatorPlayerId >= 0)) {
			wholeText += "\r\n";
			wholeText += localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_CREATOR, "Creator : p#");
			wholeText += std::to_string(unitInfo->creatorPlayerId);
			linesCount++;
		}

		this->AddTextBox(&this->edtStrengthWeakness, wholeText.c_str(), 0, 30, currentYPos, 450, 10 + linesCount * 14, true, true, false, AOE_FONTS::AOE_FONT_SMALL_TEXT_10);
		currentYPos += 14 + linesCount * 14;
	}

	// Conversion resistance : only for living/building (other units can't be converted)
	if (unitBase->DerivesFromTrainable()) {
		float conversionResistance = ROCKNROR::crInfo.GetConversionResistance(unitPlayer->civilizationId, unitDefBase);
		std::string convResistText = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_CONVERSION_RESISTANCE, "Conversion resistance");
		convResistText += "=";
		convResistText += std::to_string(conversionResistance);
		this->AddLabel(&this->lblConversionResistance, convResistText.c_str(), 30, currentYPos, 300, 16);
		currentYPos += 20;
	}

	// Building : future potential techs/units
	if (!buildingTechAndUnitInfo.empty() && (techToShowCount > 0) && isMyUnit) {
		this->AddLabel(&this->lblBuildingTechsMessage, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_FUTURE_UNAVAILABLE_TECHS, "Future technologies, not available yet:"), 30, currentYPos, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		currentYPos += 20;
		this->AddTextBox(&this->edtBuildingTechs, buildingTechAndUnitInfo.c_str(), 0, 30, currentYPos, 450, 20 + techToShowCount * 14, true, true);
		currentYPos += 20 + techToShowCount * 14 + 20;
	}

	if (isMyUnit && (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		this->AddButton(&this->btnMakeMainUnitForShortcutSelection,
			localizationHandler.GetTranslation(CRLANG_ID_BTN_UNIT_MAKE_MAIN_FOR_KEY_SHORTCUT_SELECTION, "Make main unit for keyboard shortcut selection"),
			30, currentYPos, 400, 22);
		currentYPos += 30;
	}

	// OK button
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(80), 
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameUnitPropertiesPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnOK) {
		this->Validate();
		this->CloseScreen(false);
		return true;
	}
	if (sender == this->btnResetAutoMove) {
		UnitCustomInfo *u = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(this->unitId);
		if (u) {
			u->ResetSpawnAutoTargetInfo();
			ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(this->unitId);
		}
		// Auto-move has been reset, hide label and button.
		AOE_METHODS::UI_BASE::ShowUIObject(this->btnResetAutoMove, false);
		AOE_METHODS::UI_BASE::ShowUIObject(this->lblChildUnitsAutoMove, false);
		return true;
	}
	if (sender == this->chkForceNotRebuildFarm) {
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRebuildFarmNone, false);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkForceRebuildFarm, false);
	}
	if (sender == this->chkForceRebuildFarm) {
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRebuildFarmNone, false);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkForceNotRebuildFarm, false);
	}
	if (sender == this->chkRebuildFarmNone) {
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkForceRebuildFarm, false);
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkForceNotRebuildFarm, false);
	}
	if (sender == this->btnMakeMainUnitForShortcutSelection) {
		AOE_STRUCTURES::PLAYER::MakeMainUnitForShortcutSelection(this->unitId);
	}
	return false; // Not one of our buttons; let ROR code be executed normally
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameUnitPropertiesPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Save changes
void InGameUnitPropertiesPopup::Validate() {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = GetUnitStruct(this->unitId); // Might be NULL
	AOE_STRUCTURES::STRUCT_PLAYER *unitPlayer = NULL;
	if (unit && unit->IsCheckSumValidForAUnitClass()) {
		unitPlayer = unit->ptrStructPlayer;
	}
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	bool isMyUnit = (controlledPlayer == unitPlayer);
	UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(this->unitId);
	float posX = -1;
	float posY = -1;
	if (unit && unit->IsCheckSumValidForAUnitClass()) {
		posX = unit->positionX;
		posY = unit->positionY;
	}
	if (isMyUnit && this->chkRebuildFarmNone && this->chkRebuildFarmNone->currentState) {
		ROCKNROR::crInfo.myGameObjects.RemoveFarmRebuildInfo(posX, posY);
	}
	if (isMyUnit && this->chkForceNotRebuildFarm && this->chkForceRebuildFarm &&
		(this->chkForceNotRebuildFarm->currentState || this->chkForceRebuildFarm->currentState)) {
		bool actionIsRebuild = (this->chkForceRebuildFarm->currentState != 0); // If false, then it is force NOT rebuild.
		AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = ROCKNROR::crInfo.GetRelevantSelectedUnitsPointer(controlledPlayer);
		for (int i = 0; i < controlledPlayer->selectedUnitCount; i++) {
			if (selectedUnits[i] && selectedUnits[i]->IsCheckSumValidForAUnitClass() &&
				(selectedUnits[i]->ptrStructPlayer == controlledPlayer)) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *curUnitDefBase = selectedUnits[i]->unitDefinition;
				if (curUnitDefBase && curUnitDefBase->IsCheckSumValidForAUnitClass() && (curUnitDefBase->DAT_ID1 == CST_UNITID_FARM)) {
					FarmRebuildInfo *fri = ROCKNROR::crInfo.myGameObjects.FindOrAddFarmRebuildInfo(selectedUnits[i]->positionX, selectedUnits[i]->positionY);
					fri->forceNotRebuild = !actionIsRebuild;
					fri->forceRebuild = actionIsRebuild;
					fri->playerId = controlledPlayer->playerId;
					fri->villagerUnitId = -1;
					fri->gameTime = controlledPlayer->ptrGlobalStruct ? controlledPlayer->ptrGlobalStruct->currentGameTime : 0;
				}
			}
		}
	}
	if (isMyUnit && this->chkAutoAttackMilitary && this->chkAutoAttackBuildings &&
		this->chkAutoAttackTowers && this->chkAutoAttackVillagers && this->chkAutoAttackWalls) {
		// Force create info object if not existing
		unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(this->unitId);
		assert(unitInfo != NULL); // Was just added if not already existing
		unitInfo->autoAttackPolicy.attackTowers = (this->chkAutoAttackTowers->currentState != 0);
		unitInfo->autoAttackPolicy.attackMilitary = (this->chkAutoAttackMilitary->currentState != 0);
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = (this->chkAutoAttackBuildings->currentState != 0);
		unitInfo->autoAttackPolicy.attackVillagers = (this->chkAutoAttackVillagers->currentState != 0);
		unitInfo->autoAttackPolicy.attackWalls = (this->chkAutoAttackWalls->currentState != 0);
		unitInfo->autoAttackPolicyIsSet = true;
		// Apply changes on all selected units
		AutoAttackPolicy flagsToApply = { true, true, true, true, true }; // All flags are relevant in popup.
		ROCKNROR::crInfo.ApplyAutoAttackPolicyToPlayerSelectedUnits(controlledPlayer, unitInfo->autoAttackPolicy, flagsToApply);
	}
}


}
}

