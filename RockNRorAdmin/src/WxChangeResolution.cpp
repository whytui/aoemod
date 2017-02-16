
#include "WxChangeResolution.h"


wxBEGIN_EVENT_TABLE(WxChangeResolution, wxDialog)
//EVT_MENU(ID_CR_CANCEL, WxChangeResolution::OnCancel)
wxEND_EVENT_TABLE()


WxChangeResolution::WxChangeResolution(wxWindow *parent, const wxString& title, const wxSize& size) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit(_T(""), 0, 0);
}


WxChangeResolution::WxChangeResolution(wxWindow *parent, const wxString& title, const wxSize& size, std::wstring EXEFileName, unsigned long int resX, unsigned long int resY) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit(EXEFileName, resX, resY);
}


WxChangeResolution::~WxChangeResolution() {
}


void WxChangeResolution::ConstructorInit(std::wstring EXEFileName, unsigned long int x, unsigned long int y) {
	this->gameFileName = EXEFileName;
	this->newResolutionX = x;
	this->newResolutionY = y;
	this->dataDRSFileName = _T("");
	this->data2DRSFileName = _T("");
	// UI
	this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP

	this->MainArea = new wxBoxSizer(wxVERTICAL);
	this->GameFileArea = new wxBoxSizer(wxHORIZONTAL);
	this->ButtonsArea = new wxBoxSizer(wxHORIZONTAL);
	this->grdResolution = new wxFlexGridSizer(2, 5, 20);

	this->edtGameFileName = new wxTextCtrl(this, wxID_ANY, this->gameFileName, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	if (this->gameFileName.empty()) {
		this->edtGameFileName->SetBackgroundColour(*wxRED);
	} else {
		this->edtGameFileName->SetBackgroundColour(*wxGREEN);
	}
	
	this->edtResolutionX = new wxTextCtrl(this, CR_Components_IDs::ID_CR_EDIT_X, std::to_wstring(this->newResolutionX));
	this->edtResolutionY = new wxTextCtrl(this, CR_Components_IDs::ID_CR_EDIT_Y, std::to_wstring(this->newResolutionY));

	this->btnSelectGameFile = new wxButton(this, CR_Components_IDs::ID_CR_SELECT_GAME_FILE, _T("Select game EXE"));
	this->btnOK = new wxButton(this, CR_Components_IDs::ID_CR_OK, _T("OK"));
	this->btnCancel = new wxButton(this, CR_Components_IDs::ID_CR_CANCEL, _T("Cancel"));
	this->btnSelectDRSData = new wxButton(this, CR_Components_IDs::ID_CR_SELECT_DATA, _T("Select data DRS file"));
	this->btnSelectDRSData2 = new wxButton(this, CR_Components_IDs::ID_CR_SELECT_DATA2, _T("Select data2 DRS file"));
	this->btnSelectDRSData->SetBackgroundColour(*wxRED);
	this->btnSelectDRSData2->SetBackgroundColour(*wxRED);

	Connect(CR_Components_IDs::ID_CR_SELECT_GAME_FILE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxChangeResolution::OnSelectGameEXE));
	Connect(CR_Components_IDs::ID_CR_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxChangeResolution::OnOK));
	Connect(CR_Components_IDs::ID_CR_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxChangeResolution::OnCancel));
	Connect(CR_Components_IDs::ID_CR_SELECT_DATA, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxChangeResolution::OnSelectData));
	Connect(CR_Components_IDs::ID_CR_SELECT_DATA2, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxChangeResolution::OnSelectData2));

	this->GameFileArea->Add(this->btnSelectGameFile, 0, wxALIGN_LEFT);
	this->GameFileArea->Add(this->edtGameFileName, 1, wxEXPAND);

	this->grdResolution->Add(new wxStaticText(this, wxID_ANY, _T("Horizontal resolution")), 1, wxEXPAND);
	this->grdResolution->Add(new wxStaticText(this, wxID_ANY, _T("Vertical resolution")), 1, wxEXPAND);
	this->grdResolution->Add(this->edtResolutionX, 1, wxEXPAND);
	this->grdResolution->Add(this->edtResolutionY, 1, wxEXPAND);
	this->grdResolution->AddSpacer(30);
	this->grdResolution->AddSpacer(30);
	this->grdResolution->Add(new wxStaticText(this, wxID_ANY, _T("data/interfac.drs file for this resolution")), 1, wxEXPAND);
	this->grdResolution->Add(new wxStaticText(this, wxID_ANY, _T("data2/interfac.drs file for this resolution")), 1, wxEXPAND);
	this->grdResolution->Add(new wxStaticText(this, wxID_ANY, _T("Pick the DRS file (for data directory)\nthat corresponds to the chosen resolution"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE), 1, wxEXPAND);
	this->grdResolution->Add(new wxStaticText(this, wxID_ANY, _T("Pick the DRS file (for data2 directory)\nthat corresponds to the chosen resolution")), 1, wxEXPAND);
	this->grdResolution->Add(this->btnSelectDRSData, 1, wxEXPAND);
	this->grdResolution->Add(this->btnSelectDRSData2, 1, wxEXPAND);

	this->ButtonsArea->Add(btnOK, 0, wxALIGN_CENTER);
	this->ButtonsArea->AddSpacer(10);
	this->ButtonsArea->Add(btnCancel, 0, wxALIGN_CENTER);

	// Main layout
	this->MainArea->AddSpacer(5);
	this->MainArea->Add(this->GameFileArea, 0, wxEXPAND);
	this->MainArea->AddSpacer(5);
	this->MainArea->Add(new wxTextCtrl(this, wxID_ANY, _T("Please type the Resolution you want to use in Rise of Rome\n")
		_T("Then, select the two DRS files for the resolution you chose. You can find some at these locations\n")
		_T("http://aoe.heavengames.com/dl-php/showfile.php?fileid=2554 (1280x1024)\n")
		_T("http://aoe.heavengames.com/dl-php/showfile.php?fileid=2481 (1920x1080)\n")
		_T("http://aoe.heavengames.com/dl-php/showfile.php?fileid=2522 (1920x1200)\n")
		_T("Each set it composed of 2 .DRS files, one for \"data\" directory, one for \"data2\" directory.\n")
		_T("Just select the two files for data/data2 directories according to their names.\n")
		_T("Then click on OK and RockNRorAdmin will patch the game automatically (this will preserve original interfac.drs files).\n")
		_T("Choosing inappropriate DRS files may cause bad display."),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY
		), 1, wxEXPAND);
	this->MainArea->AddSpacer(15);
	this->MainArea->Add(this->grdResolution, 1, wxEXPAND);
	this->MainArea->AddSpacer(5);
	this->MainArea->Add(this->ButtonsArea, 0, wxALIGN_BOTTOM);
	this->SetSizer(MainArea);
}


void WxChangeResolution::OnCancel(wxCommandEvent& event)
{
	this->EndModal(wxCANCEL);
}


void WxChangeResolution::OnOK(wxCommandEvent& event)
{
	if (this->gameFileName.empty() || this->dataDRSFileName.empty() || this->data2DRSFileName.empty()) {
		wxMessageBox(_T("Please first select game EXE file and both DRS files"), _T("Change resolution"), wxICON_WARNING | wxOK, this);
		return;
	}
	// Get / control values
	unsigned long int resX, resY;
	resX = _wtoi(this->edtResolutionX->GetValue().c_str());
	resY = _wtoi(this->edtResolutionY->GetValue().c_str());
	if ((resX == 0) || (resY == 0)) {
		wxMessageBox(_T("Please type valid resolution X/Y values"), _T("Change resolution"), wxICON_WARNING | wxOK, this);
		return;
	}
	// Update public variables
	this->newResolutionX = resX;
	this->newResolutionY = resY;

	this->EndModal(wxOK);
}


void WxChangeResolution::OnSelectGameEXE(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Choose game file EXE to patch"), "", "", "EXE files (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	this->btnSelectGameFile->SetBackgroundColour(*wxGREEN);
	this->edtGameFileName->SetBackgroundColour(*wxGREEN);
	this->gameFileName = openFileDialog.GetPath();
	this->edtGameFileName->SetValue(this->gameFileName);
}


void WxChangeResolution::OnSelectData(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Choose DRS file for data directory"), "", "", "DRS files (*.drs)|*.drs", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	this->btnSelectDRSData->SetBackgroundColour(*wxGREEN);
	this->dataDRSFileName = openFileDialog.GetPath();
}


void WxChangeResolution::OnSelectData2(wxCommandEvent& event) {
	wxFileDialog openFileDialog(this, _("Choose DRS file for data2 directory"), "", "", "DRS files (*.drs)|*.drs", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	this->btnSelectDRSData2->SetBackgroundColour(*wxGREEN);
	this->data2DRSFileName = openFileDialog.GetPath();
}

