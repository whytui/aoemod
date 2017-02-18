#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <AOE_const_drs.h>
#include "CustomPopupBase.h"
#include "SimpleEditText.h"
#include "InputBox.h"
#include "EditTerrainPopup.h"
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "CustomPopupSystem.h"


enum SC_INFO_POPUP_TO_OPEN { PTO_NONE = 0, PTO_TRIGGER, PTO_AI, PTO_PER, PTO_TERRAIN_EDIT, PTO_VICTORY_CONDITION, PTO_COUNT };

class EditorScenarioInfoPopup : public CustomPopupBase {
public:
	EditorScenarioInfoPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;
	void OnAfterClose(bool isCancel) override;

	// Provide the pointer to the boolean that will be updated according to user choice.
	void SetVarToUpdate_allowUnitOverlapping(bool *varToUpdate);
	void SetVarToUpdate_disableHillModeChecks(bool *varToUpdate);
	void SetVarToUpdate_disableTerrainRestrictionChecks(bool *varToUpdate);
	void SetVarToUpdate_lengthenCombatMode(long int *varToUpdate);

	// Opens the custom "scenario info" popup in editor
	// Returns true if OK.
	static bool OpenCustomEditorScenarioInfoPopup();

private:
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPlayerId;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnTriggers;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnAI;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnPER;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnTerrainEdit;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnVictoryCondition;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAllowUnitOverlapping;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkDisableHillModeCheck;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkDisableTerrainRestrictions;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkLengthenCombatMode;
	SC_INFO_POPUP_TO_OPEN popupToOpen;
	int playerId;
	bool *varToUpdate_allowUnitOverlapping;
	bool *varToUpdate_disableHillModeChecks;
	bool *varToUpdate_disableTerrainRestrictionChecks;
	long int *varToUpdate_lengthenCombatMode;
};
