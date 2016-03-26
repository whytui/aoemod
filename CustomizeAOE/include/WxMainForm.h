#pragma once
#include <customResolution.h>
#include <triggerDefinition.h>
#include "Version.h"
#include "mystrings.h"
#include "EmpiresX_API.h"
#include "CAOE_UI.h"
#include "ddraw_reg_colorFix.h"
#include "WxEditFile.h"
#include "FileSelector.h"
#include "WxChangeResolution.h"
#include "WxInstallCustomROR.h"
#include "WxSampleTrigger.h"
#include "WxDebugMainForm.h"
#include "InstallManager.h"


//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class WxMainForm : public wxFrame
{
public:
	WxMainForm(const wxString& title, const wxPoint& pos, const wxSize& size);
	~WxMainForm();
	void SetAPI(EmpiresX_API *value) { this->e_api = value; };
	EmpiresX_API *GetAPI() { return this->e_api; }
	void SetMainUIObject(CAOE_UI *value) { this->objAOE = value; }
	CAOE_UI *GetMainUIObject() { return this->objAOE; }
private:
	EmpiresX_API *e_api;
	CAOE_UI *objAOE;

	wxTextCtrl *txtLog;

	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnSelect(wxCommandEvent& event);
	void OnSelectFromRegistry(wxCommandEvent& event);
	void OnRunGame(wxCommandEvent& event);
	void OnEditGameFile(wxCommandEvent& event);
	void OnCloseGameFile(wxCommandEvent& event);
	void OnFixDDrawColorBug(wxCommandEvent& event);
	void OnInstallCustomROR(wxCommandEvent& event);
	void OnInstallCustomResolution(wxCommandEvent& event);
	void OnInstallSuggestedOptions(wxCommandEvent& event);
	void OnExportTriggerHTMLDoc(wxCommandEvent& event);
	void OnSampleTrigger(wxCommandEvent& event);
	void OnMenuDebug(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

	// Open empiresx.exe file. If force=true, then tech fixes will be installed without asking the user.
	bool FileOpen(bool force = false);
	void FileClose();
	void EditFile();
	void RunGame();
	void FixDDrawColorBug();
	void InstallCustomROR();
	void InstallSuggestedOptions();
	void ChooseCustomResolution();
	void ExportTriggerHTMLDocumentation();
	void OpenSampleTriggerWindow();
	void OpenDebugWindow();
};

enum
{
	ID_Select = 1,
	ID_SelectFromReg = 2,
	ID_RunGame = 3,
	ID_EditGameFile = 4,
	ID_InstallCustomROR = 5,
	ID_InstallCustomResolution = 6,
	ID_InstallSuggestedOptions = 7,
	ID_CloseGameFile = 8,
	ID_FixDDrawColorBug = 9,
	ID_GenTriggerDoc = 100,
	ID_GenTriggerSample = 101,
	ID_Debug = 200
};


#pragma warning(pop)
