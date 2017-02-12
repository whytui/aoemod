#include "../include/EditorEditUnitInfoPopup.h"




// Opens the custom "edit unit" in editor
// Returns true if OK.
bool EditorEditUnitInfoPopup::OpenCustomEditorEditUnitPopup() {
	return (CUSTOMROR::customPopupSystem.OpenCustomGamePopup<EditorEditUnitInfoPopup>(500, 400, true) != NULL);
}



EditorEditUnitInfoPopup::EditorEditUnitInfoPopup() {
	this->_ResetPointers();
}

void EditorEditUnitInfoPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->unit = NULL;
	this->allSelectedUnits.clear();
	this->edtStatus = NULL;
	this->chkbox_s0 = NULL;
	this->chkbox_s2 = NULL;
	this->chkbox_s4 = NULL;
	this->lbl_s0 = NULL;
	this->lbl_s2 = NULL;
	this->lbl_s4 = NULL;
	this->cbxPlayerOwner = NULL;
}


void EditorEditUnitInfoPopup::_AddPopupContent() {
	if (this->IsClosed() || (this->popup == NULL)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetPlayerStruct(global->humanPlayerId);
	if (!controlledPlayer || !controlledPlayer->IsCheckSumValid() || (controlledPlayer->selectedUnitCount <= 0)) { return; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = CUSTOMROR::crInfo.GetMainSelectedUnit(controlledPlayer);
	if (!unit || (!unit->IsCheckSumValidForAUnitClass())) { return; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = CUSTOMROR::crInfo.GetRelevantSelectedUnitsBasePointer(controlledPlayer);
	for (int i = 0; i < controlledPlayer->selectedUnitCount; i++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *curUnit = selectedUnits[i];
		if (curUnit && curUnit->IsCheckSumValidForAUnitClass()) {
			this->allSelectedUnits.insert(curUnit);
		}
	}
	AOE_STRUCTURES::STRUCT_PLAYER *unitPlayer = unit->ptrStructPlayer;
	if (!unitPlayer || !unitPlayer->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	if (!unitDef) { return; }
	assert(unitDef->IsCheckSumValidForAUnitClass());
	this->unit = unit;
	this->initialOwner = unitPlayer->playerId;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblUnusedPtr = NULL;
	this->AddLabel(this->popup, &lblUnusedPtr, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_TITLE, "Edit a unit"),
		180, 10, 200, 24, AOE_FONTS::AOE_FONT_BIG_LABEL);
	char *name = "";
	short int DATID = -1;
	if (unitDef->ptrUnitName) {
		name = unitDef->ptrUnitName;
		DATID = unitDef->DAT_ID1;
	}
	char buffer[200];
	sprintf_s(buffer, "Unit ID = %ld - name=%s (DATID=%d). Pos(x,y)=%.1f, %.1f", unit->unitInstanceId, name, DATID, unit->positionX, unit->positionY);
	this->AddLabel(this->popup, &lblUnusedPtr, buffer, 50, 50, 400, 24, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	
	const char *labelText0 = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_INITIAL, "Initial");
	const char *labelText4 = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_DYING, "Dying");
	bool disableInitial = true;

	if (unit->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) {
		labelText0 = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_NOTBUILT, "Not built");
		disableInitial = false;
	}
	// Warning: do NOT use unit->unitType, it is not correctly valued in editor !!!
	if ((unitDef->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TREE) ||
		(unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal) ||
		(unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPredatorAnimal)
		) {
		labelText4 = localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_GATHERABLE, "Gatherable");
	}

	this->AddLabel(this->popup, &this->lbl_s0, labelText0, 160, 80, 60, 24, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(this->popup, &this->lbl_s2, localizationHandler.GetTranslation(CRLANG_ID_UNITPROP_NORMAL_STATUS, "Normal"), 230, 80, 60, 24, AOE_FONT_STANDARD_TEXT);
	this->AddLabel(this->popup, &this->lbl_s4, labelText4, 300, 80, 70, 24, AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(this->popup, &this->chkbox_s0, 170, 110, 24, 24);
	this->AddCheckBox(this->popup, &this->chkbox_s2, 240, 110, 24, 24);
	this->AddCheckBox(this->popup, &this->chkbox_s4, 310, 110, 24, 24);
	// Check current status' box
	switch (unit->unitStatus) {
	case 0:
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s0, true);
		break;
	case 2:
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s2, true);
		break;
	case 4:
		AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s4, true);
		break;
	}
	if (disableInitial) {
		this->chkbox_s0->readOnly = 1;
		AOE_METHODS::UI_BASE::RefreshUIObject(this->chkbox_s0);
	}
	// Player that owns the unit
	this->AddComboBox(this->popup, &this->cbxPlayerOwner, 20, 20, 120, 20, 120, 20);
	if (this->cbxPlayerOwner) {
		this->cbxPlayerOwner->Clear();
		AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *se = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_METHODS::GetScreenFromName(scenarioEditorScreenName);
		long int totalPlayerCount = global->playerTotalCount;
		if (se && se->IsCheckSumValid()) {
			totalPlayerCount = se->pl_cbb_playerCount->GetSelectedIndex() + 2; // +2 because: add gaia AND index starts at 0, not 1.
		}
		assert(totalPlayerCount < 10);
		if (totalPlayerCount >= 10) { totalPlayerCount = 9; }
		char bufNumber[] = "Player  ";
		for (char i = 0; i < (char)totalPlayerCount; i++) {
			bufNumber[7] = '0' + i;
			AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxPlayerOwner, i, bufNumber);
		}
		this->cbxPlayerOwner->SetSelectedIndex(this->initialOwner);
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditorEditUnitInfoPopup::OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (sender->checked) {
		if (sender == this->chkbox_s0) {
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s2, false);
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s4, false);
		}
		if (sender == this->chkbox_s2) {
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s0, false);
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s4, false);
		}
		if (sender == this->chkbox_s4) {
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s2, false);
			AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkbox_s0, false);
		}
	}
	return false;
}


void EditorEditUnitInfoPopup::OnBeforeClose(bool isCancel) {
	if (isCancel) { return; }
	// Note: setting shortcutNumber can show the unit's status, but only works for player #1. Fix in 0x4A78D9 to allow other players
	if (this->chkbox_s0 && this->chkbox_s2 && this->chkbox_s4) {
		for each (auto curUnit in this->allSelectedUnits)
		{
			if (this->chkbox_s0 && (this->chkbox_s0->checked)) {
				curUnit->unitStatus = AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_0_NOT_BUILT;
				curUnit->shortcutNumber = 1;
			}
			if (this->chkbox_s2 && (this->chkbox_s2->checked)) {
				curUnit->unitStatus = AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY;
				curUnit->shortcutNumber = 0;
			}
			if (this->chkbox_s4 && (this->chkbox_s4->checked)) {
				curUnit->unitStatus = AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_4_DYING_ANIMATION;
				curUnit->shortcutNumber = 3;
			}
		}
	}
	// Change owner
	if (this->cbxPlayerOwner) {
		int newOwner = this->cbxPlayerOwner->GetSelectedIndex();
		if (newOwner != this->initialOwner) {
			for each (auto curUnit in this->allSelectedUnits)
			{
				AOE_METHODS::UNIT::ChangeUnitOwner(curUnit, GetPlayerStruct(newOwner));
			}
		}
	}
	this->ResetPointers();
}


void EditorEditUnitInfoPopup::OnAfterClose(bool isCancel) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	// When closing "edit unit" popup, restore unit selection mouse cursor (more practical).
	settings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
}

