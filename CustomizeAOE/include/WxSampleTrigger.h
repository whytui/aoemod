#pragma once
#include <string>
#include <mystrings.h>
#include <triggerDefinition.h>

//#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(push)
#pragma warning(disable:4996) // allow use of deprecated like strcpy

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class WxSampleTrigger : public wxDialog
{
public:
	WxSampleTrigger(wxWindow *parent, const wxString& title, const wxSize& size);
	~WxSampleTrigger();
	void ConstructorInit();

private:
	wxBoxSizer *GlobalArea;
	wxBoxSizer *HeaderArea;
	wxBoxSizer *EventAndActionArea;
	wxBoxSizer *ParametersArea;
	wxBoxSizer *GenerateButtonArea;
	wxBoxSizer *TriggerArea;
	wxBoxSizer *BottomButtonsArea;
	wxStaticText *lblHeader;
	wxComboBox *cbbEvents;
	wxComboBox *cbbActions;
	wxCheckBox *cbxBeforeTrigger;
	wxCheckBox *cbxAfterTrigger;
	wxCheckBox *cbxDiffLevel;
	wxTextCtrl *edtTriggerSampleText;
	wxButton *btnGenerateTriggerText;
	wxButton *btnOK;

	void OnOK(wxCommandEvent& event);
	void OnGenerate(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

	void GenerateTriggerText();
};

enum CR_SampleTrigger_IDs
{
	ID_ST_OK = 1,
	ID_ST_GENERATE,
	ID_ST_CBB_EVENTS,
	ID_ST_CBB_ACTIONS,
	ID_ST_CBX_BEFORE_TRG,
	ID_ST_CBX_AFTER_TRG
};


#pragma warning(pop)
