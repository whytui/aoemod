#pragma once
#include <assert.h>
#include <string.h>
#include <ROR_structures.h>
#include <pointers.h>
#include <mystrings.h>
#include "crCommon.h"
#include "crCommands.h"


// Base class to implement custom popups.
// Technically, the created (AOE) popup objects are based on game options popup.
// Overload virtual methods to define your own popup behaviour (create content, manage onclick/before&after close events)
// Warning: added components must be freed using "AOEDestructor". Please call AddObjectInContentList for each created UI component so that is will be freed automatically.
class CustomPopup {
public:
	CustomPopup();
	CustomPopup(CustomRORCommand *crCommand);
	~CustomPopup() {};

	// The popup to open after this one is closed (optional, of course)
	CustomPopup *nextPopup;

	// To provide crCommand (shared) object
	void SetCRCommand(CustomRORCommand *crCommand);
	// Reset all internal pointers (base class + child classes)
	void ResetPointers();
	// Call this to open a new popup (=>this)
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *OpenPopup(long int hSize, long int vSize, bool withCancelBtn);
	// Call this to close the popup. This will handle pre/post close events.
	void ClosePopup(bool isCancel);
	// Indicates this popup object has been closed and can be deleted safely.
	bool IsClosed() { return this->isClosed; }
	// Get the (AOE) popup object
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *GetAOEPopupObject() { return this->popup; }

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) { return false; };

protected:
	CustomRORInfo *crInfo;
	CustomRORCommand *crCommand;
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *popup;
	bool isClosed; // Indicates this popup object has been closed and can be deleted safely.
	long int hSize, vSize;

	// Overload to manage class-specific pointers to reset
	virtual void _ResetPointers() {};
	// Overload this method to add popup (specific) content. Always call AddObjectInContentList(...) for each created underlying UI object.
	virtual void _AddPopupContent() {};
	// Overload this method to manage actions to do just before the popup is closed.
	virtual void OnBeforeClose(bool isCancel) { };
	// Overload this method to manage actions to do just after the popup is closed (it is possible here to open another popup)
	virtual void OnAfterClose(bool isCancel) { };
	// Use it to list all UI objects (labels, buttons...) that are created(added) to popup content, so they are automatically freed when popup is closed.
	inline void AddObjectInContentList(ROR_STRUCTURES_10C::STRUCT_ANY_UI *obj);
private:
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnOK;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnCancel;

	// Call this to create popup's specific content
	void AddPopupContent() {
		if (this->IsClosed() || (this->popup == NULL)) { return; }
		this->_AddPopupContent();
	};
};


// *****************************
// End of base class
// Below are child classes, they implement specific popups with their own content/behavior.
// *****************************



class InGameCustomRorOptionsPopup : public CustomPopup {
public:
	InGameCustomRorOptionsPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;
	// Specific method to use instead of OpenPopup (special treatments for ingame customROR menu).
	// previousPopup should be game menu popup object.
	ROR_STRUCTURES_10C::STRUCT_ANY_UI * CloseMenuAndOpenPopup(ROR_STRUCTURES_10C::STRUCT_ANY_UI *previousPopup);
	// Popup content objects
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *customOptionButtonVar; // "OK" button of customROR's options popup. NULL means popup does not exist. See HasOpenedCustomGamePopup.
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *customOptionHumanPenaltyTextVar;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *customOptionHumanPenaltyLabelVar;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *customOptionGameSpeedTextVar;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *customOptionGameSpeedLabelVar;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *customOptionHeaderLabelVar;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *customOptionFreeTextVar;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *customOptionFreeTextLabelVar;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *customOptionFreeTextAnswerVar;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnTechTreeInfo;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAutoRebuildFarms;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoRebuildFarms;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMaxFood;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoRebuildFarmsMaxFood;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMinWood;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoRebuildFarmsMinWood;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMaxFarms;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoRebuildFarmsMaxFarms;

private:
	bool openTechTreeInfo;
};



class EditorEditUnitInfoPopup : public CustomPopup {
public:
	EditorEditUnitInfoPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;
private:
	ROR_STRUCTURES_10C::STRUCT_UNIT *unit;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtStatus;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lbl_s0;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lbl_s2;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lbl_s4;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkbox_s0;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkbox_s2;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkbox_s4;
};


enum SC_INFO_POPUP_TO_OPEN { PTO_NONE = 0, PTO_TRIGGER, PTO_AI, PTO_PER, PTO_TERRAIN_EDIT, PTO_COUNT };

class EditorScenarioInfoPopup : public CustomPopup {
public:
	EditorScenarioInfoPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) override;
	void OnAfterClose(bool isCancel) override;

	// Provide the pointer to the boolean that will be updated according to user choice.
	void SetVarToUpdate_allowUnitOverlapping(bool *varToUpdate);
	void SetVarToUpdate_disableHillModeChecks(bool *varToUpdate);
	void SetVarToUpdate_disableTerrainRestrictionChecks(bool *varToUpdate);
	void SetVarToUpdate_lengthenCombatMode(long int *varToUpdate);
private:
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblTitle;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtPlayerId;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnTriggers;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnAI;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnPER;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnTerrainEdit;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAllowUnitOverlapping;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkDisableHillModeCheck;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkDisableTerrainRestrictions;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkLengthenCombatMode;
	SC_INFO_POPUP_TO_OPEN popupToOpen;
	int playerId;
	bool *varToUpdate_allowUnitOverlapping;
	bool *varToUpdate_disableHillModeChecks;
	bool *varToUpdate_disableTerrainRestrictionChecks;
	long int *varToUpdate_lengthenCombatMode;
};


class SimpleEditTextPopup : public CustomPopup {
public:
	SimpleEditTextPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly);
	void OnBeforeClose(bool isCancel) override;
	bool isForTriggers;
private:
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtText;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblTitle;
	long int maxLength;
	char *bufferToWrite;
};


class EditMapSizeXYPopup : public CustomPopup {
public:
	EditMapSizeXYPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void _AddPopupContent() override;
	void OnAfterClose(bool isCancel) override;
	bool isForTriggers;
private:
	long int sizeX;
	long int sizeY;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtSizeX;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtSizeY;
};


class EditTerrainPopup : public CustomPopup {
public:
	EditTerrainPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	void OnBeforeClose(bool isCancel) override;
	bool isForTriggers;
private:
	long int mapSizeX, mapSizeY;
	long int minPosX, minPosY, maxPosX, maxPosY;
	char terrainId;
	char elevation;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtMinPosX;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtMinPosY;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtMaxPosX;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtMaxPosY;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtTerrainId;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtElevation;
};


class GenNewTriggerPopup : public CustomPopup {
public:
	GenNewTriggerPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
private:
	ROR_STRUCTURES_10C::STRUCT_UI_COMBOBOX *cbxEventType;
	ROR_STRUCTURES_10C::STRUCT_UI_COMBOBOX *cbxActionType;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtTriggerText;
};

