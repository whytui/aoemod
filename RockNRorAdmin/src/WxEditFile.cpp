
#include "WxEditFile.h"

wxBEGIN_EVENT_TABLE(WxEditFile, wxDialog)
//EVT_CONTEXT_MENU(WxEditFile::OnContextMenu); // Does not compile, WTF.
wxEND_EVENT_TABLE()



AOEEditTreeItemData::AOEEditTreeItemData(BINSEQ_CATEGORIES category, bool isAdvanced) {
	this->category = category;
	this->isAdvanced = isAdvanced;
	this->isPending = false;
	this->refSequenceName = _T("");
	switch (this->category) {
	case BC_MANAGE_AI:
		if (!this->isAdvanced) {
			this->refSequenceName = _T("Init_is_computer_for_AI_1");
		}
		this->labelText = _T("This section allows you to enable ManageAI features.");
		break;
	case BC_OBSOLETES:
		this->labelText = _T("This section contains various obsolete information. You should not use this unless you are really sure about what you're doing.");
		break;
	case BC_OPTIONS:
		this->labelText = _T("This section contains various options.");
		break;
	case BC_RESOLUTION:
		this->labelText = _T("This section allows you to change game resolution.Be careful, you will need the appropriate data / interfac.drs and data2 / interfac.drs file if you change this. ");
		if (this->isAdvanced) {
			this->labelText += _T("Change the values individually at your own risks.");
		}
		// Do not set this->refSequenceName, it is a special case (needs to read 2 numbers)
		break;
	case BC_ROR_API:
		this->labelText = _T("This section allows you to enable ROR_API feature / mod manager. It is required to use windowed mode. ");
		if (this->isAdvanced) {
			this->labelText += _T("You REALLY should not change these options individually.");
		} else {
			this->refSequenceName = _T("ROR_API_VarDataUpdate");
		}
		break;
	case BC_SELECTED_UNITS:
		this->labelText = _T("This section allows you to change the maximum number of selected units. ");
		if (this->isAdvanced) {
			this->labelText += _T("You REALLY should not change these options individually.");
		} else {
			this->refSequenceName = _T("InitBuffer1");
		}
		break;
	case BC_TECH_FIXES:
		this->labelText = _T("This section contains all required information to ensure your version contains no known bug and is compatible with customization / mods. ")
			_T("You should not change this manually.");
		break;
	case BC_AUDIO_VIDEO:
		this->labelText = _T("This section contains audio/video options (excluding custom resolutions)");
		break;
	case BC_VEG_WINDOWED_MODE:
		this->labelText = _T("This section contains obsolete changes for VEG's mod. You should not use this unless you are really sure about what you're doing.");
		break;
	case BC_WINDOWED_MODE:
		this->labelText = _T("This section allows you to run the game in windowed mode instead of fullscreen. This requires ROR_API.dll, wndmode.dll and the appropriate configuration in ROR_API.conf. ")
			_T("Make sure you also enable ROR_API features in this program.");
		if (this->isAdvanced) {
			this->labelText += _T("You should not enable/disable these options individually.");
		} else {
			this->refSequenceName = _T("ScreenToClient1");
		}
		break;
	default:
		if (this->isPending) {
			this->labelText = _T("This section shows you pending changes.");
		} else {
			this->labelText = _T("Please select a sub-section");
		}
	}
}

AOEEditTreeItemData::AOEEditTreeItemData(bool isPending) {
	this->category = BC_NONE;
	this->isAdvanced = false;
	this->isPending = isPending;
	this->labelText = _T("");
	this->refSequenceName = _T("");
	if (this->isPending) {
		this->labelText = _T("This section shows you pending changes.");
	} else {
		this->labelText = _T("Please select a sub-section");
	}
}



WxEditFile::WxEditFile(wxWindow *parent, const wxString& title, const wxSize& size) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit(NULL);
}

WxEditFile::WxEditFile(wxWindow *parent, const wxString& title, const wxSize& size, EmpiresX_API *api) : wxDialog(parent, -1, title, wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxNO_DEFAULT)
{
	this->ConstructorInit(api);
}

WxEditFile::~WxEditFile() {

}

void WxEditFile::ConstructorInit(EmpiresX_API *api) {
	this->e_api = api;
	this->objectToFocusAfterClickOnTree = NULL;
	this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP

	this->MainArea = new wxBoxSizer(wxVERTICAL);
	this->TreeAndNodesArea = new wxBoxSizer(wxHORIZONTAL);
	this->TreeArea = new wxBoxSizer(wxVERTICAL);
	this->BottomButtonsArea = new wxBoxSizer(wxHORIZONTAL);
	this->NodeContentArea = new  wxBoxSizer(wxVERTICAL);
	this->NodeHeaderArea = new  wxBoxSizer(wxHORIZONTAL);
	this->NodeCentralArea = new  wxBoxSizer(wxHORIZONTAL);
	this->NodeFooterArea = new  wxBoxSizer(wxHORIZONTAL);

	// Containers / Scrollbars
	EmptyNodeArea = new wxBoxSizer(wxVERTICAL);
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		//NodeScrollContainer_Main[category] = new wxBoxSizer(wxVERTICAL);
		NodeScrollContainer_Detail[category] = new wxBoxSizer(wxVERTICAL);
		//NodeScroller_Main[category] = new wxScrolledWindow(this);
		NodeScroller_Detail[category] = new wxScrolledWindow(this);
		//NodeScrolledSizer_Main[category] = new wxBoxSizer(wxVERTICAL);
		NodeScrolledSizer_Detailed[category] = new wxFlexGridSizer(6, 2, 2);
	}
	this->NodeSizer_Pending = new wxBoxSizer(wxVERTICAL);
	this->NodeSizer_General = new wxBoxSizer(wxVERTICAL);

	// Create treeview and its nodes
	this->treeView = new wxTreeCtrl(this, ID_TREE, wxDefaultPosition, wxSize(170, 400), wxTR_DEFAULT_STYLE | wxTR_SINGLE | wxTR_HAS_BUTTONS,
		wxDefaultValidator, wxTreeCtrlNameStr);
	// Note: the AOEEditTreeItemData objects are deleted when the tree is deleted.
	TI_Root = this->treeView->AddRoot("Root", -1, -1, new AOEEditTreeItemData(false));
	TI_TechFixes = this->treeView->AppendItem(TI_Root, "Tech fixes", -1, -1, new AOEEditTreeItemData(BC_TECH_FIXES, false));
	TI_Resolution = this->treeView->AppendItem(TI_Root, "Resolution", -1, -1, new AOEEditTreeItemData(BC_RESOLUTION, false));
	TI_WndMode = this->treeView->AppendItem(TI_Root, "Windowed mode", -1, -1, new AOEEditTreeItemData(BC_WINDOWED_MODE, false));
	TI_SelectedUnits = this->treeView->AppendItem(TI_Root, "Selected units", -1, -1, new AOEEditTreeItemData(BC_SELECTED_UNITS, false));
	TI_ROR_API = this->treeView->AppendItem(TI_Root, "ROR API", -1, -1, new AOEEditTreeItemData(BC_ROR_API, false));
	TI_ManageAI = this->treeView->AppendItem(TI_Root, "Manage AI", -1, -1, new AOEEditTreeItemData(BC_MANAGE_AI, false));
	TI_AudioVideo = this->treeView->AppendItem(TI_Root, "Audio/video", -1, -1, new AOEEditTreeItemData(BC_AUDIO_VIDEO, false));
	TI_Options = this->treeView->AppendItem(TI_Root, "Options", -1, -1, new AOEEditTreeItemData(BC_OPTIONS, true));
	TI_Obsolete = this->treeView->AppendItem(TI_Root, "Obsolete", -1, -1, new AOEEditTreeItemData(BC_OBSOLETES, false));
	TI_Pending = this->treeView->AppendItem(TI_Root, "Pending changes", -1, -1, new AOEEditTreeItemData(true));
	// details nodes
	TI_TechFixes_dtl = this->treeView->AppendItem(TI_TechFixes, "Advanced", -1, -1, new AOEEditTreeItemData(BC_TECH_FIXES, true));
	TI_Resolution_dtl = this->treeView->AppendItem(TI_Resolution, "Advanced", -1, -1, new AOEEditTreeItemData(BC_RESOLUTION, true));
	TI_WndMode_dtl = this->treeView->AppendItem(TI_WndMode, "Advanced", -1, -1, new AOEEditTreeItemData(BC_WINDOWED_MODE, true));
	TI_SelectedUnits_dtl = this->treeView->AppendItem(TI_SelectedUnits, "Advanced", -1, -1, new AOEEditTreeItemData(BC_SELECTED_UNITS, true));
	TI_ROR_API_dtl = this->treeView->AppendItem(TI_ROR_API, "Advanced", -1, -1, new AOEEditTreeItemData(BC_ROR_API, true));
	TI_ManageAI_dtl = this->treeView->AppendItem(TI_ManageAI, "Advanced", -1, -1, new AOEEditTreeItemData(BC_MANAGE_AI, true));
	TI_AudioVideo_dtl = this->treeView->AppendItem(TI_AudioVideo, "Advanced", -1, -1, new AOEEditTreeItemData(BC_AUDIO_VIDEO, true));
	TI_Obsolete_VEG = this->treeView->AppendItem(TI_Obsolete, "VEG mod", -1, -1, new AOEEditTreeItemData(BC_VEG_WINDOWED_MODE, true));
	TI_Obsolete_dtl = this->treeView->AppendItem(TI_Obsolete, "Other obsoletes", -1, -1, new AOEEditTreeItemData(BC_OBSOLETES, true));
	Connect(EF_ComponentIDs::ID_TREE, wxEVT_TREE_SEL_CHANGED, wxCommandEventHandler(WxEditFile::OnTreeSelChange));
	Connect(EF_ComponentIDs::ID_TREE, wxEVT_TREE_GET_INFO, wxCommandEventHandler(WxEditFile::OnTreeGetInfo));
	// Some color to make the tree beautiful :)
	this->treeView->SetItemBackgroundColour(TI_TechFixes_dtl, wxColor("orange"));
	this->treeView->SetItemBackgroundColour(TI_Obsolete, wxColor("orange"));
	this->treeView->SetItemBackgroundColour(TI_Obsolete_VEG, wxColor("orange"));
	this->treeView->SetItemBackgroundColour(TI_Obsolete_dtl, wxColor("orange"));
	this->treeView->SetItemBackgroundColour(TI_Pending, wxColor("yellow"));
	this->treeView->Expand(TI_Root);

	// Buttons
	this->btnUndoAll = new wxButton(this, EF_ComponentIDs::ID_UNDO_ALL, "Undo all");
	this->btnSave = new wxButton(this, EF_ComponentIDs::ID_SAVE, "Save");
	this->btnClose = new wxButton(this, EF_ComponentIDs::ID_CLOSE, "Close");
	this->btnResetToDefault = new wxButton(this, EF_ComponentIDs::ID_RESET_TO_DEFAULT, "Reset to default");
	Connect(EF_ComponentIDs::ID_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxEditFile::OnClose));
	Connect(EF_ComponentIDs::ID_SAVE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxEditFile::OnSave));
	Connect(EF_ComponentIDs::ID_UNDO_ALL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxEditFile::OnUndoAll));
	Connect(EF_ComponentIDs::ID_RESET_TO_DEFAULT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxEditFile::OnResetToDefault));

	// Various controls
	this->cbxEnableFeature = new wxCheckBox(this, EF_ComponentIDs::ID_CBX_ENABLE_FEATURE, "Enable feature", wxDefaultPosition, wxDefaultSize,
		wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
	this->edtPendingChanges = new wxTextCtrl(this, EF_ComponentIDs::ID_EDT_PENDING, "", wxDefaultPosition, wxSize(500, 100),
		wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP);
	this->edtPendingChanges->SetMinSize(wxSize(400, 100));
	this->lblNodeHeader = new wxStaticText(this, wxID_ANY, "");
	this->lblSimpleNodeHelp = new wxStaticText(this, wxID_ANY, "");
	Connect(EF_ComponentIDs::ID_CBX_ENABLE_FEATURE, wxEVT_CHECKBOX, wxCommandEventHandler(WxEditFile::OnEnableFeatureClick));

	// Tree + node content zone
	this->TreeArea->Add(this->treeView, 0, wxALIGN_LEFT); // 0 : not stretchable
	this->TreeAndNodesArea->Add(this->TreeArea, 0, wxALIGN_LEFT); // 0 : not stretchable
	this->TreeAndNodesArea->AddSpacer(5);
	this->TreeAndNodesArea->Add(this->NodeContentArea, 1, wxEXPAND);

	// Node content zone
	this->NodeContentArea->Add(this->NodeHeaderArea, 0, wxALIGN_TOP); // 0 : not stretchable
	this->NodeHeaderArea->SetMinSize(100, 60);
	this->NodeContentArea->AddSpacer(5);
	this->NodeContentArea->Add(this->NodeCentralArea, 1, wxEXPAND | wxALL);

	// Main node container => specialized containers
	this->NodeCentralArea->Add(this->EmptyNodeArea, 1, wxEXPAND | wxALL);
	this->NodeCentralArea->Add(this->NodeSizer_Pending, 1, wxEXPAND | wxALL);
	this->NodeCentralArea->Add(this->NodeSizer_General, 1, wxEXPAND | wxALL);

	//this->NodeCentralArea->Add(this->NodeScrollContainer_Pending, 1, wxEXPAND | wxALL);
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		//this->NodeCentralArea->Add(this->NodeScrollContainer_Main[category], 1, wxEXPAND | wxALL);
		this->NodeCentralArea->Add(this->NodeScrollContainer_Detail[category], 1, wxEXPAND | wxALL);
	}

	// Scrollers (node area). Sizer1 contains scroller contains sizer2...
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		//this->NodeScrollContainer_Main[category]->Add(this->NodeScroller_Main[category], 1, wxEXPAND | wxALL);
		this->NodeScrollContainer_Detail[category]->Add(this->NodeScroller_Detail[category], 1, wxEXPAND | wxALL);
		//this->NodeScroller_Main[category]->SetSizer(this->NodeScrolledSizer_Main[category]);
		this->NodeScroller_Detail[category]->SetSizer(this->NodeScrolledSizer_Detailed[category]);
		//this->NodeScroller_Main[category]->SetScrollRate(10, 10);
		this->NodeScroller_Detail[category]->SetScrollRate(10, 10);
		//this->NodeScroller_Main[category]->SetAutoLayout(true);
		this->NodeScroller_Detail[category]->SetAutoLayout(true);
		//this->NodeScrollContainer_Main[category]->SetMinSize(wxSize(800, 800));
		this->NodeScrollContainer_Detail[category]->SetMinSize(wxSize(800, 800));
	}

	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		//this->NodeScrollContainer_Main[category]->Show(false);
		this->NodeScrollContainer_Detail[category]->Show(false);
	}
	this->EmptyNodeArea->Add(new wxStaticText(this, wxID_ANY, "Please select another section."));
	this->EmptyNodeArea->Show(false);

	this->NodeContentArea->Add(this->NodeFooterArea, 0, wxALIGN_BOTTOM);
	this->NodeHeaderArea->Add(this->lblNodeHeader, 0, wxALIGN_LEFT);
	this->NodeFooterArea->Add(this->btnResetToDefault, 0, wxALIGN_RIGHT);
	// Specialized nodes
	this->NodeSizer_General->Add(this->lblSimpleNodeHelp, 0, wxALIGN_LEFT | wxALIGN_TOP);
	this->NodeSizer_General->Add(this->cbxEnableFeature, 0, wxALIGN_LEFT | wxALIGN_TOP);
	this->NodeSizer_General->AddStretchSpacer(1); // To occupy lower part of the "node content" area.
	this->NodeSizer_Pending->Add(this->edtPendingChanges, 1, wxEXPAND | wxALL);
	this->NodeSizer_General->Show(false);
	this->NodeSizer_Pending->Show(false);

	// Bottom buttons zone
	this->BottomButtonsArea->Add(this->btnUndoAll, 0, wxEXPAND);
	this->BottomButtonsArea->Add(this->btnSave, 0, wxEXPAND);
	this->BottomButtonsArea->Add(this->btnClose, 0, wxEXPAND);

	// Main layout
	this->MainArea->AddSpacer(2);
	this->MainArea->Add(this->TreeAndNodesArea, 1, wxALIGN_TOP);
	this->MainArea->Add(this->BottomButtonsArea, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
	
	SetDefaultItem(btnClose);
	this->SetSizer(MainArea);
	this->CreateNodesContent();
}


// Create UI objects for nodes
void WxEditFile::CreateNodesContent() {
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		CreateDetailedNodeContent(category);
	}
}


// This method creates all UI objects for "detailed" nodes.
void WxEditFile::CreateDetailedNodeContent(BINSEQ_CATEGORIES category) {
	wxFlexGridSizer *sizer = this->NodeScrolledSizer_Detailed[category];
	wxScrolledWindow *scroller = this->NodeScroller_Detail[category];
	EmpiresXFileManager *m = this->e_api->GetAOEManager();

	// Titles
	wxStaticText *headerLabel;
	headerLabel = new wxStaticText(scroller, wxID_ANY, "Default"), 0, wxALIGN_CENTER;
	headerLabel->SetToolTip(new wxToolTip("This column always corresponds to the original game option. Click the button to select this option."));
	sizer->Add(headerLabel);
	headerLabel = new wxStaticText(scroller, wxID_ANY, "Other"), 0, wxALIGN_CENTER;
	headerLabel->SetToolTip(new wxToolTip("This column corresponds to the other possible option (different from game default). Click the button to select this option. See next column for options that include a variable value."));
	sizer->Add(headerLabel);
	sizer->Add(new wxStaticText(scroller, wxID_ANY, "Your value"), 0, wxALIGN_CENTER);
	sizer->Add(new wxStaticText(scroller, wxID_ANY, "Current value"), 0, wxALIGN_CENTER);
	sizer->Add(new wxStaticText(scroller, wxID_ANY, "Default value   "), 0, wxALIGN_CENTER);
	sizer->Add(new wxStaticText(scroller, wxID_ANY, "Name (see tooltips for info)"), 0, wxALIGN_CENTER);

	int seqdefIndex = m->IteratorFirst(category);
	while (seqdefIndex > -1) {
		int userChoice = m->GetUserSelection(category, seqdefIndex);
		int fileSelection;
		try {
			fileSelection = m->GetFileSelection(category, seqdefIndex); // throws if file is not ok or if config is not valid (bad offset...)
		}
		catch (std::exception &e) {
			wxString message = "An error occurred, internal data may be erronenous. ";
			message.Append(e.what());
			wxMessageBox(message, "RockNRor Admin", wxOK | wxICON_ERROR, this);
			return;
		}
		std::wstring fileVarValue = _T("none");
		std::wstring userVarValue = _T("none");
		std::wstring defaultVarValue = _T("none");
		try {
			if (userChoice > -1) { userVarValue = m->DumpUserVarValue(category, seqdefIndex); } // throws if no selection
			if (fileSelection > -1) { fileVarValue = m->DumpFileVarValue(category, seqdefIndex); } // only throws if no file (or setup error)
			defaultVarValue = m->DumpDefaultVarValue(category, seqdefIndex);
		}
		catch (...) {
			return;
		}
		SEQ_VAR_TYPES vtype = m->SeqDefHasVariable(category, seqdefIndex);
		std::wstring seqDefName = m->GetSequenceDefName(category, seqdefIndex);
		std::wstring seqDefDesc = m->GetSequenceDefDescription(category, seqdefIndex);
		FUNC_MEANING meaning0 = m->GetFuncMeaningFromSeqIndex(category, seqdefIndex, 0);
		FUNC_MEANING meaning1 = FM_NO_CHOICE;
		if (vtype == SVT_NO_VARIABLE) { // no variable = 2 sequences in our usage
			meaning1 = m->GetFuncMeaningFromSeqIndex(category, seqdefIndex, 1);
		}

		AdvancedSeqButtonInfo *btnInfo0 = new AdvancedSeqButtonInfo();
		AdvancedSeqButtonInfo *btnInfo1 = new AdvancedSeqButtonInfo();
		btnInfo0->category = category;
		btnInfo0->seqDefIndex = seqdefIndex;
		btnInfo0->seqIndex = 0;
		btnInfo0->hasOtherButton = false;

		wxColour col0 = wxColour();
		wxColour col1 = wxColour();
		this->ComputeButtonColor(col0, col1, userChoice, fileSelection);

		// Buttons
		wxButton *btn0 = new wxButton(scroller, wxID_ANY, FUNC_MEANING_NAME[meaning0], wxDefaultPosition, wxSize(45, 20));
		wxButton *btn1;
		btn0->SetBackgroundColour(col0);
		btn0->SetRefData(btnInfo0);
		sizer->Add(btn0, 0, wxALIGN_LEFT);
		Connect(btn0->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxEditFile::OnFuncMeaningButtonClick));
		if (fileSelection == -1) { btn0->Enable(false); } // Disabled if file is not consistent
		btnInfo0->otherButton = btn0;
		NodeButtons0[category].push_back(btn0);

		if (vtype == SVT_NO_VARIABLE) {
			btn1 = new wxButton(scroller, wxID_ANY, FUNC_MEANING_NAME[meaning1], wxDefaultPosition, wxSize(45, 20));

			btn1->SetBackgroundColour(col1);
			sizer->Add(btn1, 0, wxALIGN_LEFT);
			Connect(btn1->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxEditFile::OnFuncMeaningButtonClick));
			if (fileSelection == -1) { btn1->Enable(false); } // Disabled if file is not consistent
			btnInfo1 = new AdvancedSeqButtonInfo();
			btnInfo1->category = category;
			btnInfo1->seqDefIndex = seqdefIndex;
			btnInfo1->seqIndex = 1;
			btnInfo1->hasOtherButton = true;
			btnInfo1->otherButton = btn0;
			btnInfo0->hasOtherButton = true;
			btnInfo0->otherButton = btn1;
			btn1->SetRefData(btnInfo1);
		} else {
			// Supposing we have 5 objects columns starting at 0 (btn0), we put a fake empty object in col1 location (instead of a button)
			wxStaticText *empty_col1 = new wxStaticText(scroller, wxID_ANY, "", wxDefaultPosition, wxSize(45, 20));
			sizer->Add(empty_col1);
		}

		// Labels - prepare common info
		AdvancedSeqVarLinkInfo *linkInfo = new AdvancedSeqVarLinkInfo();
		linkInfo->category = category;
		linkInfo->seqDefIndex = seqdefIndex;
		linkInfo->variableType = vtype;
		// Create UI according to variable presence
		if (vtype != SVT_NO_VARIABLE) {
			if (vtype == SVT_FLOAT_4B) {
				if (userVarValue.size() > FLOAT_MAX_CHARS_TO_DISPLAY) userVarValue.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
				if (fileVarValue.size() > FLOAT_MAX_CHARS_TO_DISPLAY) fileVarValue.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
				if (defaultVarValue.size() > FLOAT_MAX_CHARS_TO_DISPLAY) defaultVarValue.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
			}
			wxHyperlinkCtrl *lblVarUser = new wxHyperlinkCtrl(scroller, wxID_ANY, userVarValue, "", wxDefaultPosition, wxSize(50, 20));
			sizer->Add(lblVarUser);
			wxStaticText *lblVarFile = new wxStaticText(scroller, wxID_ANY, fileVarValue, wxDefaultPosition, wxSize(50, 20));
			sizer->Add(lblVarFile);
			
			AdvancedSeqVarLinkInfo *linkInfo2 = new AdvancedSeqVarLinkInfo(); // Create a dedicated object or it will be freed twice and the program will crash !
			linkInfo2->category = category;
			linkInfo2->seqDefIndex = seqdefIndex;
			linkInfo2->variableType = vtype;
			lblVarUser->SetRefData(linkInfo2);
			if (fileSelection == -1) {
				lblVarUser->Enable(false);
				lblVarUser->SetForegroundColour(*wxRED);
				lblVarUser->Layout();
			} else {
				Connect(lblVarUser->GetId(), wxEVT_HYPERLINK, wxCommandEventHandler(WxEditFile::OnValueLinkClick));
			}
			btnInfo0->userSelLabel = lblVarUser;
			btnInfo0->fileSelLabel = lblVarFile;
			btnInfo1->userSelLabel = lblVarUser;
			btnInfo1->fileSelLabel = lblVarFile;
		} else {
			wxStaticText *empty_col2 = new wxStaticText(scroller, wxID_ANY, "", wxDefaultPosition, wxSize(50, 20)); //instead of hyperlink
			wxStaticText *empty_col3 = new wxStaticText(scroller, wxID_ANY, "", wxDefaultPosition, wxSize(50, 20)); // instead of file var value
			sizer->Add(empty_col2);
			sizer->Add(empty_col3);
		}

		wxStaticText *lblDefaultVarValue = new wxStaticText(scroller, wxID_ANY, defaultVarValue, wxDefaultPosition, wxDefaultSize, wxEXPAND);
		sizer->Add(lblDefaultVarValue);
		
		wxStaticText *lbl = new wxStaticText(scroller, wxID_ANY, seqDefName, wxDefaultPosition, wxDefaultSize, wxEXPAND);
		lbl->SetMinSize(wxSize(300, 16));
		sizer->Add(lbl);
		lbl->SetToolTip(new wxToolTip(seqDefDesc));
		lbl->SetRefData(linkInfo); // Also store link info here, useful for right-click.
#ifdef _DEBUG
		Connect(lbl->GetId(), wxEVT_CONTEXT_MENU, wxCommandEventHandler(WxEditFile::OnDescContextMenu_Fake));
#endif

		// Next...
		seqdefIndex = this->e_api->GetAOEManager()->IteratorNext();
	}
	//sizer->Layout();
}


void WxEditFile::CancelPending() {
	this->e_api->CancelPendingChanges();
	this->ShowNodeContent(); // Refresh UI
}


void WxEditFile::SaveToFile() {
	int result = this->e_api->WriteChangesToFile(); // this does not throw exceptions
	wxString message = "Invalid return code. WTF ?";
	if (result == -1) { message = "An error occurred while writing changes."; }
	if (result == -2) { message = "Could not save changes: some of them are not compatible with current file content."; }
	if (result == -3) { message = "Could not open file for write. Make sure you have the necessary rights or close other programs that may use the file."; }
	if (result == -4) { message = "The file is not open. Open it again in main window."; }
	if (result == -5) { message = "A serious technical error occurred :("; }
	if (result >= 0) { message = "The changes were saved successfully."; }
	wxMessageBox(message, "Save changes to file", wxOK | wxICON_EXCLAMATION);
	if (!this->e_api->IsFileOpen()) {
		if (!this->e_api->OpenEmpiresXFile()) { this->Close(); }
	}
	
	this->ShowNodeContent(); // Refresh UI
}


void WxEditFile::ResetNodeToDefault() {
	wxTreeItemId currentSelection = this->treeView->GetSelection();
	AOEEditTreeItemData *data = (AOEEditTreeItemData*) this->treeView->GetItemData(currentSelection);
	// Check invalid configurations
	if (data == NULL) { return; } else {
		if ((data->category == BC_NONE) && (!data->isAdvanced) && (!data->isPending)) { return; }
	}
	if (data->category == BC_NONE) {
		wxMessageBox("Cannot reset to default in this screen", "RockNRor Admin", wxOK | wxICON_INFORMATION);
		return;
	}
	this->e_api->ResetToDefault(data->category);
	this->e_api->CancelPendingChanges(data->category, true); // Remove invalids if we added some
	this->ShowNodeContent();
}


void WxEditFile::ChangeNode() {
	//this->disableUI_Events = true;
	this->ShowNodeContent(NULL);
	//this->disableUI_Events = false;
}


// Compute colors for the 2 buttons (choice0 / choice1)
// Color for a button according to corresponding sequence ("choice")...
// Not selected, not file current sequence => Grey
// No user selection, button=file current sequence => blue
// User selection =  file current sequence => green
// User selection <> file current sequence => yellow
// Inconsistent data => red
void WxEditFile::ComputeButtonColor(wxColour &col0, wxColour &col1, int userChoice, int fileSelection) {
	col0.Set(color_lightGrey);
	col1.Set(color_lightGrey);
	if (userChoice == -1) {
		if (fileSelection == 0) {
			col0.Set(color_lightBlue);
		}
		if (fileSelection == 1) {
			col1.Set(color_lightBlue);
		}
	}
	if (userChoice == 0) {
		if (fileSelection == 0) {
			col0.Set(color_lightGreen);
		} else {
			col0.Set(color_lightYellow);
		}
	}
	if (userChoice == 1) {
		if (fileSelection == 1) {
			col1.Set(color_lightGreen);
		} else {
			col1.Set(color_lightYellow);
		}
	}
	if (fileSelection < 0) {
		col0.Set(color_lightOrange);
		col1.Set(color_lightOrange);
	}
}


// Updates an "advanced" node content.
void WxEditFile::ShowAdvancedNode(BINSEQ_CATEGORIES category) {
	EmpiresXFileManager *m = this->e_api->GetAOEManager();
	for (std::vector<wxButton*>::iterator it = NodeButtons0[category].begin(); it != NodeButtons0[category].end(); it++) {
		AdvancedSeqButtonInfo *info = (AdvancedSeqButtonInfo*)(*it)->GetRefData();
		// Refresh button colors
		this->UpdateButtonColor(*it, false);

		// Get variable values (as text)
		int userChoice = m->GetUserSelection(category, info->seqDefIndex);
		int fileSelection = m->GetFileSelection(category, info->seqDefIndex); // throws if file is not ok
		std::wstring fileVarValue = _T("none");
		std::wstring userVarValue = _T("none");
		try {
			if (userChoice > -1) { userVarValue = m->DumpUserVarValue(category, info->seqDefIndex); } // throws if no selection
			if (fileSelection > -1) { fileVarValue = m->DumpFileVarValue(category, info->seqDefIndex); } // only throws if no file
			if (m->SeqDefHasVariable(category, info->seqDefIndex) == SVT_FLOAT_4B) {
				if (userVarValue.size() > FLOAT_MAX_CHARS_TO_DISPLAY) userVarValue.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
				if (fileVarValue.size() > FLOAT_MAX_CHARS_TO_DISPLAY) fileVarValue.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
			}
		}
		catch (...) {
			//wxMessageBox("An error occurred with variable value", "Error", wxOK | wxICON_ERROR, this);
			//return;
			userVarValue = _T("?");
			fileVarValue = _T("?");
		}

		// Update labels if they really exist
		if (info->userSelLabel != NULL) {
			info->userSelLabel->SetLabelText(userVarValue);
			if (fileSelection == -1) { info->userSelLabel->SetBackgroundColour(color_lightOrange); }
		}
		if (info->fileSelLabel != NULL) {
			info->fileSelLabel->SetLabelText(fileVarValue);
		}
	}
}



// Updates UI (node content)
void WxEditFile::ShowNodeContent(wxControl *focusedControl) {
	bool invalidNode = false;
	wxTreeItemId currentSelection = this->treeView->GetSelection();
	AOEEditTreeItemData *data = (AOEEditTreeItemData*) this->treeView->GetItemData(currentSelection);
	// Check invalid configurations
	if (data == NULL) { invalidNode = true; } else {
		if ((data->category == BC_NONE) && (!data->isAdvanced) && (!data->isPending)) { invalidNode = true; }
	}
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		//this->NodeScrollContainer_Main[category]->Show(false);
		this->NodeScrollContainer_Detail[category]->Show(false);
	}
	this->NodeSizer_Pending->Show(false);
	this->NodeSizer_General->Show(false);
	this->EmptyNodeArea->Show(false);

	if (invalidNode || ((data->category == BC_OBSOLETES) && (!data->isAdvanced))) {
		this->lblNodeHeader->SetLabelText("");
		this->EmptyNodeArea->Show(true);
		return;
	}

	// Update node header label
	this->lblNodeHeader->SetLabelText(data->labelText);
	int x = this->GetSize().GetWidth() - this->lblNodeHeader->GetPosition().x - 20;
	this->lblNodeHeader->SetMaxSize(wxSize(x, 50));
	this->lblNodeHeader->Wrap(x);

	if (data->isPending) {
		// Pending changes
		std::wstring ws = this->e_api->DumpPendingsToWString(_T(", \n"));
		this->edtPendingChanges->Clear();
		this->edtPendingChanges->AppendText("List of pending changes:\n");
		this->edtPendingChanges->AppendText(ws);
		this->NodeSizer_Pending->Show(true);
		this->NodeSizer_Pending->Layout();
		this->NodeCentralArea->Layout();
		this->Layout();
	} else {
		if (data->isAdvanced) {
			this->NodeScrollContainer_Detail[data->category]->Show(true);
			this->NodeScrollContainer_Detail[data->category]->Layout();
			this->NodeCentralArea->Layout();
			this->Layout(); // Necessary to make sure the node is correctly resized (or it will be very small horizontally)
			//this->NodeScroller_Detail[data->category]->SetFocus(); // Does not work
		} else {
			//this->NodeScrollContainer_Main[data->category]->Show(true);
			//this->NodeScrollContainer_Main[data->category]->Layout();
			this->NodeSizer_General->Show(true);
			this->NodeCentralArea->Layout();
			this->Layout(); // Necessary to make sure the node is correctly resized (or it will be very small horizontally)
		}
	}

	// Detail screens (update colors...)
	if (data->isAdvanced) {
		this->ShowAdvancedNode(data->category);
	}

	// "General" Nodes...
	wxCheckBoxState newCbxStatus = this->cbxEnableFeature->Get3StateValue();
	// Compute "General" Node checkbox state (for some nodes, not all).
	if (!data->isAdvanced && !data->isPending && !data->refSequenceName.empty()) {
		try {
			int userChoice = this->e_api->GetAOEManager()->GetUserSelection(data->category, data->refSequenceName);
			int fileSelection = this->e_api->GetAOEManager()->GetFileSelection(data->category, data->refSequenceName);
			userChoice = (userChoice > -1) ? userChoice : fileSelection;
			newCbxStatus = (userChoice != 0) ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED;
		}
		catch (...) {
			// May occur if no sequence was found (missing definition - missing setup for current version).
			wxMessageBox(_T("Could not retrieve all internal information. Setup may be missing for this version of the game."), _T("Customize game file"), wxOK | wxICON_WARNING);
			newCbxStatus = wxCheckBoxState::wxCHK_UNCHECKED;
		}
	}
	// Compute "General" Node checkbox state (tech fixes).
	if ((data->category == BC_TECH_FIXES) && !data->isAdvanced) {
		try {
			newCbxStatus = this->e_api->HasMissingTechFixes() ? wxCheckBoxState::wxCHK_UNCHECKED : wxCheckBoxState::wxCHK_CHECKED;
		}
		catch (...) {
			wxMessageBox("An error occurred. Maybe this file version is not fully supported.", "RockNRor Admin", wxOK | wxICON_ERROR, this);
			newCbxStatus = wxCheckBoxState::wxCHK_UNCHECKED;
		}
	}
	// Compute "General" Node checkbox state (resolution).
	if ((data->category == BC_RESOLUTION) && !data->isAdvanced) {
		short int x, y;
		try {
			int userChoice = this->e_api->GetAOEManager()->GetUserSelection(data->category, _T("HSize1"));
			int fileSelection = this->e_api->GetAOEManager()->GetFileSelection(data->category, _T("HSize1"));
			if (userChoice == -1) {
				this->e_api->GetAOEManager()->GetFileVarValue(data->category, _T("HSize1"), &x);
			} else {
				this->e_api->GetAOEManager()->GetUserVarValue(data->category, _T("HSize1"), &x);
			}
			userChoice = this->e_api->GetAOEManager()->GetUserSelection(data->category, _T("VSize1"));
			fileSelection = this->e_api->GetAOEManager()->GetFileSelection(data->category, _T("VSize1"));
			if (userChoice == -1) {
				this->e_api->GetAOEManager()->GetFileVarValue(data->category, _T("VSize1"), &y);
			} else {
				this->e_api->GetAOEManager()->GetUserVarValue(data->category, _T("VSize1"), &y);
			}
			newCbxStatus = ((x == 1024) && (y == 768)) ? wxCheckBoxState::wxCHK_UNCHECKED : wxCheckBoxState::wxCHK_CHECKED;
		}
		catch (...) {
			// May occur if no sequence was found (missing definition - missing setup for current version).
			newCbxStatus = wxCheckBoxState::wxCHK_UNCHECKED;
		}
	}
	if (!data->isAdvanced && (data->category != BC_OBSOLETES)) {
		this->cbxEnableFeature->Set3StateValue(newCbxStatus); // Actually update checkbox status
	}

	// Manage focus
	this->objectToFocusAfterClickOnTree = NULL;
	if (focusedControl) {
		focusedControl->SetFocus();
		this->objectToFocusAfterClickOnTree = focusedControl;
	} else {
		if (data->isAdvanced) {
			if (NodeButtons0[data->category].size() > 0) {
				// Give focus to first object (a button) so that mouse wheel works
				NodeButtons0[data->category][0]->SetFocus(); // Unfortunately, only works if tree sel change was made with keyboard. Thanks wxWidgets !
				this->objectToFocusAfterClickOnTree = NodeButtons0[data->category][0];
			}
		}
	}
}

void WxEditFile::OnClose(wxCommandEvent& event)
{
	this->Close(false);
}

void WxEditFile::OnSave(wxCommandEvent& event)
{
	this->SaveToFile();
}

void WxEditFile::OnResetToDefault(wxCommandEvent& event)
{
	this->ResetNodeToDefault();
}

void WxEditFile::OnUndoAll(wxCommandEvent& event)
{
	CancelPending();
}

void WxEditFile::OnTreeSelChange(wxCommandEvent& event) {
	this->ChangeNode();
}

// This is a hack to workaround one of numerous wxWidget bugs
// If we do some SetFocus() in OnTreeSelChange, it doesn't work (for mouse events) because tree re-gets focus afterwards. (but it works with keyboard arrows !!)
void WxEditFile::OnTreeGetInfo(wxCommandEvent& event) {
	if (objectToFocusAfterClickOnTree) {
		objectToFocusAfterClickOnTree->SetFocus();
	}
	//objectToFocusAfterClickOnTree = NULL;
}


void WxEditFile::DoROR_API_checks() {
	std::wstring message = _T("");
	std::wstring gameDir = extractDirectory(this->e_api->GetFileName());
	bool error = false;
	std::wstring tmpName = gameDir + _T("\\ROR_API.dll");
	if (!PathFileExists(tmpName.c_str())) {
		error = true;
		message += _T("ROR_API.dll is missing.\n");
	}
	tmpName = gameDir + _T("\\ROR_API.conf");
	if (!PathFileExists(tmpName.c_str())) {
		error = true;
		message += _T("ROR_API.conf is missing [not critical].\n");
	}
	if (error) {
		message = _T("Warning: this option may cause game crashes if all prerequisites are not respected.\n") + message;
		wxMessageBox(message, "RockNRor Admin", wxOK | wxICON_WARNING, this);
	}
}

// When the user clicks on "enable" checkbox at feature level
void WxEditFile::OnEnableFeatureClick(wxCommandEvent& event) {
	wxTreeItemId currentSelection = this->treeView->GetSelection();
	AOEEditTreeItemData *data = (AOEEditTreeItemData*) this->treeView->GetItemData(currentSelection);
	// Check invalid configurations
	if (data == NULL) { return; } else {
		if ((data->category == BC_NONE) && (!data->isAdvanced) && (!data->isPending)) { return; }
	}

	if (this->cbxEnableFeature->Get3StateValue() == wxCheckBoxState::wxCHK_UNDETERMINED) {
		this->e_api->CancelPendingChanges(data->category);
		return;
	}

	// Here it can only be checked or unchecked (REALLY a bool)
	bool enable = this->cbxEnableFeature->Get3StateValue() == wxCheckBoxState::wxCHK_CHECKED;
	bool cancelled = false;
	switch (data->category) {
		// Restricted features: Tech/obsoletes
	case BC_TECH_FIXES:
		if (!enable) { return; }
		this->e_api->SetAllTechFixes();
		break;
	case BC_OBSOLETES:
		if (!enable) { return; }
		this->e_api->RemoveObsoletes();
	case BC_VEG_WINDOWED_MODE:
		// See obsoletes...
		break;
		// "Simple" features: Only ON/OFF:
	case BC_MANAGE_AI:
		this->e_api->SetManageAI(enable);
		break;
	case BC_AUDIO_VIDEO:
		this->e_api->SetAllAudioVideoOptions(enable);
		break;
	case BC_ROR_API:
		//this->e_api->SetAllROR_API_optionals(enable); // Do not install optionals, RockNRor can install them dynamically
		this->e_api->SetROR_API(enable);
		if (enable) {
			// Checks on prerequisites
			this->DoROR_API_checks();
		}
		break;
	case BC_WINDOWED_MODE:
		// Windowed mode requires ROR_API.
		//this->e_api->SetAllROR_API_optionals(enable); // Do not install optionals, RockNRor can install them dynamically
		this->e_api->SetROR_API(enable);
		this->e_api->SetWindowedMode(enable);
		this->DoROR_API_checks();
		break;
		// Others
	case BC_RESOLUTION:
		unsigned short int x, y;
		if (enable) {
			x = (unsigned short int) wxGetNumberFromUser("Please provide the new Horizontal resolution value", "Horizontal resolution", "RockNRor Admin", 1024, 0, 50000, this);
			if (x <= 0) {
				cancelled = true; 
				break;
			}
			y = (unsigned short int) wxGetNumberFromUser("Please provide the new Vertical resolution value", "Vertical resolution", "RockNRor Admin", 768, 0, 50000, this);
			if (y <= 0) {
				cancelled = true; 
				break;
			}
		} else {
			// We also could use this->e_api->ResetToDefault(BC_RESOLUTION);
			x = 1024;
			y = 768;
		}
		this->e_api->SetResolution(x, y, true);
		break;
	case BC_SELECTED_UNITS:
		unsigned short int nb;
		if (enable) {
			nb = (unsigned short int) wxGetNumberFromUser("Please provide a new value", "Maximum number of units that can be selected at once", "RockNRor Admin", 25, 0, 255, this);
			if ((nb <= 0) || (nb > 255)) {
				cancelled = true;
				break;
			}
			this->e_api->SetMaxSelectedUnits((unsigned char)nb);
		} else {
			this->e_api->ResetToDefault(BC_SELECTED_UNITS);
		}
		break;
		//case BC_OPTIONS: / no UI for this
	default:
		break;
	}
	if (cancelled) {
		this->ShowNodeContent(); // A refresh is required if user cancelled the operation, or if an error occurred.
	}
}


// Updates button's color and also update related button's (if any) color.
// If clickOnButton is true, this will update user selection (click on the button).
void WxEditFile::UpdateButtonColor(wxButton *btn, bool clickOnButton) {
	try {
		wxColour col0;
		wxColour col1;
		AdvancedSeqButtonInfo *info = (AdvancedSeqButtonInfo*)btn->GetRefData();
		BINSEQ_CATEGORIES category = info->category;
		int seqDefIndex = info->seqDefIndex;
		EmpiresXFileManager *m = this->e_api->GetAOEManager();
		int userChoice;

		int fileSelection = m->GetFileSelection(category, seqDefIndex); // throws if file is not ok
		if (fileSelection == -1) { // Should not happen because buttons should be disabled ?
			btn->Enable(false);
			if (info->otherButton) {
				info->otherButton->Enable(false);
			}
			//return;
		} else {
			btn->Enable(true);
			if (info->otherButton) {
				info->otherButton->Enable(true);
			}
		}
		if (clickOnButton) {
			// Update user selection
			if ((fileSelection == info->seqIndex) && (info->hasOtherButton)) {
				userChoice = -1; // Choose the current "file selection" = the same as leave unchanged (if there is >1 choice : do NOT cancel variable changes !)
			} else {
				userChoice = info->seqIndex;
			}
			// For sequences with variable (only 1 button), we must NOT set user selection if no variable value is provided too (the value would be undefined)
			if (m->SeqDefHasVariable(category, seqDefIndex) == SEQ_VAR_TYPES::SVT_NO_VARIABLE) {
				m->SetUserSelection(category, seqDefIndex, userChoice);
			}
		}
		else {
			// Just update color (do not click - do not make a selection)
			userChoice = m->GetUserSelection(category, seqDefIndex);
		}

		// Now update the color
		this->ComputeButtonColor(col0, col1, userChoice, fileSelection);
		if (info->seqIndex == 1) {
			btn->SetBackgroundColour(col1);
			info->otherButton->SetBackgroundColour(col0);
		} else {
			btn->SetBackgroundColour(col0);
			if (info->hasOtherButton) {
				info->otherButton->SetBackgroundColour(col1);
			}
		}
	}
	catch (...) {}
}


// Manage click on ON or OFF button
void WxEditFile::OnFuncMeaningButtonClick(wxCommandEvent& event) {
	UpdateButtonColor((wxButton*)event.GetEventObject(), true); // Click the button and update color
}

void WxEditFile::OnValueLinkClick(wxCommandEvent& event) {
	try {
		//System::Windows::Forms::LinkLabel ^lbl = (System::Windows::Forms::LinkLabel^) sender;
		wxHyperlinkCtrl *lbl = (wxHyperlinkCtrl*)event.GetEventObject();
		//AdvancedSeqVarLinkInfo ^linkInfo = (AdvancedSeqVarLinkInfo^)lbl->Tag;
		AdvancedSeqVarLinkInfo *linkInfo = (AdvancedSeqVarLinkInfo*) lbl->GetRefData();
		EmpiresXFileManager *m = this->e_api->GetAOEManager();
		int fileSelection = m->GetFileSelection(linkInfo->category, linkInfo->seqDefIndex); // throws if file is not ok
		if (fileSelection == -1) { return; } // Should not happen because buttons should be disabled

		std::wstring sTmp = _T("");
		if (m->GetUserSelection(linkInfo->category, linkInfo->seqDefIndex) > -1) {
			sTmp = m->DumpUserVarValue(linkInfo->category, linkInfo->seqDefIndex);
		}

		std::wstring strResult = wxGetTextFromUser("Please type variable value", "RockNRor Admin", sTmp, this, -1, -1, true);
		if (strResult.empty()) { return; }
		char int1;
		short int int2;
		int int4;
		float f;
		switch (linkInfo->variableType) {
		case SVT_FLOAT_4B:
			f = stof(strResult); // raises InvalidArgument
			// We must select the sequence (0 here, as "variables" sequence def only have 1 possible sequence).
			// The engine refuses to set variable value when no user selection has been made.
			m->SetUserSelection(linkInfo->category, linkInfo->seqDefIndex, 0);
			m->SetUserVarValue(linkInfo->category, linkInfo->seqDefIndex, &f);
			break;
		case SVT_INT_1B:
			int1 = stoi(strResult);
			// We must select the sequence (0 here, as "variables" sequence def only have 1 possible sequence).
			// The engine refuses to set variable value when no user selection has been made.
			m->SetUserSelection(linkInfo->category, linkInfo->seqDefIndex, 0);
			m->SetUserVarValue(linkInfo->category, linkInfo->seqDefIndex, &int1);
			break;
		case SVT_INT_2B:
			int2 = stoi(strResult);
			// We must select the sequence (0 here, as "variables" sequence def only have 1 possible sequence).
			// The engine refuses to set variable value when no user selection has been made.
			m->SetUserSelection(linkInfo->category, linkInfo->seqDefIndex, 0);
			m->SetUserVarValue(linkInfo->category, linkInfo->seqDefIndex, &int2);
			break;
		case SVT_INT_4B:
			int4 = stoi(strResult);
			// We must select the sequence (0 here, as "variables" sequence def only have 1 possible sequence).
			// The engine refuses to set variable value when no user selection has been made.
			m->SetUserSelection(linkInfo->category, linkInfo->seqDefIndex, 0);
			m->SetUserVarValue(linkInfo->category, linkInfo->seqDefIndex, &int4);
			break;
		default: return;
		}
		sTmp = m->DumpUserVarValue(linkInfo->category, linkInfo->seqDefIndex);
		if (linkInfo->variableType == SVT_FLOAT_4B) {
			if (sTmp.size() > FLOAT_MAX_CHARS_TO_DISPLAY) sTmp.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
			if (sTmp.size() > FLOAT_MAX_CHARS_TO_DISPLAY) sTmp.resize(FLOAT_MAX_CHARS_TO_DISPLAY);
		}
		lbl->SetLabelText(sTmp);
	}
	catch (std::invalid_argument) {
		wxMessageBox("The value is incorrect", "RockNRor Admin", wxOK | wxICON_ERROR, this);
	}
	catch (std::out_of_range) {
		wxMessageBox("The value is out of range", "RockNRor Admin", wxOK | wxICON_ERROR, this);
	}
	catch (std::exception &e) {
		wxMessageBox(e.what(), "RockNRor Admin", wxOK |wxICON_ERROR, this);
	}
	catch (...) {
		wxMessageBox("Unknown exception", "RockNRor Admin", wxOK | wxICON_ERROR, this);
	}
}


// Opens an options popup related to specific sequence
void WxEditFile::OnDescContextMenu_Fake(wxCommandEvent& event)
{
	wxStaticText *srcObject = (wxStaticText*)event.GetEventObject();
	AdvancedSeqVarLinkInfo *linkInfo = (AdvancedSeqVarLinkInfo*)srcObject->GetRefData();

	if (linkInfo->category == BC_NONE) { return; }
	bool modified = false;
	WxBinSeqWindow *bsWindow = new WxBinSeqWindow(this, _T("Binary sequence"), wxSize(600, 500), this->e_api,
		linkInfo->category,linkInfo->seqDefIndex);
	try {
		bsWindow->ShowModal();
	}
	catch (...) {
		wxMessageBox("An error occurred", "RockNRor Admin", wxOK | wxICON_INFORMATION);
	}
	modified = bsWindow->ModifiedData();
	bsWindow->Destroy();
	if (modified) {
		this->ShowNodeContent(srcObject);
	} else {
		srcObject->SetFocus(); // When popup is closed wxWidgets some kind of reloads the frame and the scrollbar returns to top :-(
	}
}


