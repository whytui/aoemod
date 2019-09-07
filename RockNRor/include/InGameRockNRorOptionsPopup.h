#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "SimpleEditText.h"
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorInfo.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "MapCopyPopup.h"
#include "autoRebuildFarmConfig.h"
#include "RnrScreenBase.h"
#include "RockNRorSettingsScreen.h"



namespace ROCKNROR {
namespace UI {
;


class InGameRockNRorOptionsPopup : public RnrScreenBase {
public:
	InGameRockNRorOptionsPopup() : RnrScreenBase("ingame rnr options") {
		this->SetWindowed(100, 100, 500, 450); // will always work (default values)
		this->SetCenteredForSize(600, 450); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::FullBlackTheme);
		this->ResetClassPointers();
		this->afterCloseGamePausePolicy = AfterClosePausePolicy::SET_PAUSE_OFF;
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
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *customOptionButtonVar; // "OK" button of RockNRor's options popup. NULL means popup does not exist. See HasOpenedCustomGamePopup.
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnRestartWithNewSeed;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionHumanPenaltyTextVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionHumanPenaltyLabelVar;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionGameSpeedTextVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionGameSpeedLabelVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionHeaderLabelVar;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionFreeTextVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionFreeTextLabelVar;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionFreeTextAnswerVar;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnTechTreeInfo;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnMapCopy;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnRnrGlobalOptions;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoRebuildFarms;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarms;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMaxFood;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarmsMaxFood;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMinWood;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarmsMinWood;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMaxFarms;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarmsMaxFarms;

	::ROCKNROR::CONFIG::AutoRebuildFarmConfig *autoRebuildFarmConfig;

	void Validate();

	void ClosePopupAndHandlePausePolicy();

	// Close this screen and triggers a game restart
	// If newSeed is true, a new seed is computed to get another random games with same parameters.
	// If newSeed is false, restarts exactly the same game
	void RestartGame(bool newSeed);
};

}
}
