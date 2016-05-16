#include "../include/popups.h"

CustomPopup::CustomPopup() {
	this->SetCRCommand(NULL);
	this->ResetPointers();
	this->isClosed = false;
}

CustomPopup::CustomPopup(CustomRORCommand *crCommand) {
	this->SetCRCommand(crCommand);
	this->ResetPointers();
	this->isClosed = false;
}

void CustomPopup::ResetPointers() {
	this->nextPopup = NULL;
	this->btnCancel = NULL;
	this->btnOK = NULL;
	this->isClosed = false;
	this->popup = NULL;
	this->_ResetPointers(); // class-specific operations
}


void CustomPopup::SetCRCommand(CustomRORCommand *crCommand) {
	this->crCommand = crCommand;
	this->crInfo = NULL;
	if (crCommand) {
		this->crInfo = crCommand->crInfo;
	}
}

// Use it to list all UI objects (labels, buttons...) that are created(added) to popup content, so they are automatically freed when popup is closed.
void CustomPopup::AddObjectInContentList(ROR_STRUCTURES_10C::STRUCT_ANY_UI *obj) {
	this->crInfo->AddObjectInPopupContentList(obj);
}


// API to add UI components and add them automatically in "objects to free" list.
bool CustomPopup::AddLabel(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL **ptrObjToCreate, char *label,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	AOE_FONTS font) {
	bool res;
	res = AOE_AddLabel(parent, ptrObjToCreate, label, hPos, vPos, hSize, vSize, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}

bool CustomPopup::AddTextBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	bool readOnly, bool multiline, bool onlyNumbers, unsigned long int font) {
		bool res;
		res = AOE_AddTextBox(parent, ptrObjToCreate, initialText, maxTextLength, hPos, vPos, hSize, vSize, 
			readOnly, multiline, onlyNumbers, font);
		if (res && ptrObjToCreate) {
			this->AddObjectInContentList(*ptrObjToCreate);
		}
		return res;
}

bool CustomPopup::AddCheckBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize) {
	bool res;
	res = AOE_AddCheckBox(parent, ptrObjToCreate, hPos, vPos, hSize, vSize);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}

bool CustomPopup::AddButton(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate, char *caption,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId, AOE_FONTS font) {
	bool res;
	res = AOE_AddButton(parent, ptrObjToCreate, caption, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}

bool CustomPopup::AddButton(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId, AOE_FONTS font) {
	bool res;
	res = AOE_AddButton(parent, ptrObjToCreate, DLL_STRING_ID, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}



// Call this to open a new popup (=>this)
ROR_STRUCTURES_10C::STRUCT_ANY_UI *CustomPopup::OpenPopup(long int hSize, long int vSize, bool withCancelBtn) {
	if (!this->crInfo) { return NULL; }
	this->hSize = hSize;
	this->vSize = vSize;
	if (this->crInfo->HasOpenedCustomGamePopup()) {
		return NULL;
	}
	this->popup = this->crInfo->OpenCustomGamePopup(hSize, vSize, withCancelBtn);
	if (this->popup != NULL) {
		this->AddPopupContent();
	}
	return this->popup;
}


// Call this to close the popup. This will handle pre/post close events.
void CustomPopup::ClosePopup(bool isCancel) {
	if (!this->crInfo) { return; }
	if (this->IsClosed()) { return; }
	this->OnBeforeClose(isCancel);
	this->crInfo->CloseCustomGamePopup();
	this->OnAfterClose(isCancel);
	// NO because we may have opened a new one in post-actions ! // this->currentCustomGamePopup = CUSTOM_GAME_POPUP_TYPES::CGPT_NONE;
	this->isClosed = true;
}


// -------------------------------------------------------------------------------------
// END of base class
// -------------------------------------------------------------------------------------


InGameCustomRorOptionsPopup::InGameCustomRorOptionsPopup() {
	this->_ResetPointers();
}


void InGameCustomRorOptionsPopup::_ResetPointers() {
	this->openTechTreeInfo = false;
	this->btnTechTreeInfo = NULL;
	this->customOptionButtonVar = NULL;
	this->customOptionFreeTextAnswerVar = NULL;
	this->customOptionFreeTextLabelVar = NULL;
	this->customOptionFreeTextVar = NULL;
	this->customOptionGameSpeedLabelVar = NULL;
	this->customOptionGameSpeedTextVar = NULL;
	this->customOptionHeaderLabelVar = NULL;
	this->customOptionHumanPenaltyLabelVar = NULL;
	this->customOptionHumanPenaltyTextVar = NULL;
	this->chkAutoRebuildFarms = NULL;
	this->lblAutoRebuildFarms = NULL;
	this->edtAutoRebuildFarmsMaxFood = NULL;
	this->lblAutoRebuildFarmsMaxFood = NULL;
	this->edtAutoRebuildFarmsMinWood = NULL;
	this->lblAutoRebuildFarmsMinWood = NULL;
	this->edtAutoRebuildFarmsMaxFarms = NULL;
	this->lblAutoRebuildFarmsMaxFarms = NULL;
}

void InGameCustomRorOptionsPopup::_AddPopupContent() {
	if (this->IsClosed() || (this->popup == NULL)) { return; }

	char customOptionHumanPenaltyTextBuffer[12];
	char customOptionGameSpeedFactorTextBuffer[12];
	char maxFarmsTextBuffer[12];
	char maxFoodTextBuffer[12];
	char minWoodTextBuffer[12];
	sprintf_s(customOptionHumanPenaltyTextBuffer, "%ld", this->crInfo->configInfo.dislike_humanPlayer);
	sprintf_s(customOptionGameSpeedFactorTextBuffer, "%d", (int)(this->crInfo->configInfo.improvedGameSpeedFactor * (float)100));
	sprintf_s(maxFarmsTextBuffer, "%d", this->crInfo->configInfo.autoRebuildFarms_maxFarms);
	sprintf_s(maxFoodTextBuffer, "%d", this->crInfo->configInfo.autoRebuildFarms_maxFood);
	sprintf_s(minWoodTextBuffer, "%d", this->crInfo->configInfo.autoRebuildFarms_minWood);


	//this->customGamePopupButtonVar
	this->AddLabel(this->popup, &this->customOptionHeaderLabelVar, "CustomROR Options", 0xD0, 0x0A, 0x100, 0x1E, AOE_FONTS::AOE_FONT_BIG_LABEL); // Title
	
	// human dislike penalty
	this->AddTextBox(this->popup, &this->customOptionHumanPenaltyTextVar, customOptionHumanPenaltyTextBuffer, 2, 0x120, 0x34, 0x30, 0x16, false, false);
	this->AddLabel(this->popup, &this->customOptionHumanPenaltyLabelVar, txtHumanPenalty, 0x10, 0x30, 0x100, 0x1E);
	// Game speeds config
	this->AddTextBox(this->popup, &this->customOptionGameSpeedTextVar, customOptionGameSpeedFactorTextBuffer, 4, 0x120, 0x4C, 0x30, 0x16, false, false);
	this->AddLabel(this->popup, &this->customOptionGameSpeedLabelVar, txtGameSpeedFactor, 0x10, 0x48, 0x100, 0x1E);
	// Farms autorebuild
	this->AddLabel(this->popup, &this->lblAutoRebuildFarms, txtAutoRebuildFarms, 0x10, 0x64, 0x100, 0x1E);
	this->AddCheckBox(this->popup, &this->chkAutoRebuildFarms, 0x120, 0x64, 0x1E, 0x1E);
	AOE_CheckBox_SetChecked(this->chkAutoRebuildFarms, this->crInfo->configInfo.enableAutoRebuildFarms);
	this->AddLabel(this->popup, &this->lblAutoRebuildFarmsMaxFood, txtAutoRebuildFarmsMaxFood, 0x10, 0x80, 0x100, 0x1E);
	this->AddLabel(this->popup, &this->lblAutoRebuildFarmsMinWood, txtAutoRebuildFarmsMinWood, 0x10, 0x98, 0x100, 0x1E);
	this->AddLabel(this->popup, &this->lblAutoRebuildFarmsMaxFarms, txtAutoRebuildFarmsMaxNumber, 0x10, 0xB0, 0x100, 0x1E);
	this->AddTextBox(this->popup, &this->edtAutoRebuildFarmsMaxFood, maxFoodTextBuffer, 5, 0x120, 0x84, 0x34, 0x16, false, false);
	this->AddTextBox(this->popup, &this->edtAutoRebuildFarmsMinWood, minWoodTextBuffer, 5, 0x120, 0x9C, 0x34, 0x16, false, false);
	this->AddTextBox(this->popup, &this->edtAutoRebuildFarmsMaxFarms, maxFarmsTextBuffer, 2, 0x120, 0xB4, 0x34, 0x16, false, false);
	
	// Free text zone
	this->AddLabel(this->popup, &this->customOptionFreeTextLabelVar, "Other commands (enter to validate)", 0x10, 0x120, 0x100, 0x1E);
	this->AddTextBox(this->popup, &this->customOptionFreeTextVar, "", 100, 0x100, 0x120, 0xB0, 0x16, false, false);
	this->AddTextBox(this->popup, &this->customOptionFreeTextAnswerVar, "", 100, 0x1C0, 0x120, 0x80, 0x16, true, false);
	
	this->AddButton(this->popup, &this->btnTechTreeInfo, "Tech tree info", 0x170, 0x34, 0xAC, 0x1E);
}

// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameCustomRorOptionsPopup::OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnTechTreeInfo) {
		this->openTechTreeInfo = true;
		this->ClosePopup(false);
		return true;
	}
	return false;
}

void InGameCustomRorOptionsPopup::OnBeforeClose(bool isCancel) {
	char *typedText;
	if (this->customOptionHumanPenaltyTextVar) {
		typedText = this->customOptionHumanPenaltyTextVar->pTypedText;
		this->crInfo->configInfo.dislike_humanPlayer = atoi(typedText); // does not raise. Returns 0 if invalid.
	}

	if (this->customOptionGameSpeedTextVar) {
		typedText = this->customOptionGameSpeedTextVar->pTypedText;
		float f = (float)(atof(typedText) / 100); // does not raise. Returns 0 if invalid.
		if (f > 1) {
			this->crInfo->configInfo.improvedGameSpeedFactor = f;
		}
	}

	if (this->customOptionFreeTextAnswerVar) {
		unsigned long int h = this->customOptionFreeTextAnswerVar->hWnd;
		ShowWindow((HWND)h, SW_HIDE);
	}

	if (this->chkAutoRebuildFarms) {
		this->crInfo->configInfo.enableAutoRebuildFarms = (this->chkAutoRebuildFarms->checked != 0);
	}
	if (this->edtAutoRebuildFarmsMaxFarms) {
		typedText = this->edtAutoRebuildFarmsMaxFarms->pTypedText;
		this->crInfo->configInfo.autoRebuildFarms_maxFarms = atoi(typedText); // does not raise. Returns 0 if invalid.
	}
	if (this->edtAutoRebuildFarmsMaxFood) {
		typedText = this->edtAutoRebuildFarmsMaxFood->pTypedText;
		this->crInfo->configInfo.autoRebuildFarms_maxFood = atoi(typedText); // does not raise. Returns 0 if invalid.
	}
	if (this->edtAutoRebuildFarmsMinWood) {
		typedText = this->edtAutoRebuildFarmsMinWood->pTypedText;
		this->crInfo->configInfo.autoRebuildFarms_minWood = atoi(typedText); // does not raise. Returns 0 if invalid.
	}
}


void InGameCustomRorOptionsPopup::OnAfterClose(bool isCancel) {
	if (this->openTechTreeInfo) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
		assert(humanPlayer && humanPlayer->IsCheckSumValid());

		SimpleEditTextPopup *tmpNextPopup = new SimpleEditTextPopup();
		tmpNextPopup->SetCRCommand(this->crCommand);
		tmpNextPopup->OpenPopup(700, 580, false);
		tmpNextPopup->AddPopupContent("Technology tree that can still be researched", 
			GetRemainingTechTreeText(humanPlayer).c_str(), 10000, NULL, false);
		this->nextPopup = tmpNextPopup;
	}
}


// Specific method to use instead of OpenPopup (special treatments for ingame customROR menu).
// previousPopup should be game menu popup object.
ROR_STRUCTURES_10C::STRUCT_ANY_UI *InGameCustomRorOptionsPopup::CloseMenuAndOpenPopup(ROR_STRUCTURES_10C::STRUCT_ANY_UI *previousPopup) {
	assert(previousPopup != NULL);
	if (!previousPopup) { return NULL; }
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *MainGameUIObj = previousPopup->ptrParentObject;
	this->popup = AOE_CreateCustomOptionsPopupFromMenu(MainGameUIObj);
	if ((this->popup != NULL) && (!this->IsClosed())) {
		// WARNING: here we are adding manually the "standard"/"common" OK button.
		// Normally, this button is created by our popups API (and freed automatically).
		// So we MUST NOT call AddObjectInContentList on this->customOptionButtonVar ! Or the game will crash (trying to free it twice)
		AOE_AddButton(this->popup, &this->customOptionButtonVar, LANG_ID_OK, 0xD8, 0x160, 0xAC, 0x1E); // OK button

		this->_AddPopupContent();
	}
	return this->popup;
}



EditorEditUnitInfoPopup::EditorEditUnitInfoPopup() {
	this->_ResetPointers();
}

void EditorEditUnitInfoPopup::_ResetPointers() {
	this->unit = NULL;
	this->edtStatus = NULL;
	this->chkbox_s0 = NULL;
	this->chkbox_s2 = NULL;
	this->chkbox_s4 = NULL;
	this->lbl_s0 = NULL;
	this->lbl_s2 = NULL;
	this->lbl_s4 = NULL;
}


void EditorEditUnitInfoPopup::_AddPopupContent() {
	if (this->IsClosed() || (this->popup == NULL)) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(global->humanPlayerId);
	if (!player || (player->selectedUnitCount <= 0)) { return; }
	ROR_STRUCTURES_10C::STRUCT_UNIT **unitArray = this->crInfo->GetRelevantSelectedUnitsPointer(player);
	if (!unitArray) { return; }
	ROR_STRUCTURES_10C::STRUCT_UNIT *unit = unitArray[0];
	if (!unit || (!unit->IsCheckSumValid())) { return; }
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
	if (!unitDef) { return; }
	this->unit = unit;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblUnusedPtr = NULL;
	this->AddLabel(this->popup, &lblUnusedPtr, "Edit a unit", 200, 10, 120, 24, AOE_FONTS::AOE_FONT_BIG_LABEL);
	char *name = "";
	short int DATID = -1;
	if (unitDef->ptrUnitName) {
		name = unitDef->ptrUnitName;
		DATID = unitDef->DAT_ID1;
	}
	char buffer[200];
	sprintf_s(buffer, "Unit ID = %ld - name=%s (DATID=%d). Pos(x,y)=%.1f, %.1f", unit->unitInstanceId, name, DATID, unit->positionX, unit->positionY);
	this->AddLabel(this->popup, &lblUnusedPtr, buffer, 50, 50, 400, 24, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	
	char initial[] = "Initial";
	char to_build[] = "Not built";
	char gatherable[] = "Gatherable";
	char dying[] = "Dying";
	char *labelText0 = initial;
	char *labelText4 = dying;
	bool disableInitial = true;

	if (unit->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) {
		labelText0 = to_build;
		disableInitial = false;
	}
	// Warning: do NOT use unit->unitType, it is not correctly valued in editor !!!
	if ((unitDef->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TREE) ||
		(unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal) ||
		(unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPredatorAnimal)
		) {
		labelText4 = gatherable;
	}

	this->AddLabel(this->popup, &this->lbl_s0, labelText0, 160, 80, 60, 24, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(this->popup, &this->lbl_s2, "Normal", 230, 80, 60, 24, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(this->popup, &this->lbl_s4, labelText4, 300, 80, 70, 24, AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(this->popup, &this->chkbox_s0, 170, 110, 24, 24);
	this->AddCheckBox(this->popup, &this->chkbox_s2, 240, 110, 24, 24);
	this->AddCheckBox(this->popup, &this->chkbox_s4, 310, 110, 24, 24);
	// Check current status' box
	switch (unit->unitStatus) {
	case 0:
		AOE_CheckBox_SetChecked(this->chkbox_s0, true);
		break;
	case 2:
		AOE_CheckBox_SetChecked(this->chkbox_s2, true);
		break;
	case 4:
		AOE_CheckBox_SetChecked(this->chkbox_s4, true);
		break;
	}
	if (disableInitial) {
		this->chkbox_s0->readOnly = 1;
		AOE_RefreshUIObject(this->chkbox_s0);
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditorEditUnitInfoPopup::OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) {
	if (sender->checked) {
		if (sender == this->chkbox_s0) {
			AOE_CheckBox_SetChecked(this->chkbox_s2, false);
			AOE_CheckBox_SetChecked(this->chkbox_s4, false);
		}
		if (sender == this->chkbox_s2) {
			AOE_CheckBox_SetChecked(this->chkbox_s0, false);
			AOE_CheckBox_SetChecked(this->chkbox_s4, false);
		}
		if (sender == this->chkbox_s4) {
			AOE_CheckBox_SetChecked(this->chkbox_s2, false);
			AOE_CheckBox_SetChecked(this->chkbox_s0, false);
		}
	}
	return false;
}


void EditorEditUnitInfoPopup::OnBeforeClose(bool isCancel) {
	if (isCancel) { return; }
	// Note: setting shortcutNumber can show the unit's status, but only works for player #1. See 4A78D9: disable jmp to allow always displaying shortcut
	if (this->chkbox_s0 && this->chkbox_s2 && this->chkbox_s4) {
		if (this->chkbox_s0 && (this->chkbox_s0->checked)) {
			this->unit->unitStatus = AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_NOT_BUILT;
			this->unit->shortcutNumber = 1;
		}
		if (this->chkbox_s2 && (this->chkbox_s2->checked)) {
			this->unit->unitStatus = AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_READY;
			this->unit->shortcutNumber = 0;
		}
		if (this->chkbox_s4 && (this->chkbox_s4->checked)) {
			this->unit->unitStatus = AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_DYING_ANIMATION;
			this->unit->shortcutNumber = 3;
		}
	}
	this->ResetPointers();
}


void EditorEditUnitInfoPopup::OnAfterClose(bool isCancel) {
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	// When closing "edit unit" popup, restore unit selection mouse cursor (more practical).
	settings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
}


// In-game unit properties popup

InGameUnitPropertiesPopup::InGameUnitPropertiesPopup() {
	this->_ResetPointers();
}

void InGameUnitPropertiesPopup::_ResetPointers() {
	this->unitId = -1;
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
	this->chkAutoAttackUnits = NULL;
}

// Create popup content for unit properties
void InGameUnitPropertiesPopup::AddPopupContent(long int unitId) {
	if (this->IsClosed() || (this->popup == NULL) || (unitId < 0)) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	ROR_STRUCTURES_10C::STRUCT_UNIT *unit = GetUnitStruct(unitId);
	ROR_STRUCTURES_10C::STRUCT_PLAYER *unitPlayer = unit->ptrStructPlayer;
	ROR_STRUCTURES_10C::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDefBase = unit->GetUnitDefBase();
	if (!unit || !unit->IsCheckSumValid() || !unitPlayer || !unitPlayer->IsCheckSumValid() ||
		!controlledPlayer || !controlledPlayer->IsCheckSumValid() ||
		!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) {
		return;
	}
	this->unitId = unitId; // Store it once we checked it is valid.
	bool isMyUnit = (controlledPlayer == unitPlayer);
	bool isTower = IsTower(unitDefBase);
	bool isMilitary = isTower || IsNonTowerMilitaryUnit(unitDefBase->unitAIType);
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50 *unitDef50 = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50 *)unitDefBase;
	bool isRangedUnit = (unitDef50->IsCheckSumValidForAUnitClass() && (unitDef50->maxRange > 0));

	const long int btnSize = 0xAC;
	const long int lblTitleHSize = 160;
	const long int lblMainInfoHSize = 400;
	std::string mainInfos = "Unit ";
#ifdef _DEBUG
	mainInfos += "id=";
	mainInfos += std::to_string(unitId);
	mainInfos += " ";
#endif
	mainInfos += "\"";
	mainInfos += unitDefBase->ptrUnitName;
	mainInfos += "\" (";
	mainInfos += unitPlayer->playerName_length16max;
	mainInfos += "). Shortcut=";
	// Shortcut / group info
	if ((unit->shortcutNumber == 0) || (unit->shortcutNumber > 10)) {
		mainInfos += "none";
	} else {
		char displayNumber = unit->shortcutNumber;
		if (displayNumber == -11) {
			displayNumber = 20;
		}
		if ((displayNumber < 0) && (displayNumber >= -11)) {
			displayNumber = displayNumber + 21; // for custom shortcut numbers (numpad, stored values are -11 to -2)
		}
		if (displayNumber > 0) {
			mainInfos += std::to_string(displayNumber);
		} else {
			mainInfos += "invalid";
		}
	}
	if (unit->shortcutNumber > 10) {
		mainInfos += " [part of a group]";
	}
	// Automove infos
	std::string autoMoveInfo = "";
	UnitCustomInfo *unitInfo = this->crInfo->myGameObjects.FindUnitCustomInfo(unitId);
	if (unitInfo && (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		if (unitInfo->spawnTargetUnitId >= 0) {
			ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *targetUnitBase = (ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *)GetUnitStruct(unitInfo->spawnTargetUnitId);
			char *targetName = NULL;
			if (targetUnitBase && targetUnitBase->IsCheckSumValidForAUnitClass() &&
				targetUnitBase->ptrStructDefUnit && targetUnitBase->ptrStructDefUnit->IsCheckSumValid()) {
				targetName = targetUnitBase->ptrStructDefUnit->ptrUnitName;
			}
			autoMoveInfo += "[Building] Child units auto-target=";
			autoMoveInfo += std::to_string(unitInfo->spawnTargetUnitId);
			if (targetName) {
				autoMoveInfo += " (";
				autoMoveInfo += targetName;
				autoMoveInfo += ")";
			}
		}
		if ((unitInfo->spawnUnitMoveToPosX >= 0) && (unitInfo->spawnUnitMoveToPosY >= 0)) {
			char buf[10];
			autoMoveInfo += "[Building] Child units auto-move to (";
			sprintf_s(buf, "%.2f", unitInfo->spawnUnitMoveToPosX);
			autoMoveInfo += buf;
			autoMoveInfo += ", ";
			sprintf_s(buf, "%.2f", unitInfo->spawnUnitMoveToPosY);
			autoMoveInfo += buf;
			autoMoveInfo += ")";
		}
	}
	// Farm auto-rebuild
	std::string farmInfo = "Auto rebuild this farm when depleted:";
	FarmRebuildInfo *farmBldInfo = this->crInfo->myGameObjects.FindFarmRebuildInfo(unit->positionX, unit->positionY);

	// Building : future potential techs/units
	std::string buildingTechAndUnitInfo = "";
	long int techToShowCount = 0;
	if (unitPlayer && isMyUnit && (unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		char nameBuffer[50];
		short int researchCount = unitPlayer->ptrResearchesStruct->researchCount;
		ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_STATUS *rs = unitPlayer->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
		for (int rid = 0; rid < researchCount; rid++) {
			if (rs[rid].currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
				if (unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].researchLocation == unitDefBase->DAT_ID1) {
					techToShowCount++;
					if (techToShowCount > 0) { buildingTechAndUnitInfo += "\n"; }
					buildingTechAndUnitInfo += "techId ";
					buildingTechAndUnitInfo += std::to_string(unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].technologyId);
					buildingTechAndUnitInfo += " = ";
					*nameBuffer = 0; // Reset string
					GetLanguageDllText(unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].languageDLLName, nameBuffer, sizeof(nameBuffer) - 1,
						unitPlayer->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].researchName);
					buildingTechAndUnitInfo += nameBuffer;
				}
			}
		}
	}

	this->AddLabel(popup, &this->lblTitle, "Unit properties", (hSize - lblTitleHSize) / 2, 10, lblTitleHSize, 30, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddLabel(popup, &this->lblMainInfos, (char*)mainInfos.c_str(), (hSize - lblMainInfoHSize) / 2, 60, lblMainInfoHSize, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	// Note: auto-move and farm info have same Y position because they can't be displayed simultaneously.
	if (!autoMoveInfo.empty()) {
		this->AddLabel(popup, &this->lblChildUnitsAutoMove, (char*)autoMoveInfo.c_str(), 30, 100, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddButton(popup, &this->btnResetAutoMove, "Disable auto-move", 340, 100, btnSize, 22, 0);
	}
	// Auto rebuild farm
	if (isMyUnit && (unitDefBase->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_FARM)) {
		this->AddLabel(popup, &this->lblFarmAutoRebuild, (char*)farmInfo.c_str(), 30, 100, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		
		ROR_STRUCTURES_10C::STRUCT_UI_LABEL *unusedLabel;
		this->AddCheckBox(popup, &this->chkRebuildFarmNone, 330, 100 - 4, 24, 24);
		this->AddCheckBox(popup, &this->chkForceRebuildFarm, 430, 100 - 4, 24, 24);
		this->AddCheckBox(popup, &this->chkForceNotRebuildFarm, 530, 100 - 4, 24, 24);
		this->AddLabel(popup, &unusedLabel, "default", 270, 100, 50, 20);
		this->AddLabel(popup, &unusedLabel, "Always", 370, 100, 50, 20);
		this->AddLabel(popup, &unusedLabel, "Never", 470, 100, 50, 20);
		
		FarmRebuildInfo *fri = this->crInfo->myGameObjects.FindFarmRebuildInfo(unit->positionX, unit->positionY);
		if (!fri) {
			AOE_CheckBox_SetChecked(this->chkRebuildFarmNone, true);
		} else {
			if (fri->forceNotRebuild) {
				AOE_CheckBox_SetChecked(this->chkForceNotRebuildFarm, true);
			} else {
				if (fri->forceRebuild) {
					AOE_CheckBox_SetChecked(this->chkForceRebuildFarm, true);
				} else {
					AOE_CheckBox_SetChecked(this->chkRebuildFarmNone, true);
				}
			}
		}
	}

#pragma message("TODO more features in unit popup")
	// Military : guard location ?
	if (isMilitary) {
		bool canHurtOtherUnits = (unitDef50->blastLevel != CST_BL_DAMAGE_TARGET_ONLY) && (unitDef50->blastRadius > 0);
		this->AddLabel(popup, &this->lblAutoAttackUnits, "Auto attack units", 30, 100, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(popup, &this->chkAutoAttackUnits, 330, 100 - 4, 24, 24);
		AUTO_ATTACK_POLICIES aap = AAP_NOT_SET;
		if (canHurtOtherUnits) {
			if (isRangedUnit) {
				aap = this->crInfo->configInfo.autoAttackOptionForBlastRangedUnits;
			} else {
				aap = this->crInfo->configInfo.autoAttackOptionForBlastMeleeUnits;
			}
		}
		// If there is a config at unit level, take it instead of global parameter.
		if (unitInfo && (unitInfo->autoAttackPolicy != AAP_NOT_SET)) {
			aap = unitInfo->autoAttackPolicy;
		}
		if ((aap == AAP_DEFAULT) || (aap == AAP_NOT_SET)) {
			AOE_CheckBox_SetChecked(this->chkAutoAttackUnits, true);
		}
	}

	// Building : future potential techs/units
	if (!buildingTechAndUnitInfo.empty() && (techToShowCount > 0)) {
		this->AddLabel(popup, &this->lblBuildingTechsMessage, "Future technologies, not available yet:", 30, 120, 300, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		
		this->AddTextBox(popup, &this->edtBuildingTechs, buildingTechAndUnitInfo.c_str(), 0, 30, 140, 450, 20 + techToShowCount * 14, true, true);
	}
}

// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameUnitPropertiesPopup::OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnResetAutoMove) {
		UnitCustomInfo *u = this->crInfo->myGameObjects.FindUnitCustomInfo(this->unitId);
		if (u) {
			u->ResetSpawnAutoTargetInfo();
			this->crInfo->myGameObjects.RemoveUnitCustomInfoIfEmpty(this->unitId);
		}
		// Auto-move has been reset, hide label and button.
		AOE_ShowUIObject(this->btnResetAutoMove, false);
		AOE_ShowUIObject(this->lblChildUnitsAutoMove, false);
		return true;
	}
	if (sender == this->chkForceNotRebuildFarm) {
		AOE_CheckBox_SetChecked(this->chkRebuildFarmNone, false);
		AOE_CheckBox_SetChecked(this->chkForceRebuildFarm, false);
	}
	if (sender == this->chkForceRebuildFarm) {
		AOE_CheckBox_SetChecked(this->chkRebuildFarmNone, false);
		AOE_CheckBox_SetChecked(this->chkForceNotRebuildFarm, false);
	}
	if (sender == this->chkRebuildFarmNone) {
		AOE_CheckBox_SetChecked(this->chkForceRebuildFarm, false);
		AOE_CheckBox_SetChecked(this->chkForceNotRebuildFarm, false);
	}
	if (sender == this->chkAutoAttackUnits) {
		// Uncheck others
	}
	return false; // Not one of our buttons; let ROR code be executed normally
}

void InGameUnitPropertiesPopup::OnBeforeClose(bool isCancel) {
	if (isCancel) {
		return;
	}
	ROR_STRUCTURES_10C::STRUCT_UNIT *unit = GetUnitStruct(this->unitId); // Might be NULL
	ROR_STRUCTURES_10C::STRUCT_PLAYER *unitPlayer = NULL;
	if (unit && unit->IsCheckSumValid()) {
		unitPlayer = unit->ptrStructPlayer;
	}
	ROR_STRUCTURES_10C::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	bool isMyUnit = (controlledPlayer == unitPlayer);
	UnitCustomInfo *unitInfo = this->crInfo->myGameObjects.FindUnitCustomInfo(this->unitId);

	float posX = -1;
	float posY = -1;
	if (unit && unit->IsCheckSumValid()) {
		posX = unit->positionX;
		posY = unit->positionY;
	}
	if (isMyUnit && this->chkRebuildFarmNone && this->chkRebuildFarmNone->checked) {
		this->crInfo->myGameObjects.RemoveFarmRebuildInfo(posX, posY);
	}
	if (isMyUnit && this->chkForceNotRebuildFarm && this->chkForceNotRebuildFarm->checked) {
		FarmRebuildInfo *fri = this->crInfo->myGameObjects.FindOrAddFarmRebuildInfo(posX, posY);
		fri->forceNotRebuild = true;
		fri->forceRebuild = false;
		fri->playerId = controlledPlayer->playerId;
		fri->villagerUnitId = -1;
		fri->gameTime = controlledPlayer->ptrGlobalStruct ? controlledPlayer->ptrGlobalStruct->currentGameTime : 0;
	}
	if (isMyUnit && this->chkForceRebuildFarm && this->chkForceRebuildFarm->checked) {
		FarmRebuildInfo *fri = this->crInfo->myGameObjects.FindOrAddFarmRebuildInfo(posX, posY);
		fri->forceNotRebuild = false;
		fri->forceRebuild = true;
		fri->playerId = controlledPlayer->playerId;
		fri->villagerUnitId = -1;
		fri->gameTime = controlledPlayer->ptrGlobalStruct ? controlledPlayer->ptrGlobalStruct->currentGameTime : 0;
	}
	if (isMyUnit && this->chkAutoAttackUnits) {
		bool checked = (bool)this->chkAutoAttackUnits->checked;
		// Force create info object if not existing
		unitInfo = this->crInfo->myGameObjects.FindOrAddUnitCustomInfo(this->unitId);
		assert(unitInfo != NULL); // Was just added if not already existing
		if (checked) {
			unitInfo->autoAttackPolicy = AAP_DEFAULT;
		} else {
			unitInfo->autoAttackPolicy = AAP_IGNORE_ALL;
		}
	}
}

void InGameUnitPropertiesPopup::OnAfterClose(bool isCancel) {}


// Scenario editor main custom popup

EditorScenarioInfoPopup::EditorScenarioInfoPopup() {
	this->_ResetPointers();
	this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
}

void EditorScenarioInfoPopup::_ResetPointers() {
	this->btnAI = NULL;
	this->btnPER = NULL;
	this->btnTerrainEdit = NULL;
	this->btnTriggers = NULL;
	this->btnVictoryCondition = NULL;
	this->edtPlayerId = NULL;
	this->lblTitle = NULL;
	this->chkAllowUnitOverlapping = NULL;
	this->chkDisableHillModeCheck = NULL;
	this->chkDisableTerrainRestrictions = NULL;
	this->chkLengthenCombatMode = NULL;
	this->varToUpdate_allowUnitOverlapping = NULL;
	this->varToUpdate_disableHillModeChecks = NULL;
	this->varToUpdate_disableTerrainRestrictionChecks = NULL;
	this->varToUpdate_lengthenCombatMode = NULL;
}

// Provide the pointer to the boolean that will be updated according to user choice.
void EditorScenarioInfoPopup::SetVarToUpdate_allowUnitOverlapping(bool *varToUpdate) {
	this->varToUpdate_allowUnitOverlapping = varToUpdate;
	if (this->chkAllowUnitOverlapping && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkAllowUnitOverlapping, *varToUpdate);
	}
}
void EditorScenarioInfoPopup::SetVarToUpdate_disableHillModeChecks(bool *varToUpdate) {
	this->varToUpdate_disableHillModeChecks = varToUpdate;
	if (this->chkDisableHillModeCheck && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkDisableHillModeCheck, *varToUpdate);
	}
}
void EditorScenarioInfoPopup::SetVarToUpdate_disableTerrainRestrictionChecks(bool *varToUpdate) {
	this->varToUpdate_disableTerrainRestrictionChecks = varToUpdate;
	if (this->chkDisableTerrainRestrictions && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkDisableTerrainRestrictions, *varToUpdate);
	}
}
void EditorScenarioInfoPopup::SetVarToUpdate_lengthenCombatMode(long int *varToUpdate) {
	this->varToUpdate_lengthenCombatMode = varToUpdate;
	if (this->chkLengthenCombatMode && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkLengthenCombatMode, (*varToUpdate != 0));
	}
}

void EditorScenarioInfoPopup::_AddPopupContent() {
	const long int btnSize = 0xAC;
	long int btnTotalSpace = hSize - (btnSize * 2);
	long int btnSimpleSpace = btnTotalSpace / 3; // btn count + 1 more space on the right
	long int btnhPos1 = btnSimpleSpace; // x position for buttons
	long int btnhPos2 = btnSize + btnSimpleSpace * 2; // x position for buttons
	this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
	this->playerId = -1;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *unused;
	this->AddLabel(popup, &this->lblTitle, "Custom editor menu", btnhPos1 - 5, 10, 190, 30, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddLabel(popup, &unused, "PlayerId :", btnhPos1, 40, 0x60, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(popup, &this->edtPlayerId, "2", 1, btnhPos1 + btnSize - 0x22, 42, 0x22, 20, false, false, true);
	this->AddButton(popup, &this->btnAI, LANG_ID_STRATEGY, btnhPos1, 80, btnSize, 30, 0);
	this->AddButton(popup, &this->btnPER, LANG_ID_PERSONALITY, btnhPos1, 120, btnSize, 30, 0);
	this->AddButton(popup, &this->btnTriggers, "Triggers", btnhPos1, 160, btnSize, 30, 0);
	this->AddButton(popup, &this->btnTerrainEdit, "Terrain edit", btnhPos1, 200, btnSize, 30, 0);
	this->AddButton(popup, &this->btnVictoryCondition, "Victory conditions", btnhPos2, 80, btnSize, 30, 0);
	long int chkSize = 30;
	long int hSpace = 15;
	long int lblhSize = 160;
	long int lblhPos = hSpace;
	long int lblhPosCol2 = (hSize - lblhSize - chkSize - hSpace * 2); // x position for checkboxes col2
	this->AddLabel(popup, &unused, "Allow unit overlapping", lblhPos, 240, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkAllowUnitOverlapping, lblhPos + lblhSize + hSpace, 240, chkSize, chkSize);
	this->AddLabel(popup, &unused, "Allow buildings on elevation", lblhPos, 280, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkDisableHillModeCheck, lblhPos + lblhSize + hSpace, 280, chkSize, chkSize);
	this->AddLabel(popup, &unused, "Disable terrain restrictions", lblhPos, 320, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkDisableTerrainRestrictions, lblhPos + lblhSize + hSpace, 320, chkSize, chkSize);
	// Checkboxes column 2
	this->AddLabel(popup, &unused, "Lengthen combat mode", lblhPosCol2, 240, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkLengthenCombatMode, lblhPosCol2 + lblhSize + hSpace, 240, chkSize, chkSize);
}


bool EditorScenarioInfoPopup::OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) {
	if (!sender || !sender->IsCheckSumValid()) { return false; }
	bool doCloseCustomPopup = false;
	if (!GetGameGlobalStructPtr()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scInfo = GetGameGlobalStructPtr()->scenarioInformation;
	assert(this->edtPlayerId != NULL);

	if (this->edtPlayerId == NULL) { return false; }
	if (sender->ptrParentObject) {
		AOE_SetFocus(sender->ptrParentObject, sender); // To validate input text.
	}
	if (this->edtPlayerId->pTypedText != NULL) {
		this->playerId = (this->edtPlayerId->pTypedText[0] - '0');
		if ((this->playerId > 8) || (this->playerId < 0)) {
			this->playerId = -1;
			return false; // do not close popup
		}
	}
	if (sender == this->btnAI) {
		if (scInfo->strategyFileSize[this->playerId - 1] <= 0) { return false; }
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_AI;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnPER) {
		if (scInfo->personalityFileSize[this->playerId - 1] <= 0) { return false; }
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_PER;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnTriggers) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_TRIGGER;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnTerrainEdit) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_TERRAIN_EDIT;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnVictoryCondition) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_VICTORY_CONDITION;
		doCloseCustomPopup = true;
	}
	if (sender == this->chkAllowUnitOverlapping) {
		if (this->varToUpdate_allowUnitOverlapping) {
			*this->varToUpdate_allowUnitOverlapping = this->chkAllowUnitOverlapping->IsChecked();
		}
		doCloseCustomPopup = false;
	}
	if (sender == this->chkDisableHillModeCheck) {
		if (this->varToUpdate_disableHillModeChecks) {
			*this->varToUpdate_disableHillModeChecks = this->chkDisableHillModeCheck->IsChecked();
		}
		doCloseCustomPopup = false;
	}
	if (sender == this->chkDisableTerrainRestrictions) {
		if (this->varToUpdate_disableTerrainRestrictionChecks) {
			*this->varToUpdate_disableTerrainRestrictionChecks = this->chkDisableTerrainRestrictions->IsChecked();
		}
		doCloseCustomPopup = false;
	}
	if (sender == this->chkLengthenCombatMode) {
		if (this->varToUpdate_lengthenCombatMode) {
			*this->varToUpdate_lengthenCombatMode = this->chkLengthenCombatMode->IsChecked();
		}
		doCloseCustomPopup = false;
	}

	if (doCloseCustomPopup) {
		this->ClosePopup(false);
		return true;
	}
	return false;
}


void EditorScenarioInfoPopup::OnAfterClose(bool isCancel) {
	if (isCancel) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid() || (settings->ptrGlobalStruct == NULL)) { return; }
	ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *sc_info = settings->ptrGlobalStruct->scenarioInformation;
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return; }
	switch (this->popupToOpen) {
	case SC_INFO_POPUP_TO_OPEN::PTO_TRIGGER:
		if (!this->nextPopup) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->SetCRCommand(this->crCommand);
			nextPopup->OpenPopup(700, 580, true);
			nextPopup->AddPopupContent("Edit triggers", GetTriggerDataPtr(sc_info), 0, NULL, false);
			nextPopup->isForTriggers = true;
			this->nextPopup = nextPopup;
		}
		break;
	case SC_INFO_POPUP_TO_OPEN::PTO_AI: // AI
		if ((this->playerId < 1) || (this->playerId > 8)) { return; }
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->scenarioInformation &&
			(settings->ptrGlobalStruct->scenarioInformation->strategyFileSize[this->playerId - 1] > 0)) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->SetCRCommand(this->crCommand);
			nextPopup->OpenPopup(700, 580, false);
			nextPopup->AddPopupContent("Get Strategy (read only)", settings->ptrGlobalStruct->scenarioInformation->strategyFileContent[this->playerId - 1],
				settings->ptrGlobalStruct->scenarioInformation->strategyFileSize[this->playerId - 1], NULL, false);
			this->nextPopup = nextPopup;
		}
		break;
	case SC_INFO_POPUP_TO_OPEN::PTO_PER: // PER
		if ((this->playerId < 1) || (this->playerId > 8)) { return; }
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->scenarioInformation &&
			(settings->ptrGlobalStruct->scenarioInformation->personalityFileSize[this->playerId - 1] > 0)) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->SetCRCommand(this->crCommand);
			nextPopup->OpenPopup(700, 580, false);
			nextPopup->AddPopupContent("Get Personality (read only)", settings->ptrGlobalStruct->scenarioInformation->personalityFileContent[this->playerId - 1],
				settings->ptrGlobalStruct->scenarioInformation->personalityFileSize[this->playerId - 1], NULL, false);
			this->nextPopup = nextPopup;
		}
		break;
	case SC_INFO_POPUP_TO_OPEN::PTO_TERRAIN_EDIT: // terrain edit
	{
		EditTerrainPopup *nextPopup = new EditTerrainPopup();
		nextPopup->SetCRCommand(this->crCommand);
		nextPopup->OpenPopup(400, 300, true);
		this->nextPopup = nextPopup;
		break;
	}
	case SC_INFO_POPUP_TO_OPEN::PTO_VICTORY_CONDITION:
	{
		InputBox_int<long int> *nextPopup = new InputBox_int<long int>();
		nextPopup->SetCRCommand(this->crCommand);
		nextPopup->OpenPopup(450, 200, true);
		nextPopup->AddPopupContent("Gold victory condition", 
			"If you want to set a victory condition on gold amount, type a non-zero value.\nYou need to select \"custom\" in general victory conditions tab.",
			settings->ptrGlobalStruct->scenarioInformation->generalVictory_goldAmount, 0, 999999, &settings->ptrGlobalStruct->scenarioInformation->generalVictory_goldAmount, false);
		this->nextPopup = nextPopup;
		break;
	}
	default:
		break;
	}
}


SimpleEditTextPopup::SimpleEditTextPopup() {
	this->_ResetPointers();
}


void SimpleEditTextPopup::_ResetPointers() {
	this->isForTriggers = false;
	this->edtText = NULL;
	this->lblTitle = NULL;
	this->bufferToWrite = NULL;
}


// This class needs parameters to create content. Call this after calling OpenPopup().
void SimpleEditTextPopup::AddPopupContent(char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly) {
	this->bufferToWrite = outputBuffer;
	if (maxLength < 1) {
		maxLength = 0x8000 - 2;
	}
	this->maxLength = maxLength;
	int labelSizeX = strlen(title) * 12;
	int labelPosX = ((this->hSize - labelSizeX) / 2) + 1;
	if (labelPosX < 1) { labelPosX = 1; }
	this->AddLabel(popup, &this->lblTitle, title, labelPosX, 10, labelSizeX, 24, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddTextBox(popup, &this->edtText, initialValue, maxLength, 10, 30, this->hSize - 20, this->vSize - 80, readOnly, true, false);
}


void SimpleEditTextPopup::OnBeforeClose(bool isCancel) {
	if (!isCancel && this->edtText) {
		AOE_SetFocus(this->edtText->ptrParentObject, NULL); // "validate" typed value.
		if (this->maxLength > 0) {

			if (this->isForTriggers) {
				// Use dedicated API to update trigger information
				this->crCommand->WriteTriggersInGameData(this->edtText->pTypedText);
			} else {
				// Standard case: write in provided buffer, if not NULL.
				if (this->bufferToWrite != NULL) {
					strcpy_s(this->bufferToWrite, this->maxLength - 1,
						this->edtText->pTypedText);
				}
			}
		}
	}
}


InputBox::InputBox() {
	this->_ResetPointers();
}


void InputBox::_ResetPointers() {
	this->edtInput = NULL;
	this->lblTitle = NULL;
	this->edtDescription = NULL;
}

void InputBox::OnBeforeClose(bool isCancel) {}



// This class needs parameters to create content. Call this after calling OpenPopup().
void InputBox::AddPopupContent(char *title, char *desc, const char *initialInputValue, long int maxLength, bool readOnly) {
	this->readOnly = readOnly;
	if (maxLength < 1) {
		maxLength = 0x8000 - 2;
	}
	this->maxLength = maxLength;
	int labelSizeX = strlen(title) * 12;
	int labelPosX = ((this->hSize - labelSizeX) / 2) + 1;
	if (labelPosX < 1) { labelPosX = 1; }
	int textSizeY = this->vSize - (24*2) - 56;
	if (textSizeY < 1) { textSizeY = 10; }
	this->AddLabel(popup, &this->lblTitle, title, labelPosX, 10, labelSizeX, 28, AOE_FONTS::AOE_FONT_BIG_LABEL);
	if (*desc != 0) {
		this->AddTextBox(popup, &this->edtDescription, desc, 10000, 20, 40, this->hSize - 40, textSizeY, true, true, false);
	}
	this->AddTextBox(popup, &this->edtInput, initialInputValue, maxLength, 60, textSizeY + 44, this->hSize - 120, 20, readOnly, true, false);
}




EditMapSizeXYPopup::EditMapSizeXYPopup() {
	this->_ResetPointers();
	this->sizeX = 0;
	this->sizeY = 0;
}


void EditMapSizeXYPopup::_ResetPointers() {
	this->edtSizeX = NULL;
	this->edtSizeY = NULL;
}


void EditMapSizeXYPopup::_AddPopupContent() {
	this->sizeX = 0;
	this->sizeY = 0;

	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	// By default, try to reuse existing map dimensions.
	if (global && global->IsCheckSumValid() && global->gameMapInfo && global->gameMapInfo->IsCheckSumValid()) {
		this->sizeX = global->gameMapInfo->mapArraySizeX;
		this->sizeY = global->gameMapInfo->mapArraySizeY;
	}
	if (this->sizeX > 255) { this->sizeX = 255; }
	if (this->sizeY > 255) { this->sizeY = 255; }

	// Popup is open. Add components
	long int btnhPos = (hSize - 0xAC) / 2;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *unused;
	char bufX[5];
	char bufY[5];
	sprintf_s(bufX, "%ld", this->sizeX);
	sprintf_s(bufY, "%ld", this->sizeY);
	this->AddLabel(popup, &unused, "Please type X and Y map size", 30, 5, 280, 20, AOE_FONT_BIG_LABEL);
	this->AddTextBox(popup, &this->edtSizeX, bufX, 3, 120, 40, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtSizeY, bufY, 3, 120, 60, 80, 20, false, false, true);
}


void EditMapSizeXYPopup::OnAfterClose(bool isCancel) {
	if (isCancel) { return; }
	char *textX = AOE_GetEditText(this->edtSizeX);
	char *textY = AOE_GetEditText(this->edtSizeY);
	long int x = atoi(textX);
	long int y = atoi(textY);
	if ((x <= 0) || (y <= 0) || (x > 255) || (y > 255)) { return; }
	this->crCommand->ScenarioEditor_customGenerateMap(x, y);
}


EditTerrainPopup::EditTerrainPopup() {
	this->_ResetPointers();
	this->elevation = 0;
	this->terrainId = 0;
	this->maxPosX = 0;
	this->maxPosY = 0;
	this->minPosX = 0;
	this->minPosY = 0;
	this->mapSizeX = -1;
	this->mapSizeY = -1;
}


void EditTerrainPopup::_ResetPointers() {
	this->edtMaxPosX = NULL;
	this->edtMaxPosY = NULL;
	this->edtMinPosX = NULL;
	this->edtMinPosY = NULL;
	this->edtElevation = NULL;
	this->edtTerrainId = NULL;
}


void EditTerrainPopup::_AddPopupContent() {
	this->mapSizeX = -1;
	this->mapSizeY = -1;
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid() && global->gameMapInfo && global->gameMapInfo->IsCheckSumValid()) {
		this->mapSizeX = global->gameMapInfo->mapArraySizeX;
		this->mapSizeY = global->gameMapInfo->mapArraySizeY;
	}
	char bufText[60];
	sprintf_s(bufText, "Map Size X,Y = %ld, %ld", this->mapSizeX, this->mapSizeY);

	// Popup is open. Add components
	long int btnhPos = (hSize - 130) / 2;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *unused;
	this->AddLabel(popup, &unused, "Edit terrain", btnhPos, 5, 130, 20, AOE_FONT_BIG_LABEL);
	this->AddLabel(popup, &unused, "Pos X (min/max)", 10, 40, 120, 20, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(popup, &unused, "Pos Y (min/max)", 10, 70, 120, 20, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(popup, &unused, "Terrain Id (0-31)", 10, 100, 120, 20, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(popup, &unused, "Elevation (0-7)", 10, 130, 120, 20, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(popup, &unused, "Tip: leave terrain/elevation empty to preserve current values.", 10, 160, 350, 20, AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(popup, &this->edtMinPosX, "0", 3, 140, 40, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtMinPosY, "0", 3, 140, 70, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtMaxPosX, "0", 3, 240, 40, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtMaxPosY, "0", 3, 240, 70, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtTerrainId, "", 2, 140, 100, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtElevation, "", 1, 140, 130, 80, 20, false, false, true);
	this->AddLabel(popup, &unused, bufText, 10, 190, 350, 20, AOE_FONT_STANDARD_TEXT);
}


void EditTerrainPopup::OnBeforeClose(bool isCancel) {
	if (isCancel) { return; }
	assert(this->edtElevation);
	if (!this->edtElevation) { return; }
	AOE_SetFocus(this->edtElevation->ptrParentObject, NULL);
	if ((this->mapSizeX < 0) || (this->mapSizeY < 0)) { return; }
	bool elevationEmpty = ((this->edtElevation->pTypedText == NULL) || (this->edtElevation->pTypedText[0] == '\0'));
	bool terrainEmpty = ((this->edtTerrainId->pTypedText == NULL) || (this->edtTerrainId->pTypedText[0] == '\0'));
	bool minXEmpty = ((this->edtMinPosX->pTypedText == NULL) || (this->edtMinPosX->pTypedText[0] == '\0'));
	bool minYEmpty = ((this->edtMinPosY->pTypedText == NULL) || (this->edtMinPosY->pTypedText[0] == '\0'));
	bool maxXEmpty = ((this->edtMaxPosX->pTypedText == NULL) || (this->edtMaxPosX->pTypedText[0] == '\0'));
	bool maxYEmpty = ((this->edtMaxPosY->pTypedText == NULL) || (this->edtMaxPosY->pTypedText[0] == '\0'));
	if (terrainEmpty && elevationEmpty) { return; } // Only 1 of {terrain, elevation} is required
	if (minXEmpty || maxXEmpty || minYEmpty || maxYEmpty) { return; } // coordinates are all requires
	long int minX, minY, maxX, maxY;
	minX = atoi(this->edtMinPosX->pTypedText);
	minY = atoi(this->edtMinPosY->pTypedText);
	maxX = atoi(this->edtMaxPosX->pTypedText);
	maxY = atoi(this->edtMaxPosY->pTypedText);
	char terrainId = atoi(this->edtTerrainId->pTypedText);
	char elevation = atoi(this->edtElevation->pTypedText);
	if ((minX > maxX) || (minY > maxY) || (maxX > this->mapSizeX) || (maxY > this->mapSizeY)) {
		return;
	}
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert((global != NULL) && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	assert(mapInfo->mapArraySizeX >= this->mapSizeX);
	assert(mapInfo->mapArraySizeY >= this->mapSizeY);
	if ((mapInfo->mapArraySizeX < this->mapSizeX) || (mapInfo->mapArraySizeY < this->mapSizeY)) { return; }

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			ROR_STRUCTURES_10C::STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				if (!elevationEmpty) { tile->SetAltitude(elevation); } // checks elevation is 0-7
				if (!terrainEmpty) { tile->SetTerrainId(terrainId); } // checks terrainId is 0-31
			}
		}
	}
}


GenNewTriggerPopup::GenNewTriggerPopup() {
	this->_ResetPointers();
}


void GenNewTriggerPopup::_ResetPointers() {
	this->cbxActionType = NULL;
	this->cbxEventType = NULL;
	this->edtTriggerText = NULL;
}


void GenNewTriggerPopup::_AddPopupContent() {

	// Popup is open. Add components
	//long int btnhPos = (hSize - 130) / 2;
	//ROR_STRUCTURES_10C::STRUCT_UI_LABEL *unused;
	
	bool success = AOE_AddComboBox(popup, &this->cbxActionType, 20, 40, 80, 80, 80, 80, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddObjectInContentList(this->cbxActionType);
	// TO DO : fix Global_OnButtonClick in crMainitf
	AOE_AddEntryInCombo(this->cbxActionType, 0, "test1");
	AOE_AddEntryInCombo(this->cbxActionType, 1, "test2");
	this->AddTextBox(popup, &this->edtTriggerText, "0", 3, 140, 40, 80, 20, false, false, true);
	//this->AddLabel(popup, &unused, bufText, 10, 190, 350, 20, AOE_FONT_STANDARD_TEXT);
}

