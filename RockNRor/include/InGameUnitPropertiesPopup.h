#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <AOE_empires_dat_concept_names.h>
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "playerHandling.h"
#include "RnrScreenBase.h"


namespace ROCKNROR {
namespace UI {
;


class InGameUnitPropertiesPopup : public RnrScreenBase {
public:
	InGameUnitPropertiesPopup(long int unitId) : RnrScreenBase("ingame unit properties") {
		this->SetWindowed(100, 100, 600, 500); // will always work (default values)
		this->SetCenteredForSize(600, 500); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::InGameOptionsTheme);
		this->unitId = unitId;
	}


	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	void CreateScreenComponents() override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnButtonClick(STRUCT_UI_BUTTON *sender) override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override;


protected:
	// Reset various pointers for this class level (to override)
	virtual void ResetClassPointers() override;

private:
	long int unitId; // Warning: Storing pointer would be dangerous if game executions continues (MP ?): unit could be destroyed in the meanwhile.
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblMainInfos;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblChildUnitsAutoMove;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblFarmAutoRebuild;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
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


	// Save changes
	void Validate();

};


}
}
