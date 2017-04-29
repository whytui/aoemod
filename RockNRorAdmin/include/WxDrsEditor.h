#pragma once
#include <DrsFileHelper.h>
#include "mystrings.h"
#include "RockNRorAdmin_UI.h"
#include "FileSelector.h"
#include "WxDrsEditorFileEdit.h"


//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>
#include <wx/filedlg.h>


class WxDrsEditor : public wxFrame
{
public:
	WxDrsEditor(wxWindow *parent, const wxString& title, const wxSize& size, string inputFilename);
	~WxDrsEditor();
	//void SetMainUIObject(RockNRorAdmin_UI *value) { this->objAOE = value; }
	//RockNRorAdmin_UI *GetMainUIObject() { return this->objAOE; }

	void SetOutputDrsFilename(std::string filename);

	std::string GetOpenDrsOutputInfo() const { return this->openDrsOutputInfo; }
	std::string GetOpenDrsOutputError() const { return this->openDrsOutputError; }
private:
	//RockNRorAdmin_UI *objAOE;
	DrsFileHelper drsFileHelper;
	std::string outputDrsFilename;
	std::string openDrsOutputInfo;
	std::string openDrsOutputError;

	wxBoxSizer *MainArea;
	wxBoxSizer *TopArea;
	wxBoxSizer *filesArea;
	wxBoxSizer *filesButtonsArea;
	wxBoxSizer *filesGridArea;
	wxBoxSizer *bottomArea;
	wxGrid *fileTypesGrid;
	wxGrid *filesGrid;
	wxButton *btnAddFile;
	wxButton *btnModifyFileInfo;
	wxButton *btnRemoveFile;
	wxButton *btnExportFile;
	wxButton *btnSortItemsById;
	wxButton *btnMoveUp;
	wxButton *btnMoveDown;
	wxButton *btnSaveAsDrs;
	wxButton *btnExitWithoutSaving;


	void ConstructorInit();
	void CreateFilesGrid();
	void OpenDrsFile(string filename);
	void RefillGrid();
	// rowIndex must be in [0;rowcount]
	void SetGridCursorPosition(int rowIndex);
	// Returns true if valid info was provided
	bool AskFileInfo(long int &inOutFileId, AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE &inOutSlpType);
	void OnBtnAddFile(wxCommandEvent& event);
	void OnBtnModifyFileInfo(wxCommandEvent& event);
	void OnBtnRemoveFile(wxCommandEvent& event);
	void OnBtnExportFile(wxCommandEvent& event);
	void OnBtnSortItemsById(wxCommandEvent& event);
	void OnBtnSaveAsDrs(wxCommandEvent& event);
	void OnBtnMoveUp(wxCommandEvent& event);
	void OnBtnMoveDown(wxCommandEvent& event);
	void OnBtnExitNoSave(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

};

enum
{
	ID_BtnAddFile = 1,
	ID_BtnRemoveFile,
	ID_BtnModifyFileInfo,
	ID_BtnExportFile,
	ID_BtnSaveAsDrs,
	ID_BtnExitNoSave,
	ID_BtnSortItemsById,
	ID_BtnMoveUp,
	ID_BtnMoveDown
};


#pragma warning(pop)
