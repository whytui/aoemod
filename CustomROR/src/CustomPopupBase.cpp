#include "../include/CustomPopupBase.h"

CustomPopupBase::CustomPopupBase() {
	this->SetCRCommand(NULL);
	this->ResetPointers();
	this->isClosed = false;
}

CustomPopupBase::CustomPopupBase(CustomRORCommand *crCommand) {
	this->SetCRCommand(crCommand);
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


void CustomPopupBase::SetCRCommand(CustomRORCommand *crCommand) {
	this->crCommand = crCommand;
	this->crInfo = NULL;
	if (crCommand) {
		this->crInfo = crCommand->crInfo;
	}
}

// Use it to list all UI objects (labels, buttons...) that are created(added) to popup content,
// so they are automatically freed when popup is closed (using AOE destructor).
void CustomPopupBase::AddObjectInContentList(AOE_STRUCTURES::STRUCT_ANY_UI *obj) {
	this->crInfo->AddObjectInPopupContentList(obj);
}


// API to add UI components and add them automatically in "objects to free" list.
bool CustomPopupBase::AddLabel(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	AOE_FONTS font) {
	bool res;
	res = AOE_AddLabel(parent, ptrObjToCreate, label, hPos, vPos, hSize, vSize, font);
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
		res = AOE_AddTextBox(parent, ptrObjToCreate, initialText, maxTextLength, hPos, vPos, hSize, vSize, 
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
	res = AOE_AddCheckBox(parent, ptrObjToCreate, hPos, vPos, hSize, vSize);
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
	res = AOE_AddButton(parent, ptrObjToCreate, caption, hPos, vPos, hSize, vSize, buttonId, font);
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
	res = AOE_AddButton(parent, ptrObjToCreate, DLL_STRING_ID, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddObjectInContentList(*ptrObjToCreate);
	}
	return res;
}



// Call this to open a new popup (=>this)
AOE_STRUCTURES::STRUCT_ANY_UI *CustomPopupBase::OpenPopup(long int hSize, long int vSize, bool withCancelBtn) {
	if (!this->crInfo) { return NULL; }
	this->hSize = hSize;
	this->vSize = vSize;
	if (this->crInfo->HasOpenedCustomGamePopup()) {
		return NULL;
	}
	this->popup = this->crInfo->OpenCustomGamePopup(hSize, vSize, withCancelBtn);
	if (this->popup != NULL) {
		this->AddPopupContent();
	}
	return this->popup;
}


// Call this to close the popup. This will handle pre/post close events.
void CustomPopupBase::ClosePopup(bool isCancel) {
	if (!this->crInfo) { return; }
	if (this->IsClosed()) { return; }
	this->OnBeforeClose(isCancel);
	this->crInfo->CloseCustomGamePopup();
	this->OnAfterClose(isCancel);
	// NO because we may have opened a new one in post-actions ! // this->currentCustomGamePopup = CUSTOM_GAME_POPUP_TYPES::CGPT_NONE;
	this->isClosed = true;
}

