
#include "WxBinSeqWindow.h"

wxBEGIN_EVENT_TABLE(WxBinSeqWindow, wxDialog)
wxEND_EVENT_TABLE()


WxBinSeqWindow::WxBinSeqWindow(wxWindow *parent, const wxString& title, const wxSize& size) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT) {
	this->ConstructorInit(NULL);
}

WxBinSeqWindow::WxBinSeqWindow(wxWindow *parent, const wxString& title, const wxSize& size, EmpiresX_API *api, BINSEQ_CATEGORIES category, int seqDefIndex) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT) {
	this->ConstructorInit(api);
	this->category = category;
	this->seqDefIndex = seqDefIndex;
	this->CreateUIContent(category, seqDefIndex);
}



WxBinSeqWindow::~WxBinSeqWindow() {

}


void WxBinSeqWindow::ConstructorInit(EmpiresX_API *api) {
	this->e_api = api;
	this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP
	modified = false;
}


void WxBinSeqWindow::CreateUIContent(BINSEQ_CATEGORIES category, int seqDefIndex) {
	this->MainArea = new wxBoxSizer(wxVERTICAL);
	this->btnClose = new wxButton(this, BSW_ComponentIDs::ID_BSW_CLOSE, _T("Close"), wxPoint(80, 80), wxDefaultSize);
	if (!e_api) {
		this->MainArea->Add(this->btnClose, 0, wxALIGN_CENTER);
		this->SetSizer(MainArea); 
		return;
	}
	EmpiresXFileManager *e_manager = e_api->GetAOEManager();
	e_manager->GetSequenceDefDescription(category, seqDefIndex);
	

	wchar_t buf[100];
	long int offset = e_manager->GetSequenceOffset(category, seqDefIndex);
	swprintf_s(buf, _T("offset = 0x%08X (%ld)"), offset, offset);

	this->edtSeqName = new wxTextCtrl(this, wxID_ANY,
		e_manager->GetSequenceDefName(category, seqDefIndex),
		wxPoint(20, 20), wxSize(50, 30), wxTE_READONLY);
	std::wstring tmpString = _T("Category=");
	tmpString += to_wstring(category);
	tmpString += _T(" ; index=");
	tmpString += to_wstring(seqDefIndex);

	this->TechInfosArea = new wxBoxSizer(wxHORIZONTAL);

	this->edtSeqTechReference = new wxTextCtrl(this, wxID_ANY,
		tmpString,
		wxPoint(20, 20), wxSize(50, 30), wxTE_READONLY);
	this->edtSeqAddress = new wxTextCtrl(this, /*wxID_ANY*/ BSW_ComponentIDs::ID_BSW_EDT_ADDRESS,
		buf,
		wxPoint(20, 20), wxSize(200, 30), wxTE_READONLY);
	this->edtSeqDesc = new wxTextCtrl(this, wxID_ANY,
		e_manager->GetSequenceDefDescription(category, seqDefIndex),
		wxPoint(20, 20), wxSize(50, 80), wxTE_MULTILINE | wxTE_READONLY);

	this->edtSeqFileContent = new wxTextCtrl(this, wxID_ANY,
		e_manager->DumpFileSeqContent(this->category, this->seqDefIndex),
		wxPoint(20, 20), wxSize(200, 30), wxTE_READONLY);
	this->edtSeqDefContent = new wxTextCtrl(this, wxID_ANY,
		e_manager->DumpAllDefSequencesContent(this->category, this->seqDefIndex),
		wxPoint(20, 20), wxSize(200, 60), wxTE_MULTILINE | wxTE_READONLY);
	this->edtExportToBinPatcherIni = new wxTextCtrl(this, wxID_ANY,
		_T("aaa\nfff"),
		wxPoint(20, 20), wxSize(200, 60), wxTE_MULTILINE | wxTE_READONLY);
	this->edtExportToBinPatcherIni->SetMinSize(wxSize(300, 60));
	this->lblSeqFileContent = new wxStaticText(this, wxID_ANY, _T("Current file content:"));
	this->lblSeqDefContent = new wxStaticText(this, wxID_ANY, _T("Sequence definitions:"));

	this->btnForceReset = new wxButton(this, BSW_ComponentIDs::ID_BSW_FORCE_RESET, _T("Force reset"), wxPoint(80, 80), wxDefaultSize);
	this->btnGenerateBinaryPatcherIni = new wxButton(this, BSW_ComponentIDs::ID_BSW_GEN_BINPATCHER_INI, _T("Generate BinaryPatcher Ini code"), wxPoint(80, 80), wxDefaultSize);

	this->TechInfosArea->Add(this->edtSeqTechReference, 1, wxEXPAND);
	this->TechInfosArea->Add(this->edtSeqAddress, 1, wxEXPAND);
	this->MainArea->Add(this->edtSeqName, 0, wxEXPAND);
	this->MainArea->Add(this->TechInfosArea, 0, wxEXPAND);
	this->MainArea->Add(this->edtSeqDesc, 0, wxEXPAND);
	this->MainArea->Add(this->lblSeqFileContent, 0, wxALIGN_CENTER);
	this->MainArea->Add(this->edtSeqFileContent, 0, wxEXPAND);
	this->MainArea->Add(this->lblSeqDefContent, 0, wxALIGN_CENTER);
	this->MainArea->Add(this->edtSeqDefContent, 1, wxEXPAND);
	this->MainArea->Add(this->btnForceReset, 0, wxALIGN_CENTER);
	this->MainArea->Add(this->btnGenerateBinaryPatcherIni, 0, wxALIGN_CENTER);
	this->MainArea->Add(this->edtExportToBinPatcherIni, 1, wxEXPAND);
	this->MainArea->Add(this->btnClose, 0, wxALIGN_CENTER);
	this->edtExportToBinPatcherIni->Hide();

	this->SetDefaultItem(btnClose);
	this->SetSizer(MainArea);

	// Events
	Connect(BSW_ComponentIDs::ID_BSW_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxBinSeqWindow::OnClose));
	Connect(BSW_ComponentIDs::ID_BSW_FORCE_RESET, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxBinSeqWindow::OnForceReset));
	Connect(BSW_ComponentIDs::ID_BSW_GEN_BINPATCHER_INI, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxBinSeqWindow::OnGenBinPatcherIniClick));
}


void WxBinSeqWindow::OnClose(wxCommandEvent& event)
{
	this->Close(false);
}


void WxBinSeqWindow::OnForceReset(wxCommandEvent& event) {
	int mresult = wxMessageBox(_T("Are you sure you want to reset this sequence to default ? This may corrupt the file."), "Customize AOE", wxYES_NO | wxICON_ASTERISK, this);
	if (mresult == wxNO) {
		return;
	}
	//e_api->ResetToDefault
	EmpiresXFileManager *e_manager = e_api->GetAOEManager();
	e_manager->SetUserSelectionToDefault(this->category, this->seqDefIndex);
	try {
		e_manager->SwitchToWriteMode();
	}
	catch (std::exception &e) {
		wxMessageBox(e.what(), _T("Failed opening file in write mode"), wxOK || wxICON_ERROR, this);
	}
	int writtenBytes = e_manager->SetFileSelection(this->category, this->seqDefIndex);
	if (writtenBytes > 0) {
		this->modified = true;
		wxMessageBox(_T("Wrote changes to file. Hope we did not corrupt it."), _T("Operation successful"), wxOK || wxICON_INFORMATION, this);
	}
	else {
		wxMessageBox(_T("Could not write information in file."), _T("Operation failed"), wxOK || wxICON_ERROR, this);
	}
	try {
		e_manager->SwitchToReadMode();
	}
	catch (std::exception &e) {
		wxMessageBox(e.what(), _T("Failed restoring read-only mode"), wxOK || wxICON_ERROR, this);
	}
}


void WxBinSeqWindow::OnGenBinPatcherIniClick(wxCommandEvent& event) {
	if (!e_api) { return; }
	EmpiresXFileManager *e_manager = e_api->GetAOEManager();
	if (!e_manager) { return; }
	std::wstring s = e_manager->ExportToBinaryPatcherIni(this->category, this->seqDefIndex);
	// Warinng: use SetValue and not SetLabel or SetLabelText because they are bugged (do not work with multiline)
	this->edtExportToBinPatcherIni->SetValue(s);
	this->edtExportToBinPatcherIni->Show();
	this->MainArea->Layout();
}

