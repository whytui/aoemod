#pragma once
#include <string>
#include "mystrings.h"



//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif



class WxDrsEditorFileEdit : public wxDialog
{
public:
	WxDrsEditorFileEdit(wxWindow *parent, const wxString& title, const wxSize& size, long int initialFileId, std::string initialType);
	~WxDrsEditorFileEdit();

	// True if the user clicked cancel button or closed the window (did NOT click OK)
	bool cancelled;
	long int initialFileId; // (unmodified) initial value
	long int resultFileId;
	std::string resultFileType;

	// Return true if a MODIFIED and VALID file Id value has been set and accepted (OK button).
	bool FileIdWasModified() const {
		return !this->cancelled && (this->initialFileId != this->resultFileId) && (this->resultFileId > 0);
	}
private:

	wxBoxSizer *mainArea;
	wxBoxSizer *editArea;
	wxBoxSizer *btnArea;
	wxButton *btnOk;
	wxButton *btnCancel;
	wxTextCtrl *edtFileId;
	wxTextCtrl *edtFileType;
	wxStaticText *lblFileId;
	wxStaticText *lblFileType;

	void ConstructorInit();

	void OnBtnOk(wxCommandEvent& event);
	void OnBtnCancel(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

};

enum
{
	ID_btnOK = 1,
	ID_btnCancel = 2
};


#pragma warning(pop)
