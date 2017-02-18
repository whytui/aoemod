#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "playerHandling.h"
#include "CustomPopupSystem.h"


class InGameUnitPropertiesPopup : public CustomPopupBase {
public:
	InGameUnitPropertiesPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(long int unitId);
	//void _AddPopupContent() override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;

	// Open the relevant "view/edit unit" popup for currently selected unit.
	// Returns true if successful.
	static bool OpenInGameUnitPropertiesPopup();

	// Open the relevant "view/edit unit" popup for provided unit.
	// Returns true if successful.
	static bool OpenInGameUnitPropertiesPopup(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

private:
	long int unitId; // Warning: Storing pointer would be dangerous if game executions continues (MP ?): unit could be destroyed in the meanwhile.
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblMainInfos;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblChildUnitsAutoMove;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblFarmAutoRebuild;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkRebuildFarmNone; // "default"
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkForceRebuildFarm;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkForceNotRebuildFarm;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnResetAutoMove;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblBuildingTechsMessage;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtBuildingTechs;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoAttackUnits;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoAttackVillagers;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoAttackMilitary;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoAttackTowers;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoAttackBuildings;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoAttackWalls;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoAttackVillagers;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoAttackMilitary;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoAttackTowers;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoAttackBuildings;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoAttackWalls;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtStrengthWeakness;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblConversionResistance;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnMakeMainUnitForShortcutSelection;
};
