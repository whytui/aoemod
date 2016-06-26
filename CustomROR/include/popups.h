#pragma once
#include <assert.h>
#include <string.h>
#include <ROR_structures.h>
#include <pointers.h>
#include <mystrings.h>
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"


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

	// API to add UI components and add them automatically in "objects to free" list.
	bool AddLabel(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
		ROR_STRUCTURES_10C::STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddTextBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
		ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		bool readOnly = false, bool multiline = false, bool onlyNumbers = false, unsigned long int font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddCheckBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
		ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize);

	bool AddButton(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
		ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate, const char *caption,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddButton(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
		ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

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


class InGameUnitPropertiesPopup : public CustomPopup {
public:
	InGameUnitPropertiesPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(long int unitId);
	//void _AddPopupContent() override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) override;
private:
	long int unitId; // Warning: Storing pointer would be dangerous if game executions continues (MP ?): unit could be destroyed in the meanwhile.
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblTitle;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblMainInfos;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblChildUnitsAutoMove;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblFarmAutoRebuild;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkRebuildFarmNone; // "default"
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkForceRebuildFarm;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkForceNotRebuildFarm;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnResetAutoMove;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblBuildingTechsMessage;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtBuildingTechs;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoAttackUnits;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoAttackVillagers;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoAttackMilitary;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoAttackTowers;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoAttackBuildings;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblAutoAttackWalls;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAutoAttackVillagers;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAutoAttackMilitary;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAutoAttackTowers;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAutoAttackBuildings;
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *chkAutoAttackWalls;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtStrengthWeakness;
};



enum SC_INFO_POPUP_TO_OPEN { PTO_NONE = 0, PTO_TRIGGER, PTO_AI, PTO_PER, PTO_TERRAIN_EDIT, PTO_VICTORY_CONDITION, PTO_COUNT };

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
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btnVictoryCondition;
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
	void AddPopupContent(const char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly);
	void OnBeforeClose(bool isCancel) override;
	bool isForTriggers;
private:
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtText;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblTitle;
	long int maxLength;
	char *bufferToWrite;
};


// A common base class to implement input boxes.
class InputBox : public CustomPopup {
public:
	InputBox();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(const char *title, const char *desc, const char *initialInputValue, long int maxLength, bool readOnly);
	void OnBeforeClose(bool isCancel) override;
protected:
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtInput;
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *lblTitle;
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX *edtDescription;
	long int maxLength;
	bool readOnly;
};


// Generic input box for integers. For float, please use InputBox_float instead (and specify maxLength).
template<typename inttype> class InputBox_int : public InputBox {
public:
	InputBox_int() { this->_ResetPointers(); }
	void _ResetPointers() override {
		this->bufferToWrite = NULL;
		this->minValue = 0;
		this->maxValue = 0;
		this->forcedMaxLength = 0;
	}
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(const char *title, const char *text, inttype initialValue, inttype minValue, inttype maxValue, inttype *outputBuffer, bool readOnly) {
		this->bufferToWrite = outputBuffer;
		long int maxLength = 0;
		std::string sFoo;
		if (this->forcedMaxLength > 0) { maxLength = this->forcedMaxLength; } else {
			// Calculate max length
			sFoo = to_string(minValue);
			maxLength = sFoo.size();
			sFoo = to_string(maxValue);
			long int otherSize = sFoo.size();
			if (otherSize > maxLength) { maxLength = otherSize; }
		}
		sFoo = to_string(initialValue);
		InputBox::AddPopupContent(title, text, sFoo.c_str(), maxLength, readOnly);
	}
	void OnBeforeClose(bool isCancel) override {
		if (isCancel || !this->edtInput || this->readOnly || !this->bufferToWrite) { return; }
		AOE_SetFocus(this->edtInput->ptrParentObject, NULL); // "validate" typed value.
		inttype value = atoi(this->edtInput->pTypedText); // atoi returns 0 if invalid, we use that behaviour too
		*this->bufferToWrite = value;
	}
protected:
	long int forcedMaxLength;
private:
	inttype minValue, maxValue;
	inttype *bufferToWrite;
};

// A specific implementation for float (use atof).
void InputBox_int<float>::OnBeforeClose(bool isCancel) {
	if (isCancel || !this->edtInput || this->readOnly || !this->bufferToWrite) { return; }
	AOE_SetFocus(this->edtInput->ptrParentObject, NULL); // "validate" typed value.
	float value = (float)atof(this->edtInput->pTypedText); // atof returns 0 if invalid, we use that behaviour too
	*this->bufferToWrite = value;
}

// Please use this derived class for float messagebox, and specify input text max length.
class InputBox_float : public InputBox_int<float> {
	void AddPopupContent(char *title, char *text, float initialValue, float minValue, float maxValue, long int maxLength, float *outputBuffer, bool readOnly) {
		if (maxLength < 1) { maxLength = 1; }
		this->forcedMaxLength = maxLength;
		InputBox_int::AddPopupContent(title, text, initialValue, minValue, maxValue, outputBuffer, readOnly);
	}
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

