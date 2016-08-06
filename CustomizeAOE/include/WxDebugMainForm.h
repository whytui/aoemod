#pragma once
#include <string>
#include <mystrings.h>
#include <AOE_structures.h>
#include <AOE_struct_debugging.h>
#include "RORConnector.h"
#include <sstream>
#include <iostream>

//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class WxDebugMainForm : public wxFrame
{
public:
	WxDebugMainForm(wxWindow *parent, const wxString& title, const wxSize& size);
	~WxDebugMainForm();
	void ConstructorInit();

private:
	wxBoxSizer *GlobalArea;
	wxBoxSizer *HeaderArea;
	wxBoxSizer *GuessStructArea;
	wxBoxSizer *GameObjectsArea;
	wxBoxSizer *OutputInfoArea;
	wxBoxSizer *BottomArea;
	wxStaticText *lblEXEInfo;
	wxStaticText *lblGuessStruct;
	wxStaticText *lblPlayerId;
	wxStaticText *lblSubId;
	wxTextCtrl *edtAddress;
	wxTextCtrl *edtOutputInfo;
	wxComboBox *edtPlayerId;
	wxTextCtrl *edtSubId;
	wxButton *btnGuessRORStruct;
	wxButton *btnOK;
	wxButton *btnShowMainObjets;
	wxButton *btnShowGatheringInfo;
	wxButton *btnFindUnit;
	wxButton *btnFindUnitDef;

	void OnOK(wxCommandEvent& event);
	void OnGuessRORStruct(wxCommandEvent& event);
	void OnFindObject(wxCommandEvent& event);
	void OnFindUnitDef(wxCommandEvent& event);
	void OnShowMainObjects(wxCommandEvent& event);
	void OnShowGatheringInfo(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

	void GuessRORStruct();
	void ShowMainObjects();
	void ShowGatheringInfo();
	void FindObject();
	void FindUnitDef();
};

enum CR_DebugMainForm_IDs
{
	ID_DMF_OK = 1,
	ID_DMF_GUESS_STRUCT,
	ID_DMF_SHOW_MAIN_OBJECTS,
	ID_DMF_FIND_UNIT,
	ID_DMF_FIND_UNIT_DEF,
	ID_DMF_SHOW_GATHERING_INFO
};


#pragma warning(pop)
