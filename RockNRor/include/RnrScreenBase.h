#pragma once
#include <assert.h>
#include <string.h>
#include <list>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_button_with_number.h>
#include <UI_components\AOE_struct_ui_combobox.h>
#include <UI_components\AOE_struct_ui_label.h>
#include <UI_components\AOE_struct_ui_listbox.h>
#include <UI_components\AOE_struct_ui_scrollbar.h>
#include <UI_components\AOE_struct_ui_textbox.h>
#include <mystrings.h>
#include "RockNRorCommon.h"
#include "RockNRorInfo.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"

namespace ROCKNROR {
namespace UI {

using namespace AOE_STRUCTURES;


// Base class to implement custom screens.
class RnrScreenBase {
public:
	enum ScreenStatus { NOT_CREATED, CREATED, CLOSED };
	
	// Constructor for a custom screen, that will be automatically added to custom UI helper.
	RnrScreenBase(const char *screenName);
	// Destructor. Being virtual is crucial to allow child classes to be deleted correctly even if calling "delete ptr_as_base_class"
	virtual ~RnrScreenBase();

	// Set screen type. Only allowed when screen has not been opened yet.
	bool SetScreenType(AOE_METHODS::UI_BASE::ScreenType newType);
	// Set background theme. Only allowed when screen has not been opened yet.
	bool SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme backgroundSlpTheme);
	// Get current screen status
	ScreenStatus GetScreenStatus() const { return this->rorScreenStatus; }
	// Get screen name (read only)
	const std::string GetScreenName() const { return this->screenName; }
	// Get background theme
	AOE_CONST_DRS::AoeScreenTheme GetBackgroundSlpTheme() const { return this->backgroundSlpTheme; }

	// Only allowed when screen has not been opened yet.
	bool SetFullscreen();
	// Only allowed when screen has not been opened yet.
	bool SetWindowed(unsigned long int posX, unsigned long int posY, unsigned long int sizeX, unsigned long int sizeY);
	// Only allowed when screen has not been opened yet.
	bool SetCenteredForSize(unsigned long int sizeX, unsigned long int sizeY);
	// Get X position for a centered object of provided size
	unsigned long int GetCenteredPositionX(unsigned long int objSizeX) const;
	unsigned long int GetScreenSizeX() const { return this->screenSizeX; }
	unsigned long int GetScreenSizeY() const { return this->screenSizeY; }

	// Actually create and show the screen in ROR UI.
	// parentScreen can be NULL.
	bool CreateScreen(STRUCT_UI_EASY_PANEL *parentScreen);

	// Close (and frees) screen in ROR UI.
	// Do not close if there are underlying screens !
	// You can call this from "this" class.
	bool CloseScreen(bool ignoreErrors);

	// Close "this" screen in ROR UI, and opens "otherScreen" instead.
	// "this" must be CREATED and otherScreen must be NOT_CREATED.
	bool OpenOtherScreenAndCloseThisOne(RnrScreenBase *otherScreen, bool nullParent);

	STRUCT_UI_EASY_PANEL *GetAoeScreenObject() const;

	// API to add UI components and add them automatically in "objects to free" list.
	bool AddLabel(STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddTextBox(STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		bool readOnly = false, bool multiline = false, bool onlyNumbers = false, unsigned long int font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddCheckBox(STRUCT_UI_BUTTON **ptrObjToCreate,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize);

	bool AddButton(STRUCT_UI_BUTTON **ptrObjToCreate, const char *caption,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddButton(STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
		unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
		long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);

	bool AddComboBox(STRUCT_UI_COMBOBOX **ptrObjToCreate, long int posX, long int posY,
		long int listSizeX, long int listSizeY, long int lblSizeX, long int lblSizeY, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT);


	// Event handlers

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT);

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnButtonClick(STRUCT_UI_BUTTON *sender);

	// Returns left position for a centered element with desired size.
	unsigned long int GetLeftCenteredPositionX(long int desiredSize) const;

protected:
	std::list<STRUCT_ANY_UI*> componentsToFree;

	void FreeComponents();

	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	virtual void CreateScreenComponents() {}

	// Reset various pointers for this class level (to override)
	virtual void ResetClassPointers() {}

private:
	RnrScreenBase();
	ScreenStatus rorScreenStatus;
	AOE_METHODS::UI_BASE::ScreenType rorScreenType;
	AOE_CONST_DRS::AoeScreenTheme backgroundSlpTheme;
	std::string screenName; // Technical screen name, should be unique
	bool isFullscreen;
	unsigned long int screenPosX, screenPosY, screenSizeX, screenSizeY;

	// Please call this for every created component in this screen. This will automatically free the components when needed.
	bool AddComponentToFreeList(STRUCT_ANY_UI *object) {
		// We try to void duplicates because it would crash at freeing time.
		if (std::find(this->componentsToFree.cbegin(), this->componentsToFree.cend(), object) == this->componentsToFree.cend()) {
			this->componentsToFree.push_back(object);
			return true;
		}
		return false;
	}

	// Reset internal pointers
	void ResetPointers() {
		this->ResetClassPointers();
	}

};


}
}
