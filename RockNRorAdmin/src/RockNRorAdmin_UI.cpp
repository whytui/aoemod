#include "RockNRorAdmin_UI.h"


RockNRorAdmin_UI::RockNRorAdmin_UI()
{
	messagesBuffer = _T("");
	sendMessagesToBuffer = false;
}


RockNRorAdmin_UI::~RockNRorAdmin_UI()
{
}

// Parent class methods implementation


// Display a message to the user
void RockNRorAdmin_UI::DisplayMessage(std::wstring message) {
	if (this->sendMessagesToBuffer) {
		this->messagesBuffer += message;
	} else {
		MessageBox(0, message.c_str(), _T("RockNRor Admin"), MB_ICONINFORMATION);
	}
}


// Returns true if the user answered Yes to the question
bool RockNRorAdmin_UI::AskForConfirmation(std::wstring question) {
	// GetActiveWindow() makes the message modal. User MUST first answer the question before doing anything in the UI.
	int answer = MessageBox(GetActiveWindow(), question.c_str(), _T("RockNRor Admin"), MB_YESNO);
	return (answer == IDYES);
}


// Unused: the main UI class runs itself what is needed...
bool RockNRorAdmin_UI::Execute() {
	//CAOE_MainForm *mainForm = new CAOE_MainForm("RockNRor Admin", wxPoint(50, 50), wxSize(450, 340));
	//mainForm->SetAPI(&this->e_api);
	//mainForm->Show(true);
	return true;
}


