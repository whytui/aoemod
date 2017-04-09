#pragma once
#include <DrsFileHelper.h>
#include "Version.h"
#include "mystrings.h"
#include "EmpiresX_API.h"
#include "RockNRorAdmin_UI.h"
#include "FileSelector.h"


//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>


class WxDrsEditor : public wxFrame
{
public:
	WxDrsEditor(wxWindow *parent, const wxString& title, const wxSize& size);
	~WxDrsEditor();
	//void SetMainUIObject(RockNRorAdmin_UI *value) { this->objAOE = value; }
	//RockNRorAdmin_UI *GetMainUIObject() { return this->objAOE; }

	void SetOutputDrsFilename(std::string filename);
private:
	//RockNRorAdmin_UI *objAOE;
	DrsFileHelper drsFileHelper;
	std::string outputDrsFilename;

	wxBoxSizer *MainArea;
	wxBoxSizer *TopArea;
	wxBoxSizer *filesArea;
	wxBoxSizer *filesButtonsArea;
	wxBoxSizer *filesGridArea;
	wxGrid *fileTypesGrid;
	wxGrid *filesGrid;
	wxButton *btnAddFile;
	wxButton *btnRemoveFile;


	void ConstructorInit();
	void CreateFilesGrid();
	void OnBtnAddFile(wxCommandEvent& event);
	void OnBtnRemoveFile(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

};

enum
{
	ID_BtnAddFile = 1,
	ID_BtnRemoveFile = 2
};


#pragma warning(pop)
