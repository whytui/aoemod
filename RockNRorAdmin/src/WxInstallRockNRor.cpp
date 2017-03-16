
#include "WxInstallRockNRor.h"


wxBEGIN_EVENT_TABLE(WxInstallRockNRor, wxDialog)
//EVT_MENU(ID_CR_CANCEL, WxChangeResolution::OnCancel)
wxEND_EVENT_TABLE()


WxInstallRockNRor::WxInstallRockNRor(wxWindow *parent, const wxString& title, const wxSize& size) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit(_T(""));
}


WxInstallRockNRor::WxInstallRockNRor(wxWindow *parent, const wxString& title, const wxSize& size, std::wstring EXEFileName, std::wstring initialSrcDirectory) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit(EXEFileName);
	this->SetSourceDirectory(initialSrcDirectory);
}


WxInstallRockNRor::~WxInstallRockNRor() {
}


void WxInstallRockNRor::ConstructorInit(std::wstring EXEFileName) {
	this->gameFileName = EXEFileName;
	this->pathToResourceFiles = _T("");
	this->hasMissingFile = true;
	// UI
	this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP

	this->MainArea = new wxBoxSizer(wxVERTICAL);
	this->GameFileArea = new wxBoxSizer(wxHORIZONTAL);
	this->ButtonsArea = new wxBoxSizer(wxHORIZONTAL);
	this->grdResourceFiles = new wxFlexGridSizer(2, 5, 20);

	this->edtGameFileName = new wxTextCtrl(this, wxID_ANY, this->gameFileName, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	if (this->gameFileName.empty()) {
		this->edtGameFileName->SetBackgroundColour(*wxRED);
	} else {
		this->edtGameFileName->SetBackgroundColour(*wxGREEN);
	}
	

	this->btnSelectGameFile = new wxButton(this, ICR_Components_IDs::ID_ICR_SELECT_GAME_FILE, _T("Select game EXE"));
	this->btnOK = new wxButton(this, ICR_Components_IDs::ID_ICR_OK, _T("OK"));
	this->btnCancel = new wxButton(this, ICR_Components_IDs::ID_ICR_CANCEL, _T("Cancel"));
	this->btnSelectRORAPI_DLL = new wxButton(this, ICR_Components_IDs::ID_ICR_SELECT_RORAPI_DLL, _T("Select ROR_API.dll"));
	this->btnSelectRORAPI_DLL->SetBackgroundColour(wxColor("red"));

	Connect(ICR_Components_IDs::ID_ICR_SELECT_GAME_FILE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxInstallRockNRor::OnSelectGameEXE));
	Connect(ICR_Components_IDs::ID_ICR_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxInstallRockNRor::OnOK));
	Connect(ICR_Components_IDs::ID_ICR_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxInstallRockNRor::OnCancel));
	Connect(ICR_Components_IDs::ID_ICR_SELECT_RORAPI_DLL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxInstallRockNRor::OnSelectRockNRor_DLL));

	this->GameFileArea->Add(this->btnSelectGameFile, 0, wxALIGN_LEFT);
	this->GameFileArea->Add(this->edtGameFileName, 1, wxEXPAND);


	this->edtRockNRor_DLL_FilePath = new wxTextCtrl(this, ICR_Components_IDs::ID_ICR_EDIT_ROCKNROR_DLL_PATH, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	this->edtROR_API_DLL_FilePath = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	this->edtROR_API_conf_FilePath = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	this->edtWndMode_FilePath = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	this->chkOverwriteFiles = new wxCheckBox(this, wxID_ANY, _T("Replace existing files"), wxDefaultPosition, wxDefaultSize, 0);
	this->chkOverwriteFiles->Set3StateValue(wxCheckBoxState::wxCHK_CHECKED);
	this->chkCreateExeForRockNRor = new wxCheckBox(this, wxID_ANY, _T("Create a dedicated EXE for RockNRor mod"), wxDefaultPosition, wxDefaultSize, 0);
	this->chkCreateExeForRockNRor->Set3StateValue(wxCheckBoxState::wxCHK_CHECKED);

	this->grdResourceFiles->Add(this->btnSelectRORAPI_DLL, 1, wxEXPAND);
	this->grdResourceFiles->Add(new wxStaticText(this, wxID_ANY, _T("Click to select ROR_API.dll from the installation archive you downloaded (please first extract all files from archive)."),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE), 1, wxEXPAND);
	this->grdResourceFiles->AddSpacer(20);
	this->grdResourceFiles->AddSpacer(20);
	this->grdResourceFiles->Add(new wxStaticText(this, wxID_ANY, _T("Path for RockNRor.dll")), 1, wxEXPAND);
	this->grdResourceFiles->Add(this->edtRockNRor_DLL_FilePath, 1, wxEXPAND);
	this->grdResourceFiles->Add(new wxStaticText(this, wxID_ANY, _T("Path for ROR_API.dll")), 1, wxEXPAND);
	this->grdResourceFiles->Add(this->edtROR_API_DLL_FilePath, 1, wxEXPAND);
	this->grdResourceFiles->Add(new wxStaticText(this, wxID_ANY, _T("Path for ROR_API.conf")), 1, wxEXPAND);
	this->grdResourceFiles->Add(this->edtROR_API_conf_FilePath, 1, wxEXPAND);
	this->grdResourceFiles->Add(new wxStaticText(this, wxID_ANY, _T("Path for wndmode.dll")), 1, wxEXPAND);
	this->grdResourceFiles->Add(this->edtWndMode_FilePath, 1, wxEXPAND);
	this->grdResourceFiles->AddSpacer(20);
	this->grdResourceFiles->Add(this->chkOverwriteFiles);
	this->grdResourceFiles->AddSpacer(20);
	this->grdResourceFiles->Add(this->chkCreateExeForRockNRor);

	this->ButtonsArea->Add(this->btnOK, 0, wxALIGN_CENTER);
	this->ButtonsArea->AddSpacer(10);
	this->ButtonsArea->Add(this->btnCancel, 0, wxALIGN_CENTER);

	// Main layout
	this->MainArea->AddSpacer(5);
	this->MainArea->Add(new wxTextCtrl(this, wxID_ANY, _T("This screen allows you to install RockNRor (requires ROR 1.0a).\n")
		_T("-------------------------------------------------\n")
		_T("First make sure all files from \"RockNRorAdmin\" archive have been extracted into 1 (single) directory. You can use any directory, it is only temporary.\n")
		_T("Then click the \"Select ROR_API.dll\" button, and choose ROR_API.dll from the temporary directory that contains all files from the archive.\n\n")
		_T("Also select the game EXE file to patch (if not have not selected it yet).\n")
		_T("If you want to preserve your original EmpiresX.EXE, just copy-paste it manually into a new file EmpiresX_xxx.EXE.\n")
		_T("=> You can then select EmpiresX_xxx.EXE, RockNRor will be installed on this file and other EXE files will remain unchanged.\n\n")
		_T("When you're ready, click on OK and RockNRorAdmin will install RockNRor mod automatically.\n\n")
		_T("Tip: after installation, open RockNRor.xml and RockNRor_civs.xml in a text editor (as admin if necessary) to configure RockNRor and additional civilzations. These files will be located in your game installation directory.\n"),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY
		), 1, wxEXPAND);
	this->MainArea->AddSpacer(15);
	this->MainArea->Add(this->grdResourceFiles, 1, wxEXPAND);
	this->MainArea->AddSpacer(5);
	this->MainArea->Add(this->GameFileArea, 0, wxEXPAND);
	this->MainArea->AddSpacer(5);
	this->MainArea->Add(this->ButtonsArea, 0, wxALIGN_BOTTOM);
	this->SetSizer(MainArea);
}


void WxInstallRockNRor::OnCancel(wxCommandEvent& event)
{
	this->EndModal(wxCANCEL);
}


void WxInstallRockNRor::OnOK(wxCommandEvent& event)
{
	if (this->gameFileName.empty()) {
		wxMessageBox(_T("Please first select game EXE file to patch."), _T("Install RockNRor"), wxICON_WARNING | wxOK, this);
		return;
	}
	if (this->pathToResourceFiles.empty()) {
		wxMessageBox(_T("Please first select ROR_API.dll file from \"source\" directory."), _T("Install RockNRor"), wxICON_WARNING | wxOK, this);
		return;
	}
	if (this->hasMissingFile) {
		wxMessageBox(_T("Some installation files are missing. Make sure you selected the correct file and that you extracted all files from the installation archive."), _T("Install RockNRor"), wxICON_WARNING | wxOK, this);
		return;
	}

	this->EndModal(wxOK);
}


void WxInstallRockNRor::OnSelectGameEXE(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Choose game file EXE to patch"), "", "", "EXE files (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	this->btnSelectGameFile->SetBackgroundColour(*wxGREEN);
	this->edtGameFileName->SetBackgroundColour(*wxGREEN);
	this->gameFileName = openFileDialog.GetPath();
	this->edtGameFileName->SetValue(this->gameFileName);
}


void WxInstallRockNRor::OnSelectRockNRor_DLL(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Choose ROR_API.dll file from installation directory"), "", "", "ROR_API.dll (ROR_API.dll)|ROR_API.dll", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	
	this->pathToResourceFiles = openFileDialog.GetDirectory();
	this->UpdateAfterSelectRockNRor_DLL();
}


bool WxInstallRockNRor::CheckTextCtrlPath(wxTextCtrl *edt) {
	bool valid = CheckFileExistence(edt->GetValue().t_str());
	if (!valid) {
		edt->SetBackgroundColour(*wxRED);
	} else {
		edt->SetBackgroundColour(*wxGREEN);
	}
	edt->Refresh(); // Another bug in wxwidgets... http://trac.wxwidgets.org/ticket/14560
	return valid;
}


void WxInstallRockNRor::SetSourceDirectory(std::wstring initialSrcDirectory) {
	this->pathToResourceFiles = initialSrcDirectory;
	this->UpdateAfterSelectRockNRor_DLL();
}


void WxInstallRockNRor::UpdateAfterSelectRockNRor_DLL() {
	// Fill text controls so that user sees source files path
	this->edtRockNRor_DLL_FilePath->SetValue(this->pathToResourceFiles + _T("\\RockNRor\\RockNRor.dll"));
	this->edtROR_API_DLL_FilePath->SetValue(this->pathToResourceFiles + _T("\\ROR_API.dll"));
	this->edtROR_API_conf_FilePath->SetValue(this->pathToResourceFiles + _T("\\ROR_API.conf"));
	this->edtWndMode_FilePath->SetValue(this->pathToResourceFiles + _T("\\wndmode.dll"));

	bool allFilesOK = true;
	allFilesOK = this->CheckTextCtrlPath(this->edtRockNRor_DLL_FilePath) && allFilesOK;
	allFilesOK = this->CheckTextCtrlPath(edtROR_API_DLL_FilePath) && allFilesOK;
	allFilesOK = this->CheckTextCtrlPath(edtROR_API_conf_FilePath) && allFilesOK;
	allFilesOK = this->CheckTextCtrlPath(edtWndMode_FilePath) && allFilesOK;
	this->hasMissingFile = !allFilesOK;
	if (this->hasMissingFile) {
		this->btnSelectRORAPI_DLL->SetBackgroundColour(*wxRED);
	} else {
		this->btnSelectRORAPI_DLL->SetBackgroundColour(*wxGREEN);
	}
}

