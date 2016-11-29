#include "../include/CustomPopupBase.h"

CustomPopupBase::CustomPopupBase() {
	this->ResetPointers();
	this->isClosed = false;
}


void CustomPopupBase::ResetPointers() {
	this->nextPopup = NULL;
	this->btnCancel = NULL;
	this->btnOK = NULL;
	this->isClosed = false;
	this->popup = NULL;
	this->_ResetPointers(); // class-specific operations
}


// Use it to list all UI objects (labels, buttons...) that are created(added) to popup content,
// so they are automatically freed when popup is closed (using AOE destructor).
void CustomPopupBase::AddObjectInContentList(AOE_STRUCTURES::STRUCT_ANY_UI *obj) {
	CUSTOMROR::crInfo.AddObjectInPopupContentList(obj);
}


// API to add UI components and add them automatically in "objects to free" list.
bool CustomPopupBase::AddLabel(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	AOE_FONTS font) {
	bool res;
	res = AOE_METHODS::AOE_AddLabel(parent, ptrObjToCreate, label, hPos, vPos, hSize, vSize, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}

bool CustomPopupBase::AddTextBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	bool readOnly, bool multiline, bool onlyNumbers, unsigned long int font) {
		bool res;
		res = AOE_METHODS::AOE_AddTextBox(parent, ptrObjToCreate, initialText, maxTextLength, hPos, vPos, hSize, vSize,
			readOnly, multiline, onlyNumbers, font);
		if (res && ptrObjToCreate) {
			this->AddObjectInContentList(*ptrObjToCreate);
		}
		return res;
}

bool CustomPopupBase::AddCheckBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize) {
	bool res;
	res = AOE_METHODS::AOE_AddCheckBox(parent, ptrObjToCreate, hPos, vPos, hSize, vSize);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}

bool CustomPopupBase::AddButton(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate, const char *caption,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId, AOE_FONTS font) {
	bool res;
	res = AOE_METHODS::AOE_AddButton(parent, ptrObjToCreate, caption, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}

bool CustomPopupBase::AddButton(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId, AOE_FONTS font) {
	bool res;
	res = AOE_METHODS::AOE_AddButton(parent, ptrObjToCreate, DLL_STRING_ID, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}


bool CustomPopupBase::AddComboBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX **ptrObjToCreate, long int posX, long int posY,
	long int listSizeX, long int listSizeY, long int lblSizeX, long int lblSizeY, AOE_FONTS font) {
	bool res;
	res = AOE_METHODS::AOE_AddComboBox(parent, ptrObjToCreate, posX, posY, listSizeX, listSizeY, lblSizeX, lblSizeY, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}


// Call this to open a new popup (=>this)
// themeSlpId is a "bina" slpid from interfac.drs with references to colors and slpids to use for buttons, etc. Basically 50051 to 50061.
AOE_STRUCTURES::STRUCT_ANY_UI *CustomPopupBase::OpenPopup(long int hSize, long int vSize, bool withCancelBtn, long int themeSlpId) {
	this->hSize = hSize;
	this->vSize = vSize;
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup()) {
		return NULL;
	}
	this->popup = CUSTOMROR::crInfo.OpenCustomGamePopup(hSize, vSize, withCancelBtn, themeSlpId);
	if (this->popup != NULL) {
		this->AddPopupContent();
	}
	return this->popup;
}


// Call this to close the popup. This will handle pre/post close events.
void CustomPopupBase::ClosePopup(bool isCancel) {
	if (this->IsClosed()) { return; }
	this->OnBeforeClose(isCancel);
	CUSTOMROR::crInfo.CloseCustomGamePopup();
	this->OnAfterClose(isCancel);
	// NO because we may have opened a new one in post-actions ! // this->currentCustomGamePopup = CUSTOM_GAME_POPUP_TYPES::CGPT_NONE;
	this->isClosed = true;
}

