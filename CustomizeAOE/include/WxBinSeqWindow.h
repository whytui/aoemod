#pragma once

#include "AOE_binData.h"
#include "EmpiresX_API.h"

#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class WxBinSeqWindow : public wxDialog
{
public:
	WxBinSeqWindow(wxWindow *parent, const wxString& title, const wxSize& size);
	WxBinSeqWindow(wxWindow *parent, const wxString& title, const wxSize& size, EmpiresX_API *api, BINSEQ_CATEGORIES category, int seqDefIndex);
	~WxBinSeqWindow();
	void SetAPI(EmpiresX_API *value) { this->e_api = value; };
	EmpiresX_API *GetAPI() { return this->e_api; }
	bool ModifiedData() { return this->modified; }

protected:
	EmpiresX_API *e_api;
	BINSEQ_CATEGORIES category;
	int seqDefIndex;
	bool modified;
	// UI objects
	wxBoxSizer *MainArea;
	wxBoxSizer *TechInfosArea;
	wxButton *btnClose;
	wxButton *btnForceReset;
	wxButton *btnGenerateBinaryPatcherIni;
	wxTextCtrl *edtSeqName;
	wxTextCtrl *edtSeqTechReference;
	wxTextCtrl *edtSeqDesc;
	wxTextCtrl *edtSeqAddress;
	wxTextCtrl *edtSeqFileContent;
	wxTextCtrl *edtSeqDefContent;
	wxTextCtrl *edtExportToBinPatcherIni;
	wxStaticText *lblSeqFileContent;
	wxStaticText *lblSeqDefContent;

	void ConstructorInit(EmpiresX_API *api);
	void CreateUIContent(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Events
	void OnClose(wxCommandEvent& event);
	void OnForceReset(wxCommandEvent& event);
	void OnGenBinPatcherIniClick(wxCommandEvent& event);

	// wxWidgets macro
	wxDECLARE_EVENT_TABLE();

};


// BinSeqWindow component IDs
enum BSW_ComponentIDs
{
	ID_BSW_CLOSE = 1,
	ID_BSW_FORCE_RESET = 2,
	ID_BSW_EDT_ADDRESS = 3,
	ID_BSW_GEN_BINPATCHER_INI = 4
};


#pragma warning(pop)
