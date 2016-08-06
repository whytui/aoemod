#pragma once
#include <assert.h>
#include <string.h>
#include <AOE_structures.h>
#include <mystrings.h>
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"


// Base class to implement custom popups.
// Technically, the created (AOE) popup objects are based on game options popup.
// Overload virtual methods to define your own popup behaviour (create content, manage onclick/before&after close events)
// Warning: added components must be freed using "AOEDestructor". Please call AddObjectInContentList for each created UI component so that is will be freed automatically.
class CustomPopupBase {
public:
	CustomPopupBase();
	CustomPopupBase(CustomRORCommand *crCommand);
	~CustomPopupBase() {};

	// The popup to open after this one is closed (optional, of course)
	CustomPopupBase *nextPopup;

	// To provide crCommand (shared) object
	void SetCRCommand(CustomRORCommand *crCommand);
	// Reset all internal pointers (base class + child classes)
	void ResetPointers();
	// Call this to open a new popup (=>this)
	AOE_STRUCTURES::STRUCT_ANY_UI *OpenPopup(long int hSize, long int vSize, bool withCancelBtn);
	// Call this to close the popup. This will handle pre/post close events.
	void ClosePopup(bool isCancel);
	// Indicates this popup object has been closed and can be deleted safely.
	bool IsClosed() { return this->isClosed; }
	// Get the (AOE) popup object
	AOE_STRUCTURES::STRUCT_ANY_UI *GetAOEPopupObject() { return this->popup; }

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) { return false; };

protected:
	CustomRORInfo *crInfo;
	CustomRORCommand *crCommand;
	AOE_STRUCTURES::STRUCT_ANY_UI *popup;
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
	inline void AddObjectInContentList(AOE_STRUCTURES::STRUCT_ANY_UI *obj);

	// API to add UI components and add them automatically in "objects to free" list.
	bool AddLabel(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
		AOE_STRUCTURES::STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddTextBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
		AOE_STRUCTURES::STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		bool readOnly = false, bool multiline = false, bool onlyNumbers = false, unsigned long int font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddCheckBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
		AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize);

	bool AddButton(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
		AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate, const char *caption,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddButton(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
		AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

private:
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel;

	// Call this to create popup's specific content
	void AddPopupContent() {
		if (this->IsClosed() || (this->popup == NULL)) { return; }
		this->_AddPopupContent();
	};
};

