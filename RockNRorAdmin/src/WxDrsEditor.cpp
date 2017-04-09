#include "WxDrsEditor.h"

wxBEGIN_EVENT_TABLE(WxDrsEditor, wxFrame)
wxEND_EVENT_TABLE()

WxDrsEditor::WxDrsEditor(wxWindow *parent, const wxString& title, const wxSize& size) : wxFrame(parent, 0, title, wxDefaultPosition, size) {
	this->ConstructorInit();
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
	this->btnRemoveFile = new wxButton(this, ID_BtnRemoveFile, _T("Remove file"));
	Connect(ID_BtnAddFile, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditor::OnBtnAddFile));
	Connect(ID_BtnRemoveFile, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDrsEditor::OnBtnRemoveFile));

	this->filesGridArea->Add(this->filesGrid);

	this->filesButtonsArea->Add(this->btnAddFile);
	this->filesButtonsArea->Add(this->btnRemoveFile);

	this->filesArea->Add(this->filesGridArea);
	this->filesArea->Add(this->filesButtonsArea);

	this->MainArea->Add(this->TopArea);
	this->MainArea->Add(this->filesArea);

	this->SetSizer(this->MainArea);
}


void WxDrsEditor::CreateFilesGrid() {
	this->filesGrid->CreateGrid(5, 4, wxGrid::wxGridSelectionModes::wxGridSelectCells);
	this->filesGrid->SetColLabelValue(0, _T("File Type")); // Set header label for column
	this->filesGrid->SetColLabelValue(1, _T("File ID")); // Set header label for column
	this->filesGrid->SetColLabelValue(2, _T("File size")); // Set header label for column
	this->filesGrid->SetColLabelValue(3, _T("Local filename")); // Set header label for column
	this->filesGrid->SetColSize(3, 300);
	this->filesGrid->EnableEditing(true); // Global editable status
	this->filesGrid->DisableDragRowSize(); // User can't resize rows
	this->filesGrid->SetCellValue(0, 0, _T("Coucou"));
	this->filesGrid->SetCellValue(2, 0, _T("Coucou20"));
	this->filesGrid->SetReadOnly(2, 0, true);

	//this->filesGrid->SetSelectionMode(wx)
}


void WxDrsEditor::SetOutputDrsFilename(std::string filename) {
	this->outputDrsFilename = filename;
}


void WxDrsEditor::OnBtnAddFile(wxCommandEvent& event) {
	std::wstring wfilename = openfilename(_T("All files (*.*)\0*.*\0"));
	if (wfilename.empty()) { return; }
	std::string filename = narrow(wfilename);
	if (!CheckFileExistence(wfilename)) {
		return;
	}

	// TODO: remove hardcoded "SLP" ! User choice or guess ? Beware lowercase and space !
	AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE slpType;
	slpType.SetFromDword(AOE_STRUCTURES::DRS_FILE_TYPE_DWORD::DFT_SLP);
	long int newFileId = 0; // TODO
	// TODO: prevent duplicate fileID (check before adding, check other file types too)
	DrsIncludedFile *addedFile = this->drsFileHelper.AddFile(slpType, newFileId, filename);
	if (!addedFile) { return; }
	long int dataSize = addedFile->dataSize;

	this->filesGrid->AppendRows(1, true);
	int rowCount = this->filesGrid->GetRows();
	this->filesGrid->SetCellValue(rowCount - 1, 0, slpType.Get4LettersExtension().GetAsCharPtr());
	this->filesGrid->SetCellValue(rowCount - 1, 1, std::to_wstring(newFileId));
	this->filesGrid->SetCellValue(rowCount - 1, 2, std::to_wstring(dataSize));
	this->filesGrid->SetCellValue(rowCount - 1, 3, wfilename);
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
	fileType.SetFrom4Chars(valueFileType.c_str());
	bool removeResult = this->drsFileHelper.RemoveFile(fileType, fileId);
	if (!removeResult) {
		MessageBox(NULL, _T("joho"), _T("fsdf"), 0);
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

