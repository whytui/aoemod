#include "WxDrsEditorFileEdit.h"

wxBEGIN_EVENT_TABLE(WxDrsEditorFileEdit, wxDialog)
wxEND_EVENT_TABLE()

WxDrsEditorFileEdit::WxDrsEditorFileEdit(wxWindow *parent, const wxString& title, const wxSize& size, long int initialFileId, std::string initialType) :
wxDialog(parent, 0, title, wxDefaultPosition, size) {
	this->initialFileId = initialFileId;
	this->resultFileId = initialFileId;
	this->resultFileType = initialType;
	this->cancelled = true;
	this->ConstructorInit();
}

WxDrsEditorFileEdit::~WxDrsEditorFileEdit() {
}


void WxDrsEditorFileEdit::ConstructorInit() {
	//this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP
	this->mainArea = new wxBoxSizer(wxVERTICAL);
	this->editArea = new wxBoxSizer(wxHORIZONTAL);
	this->btnArea = new wxBoxSizer(wxHORIZONTAL);

	this->lblFileId = new wxStaticText(this, -1, _T("File ID"));
	this->lblFileType = new wxStaticText(this, -1, _T("File type"));
	this->edtFileId = new wxTextCtrl(this, -1, wxEmptyString);
	this->edtFileType = new wxTextCtrl(this, -1, wxEmptyString);

	this->btnOk = new wxButton(this, ID_btnOK, _T("OK"));
	this->btnCancel = new wxButton(this, ID_btnCancel, _T("Cancel"));

	Connect(ID_btnOK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditorFileEdit::OnBtnOk));
	Connect(ID_btnCancel, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditorFileEdit::OnBtnCancel));

	this->editArea->Add(this->lblFileId, 0, wxALIGN_CENTER);
	this->editArea->AddSpacer(5);
	this->editArea->Add(this->edtFileId);
	this->editArea->AddSpacer(20);
	this->editArea->Add(this->lblFileType, 0, wxALIGN_CENTER);
	this->editArea->AddSpacer(5);
	this->editArea->Add(this->edtFileType);

	this->btnArea->Add(this->btnOk, 0, wxALIGN_BOTTOM);
	this->btnArea->AddSpacer(10);
	this->btnArea->Add(this->btnCancel, 0, wxALIGN_BOTTOM);

	this->mainArea->AddSpacer(20);
	this->mainArea->Add(this->editArea, 0, wxALIGN_CENTER);
	this->mainArea->AddSpacer(30);
	this->mainArea->Add(this->btnArea, 0, wxALIGN_CENTER);

	this->edtFileId->SetValue(std::to_string(this->resultFileId));
	this->edtFileType->SetValue(this->resultFileType);
	this->SetSizer(this->mainArea);
}


void WxDrsEditorFileEdit::OnBtnOk(wxCommandEvent& event) {
	std::string tmpId = this->edtFileId->GetValue();
	int resFileId = StrToInt(tmpId.c_str(), -1);
	if (resFileId <= 0) {
		wxMessageBox(_T("File ID must be >0"), _T("DRS Editor"), wxICON_ERROR);
		return;
	}
	std::string resFileType = this->edtFileType->GetValue();
	if (resFileType.empty()) {
		wxMessageBox(_T("File type can't be empty"), _T("DRS Editor"), wxICON_ERROR);
		return;
	}
	if (resFileType.size() > 4) {
		wxMessageBox(_T("File type can't be more than 4 chars"), _T("DRS Editor"), wxICON_ERROR);
		return;
	}
	if (resFileType.size() < 3) {
		wxMessageBox(_T("File type should be 3 chars long"), _T("DRS Editor"), wxICON_ERROR);
		return;
	}
	if ((resFileType.size() == 4) && (resFileType.at(3) != 'a')) {
		wxMessageBox(_T("4-chars extension is only allowed with 'a' ending character (like 'bina')"), _T("DRS Editor"), wxICON_ERROR);
		return;
	}

	this->resultFileId = resFileId;
	this->resultFileType = resFileType;
	this->cancelled = false;
	this->Close();
}


void WxDrsEditorFileEdit::OnBtnCancel(wxCommandEvent& event) {
	this->Close();
}
