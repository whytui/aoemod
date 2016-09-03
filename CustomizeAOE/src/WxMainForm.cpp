
#include "WxMainForm.h"

wxBEGIN_EVENT_TABLE(WxMainForm, wxFrame)
EVT_MENU(ID_Select, WxMainForm::OnSelect)
EVT_MENU(ID_SelectFromReg, WxMainForm::OnSelectFromRegistry)
EVT_MENU(ID_RunGame, WxMainForm::OnRunGame)
EVT_MENU(ID_EditGameFile, WxMainForm::OnEditGameFile)
EVT_MENU(ID_CloseGameFile, WxMainForm::OnCloseGameFile)
EVT_MENU(ID_InstallCustomROR, WxMainForm::OnInstallCustomROR)
EVT_MENU(ID_InstallCustomResolution, WxMainForm::OnInstallCustomResolution)
EVT_MENU(ID_InstallSuggestedOptions, WxMainForm::OnInstallSuggestedOptions)
EVT_MENU(ID_FixDDrawColorBug, WxMainForm::OnFixDDrawColorBug)
EVT_MENU(ID_GenTriggerDoc, WxMainForm::OnExportTriggerHTMLDoc)
EVT_MENU(ID_GenTriggerSample, WxMainForm::OnSampleTrigger)
#ifdef _DEBUG
EVT_MENU(ID_Debug, WxMainForm::OnMenuDebug)
#endif
EVT_MENU(wxID_EXIT, WxMainForm::OnExit)
EVT_MENU(wxID_ABOUT, WxMainForm::OnAbout)
wxEND_EVENT_TABLE()


WxMainForm::WxMainForm(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	// We MUST control that initializations do not raise errors or the UI won't show up and the process will freeze.
	try {
		this->objAOE = new CAOE_UI();
		this->e_api = objAOE->GetAPI();
	}
	catch (std::exception &e) {
		std::string s = "An initialization error occurred.\n";
		s += e.what();
		wxMessageBox(s);
		CreateStatusBar();
		SetStatusText("The program could not initialize properly.");
		return;
	}

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Select, "Select AOE/ROR file...\tCtrl-O", "Select Rise of Rome executable file");
	menuFile->Append(ID_SelectFromReg, "Select AOE/ROR file from registry\tCtrl-R", "Automatically select file from registry information");
	menuFile->Append(ID_RunGame, "Run selected game file\tF9", "Run game using selected EXE");
	menuFile->Append(ID_EditGameFile, "Edit game file\tCtrl-E", "Open CustomizeAOE file edition screen.");
	menuFile->Append(ID_CloseGameFile, "Close game file\tCtrl-W", "Close currently selected file to unlock it.");
	menuFile->AppendSeparator();
	menuFile->Append(ID_InstallCustomROR, "Install CustomROR...", "Add CustomROR plugin to your AOE - ROR installation.");
	menuFile->Append(ID_InstallCustomResolution, "Change game resolution...", "Update game and add custom files to handle custom resolutions.");
	menuFile->Append(ID_InstallSuggestedOptions, "Install Suggested Options", "Automatically patch currently selected file with suggested options, including 1920*1200 resolution.");
	menuFile->Append(ID_FixDDrawColorBug, "Fix DirectDraw color bug", "Updates regitry to fix DirectDraw color bug on currently selected file.");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuTriggers = new wxMenu;
	menuTriggers->Append(ID_GenTriggerDoc, "Get triggers HTML documentation");
	menuTriggers->Append(ID_GenTriggerSample, "Generate trigger samples");
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuTriggers, "&Triggers");
#ifdef _DEBUG
	wxMenu *menuDebug = new wxMenu;
	menuDebug->Append(ID_Debug, "Debug Rise of Rome");
	menuBar->Append(menuDebug, "&Debug");
#endif
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("");
	
	this->txtLog = new wxTextCtrl(this, wxID_ANY,
		_T("Welcome. Do not forget to backup you original game EXE file.\n")
		_T("You can also customize/run a copy of empiresX.exe and keep empiresX.exe unchanged.\n")
		_T("Latest Version: http://aoe.heavengames.com/dl-php/showfile.php?fileid=2464 \n")
		_T("\nPlease RUN ME AS ADMIN or most features will probably not work !\n\n"),
		wxPoint(50, 50), wxSize(50, 50), wxTE_MULTILINE | wxTE_READONLY);
}

WxMainForm::~WxMainForm() {
	if (this->e_api) {
		this->e_api->CloseEmpiresXFile(); // returns bool =is_ok
		this->e_api = NULL;
	}
	if (this->objAOE) {
		delete this->objAOE;
		this->objAOE = NULL;
	}
}

void WxMainForm::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void WxMainForm::OnAbout(wxCommandEvent& event)
{
	wxString txt;
	txt = _T("Customize AOE ");
	txt += VER_FILE_VERSION_STR;
	txt += _T("\nThis program is free to use.\nIf you use this tool to develop your programs/mods, please clearly mention it and provide a link if possible.\nForum topic: http://aoe.heavengames.com/cgi-bin/aoecgi/display.cgi?action=ct&f=17,6454,,30");
	wxMessageBox(txt, "About CustomizeAOE", wxOK | wxICON_INFORMATION);
}

void WxMainForm::OnSelect(wxCommandEvent& event)
{
	bool success = this->e_api->SetFileNameManually();
	if (success) {
		SetStatusText("Selection: " + this->e_api->GetFileName());
		this->FileOpen();
	} else {
		SetStatusText("Failed to open the file");
	}
}

void WxMainForm::OnSelectFromRegistry(wxCommandEvent& event)
{
	bool success = this->e_api->SetFileNameFromRegistry();
	if (success) {
		SetStatusText("Selection: " + this->e_api->GetFileName());
		this->FileOpen();
	} else {
		SetStatusText("Failed to open the file");
	}
}


void WxMainForm::OnRunGame(wxCommandEvent& event)
{
	this->RunGame();
}


void WxMainForm::OnEditGameFile(wxCommandEvent& event)
{
	this->EditFile();
}


void WxMainForm::OnCloseGameFile(wxCommandEvent& event) {
	this->FileClose();
}


void WxMainForm::OnFixDDrawColorBug(wxCommandEvent& event) {
	this->FixDDrawColorBug();
}


bool WxMainForm::FileOpen(bool force) {
	bool result = false;
	if (this->e_api->GetFileName() == _T("")) { return false; }
	this->objAOE->sendMessagesToBuffer = true; // Do not display message boxes.
	if (this->objAOE->OpenEmpiresX(true, force)) {
		SetStatusText(this->e_api->GetFileVersion());
		result = true;
	} else {
		SetStatusText("Could not open file (locked or not compatible)");
	}

	this->txtLog->AppendText(this->objAOE->GetMessages());
	this->objAOE->FlushMessages();
	this->objAOE->sendMessagesToBuffer = false; // Restore "message box" mode
	return result;
}


void WxMainForm::FileClose() {
	SetStatusText("Closed current file");
	if (!this->e_api->CloseEmpiresXFile()) {
		wxMessageBox("An error occurred while closing file", "CustomizeAOE", wxOK | wxICON_ERROR);
	}
}

void WxMainForm::EditFile() {
	if (!this->e_api->IsFileOpen()) {
		SetStatusText("Error: no file is open");
		return;
	}
	std::wstring wTitle = "Customize game file " + this->e_api->GetFileName();
	WxEditFile *editFileWidget = new WxEditFile(this, wTitle, wxSize(800, 600), this->e_api);
	//this->SetCursor(wxCursor(...));
	//this->SetStatusText("Opening edition window"); // does not work properly... even with a refresh
	this->Hide();
	try {
		editFileWidget->ShowModal();
		//this->SetStatusText("");
	}
	catch (...) {
		wxMessageBox("An error occurred", "Customize AOE", wxOK | wxICON_INFORMATION);
	}
	this->Show();
	delete editFileWidget;
}


void WxMainForm::RunGame() {
	std::wstring filename = this->e_api->GetFileName();
	if (filename.empty()) { return; }
	std::wstring dirname = extractDirectory(filename);
	//filename.substr(0, filename.find_last_of(_T("/\\")));
	SetCurrentDirectory(dirname.c_str());
	// Add a fake "" in start command => fake window title. It is a workaround for start command bug.
	std::string syscmd = "start \"\" \"";
	syscmd += narrow(filename);
	syscmd += "\"";
	SetStatusText("Running game in a new process");
	system(syscmd.c_str());
}


// Add registry keys to fix direct draw bug on Win7+ versions
void WxMainForm::FixDDrawColorBug() {
	std::wstring fileNameToFix = this->e_api->GetFileName();
	if (fileNameToFix.empty()) {
		wxFileDialog openFileDialog(this, _("Choose EXE file to fix (color bug)"), "", "", "EXE files (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if (openFileDialog.ShowModal() == wxID_CANCEL) {
			return;
		}
		fileNameToFix = openFileDialog.GetPath();
	}
	if (fileNameToFix.empty()) {
		return;
	}

	if (FixRegistryForDDrawColorBug(fileNameToFix)) {
		std::wstring message = _T("Fix has been installed in windows registry for ");
		message += fileNameToFix;
		SetStatusText(message);
		message += _T("\n");
		this->txtLog->WriteText(message);
	} else {
		std::wstring message = _T("Could not install fix in windows registry for ");
		message += fileNameToFix;
		SetStatusText(message);
		message += _T("\n");
		this->txtLog->WriteText(message);
	}
}


// This automatically installs CustomROR : copies files from user-provided source dir and patches EXE file.
void WxMainForm::InstallCustomROR() {
	// Open UI for user choices
	WxInstallCustomROR *wInstallCROR = new WxInstallCustomROR(this, _T("Select installation files for customROR"), wxSize(800, 580),
		this->e_api->GetFileName());
	int result = wInstallCROR->ShowModal();
	std::wstring srcDirName = wInstallCROR->pathToResourceFiles;
	std::wstring selectedGameEXE = wInstallCROR->gameFileName;
	bool userWantsToOverwriteFiles = wInstallCROR->ReplaceFiles();

	delete wInstallCROR;
	if (result != wxOK) {
		SetStatusText(_T("Cancelled by user, did not proceed to installation."));
		return;
	}

	// Now we're ok with user inputs.
	bool openedSuccessfully = false;

	// Case when user selected another EXE (or none was initially selected)
	if (selectedGameEXE != this->e_api->GetFileName()) {
		this->e_api->CloseEmpiresXFile();
		this->e_api->SetFileName(selectedGameEXE);
		openedSuccessfully= this->e_api->OpenEmpiresXFile();
		if (!openedSuccessfully) { return; }
	}
	std::wstring ROR_filename = this->e_api->GetFileName();
	std::wstring shortMessage, logs;
	// Install files
	bool copyFilesSuccess = installCustomRORFiles(srcDirName, ROR_filename, userWantsToOverwriteFiles, shortMessage, logs);
	txtLog->AppendText(logs);
	if (!copyFilesSuccess) {
		wxMessageBox(shortMessage, "Customize AOE", wxOK | wxICON_INFORMATION);
		SetStatusText(_T("Installation failed."));
		return; // Do not continue with patches.
	}
	
	// Install EXE patches for CustomROR / ROR_API
	std::wstring message;
	bool resultPatch; // true as long as things are OK
	std::wstring patchErrors = _T("");
	if (!this->e_api->CloseEmpiresXFile()) {
		// Message in log ?
	}
	resultPatch = this->FileOpen(true);
	std::wstring version = this->e_api->GetFileVersion();
	if (resultPatch && (version.compare(GET_AOE_FILE_VERSION_LABEL(AOE_VERSION_ROR1_0C)))) {
		if (wxMessageBox(_T("This is not 1.0c Rise of Rome version. Do you want to try to install customROR anyway ?"),
			_T("Install customROR"), wxICON_WARNING | wxYES_NO) == wxNO) {
			this->txtLog->AppendText(_T("Wrong or unsupported version.\n"));
			SetStatusText(_T("Cancelled by user, did not proceed to installation."));
			resultPatch = false;
			return;
		} else {
			this->txtLog->AppendText(_T("Wrong or unsupported version, but proceed anyway.\n"));
			this->txtLog->AppendText(_T("*** If you are using another version than ROR1.0c, you need to modify ROR_API.conf manually after the installation process\n"));
			this->txtLog->AppendText(_T("*** to replace the line 'CustomROR\\CustomROR.dll by the CustomROR version that matches AOE/ROR version.\n"));
			wxMessageBox(_T("Please read log in main window for instructions to finish installation for non-ROR1.0c version"),
				_T("Install customROR"), wxICON_WARNING | wxOK);
		}
	}
	if (resultPatch) { this->txtLog->AppendText(_T("...Installing technical fixes and remove obsolete changes.\n")); }
	int writeCallResult = 0;
	if (resultPatch) {
		resultPatch = resultPatch && this->e_api->RemoveObsoletes();
		resultPatch = resultPatch && this->e_api->SetAllTechFixes();
		writeCallResult = this->e_api->WriteChangesToFile();
		if (writeCallResult < 0) {
			resultPatch = false;
			this->txtLog->AppendText(_T("Encountered an error, technical code=") + std::to_wstring(writeCallResult) + _T(".\n"));
		}
	}
	if (resultPatch) { // Technically this if is not necessary due to bool evaluation !
		this->txtLog->AppendText(_T("...Installing features: ROR_API support, Windowed Mode, MaxSelectedUnits.\n"));
		resultPatch = resultPatch && this->e_api->SetBasicCustomROROptions();
		writeCallResult = this->e_api->WriteChangesToFile();
		if (writeCallResult < 0) {
			resultPatch = false;
			this->txtLog->AppendText(_T("Encountered an error, technical code=") + std::to_wstring(writeCallResult) + _T(".\n"));
		}
	}
	if (resultPatch) { // Technically this if is not necessary due to bool evaluation !
		this->txtLog->AppendText(_T("...Installing some other useful improvements (like additional game options).\n"));
		bool patchedOtherOptions = this->e_api->SetOtherCustomROROptions();
		writeCallResult = this->e_api->WriteChangesToFile();
		if (writeCallResult < 0) {
			patchedOtherOptions = false;
			this->txtLog->AppendText(_T("Encountered an error, technical code=") + std::to_wstring(writeCallResult) + _T(".\n"));
		}
		if (!patchedOtherOptions) {
			this->txtLog->AppendText(_T("...Warning: Additional game options could not be installed. This may be due to an incompatible version.\n"));
		}
	}

	if (!resultPatch) {
		patchErrors += _T("Could not patch Rise of Rome executable. Make sure CustomizeAOE has admin rights and that destination files/directory are not locked.\n");
	} else {
		message = _T("Patched ") + ROR_filename + _T("\n");
		this->txtLog->AppendText(message);
		this->txtLog->AppendText(_T("== Please use the dedicated menu entry to change game resolution ==\n"));
		this->txtLog->AppendText(_T("== Please use \"Edit\" (CTRL-E) menu to customize other specific options/features ==\n"));
		this->txtLog->AppendText(_T("== Read CustomROR_README.txt to configure customROR ==\n"));
	}

	if (!resultPatch) {
		wxMessageBox(patchErrors.c_str(), "Customize AOE", wxOK | wxICON_INFORMATION);
		this->txtLog->AppendText(_T("CustomROR installation ended with errors/warnings.\n"));
	} else {
		this->txtLog->AppendText(_T("CustomROR has been successfully installed.\n"));
	}
	SetStatusText(_T("End of CustomROR installation process"));
}


void WxMainForm::InstallSuggestedOptions() {
	if (!this->e_api->IsFileOpen()) {
		SetStatusText("Error: no file is open");
		return;
	}
	if (this->e_api->SetSuggestedOptions()) {
		if (this->e_api->WriteChangesToFile() >= 0) {
			SetStatusText("Successfully patched game file.");
			txtLog->AppendText(_T("Successfully patched game file with suggested options.\nYou can still use \"edit game file\" menu to change (back) any option, if needed.\n"));
			return;
		}
	}
	SetStatusText("Could not patch game file.");
	txtLog->AppendText(_T("Could not install all suggested patches. Use \"edit game file\" menu to do it manually.\n"));
}


void WxMainForm::ChooseCustomResolution() {
	if (!this->e_api) {
		SetStatusText("Error: missing internal API object");
		return;
	}

	unsigned short int x, y;
	bool hasValidResolution = this->e_api->IsFileOpen();
	if ((!hasValidResolution) || (!this->e_api->GetCurrentResolution(&x, &y))) {
		x = 0;
		y = 0;
	}

	WxChangeResolution *wChangeRes = new WxChangeResolution(this, _T("Choose resolution settings"), wxSize(600, 450),
		this->e_api->GetFileName(), x, y);
	int result = wChangeRes->ShowModal();
	std::wstring dataFile = wChangeRes->dataDRSFileName;
	std::wstring data2File = wChangeRes->data2DRSFileName;
	int resX = wChangeRes->newResolutionX;
	int resY = wChangeRes->newResolutionY;
	
	if (result != wxOK) {
		delete wChangeRes;
		return;
	}

	// Case when user selected another EXE (or none was initially selected)
	if (wChangeRes->gameFileName != this->e_api->GetFileName()) {
		this->e_api->CloseEmpiresXFile();
		this->e_api->SetFileName(wChangeRes->gameFileName);
		this->e_api->OpenEmpiresXFile();
	}
	delete wChangeRes;

	// Get directory info
	std::wstring ROR_filename = _T("");
	ROR_filename = this->e_api->GetFileName();
	if (ROR_filename.empty()) {
		SetStatusText(_T("Could not retrieve target directory, did not proceed to resolution update."));
		wxMessageBox(_T("Could not retrieve target directory, abort."), "Customize AOE", wxOK | wxICON_INFORMATION);
		return;
	}
	std::wstring destDirName = extractDirectory(ROR_filename);
	std::wstring message;

	// Now change resolution
	std::wstring logs;
	bool copyFilesSuccess = installResolutionFiles(dataFile, data2File, ROR_filename, logs);
	txtLog->AppendText(logs);
	if (!copyFilesSuccess) {
		wxMessageBox(_T("Could not create all necessary files, abort."), "Customize AOE", wxOK | wxICON_INFORMATION);
		SetStatusText(_T("Installation failed."));
		return; // Do not continue with patches.
	}
	
	// Patch game file
	bool success = e_api->SetResolution(resX, resY, true);
	if (success) {
		success = success && (e_api->WriteChangesToFile(BC_RESOLUTION) >= 0);
	}
	if (success) {
		txtLog->AppendText(_T("Successfully patched target file with resolution changes.\n"));
		txtLog->AppendText(_T("Use in-game menu to set maximum resolution so that the changes become effective.\n"));
		SetStatusText("Resolution has been changed");
		// Check requirement (customROR)
		wxMessageBox(_T("To make sure custom resolution works properly, please make sure you installed CustomROR using \"file/Install CustomROR\" menu."),
			_T("Check requirement"), wxOK | wxICON_INFORMATION, this);
	} else {
		txtLog->AppendText(_T("Game file could not be patched.\n"));
		SetStatusText("Resolution could not be changed");
	}
}


void WxMainForm::OnInstallCustomROR(wxCommandEvent& event) {
	this->InstallCustomROR();
}


void WxMainForm::OnInstallCustomResolution(wxCommandEvent& event) {
	this->ChooseCustomResolution();
}


void WxMainForm::OnInstallSuggestedOptions(wxCommandEvent& event) {
	this->InstallSuggestedOptions();
}

void WxMainForm::OnExportTriggerHTMLDoc(wxCommandEvent& event) {
	this->ExportTriggerHTMLDocumentation();
}

void WxMainForm::OnSampleTrigger(wxCommandEvent& event) {
	this->OpenSampleTriggerWindow();
}

void WxMainForm::OnMenuDebug(wxCommandEvent& event) {
	this->OpenDebugWindow();
}

void WxMainForm::ExportTriggerHTMLDocumentation() {
	std::wstring wTitle = _T("Please choose a directory to export triggers documentation");
	wxDirDialog *dirDialog = new wxDirDialog(this, wTitle, wxEmptyString);
	bool success = dirDialog->ShowModal() == wxID_OK;
	wxString dir = dirDialog->GetPath();
	success = success && !dir.empty();
	delete dirDialog;
	if (!success) { return; }
	wxString filename = dir + "\\ROR_triggers_documentation.html";
	std::string htmlDoc = CR_TRIGGERS::TriggersDefinitionToHTML();
	// Write HTML documentation to a file on disk.
	FILE *f;
	int res = _tfopen_s(&f, filename.c_str(), _T("w"));// Overwrite existing file
	if (res) {
		return;
	}
	fwrite(htmlDoc.c_str(), strlen(htmlDoc.c_str()), sizeof(char), f);
	fclose(f);
	this->txtLog->AppendText(_T("Wrote file : " + filename + "\n"));
	// Automatically open the HTML file.
	system("start \"\" \"" + filename.c_str() + "\"");
}


void WxMainForm::OpenSampleTriggerWindow() {
	WxSampleTrigger *wSampleTrigger = new WxSampleTrigger(this, _T("Create trigger samples"), wxSize(650, 500));
	int result = wSampleTrigger->ShowModal();
	delete wSampleTrigger;
}


void WxMainForm::OpenDebugWindow() {
	//WxDebugMainForm wDebugForm = WxDebugMainForm(this, _T("hey"), wxSize(800, 600));
	//wDebugForm.Show(true);
	WxDebugMainForm *wDebugForm = new WxDebugMainForm(this, _T("hey"), wxSize(800, 600));
	wDebugForm->Show(true);
}
