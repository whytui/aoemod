
#include "WxSampleTrigger.h"


wxBEGIN_EVENT_TABLE(WxSampleTrigger, wxDialog)
//EVT_MENU(ID_CR_CANCEL, WxChangeResolution::OnCancel)
wxEND_EVENT_TABLE()


WxSampleTrigger::WxSampleTrigger(wxWindow *parent, const wxString& title, const wxSize& size) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit();
}


WxSampleTrigger::~WxSampleTrigger() {
}


void WxSampleTrigger::ConstructorInit() {

	// UI
	this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP

	this->GlobalArea = new wxBoxSizer(wxVERTICAL);
	this->HeaderArea = new wxBoxSizer(wxHORIZONTAL);
	this->EventAndActionArea = new wxBoxSizer(wxHORIZONTAL);
	this->ParametersArea = new wxBoxSizer(wxVERTICAL);
	this->GenerateButtonArea = new wxBoxSizer(wxHORIZONTAL);
	this->TriggerArea = new wxBoxSizer(wxHORIZONTAL);
	this->BottomButtonsArea = new wxBoxSizer(wxHORIZONTAL);

	const int eventCount = CR_TRIGGERS::TRIGGER_EVENTS_TYPES_COUNT;
	const int actionCount = CR_TRIGGERS::TRIGGER_ACTION_TYPES_COUNT;
	wxString events[eventCount];
	wxString actions[actionCount];
	// Prepare events and actions lists.
	for (CR_TRIGGERS::TRIGGER_EVENT_TYPES e = CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE;
		e < CR_TRIGGERS::TRIGGER_EVENTS_TYPES_COUNT; e = CR_TRIGGERS::TRIGGER_EVENT_TYPES(e + 1)) {
		events[e] = CR_TRIGGERS::TriggerEventTypeToText(e);
	}
	for (CR_TRIGGERS::TRIGGER_ACTION_TYPES a = CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE;
		a < CR_TRIGGERS::TRIGGER_ACTION_TYPES_COUNT; a = CR_TRIGGERS::TRIGGER_ACTION_TYPES(a + 1)) {
		actions[a] = CR_TRIGGERS::TriggerActionTypeToText(a);
	}

	this->btnOK = new wxButton(this, ID_ST_OK, _T("Close"));
	this->btnGenerateTriggerText = new wxButton(this, ID_ST_GENERATE, _T("Generate"));
	this->edtTriggerSampleText = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->cbbEvents = new wxComboBox(this, ID_ST_CBB_EVENTS, _T("Choose an event"), wxDefaultPosition, wxSize(200, 40), eventCount, events);
	this->cbbActions = new wxComboBox(this, ID_ST_CBB_EVENTS, _T("Choose an action"), wxDefaultPosition, wxSize(200, 40), actionCount, actions);
	this->cbxBeforeTrigger = new wxCheckBox(this, ID_ST_CBX_BEFORE_TRG, _T("Before it is executed, this trigger runs another trigger."));
	this->cbxAfterTrigger = new wxCheckBox(this, ID_ST_CBX_BEFORE_TRG, _T("After it has been executed, this trigger runs another trigger."));
	this->cbxDiffLevel = new wxCheckBox(this, ID_ST_CBX_BEFORE_TRG, _T("Trigger is restricted to a specific difficulty level."));
	this->lblHeader = new wxStaticText(this, wxID_ANY, _T("Generate trigger samples using this interface.\nYou can then copy them and paste them directly in ROR scenario editor triggers edition window (press F3 - requires RockNRor)."),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	Connect(ID_ST_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxSampleTrigger::OnOK));
	Connect(ID_ST_GENERATE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxSampleTrigger::OnGenerate));

	this->HeaderArea->Add(this->lblHeader, 1, wxEXPAND);
	this->HeaderArea->SetMinSize(wxSize(500, 50));
	this->EventAndActionArea->Add(this->cbbEvents, 0, wxALIGN_LEFT);
	this->EventAndActionArea->AddSpacer(10);
	this->EventAndActionArea->Add(this->cbbActions, 0, wxALIGN_RIGHT);
	this->TriggerArea->Add(this->edtTriggerSampleText, 1, wxEXPAND);

	this->GenerateButtonArea->Add(btnGenerateTriggerText, 0, wxALIGN_CENTER);
	this->ParametersArea->Add(cbxBeforeTrigger, 0, wxALIGN_CENTER);
	this->ParametersArea->AddSpacer(5);
	this->ParametersArea->Add(cbxAfterTrigger, 0, wxALIGN_CENTER);
	this->ParametersArea->AddSpacer(5);
	this->ParametersArea->Add(cbxDiffLevel, 0, wxALIGN_CENTER);
	this->BottomButtonsArea->Add(btnOK, 0, wxALIGN_CENTER);

	// Main layout
	this->GlobalArea->Add(this->HeaderArea, 0, wxALIGN_TOP | wxALIGN_CENTER);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->EventAndActionArea, 0, wxALIGN_TOP | wxALIGN_CENTER);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->ParametersArea, 0, wxALIGN_TOP | wxALIGN_CENTER);
	this->GlobalArea->AddSpacer(10);
	this->GlobalArea->Add(this->GenerateButtonArea, 0, wxALIGN_TOP | wxALIGN_CENTER);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->TriggerArea, 1, wxEXPAND);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->BottomButtonsArea, 0, wxALIGN_BOTTOM | wxALIGN_CENTER);
	this->SetSizer(GlobalArea);
}


void WxSampleTrigger::OnOK(wxCommandEvent& event)
{
	this->EndModal(wxOK);
}

void WxSampleTrigger::OnGenerate(wxCommandEvent& event)
{
	this->GenerateTriggerText();
}


void WxSampleTrigger::GenerateTriggerText() {
	CR_TRIGGERS::TRIGGER_EVENT_TYPES evtId = (CR_TRIGGERS::TRIGGER_EVENT_TYPES) this->cbbEvents->GetSelection();
	CR_TRIGGERS::TRIGGER_ACTION_TYPES actId = (CR_TRIGGERS::TRIGGER_ACTION_TYPES) this->cbbActions->GetSelection();
	if (evtId < 0) { evtId = CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE; }
	if (actId < 0) { actId = CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE; }
	
	std::wstring trgText = _T("TRIGGER\r\n");
	trgText += widen(CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_NAME));
	trgText += _T("=<this trigger name>\r\n");
	if (this->cbxBeforeTrigger->IsChecked()) {
		trgText += widen(CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_RUN_BEFORE));
		trgText += _T("=<other trigger name>\r\n");
	}
	if (this->cbxAfterTrigger->IsChecked()) {
		trgText += widen(CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_RUN_AFTER));
		trgText += _T("=<other trigger name>\r\n");
	}
	if (this->cbxDiffLevel->IsChecked()) {
		trgText += widen(CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_DIFF_LEVEL));
		trgText += _T("=<[0-4] value, 0 is hardest.>\r\n");
	}
	trgText += widen(CR_TRIGGERS::GenerateEventTriggerString(evtId, false));
	trgText += _T("\r\n");
	trgText += widen(CR_TRIGGERS::GenerateActionTriggerString(actId, false));
	trgText += _T("\r\n");

	this->edtTriggerSampleText->SetValue(trgText);
}

