
#include "WxDebugMainForm.h"


wxBEGIN_EVENT_TABLE(WxDebugMainForm, wxFrame)
//EVT_MENU(ID_DMF_OK, WxDebugMainForm::OnOK)
//EVT_MENU(ID_CR_CANCEL, WxChangeResolution::OnCancel)
wxEND_EVENT_TABLE()


WxDebugMainForm::WxDebugMainForm(wxWindow *parent, const wxString& title, const wxSize& size) : wxFrame(parent, 0, title, wxDefaultPosition, size)
{
	this->ConstructorInit();
}


WxDebugMainForm::~WxDebugMainForm() {
}


void WxDebugMainForm::ConstructorInit() {

	// UI
	//this->SetWindowStyle(wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxMINIMIZE_BOX); // wxDIALOG_EX_CONTEXTHELP

	this->GlobalArea = new wxBoxSizer(wxVERTICAL);
	this->HeaderArea = new wxBoxSizer(wxHORIZONTAL);
	this->GuessStructArea = new wxBoxSizer(wxHORIZONTAL);
	this->GameObjectsArea = new wxBoxSizer(wxHORIZONTAL);
	this->OutputInfoArea = new wxBoxSizer(wxHORIZONTAL);
	this->BottomArea = new wxBoxSizer(wxHORIZONTAL);

	this->btnOK = new wxButton(this, ID_DMF_OK, _T("Close"));
	this->btnGuessRORStruct = new wxButton(this, ID_DMF_GUESS_STRUCT, _T("Guess ROR structure"));
	this->btnShowMainObjets = new wxButton(this, ID_DMF_SHOW_MAIN_OBJECTS, _T("Show main structures"));
	this->btnShowGatheringInfo = new wxButton(this, ID_DMF_SHOW_GATHERING_INFO, _T("Debug gathering"));
	this->btnFindUnit = new wxButton(this, ID_DMF_FIND_UNIT, _T("Find unit"));
	this->btnFindUnitDef = new wxButton(this, ID_DMF_FIND_UNIT_DEF, _T("Find unitDef"));

	Connect(ID_DMF_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDebugMainForm::OnOK));
	Connect(ID_DMF_GUESS_STRUCT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDebugMainForm::OnGuessRORStruct));
	Connect(ID_DMF_SHOW_MAIN_OBJECTS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDebugMainForm::OnShowMainObjects));
	Connect(ID_DMF_SHOW_GATHERING_INFO, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDebugMainForm::OnShowGatheringInfo));
	Connect(ID_DMF_FIND_UNIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDebugMainForm::OnFindObject));
	Connect(ID_DMF_FIND_UNIT_DEF, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(WxDebugMainForm::OnFindUnitDef));

	this->lblEXEInfo = new wxStaticText(this, wxID_ANY, _T("Debug running Rise of Rome game"),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	this->edtAddress = new wxTextCtrl(this, wxID_ANY, _T("00000000"), wxDefaultPosition, wxDefaultSize, 0);
	this->lblGuessStruct = new wxStaticText(this, wxID_ANY, _T("Type hexadecimal address to analyze (\"0x\" is optional)"),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	wxString playerIDs[9] = { _T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), };
	this->lblPlayerId = new wxStaticText(this, wxID_ANY, _T("Player Id"));
	this->edtPlayerId = new wxComboBox(this, wxID_ANY, _T("0"), wxDefaultPosition, wxDefaultSize, 9, playerIDs);
	this->edtPlayerId->SetSelection(0);
	this->lblSubId = new wxStaticText(this, wxID_ANY, _T("Object Id"));
	this->edtSubId = new wxTextCtrl(this, wxID_ANY, _T("0"), wxDefaultPosition, wxDefaultSize, 0);
	this->edtOutputInfo = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	this->HeaderArea->Add(this->lblEXEInfo);

	this->GuessStructArea->Add(this->lblGuessStruct);
	this->GuessStructArea->AddSpacer(5);
	this->GuessStructArea->Add(this->edtAddress);
	this->GuessStructArea->AddSpacer(5);
	this->GuessStructArea->Add(this->btnGuessRORStruct);
	this->GameObjectsArea->Add(this->lblPlayerId);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->edtPlayerId);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->lblSubId);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->edtSubId);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->btnShowMainObjets);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->btnShowGatheringInfo);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->btnFindUnit);
	this->GameObjectsArea->AddSpacer(5);
	this->GameObjectsArea->Add(this->btnFindUnitDef);
	this->OutputInfoArea->Add(this->edtOutputInfo, 1, wxEXPAND);
	this->BottomArea->Add(btnOK);

	// Main layout
	this->GlobalArea->Add(this->HeaderArea, 0, wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL);
	this->GlobalArea->AddSpacer(10);
	this->GlobalArea->Add(this->GuessStructArea, 0, wxALIGN_CENTER_HORIZONTAL);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->GameObjectsArea, 0, wxALIGN_CENTER_HORIZONTAL);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->OutputInfoArea, 1, wxEXPAND);
	this->GlobalArea->AddSpacer(5);
	this->GlobalArea->Add(this->BottomArea, 0, wxALIGN_CENTER_HORIZONTAL);
	this->SetSizer(GlobalArea);
}


void WxDebugMainForm::OnOK(wxCommandEvent& event)
{
	this->Close();
}

void WxDebugMainForm::OnGuessRORStruct(wxCommandEvent& event)
{
	this->GuessRORStruct();
}

void WxDebugMainForm::OnFindObject(wxCommandEvent& event)
{
	this->FindObject();
}

void WxDebugMainForm::OnFindUnitDef(wxCommandEvent& event)
{
	this->FindUnitDef();
}

void WxDebugMainForm::OnShowMainObjects(wxCommandEvent& event)
{
	this->ShowMainObjects();
}

void WxDebugMainForm::OnShowGatheringInfo(wxCommandEvent& event)
{
	this->ShowGatheringInfo();
}


void WxDebugMainForm::GuessRORStruct() {
	unsigned int address;
	std::stringstream ss;
	// Read entry value. It can contain "0x" or not, but it is ALWAYS considered hexadecimal.
	ss << std::hex << this->edtAddress->GetValue();
	ss >> address;
	if (address == 0) { return; }

	RORConnector rConnector = RORConnector();
	if (!rConnector.ConnectToROR()) {
		return;
	}

	HANDLE hROR = rConnector.GetRORProcessHandle();
	if (hROR) {
		char *structName = ROR_STRUCTURES_10C::DebugStructAtAddress(hROR, address);
		//this->edtAddress->SetLabelText(widen(structName));
		this->lblEXEInfo->SetLabelText(widen(structName));
	}

	rConnector.DisconnectFromROR();
}


void WxDebugMainForm::ShowMainObjects() {
	this->edtOutputInfo->Clear();
	RORConnector rConnector = RORConnector();
	if (!rConnector.ConnectToROR()) {
		return;
	}
	long int objId = -1;
	wxString input = this->edtSubId->GetValue();
	if (input.IsNumber()) {
		if (!input.ToLong(&objId, 10)) {
			objId = -1;
		}
	}

	HANDLE hROR = rConnector.GetRORProcessHandle();
	ROR_STRUCTURES_10C::RORDebugger *rd = new ROR_STRUCTURES_10C::RORDebugger();
	if (hROR) {
		rd->handleROR = hROR;
		bool valid = rd->RefreshMainGameStructs();
		std::string s = rd->ExportMainStructInfo();
		this->edtOutputInfo->WriteText(s);

		s = rd->ExportHostedRORObject(&rd->gameSettings, rd->gameSettingsAddr);
		this->edtOutputInfo->WriteText("\n");
		this->edtOutputInfo->WriteText(s);
		this->edtOutputInfo->WriteText("\n");
		s = rd->ExportHostedRORObject(&rd->gameGlobal, (unsigned long int) rd->gameSettings.ptrGlobalStruct);
		this->edtOutputInfo->WriteText("\n");
		this->edtOutputInfo->WriteText(s);
		this->edtOutputInfo->WriteText("\n");
	}
	rConnector.DisconnectFromROR();
}


// Collect and display info about a player Gathering strategy/status
void WxDebugMainForm::ShowGatheringInfo() {
	this->edtOutputInfo->Clear();
	RORConnector rConnector = RORConnector();
	if (!rConnector.ConnectToROR()) {
		return;
	}
	long int objId = -1;
	wxString input = this->edtSubId->GetValue();
	if (input.IsNumber()) {
		if (!input.ToLong(&objId, 10)) {
			objId = -1;
		}
	}

	HANDLE hROR = rConnector.GetRORProcessHandle();
	ROR_STRUCTURES_10C::RORDebugger *rd = new ROR_STRUCTURES_10C::RORDebugger();
	if (hROR) {
		rd->handleROR = hROR;
		bool valid = rd->RefreshMainGameStructs();
		int playerId = this->edtPlayerId->GetSelection();
		if ((playerId < 1) || (playerId >= rd->gameGlobal.playerTotalCount)) {
			delete rd;
			return;
		}
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = &rd->players[playerId];
		ROR_STRUCTURES_10C::STRUCT_AI *ai = &rd->playersAI[playerId];
		//ai->structConAI.townCenter_posX
		//ai->structBuildAI.strategyFileName
		std::string s = "InfAI - ";
		long int offset = ((long)&rd->playersAI[playerId].structInfAI - (long)&rd->playersAI[playerId]);
		s += GetHexStringAddress(((long int)rd->players[playerId].ptrAIStruct) + offset);
		s += " - dropSiteDistances forage/wood/stone/gold = ";
		s += std::to_string(ai->structInfAI.bestForageDropSiteDistance);
		s += " ";
		s += std::to_string(ai->structInfAI.bestWoodDropSiteDistance);
		s += " ";
		s += std::to_string(ai->structInfAI.bestStoneDropSiteDistance);
		s += " ";
		s += std::to_string(ai->structInfAI.bestGoldDropSiteDistance);
		s += "\ntargets = ";
		s += std::to_string(ai->structInfAI.bestForageUnitId);
		if (ai->structInfAI.bestForageUnitId >= 0) {
			s += "(";
			s += rd->GetUnitName(ai->structInfAI.bestForageUnitId);
			s += ")";
		}
		s += " ";
		s += std::to_string(ai->structInfAI.bestWoodUnitId);
		if (ai->structInfAI.bestWoodUnitId >= 0) {
			s += "(";
			s += rd->GetUnitName(ai->structInfAI.bestWoodUnitId);
			s += ")";
		}
		s += " ";
		s += std::to_string(ai->structInfAI.bestStoneUnitId);
		if (ai->structInfAI.bestStoneUnitId >= 0) {
			s += "(";
			s += rd->GetUnitName(ai->structInfAI.bestStoneUnitId);
			s += ")";
		}
		s += " ";
		s += std::to_string(ai->structInfAI.bestGoldUnitId);
		if (ai->structInfAI.bestGoldUnitId >= 0) {
			s += "(";
			s += rd->GetUnitName(ai->structInfAI.bestGoldUnitId);
			s += ")";
		}
		s += "\nForestTiles = ";
		s += std::to_string(ai->structInfAI.foundForestTiles);
		s += "\nUnit Lists...\n+114: count = ";
		s += std::to_string(ai->structInfAI.unitElemListSize); // unitElemListSize (array size)
		s += "\nListA count = ";
		s += std::to_string(ai->structInfAI.creatableAndGatherableUnits.usedElements);
		s += "\nListB count = ";
		s += std::to_string(ai->structInfAI.playerCreatableUnits.usedElements);
		s += "\nListC count = ";
		s += std::to_string(ai->structInfAI.artefactsAndFlags.usedElements);
		s += "\nBuildings list, count = ";
		s += std::to_string(ai->structInfAI.buildingUnits.usedElements);
		s += ";";
		s += std::to_string(ai->structInfAI.buildingUnits.arraySize);

		s += "\n\nList A= ";
		s += rd->GetUnitNamesFromArrayOfID((long)ai->structInfAI.creatableAndGatherableUnits.unitIdArray, ai->structInfAI.creatableAndGatherableUnits.usedElements);
		s += "\n\nList B= ";
		s += rd->GetUnitNamesFromArrayOfID((long)ai->structInfAI.playerCreatableUnits.unitIdArray, ai->structInfAI.playerCreatableUnits.usedElements);
		s += "\n\nBuildings= ";
		s += rd->GetUnitNamesFromArrayOfID((long)ai->structInfAI.buildingUnits.unitIdArray, ai->structInfAI.buildingUnits.usedElements);
		s += "\n";

		s += "\n";
		s += "TacAI - ";
		offset = ((long)&rd->playersAI[playerId].structTacAI - (long)&rd->playersAI[playerId]);
		s += GetHexStringAddress(((long int)rd->players[playerId].ptrAIStruct) + offset);
		s += "\nVillagersCount=";
		s += std::to_string(ai->structTacAI.allVillagers.usedElements);
		s += " gatherersCount_desired=";
		for (int i = 0; i < 4; i++) {
			s += std::to_string(ai->structTacAI.gathererCount_desired[i]);
			s += " ";
		}
		s += " gatherersCount_actual=";
		for (int i = 0; i < 4; i++) {
			s += std::to_string(ai->structTacAI.gathererCount_actual[i]);
			s += " ";
		}
		s += " ExtraResource=";
		for (int i = 0; i < 4; i++) {
			s += std::to_string(ai->structTacAI.extraResourceAmount[i]);
			s += " ";
		}
		s += " resTypesOrder=";
		for (int i = 0; i < 4; i++) {
			s += std::to_string(ai->structTacAI.extraResourceTypeNeededByPriority[i]);
			s += " ";
		}
		/*s += " +D58=";
		for (int i = 0; i < 4; i++) {
			s += std::to_string(ai->structTacAI.unknown_D58_villagerCount[i]);
			s += " ";
		}*/

		s += "\nvillagerTasksRelevantCount=";
		s += std::to_string(ai->structTacAI.villagerTasksRelevantElemCount);
		s += " nonExplorerVillagerCount=";
		s += std::to_string(ai->structTacAI.nonExplorerVillagersCount);
		s += " otherVillagerTableCount10C=";
		s += std::to_string(ai->structTacAI.villagerExplorers.maxElementCount);
		s += " otherVillagerTableCount110=";
		s += std::to_string(ai->structTacAI.villagerExplorers.arraySize);

		/*s += " D5C=";
		s += std::to_string(ai->structTacAI.unknown_D5C_villagerCount);
		s += " D60=";
		s += std::to_string(ai->structTacAI.unknown_D60_villagerCount);
		s += " D64=";
		s += std::to_string(ai->structTacAI.unknown_D64_villagerCount);*/

		s += "\nTO DO...\n";

		this->edtOutputInfo->WriteText(s);
	}
	delete rd;
	rConnector.DisconnectFromROR();
}



// Find unit
void WxDebugMainForm::FindObject() {
	this->edtOutputInfo->Clear();
	RORConnector rConnector = RORConnector();
	if (!rConnector.ConnectToROR()) {
		return;
	}
	long int objId = -1;
	wxString input = this->edtSubId->GetValue();
	if (input.IsNumber()) {
		if (!input.ToLong(&objId, 10)) {
			objId = -1;
		}
	}

	HANDLE hROR = rConnector.GetRORProcessHandle();
	ROR_STRUCTURES_10C::RORDebugger *rd = new ROR_STRUCTURES_10C::RORDebugger();
	if (hROR && objId >= 0) {
		rd->handleROR = hROR;
		bool valid = rd->RefreshMainGameStructs();
		std::string s;

		// Display player (to move)
		int playerId = this->edtPlayerId->GetSelection();
		if ((playerId >= 0) && (playerId < rd->gameGlobal.playerTotalCount)) {
			s = rd->ExportHostedRORObject(&rd->players[playerId], rd->playerAddresses[playerId]);
			this->edtOutputInfo->WriteText("\n\n");
			this->edtOutputInfo->WriteText(s);
			this->edtOutputInfo->WriteText("\n");
		}

		unsigned long int RORunitAddr;
		ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *unit = rd->GetUnitFromRORData(objId, RORunitAddr);
		if (unit != NULL) {
			ROR_STRUCTURES_10C::STRUCT_UNIT_TREE *tree = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_LIVING *living = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING *building = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_DEAD_FISH *deadfish = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_DOPPLEGANGER *dopple = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_FLAG *flag = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_BIRD *bird = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_PROJECTILE *proj = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50 *t50 = NULL;
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_TREE*)unit)->IsTypeValid()) { tree = (ROR_STRUCTURES_10C::STRUCT_UNIT_TREE*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_LIVING*)unit)->IsTypeValid()) { living = (ROR_STRUCTURES_10C::STRUCT_UNIT_LIVING*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING*)unit)->IsTypeValid()) { building = (ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_DEAD_FISH*)unit)->IsTypeValid()) { deadfish = (ROR_STRUCTURES_10C::STRUCT_UNIT_DEAD_FISH*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_DOPPLEGANGER*)unit)->IsTypeValid()) { dopple = (ROR_STRUCTURES_10C::STRUCT_UNIT_DOPPLEGANGER*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_FLAG*)unit)->IsTypeValid()) { flag = (ROR_STRUCTURES_10C::STRUCT_UNIT_FLAG*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_BIRD*)unit)->IsTypeValid()) { bird = (ROR_STRUCTURES_10C::STRUCT_UNIT_BIRD*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_PROJECTILE*)unit)->IsTypeValid()) { proj = (ROR_STRUCTURES_10C::STRUCT_UNIT_PROJECTILE*)unit; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50*)unit)->IsTypeValid()) { t50 = (ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50*)unit; }
			// Breakpoint here to see unit members
			free(unit);

			this->edtOutputInfo->WriteText("\n\n");
			s = rd->ExportRemoteRORObject((ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *)RORunitAddr);
			this->edtOutputInfo->WriteText(s);
			this->edtOutputInfo->WriteText("\n");
		}
	}

	rConnector.DisconnectFromROR();
}



void WxDebugMainForm::FindUnitDef() {
	this->edtOutputInfo->Clear();
	RORConnector rConnector = RORConnector();
	if (!rConnector.ConnectToROR()) {
		return;
	}
	long int unitDefId = -1;
	wxString input = this->edtSubId->GetValue();
	if (input.IsNumber()) {
		if (!input.ToLong(&unitDefId, 10)) {
			unitDefId = -1;
		}
	}

	HANDLE hROR = rConnector.GetRORProcessHandle();
	ROR_STRUCTURES_10C::RORDebugger *rd = new ROR_STRUCTURES_10C::RORDebugger();
	if (hROR) {
		rd->handleROR = hROR;
		bool valid = rd->RefreshMainGameStructs();
		// Selected player
		int playerId = this->edtPlayerId->GetSelection();
		if ((playerId < 0) || (playerId >= rd->gameGlobal.playerTotalCount)) {
			delete rd;
			return;
		}
		std::string s;
		unsigned long int RORunitDefAddr;
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDef = rd->GetUnitDefFromRORData(playerId, unitDefId, RORunitDefAddr);
		if (unitDef != NULL) {
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_TREE *tree = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *living = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *building = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_DEAD_FISH *deadfish = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_DOPPLEGANGER *dopple = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_FLAG *flag = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_BIRD *bird = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_PROJECTILE *proj = NULL;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50 *t50 = NULL;
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_TREE*)unitDef)->IsTypeValid()) { tree = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_TREE*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING*)unitDef)->IsTypeValid()) { living = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING*)unitDef)->IsTypeValid()) { building = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_DEAD_FISH*)unitDef)->IsTypeValid()) { deadfish = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_DEAD_FISH*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_DOPPLEGANGER*)unitDef)->IsTypeValid()) { dopple = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_DOPPLEGANGER*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_FLAG*)unitDef)->IsTypeValid()) { flag = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_FLAG*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_BIRD*)unitDef)->IsTypeValid()) { bird = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BIRD*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_PROJECTILE*)unitDef)->IsTypeValid()) { proj = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_PROJECTILE*)unitDef; }
			if (((ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50*)unitDef)->IsTypeValid()) { t50 = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50*)unitDef; }
			// Breakpoint here to see unit def members
			free(unitDef);

			this->edtOutputInfo->WriteText("UnitDefinition addr = ");
			this->edtOutputInfo->WriteText(GetHexStringAddress(RORunitDefAddr));
			this->edtOutputInfo->WriteText("\n");
			s = rd->ExportRemoteRORObject((ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *)RORunitDefAddr);
			this->edtOutputInfo->WriteText(s);
			this->edtOutputInfo->WriteText("\n");
		}
	}

	delete rd;
	rConnector.DisconnectFromROR();
}

