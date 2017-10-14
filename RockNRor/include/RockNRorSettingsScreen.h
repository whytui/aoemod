#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_label.h>
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "RnrScreenBase.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace UI {
;


class RockNRorSettingsScreen : public RnrScreenBase {
public:
	RockNRorSettingsScreen() : RnrScreenBase("RockNRor settings") {
		this->SetFullscreen();
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::GameSettingsTheme);
		this->ResetClassPointers();
	}

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(STRUCT_UI_BUTTON *sender) override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override;

protected:
	// Reset various pointers for this class level (to override)
	void ResetClassPointers() override;

	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	void CreateScreenComponents() override;

private:
	STRUCT_UI_BUTTON *btnOK;
	STRUCT_UI_BUTTON *chkGenRandomTechTreeRMGames;
	STRUCT_UI_BUTTON *chkRPGGameModeInRandomGames;
	STRUCT_UI_BUTTON *chkRPGGameModeInScenario;
	STRUCT_UI_BUTTON *chkGenStrategyInRM;
	//STRUCT_UI_BUTTON *chkGenStrategyInDM;
	STRUCT_UI_BUTTON *chkNoWallsInRMDM;
	STRUCT_UI_BUTTON *chkNoNeutralDiplomacy;
	STRUCT_UI_BUTTON *chkNoDockInLandMaps;
	STRUCT_UI_BUTTON *chkAlwaysShareExplorationInRandomGames;
	STRUCT_UI_BUTTON *chkImproveAI;
	STRUCT_UI_BUTTON *chkFixLogisticsBug;
	STRUCT_UI_BUTTON *chkImproveCityPlan;


	void InitInputs();
	void UpdateConfigFromCheckbox(STRUCT_UI_BUTTON *checkbox);

};

}
}

