#include "WxDrsEditor.h"

wxBEGIN_EVENT_TABLE(WxDrsEditor, wxFrame)
wxEND_EVENT_TABLE()

WxDrsEditor::WxDrsEditor(wxWindow *parent, const wxString& title, const wxSize& size, string inputFilename) : 
wxFrame(parent, 0, title, wxDefaultPosition, size) {
	this->ConstructorInit();
	if (!inputFilename.empty() && CheckFileExistence(inputFilename.c_str())) {
		this->OpenDrsFile(inputFilename);
	}
}

WxDrsEditor::~WxDrsEditor() {
}


void WxDrsEditor::ConstructorInit() {
	this->outputDrsFilename = "";

	//this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP
	this->MainArea = new wxBoxSizer(wxVERTICAL);
	this->TopArea = new wxBoxSizer(wxVERTICAL);
	this->filesArea = new wxBoxSizer(wxHORIZONTAL);
	this->filesGridArea = new wxBoxSizer(wxVERTICAL);
	this->filesButtonsArea = new wxBoxSizer(wxVERTICAL);

	/*this->fileTypesGrid = new wxGrid(this, -1, wxPoint(), wxSize(500, 200));
	this->fileTypesGrid->CreateGrid(5, 3, wxGrid::wxGridSelectCells);
	this->fileTypesGrid->DisableDragRowSize(); // User can't resize rows
	this->fileTypesGrid->SetColLabelValue(0, _T("File type"));*/

	this->filesGrid = new wxGrid(this, -1, wxPoint(), wxSize(600, 480));
	this->CreateFilesGrid();
	// TODO: make read only by columns (for loop?)

	this->btnAddFile = new wxButton(this, ID_BtnAddFile, _T("Add a file"));
	this->btnModifyFileInfo = new wxButton(this, ID_BtnModifyFileInfo, _T("Modify file info"));
	this->btnRemoveFile = new wxButton(this, ID_BtnRemoveFile, _T("Remove file"));
	Connect(ID_BtnAddFile, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditor::OnBtnAddFile));
	Connect(ID_BtnModifyFileInfo, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditor::OnBtnModifyFileInfo));
	Connect(ID_BtnRemoveFile, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditor::OnBtnRemoveFile));

	this->filesGridArea->Add(this->filesGrid);

	this->filesButtonsArea->Add(this->btnAddFile);
	this->filesButtonsArea->Add(this->btnModifyFileInfo);
	this->filesButtonsArea->Add(this->btnRemoveFile);

	this->filesArea->Add(this->filesGridArea);
	this->filesArea->Add(this->filesButtonsArea);

	this->MainArea->Add(this->TopArea);
	this->MainArea->Add(this->filesArea);

	this->SetSizer(this->MainArea);
}


void WxDrsEditor::CreateFilesGrid() {
	this->filesGrid->CreateGrid(0, 4, wxGrid::wxGridSelectionModes::wxGridSelectCells);
	this->filesGrid->SetColLabelValue(0, _T("File Type")); // Set header label for column
	this->filesGrid->SetColLabelValue(1, _T("File ID")); // Set header label for column
	this->filesGrid->SetColLabelValue(2, _T("File size")); // Set header label for column
	this->filesGrid->SetColLabelValue(3, _T("Local filename")); // Set header label for column
	this->filesGrid->SetColSize(1, 45);
	this->filesGrid->SetColSize(2, 70);
	this->filesGrid->SetColSize(3, 300);
	this->filesGrid->EnableEditing(false); // Global editable status
	this->filesGrid->DisableDragRowSize(); // User can't resize rows
}


void WxDrsEditor::RefillGrid() {
	this->filesGrid->ClearGrid();
	for (auto it = this->drsFileHelper.currentDrsWorkingSet.begin(); it != this->drsFileHelper.currentDrsWorkingSet.end(); it++) {
		DrsSetOfIncludedFiles *curSet = *it;
		const char *extName = curSet->fileType.Get4LettersExtension().GetAsCharPtr();

		for (auto it_f = curSet->myFiles.begin(); it_f != curSet->myFiles.end(); it_f++) {
			DrsIncludedFile *curFile = *it_f;
			int newRowIndex = this->filesGrid->GetRows();
			this->filesGrid->AppendRows(1, true);
			this->filesGrid->SetCellValue(newRowIndex, 0, extName);
			this->filesGrid->SetCellValue(newRowIndex, 1, to_string(curFile->fileId));
			this->filesGrid->SetCellValue(newRowIndex, 2, to_string(curFile->dataSize));
			this->filesGrid->SetCellValue(newRowIndex, 3, curFile->localFileName);
		}
	}
}


void WxDrsEditor::OpenDrsFile(string filename) {
	if (filename.empty()) { return; }
	bool res = this->drsFileHelper.ReadDrsFile(filename);
	//std::string msg = drsHelper.GetLastInfos();
	if (!res) {
		// drsHelper.GetLastErrors()
	}
	this->RefillGrid();
}


void WxDrsEditor::SetOutputDrsFilename(std::string filename) {
	this->outputDrsFilename = filename;
}


// Returns true if valid info was provided
bool WxDrsEditor::AskFileInfo(long int &inOutFileId, AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE &inOutSlpType) {
	string initialFileExt = inOutSlpType.Get4LettersExtension().GetAsCharPtr();
	initialFileExt.erase(initialFileExt.find_last_not_of(" \n\r\t") + 1); // trim right
	tolower(initialFileExt);
	if (initialFileExt == string("bin")) {
		initialFileExt = string("bina");
	}

	bool inputIsValid = false;
	string typedFileType = "";
	while (!inputIsValid) {
		int workFileId = inOutFileId;
		WxDrsEditorFileEdit *fileEdit = new WxDrsEditorFileEdit(this, _T("Add file"), wxSize(400, 180), inOutFileId, initialFileExt);
		fileEdit->ShowModal();
		if (fileEdit->cancelled) {
			inOutSlpType.SetFromDword(0);
			return false; // Cancelling is also a way out of this loop...
		}
		workFileId = fileEdit->resultFileId;
		typedFileType = fileEdit->resultFileType;
		tolower(typedFileType);
		if ((workFileId < 0) || (typedFileType.size() < 3)) {
			wxMessageBox(_T("Invalid file ID or file extension, please retry"), _T("DRS Editor"), wxICON_ERROR);
			continue; // retry...
		}

		char typeBuffer[5];
		for (int i = 0; i < 4; i++) {
			if (((int)typedFileType.size()) - 1 >= i) {
				typeBuffer[i] = typedFileType.at(i);
			} else {
				typeBuffer[i] = ' '; // especially for 3-chars extensions (most cases)
			}
		}
		inOutSlpType.SetFrom4Chars(typeBuffer);

		if (fileEdit->FileIdWasModified() && (this->drsFileHelper.FindFileWithId(workFileId) != NULL)) {
			wxMessageBox(_T("This object ID is already used, please choose another one"), _T("DRS Editor"), wxICON_ERROR);
			inputIsValid = false;
		} else {
			inputIsValid = true;
			inOutFileId = workFileId;
		}
	}
	return inputIsValid;
}


void WxDrsEditor::OnBtnAddFile(wxCommandEvent& event) {
	std::wstring wfilename = openfilename(
		_T("All files (*.*)\0*.*\0")
		_T("SLP files (*.slp)\0*.slp\0")
		_T("SHP files (*.shp)\0*.shp\0")
		_T("WAV files (*.wav)\0*.wav\0")
		);
	if (wfilename.empty()) { return; }
	std::string filename = narrow(wfilename);
	if (!CheckFileExistence(wfilename)) {
		return;
	}
	string inputFileExt = extractFileExtension(filename);
	string initialFileExt = "";
	if (!inputFileExt.empty() && (inputFileExt.size() <= 3)) {
		initialFileExt = inputFileExt;
		tolower(initialFileExt);
	}
	if (initialFileExt == string("bin")) {
		initialFileExt = string("bina");
	}

	long int typedFileId = -1;
	AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE slpType;
	if (inputFileExt.size() < 4) {
		inputFileExt.append("    "); // Make sure the c_str() is 4 char min
	}
	slpType.SetFrom4Chars(inputFileExt.c_str());
	if (!this->AskFileInfo(typedFileId, slpType)) {
		return;
	}

	DrsIncludedFile *addedFile = this->drsFileHelper.AddFile(slpType, typedFileId, filename);
	if (!addedFile) { return; }
	long int dataSize = addedFile->dataSize;

	this->filesGrid->AppendRows(1, true);
	int rowCount = this->filesGrid->GetRows();
	this->filesGrid->SetCellValue(rowCount - 1, 0, slpType.Get4LettersExtension().GetAsCharPtr());
	this->filesGrid->SetCellValue(rowCount - 1, 1, std::to_wstring(typedFileId));
	this->filesGrid->SetCellValue(rowCount - 1, 2, std::to_wstring(dataSize));
	this->filesGrid->SetCellValue(rowCount - 1, 3, wfilename);
}


void WxDrsEditor::OnBtnModifyFileInfo(wxCommandEvent& event) {
	int selRow = this->filesGrid->GetCursorRow();
	if (selRow < 0) { return; }

	// Retrieve data from grid
	std::wstring wvalueFileId = this->filesGrid->GetCellValue(selRow, 1);
	std::string valueFileId = narrow(wvalueFileId);
	std::wstring wvalueFileType = this->filesGrid->GetCellValue(selRow, 0);
	towlower(wvalueFileType); // force lowercase
	std::string valueFileType = narrow(wvalueFileType);
	int oldFileId = StrToInt(valueFileId.c_str(), -1);

	// Ask updated file info
	string inputFileExt = valueFileType;
	if (inputFileExt.size() < 4) {
		inputFileExt.append("    "); // Make sure length is at least 4 because of SetFrom4Chars(...)
	}
	long int typedFileId = oldFileId;
	AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType;
	fileType.SetFrom4Chars(inputFileExt.c_str());
	if (!this->AskFileInfo(typedFileId, fileType)) {
		return;
	}

	// Update internal data
	DrsIncludedFile *objFile = this->drsFileHelper.FindFileWithId(oldFileId);
	if (!objFile) {
		wxMessageBox(_T("Could not update internal data"), _T("DRS Editor"), wxICON_ERROR);
		return;
	}
	objFile->fileId = typedFileId;
	// Change file type in internal data
	this->drsFileHelper.ChangeFileCategory(objFile->fileId, fileType);

	// Update grid
	this->filesGrid->SetCellValue(selRow, 0, fileType.Get4LettersExtension().GetAsCharPtr());
	this->filesGrid->SetCellValue(selRow, 1, std::to_wstring(typedFileId));
}


void WxDrsEditor::OnBtnRemoveFile(wxCommandEvent& event) {
	int selRow = -1;
	wxGridCellCoordsArray totalSelection = this->filesGrid->GetSelectedCells();
	int selCount = totalSelection.Count();
	if (selCount < 1) { 
		selRow = this->filesGrid->GetCursorRow();
	} else {
		// TODO: try to handle multi rows ?
		wxGridCellCoords firstCoord = totalSelection[0];
		int selRow = firstCoord.GetRow();
	}
	if (selRow < 0) { return; }
	
	// Actually remove file in drsHelper
	std::wstring wvalueFileId = this->filesGrid->GetCellValue(selRow, 1);
	std::string valueFileId = narrow(wvalueFileId);
	std::wstring wvalueFileType = this->filesGrid->GetCellValue(selRow, 0);
	towlower(wvalueFileType); // force lowercase
	std::string valueFileType = narrow(wvalueFileType);
	int fileId = StrToInt(valueFileId.c_str(), -1);
	AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType;
	if (valueFileType.size() < 4) {
		valueFileType.append("    "); // make sure c_str() will be at least 4 char long.
	}
	fileType.SetFrom4Chars(valueFileType.c_str());
	bool removeResult = this->drsFileHelper.RemoveFile(fileType, fileId);
	if (!removeResult) {
		wxMessageBox(_T("Failed to remove"), _T("DRS Editor"), wxICON_ERROR);
	}
	bool success = this->filesGrid->DeleteRows(selRow, 1, true);
	this->filesGrid->ClearSelection();
	int rowCount = this->filesGrid->GetRows();
	if (selRow >= rowCount) { selRow = rowCount - 1; }
	this->filesGrid->GoToCell(selRow, 0);
	this->filesGrid->SelectBlock(selRow, 0, selRow, 0, false); // otherwise the user can no longer see the cursor position ! (wxwidgets bug)
}

// Useful stuff: GoToCell(row, col)
//this->filesGrid->GetCellValue();

