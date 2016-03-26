#pragma once
#include <string>


//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class WxChangeResolution : public wxDialog
{
public:
	WxChangeResolution(wxWindow *parent, const wxString& title, const wxSize& size);
	WxChangeResolution(wxWindow *parent, const wxString& title, const wxSize& size, std::wstring EXEFileName, unsigned long int resX, unsigned long int resY);
	~WxChangeResolution();
	void ConstructorInit(std::wstring EXEFileName, unsigned long int x, unsigned long int y);

	unsigned long int newResolutionX;
	unsigned long int newResolutionY;
	std::wstring gameFileName;
	std::wstring dataDRSFileName;
	std::wstring data2DRSFileName;
private:
	//EmpiresX_API *e_api;
	// UI objects
	wxBoxSizer *MainArea;
	wxBoxSizer *GameFileArea;
	wxBoxSizer *ButtonsArea;
	wxFlexGridSizer *grdResolution;
	wxTextCtrl *edtGameFileName;
	wxTextCtrl *edtResolutionX;
	wxTextCtrl *edtResolutionY;
	wxButton *btnSelectGameFile;
	wxButton *btnSelectDRSData;
	wxButton *btnSelectDRSData2;
	wxButton *btnOK;
	wxButton *btnCancel;

	void OnCancel(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnSelectGameEXE(wxCommandEvent& event);
	void OnSelectData(wxCommandEvent& event);
	void OnSelectData2(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

};

enum CR_Components_IDs
{
	ID_CR_CANCEL = 1,
	ID_CR_OK = 2,
	ID_CR_EDIT_X = 3,
	ID_CR_EDIT_Y = 4,
	ID_CR_SELECT_DATA = 5,
	ID_CR_SELECT_DATA2 = 6,
	ID_CR_SELECT_GAME_FILE = 7
};


#pragma warning(pop)
