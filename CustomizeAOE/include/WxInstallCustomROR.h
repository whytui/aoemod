#pragma once
#include <string>
#include "FileSelector.h"


//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class WxInstallCustomROR : public wxDialog
{
public:
	WxInstallCustomROR(wxWindow *parent, const wxString& title, const wxSize& size);
	WxInstallCustomROR(wxWindow *parent, const wxString& title, const wxSize& size, std::wstring EXEFileName);
	~WxInstallCustomROR();
	void ConstructorInit(std::wstring EXEFileName);

	std::wstring gameFileName;
	std::wstring pathToResourceFiles;
	bool hasMissingFile; // true if at least one of the "source" files is missing (does not take game EXE into account)

	bool ReplaceFiles() { return (chkOverwriteFiles != NULL) && chkOverwriteFiles->IsChecked(); };
private:
	wxBoxSizer *MainArea;
	wxBoxSizer *GameFileArea;
	wxBoxSizer *ButtonsArea;
	wxFlexGridSizer *grdResourceFiles;
	wxTextCtrl *edtGameFileName;
	wxTextCtrl *edtCustomROR_DLL_FilePath;
	wxTextCtrl *edtCustomROR_XML_FilePath;
	wxTextCtrl *edtCustomROR_civs_XML_FilePath;
	wxTextCtrl *edtCustomROR_DRS_FilePath;
	wxTextCtrl *edtROR_API_DLL_FilePath;
	wxTextCtrl *edtROR_API_conf_FilePath;
	wxTextCtrl *edtWndMode_FilePath;
	wxCheckBox *chkOverwriteFiles;
	wxButton *btnSelectGameFile;
	wxButton *btnSelectCustomROR_DLL;
	wxButton *btnOK;
	wxButton *btnCancel;

	// Returns true if the text control value is a valid path.
	// Sets object background color to red/green according to result
	bool CheckTextCtrlPath(wxTextCtrl *edt);
	void OnCancel(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnSelectGameEXE(wxCommandEvent& event);
	void OnSelectCustomROR_DLL(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

};

enum ICR_Components_IDs
{
	ID_ICR_CANCEL = 1,
	ID_ICR_OK = 2,
	ID_ICR_SELECT_CUSTOMROR_DLL = 3,
	ID_ICR_SELECT_GAME_FILE = 4,
	ID_ICR_EDIT_CUSTOMROR_DLL_PATH = 5
};


#pragma warning(pop)
