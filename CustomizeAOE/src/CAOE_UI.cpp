#include "CAOE_UI.h"


CAOE_UI::CAOE_UI()
{
	messagesBuffer = _T("");
	sendMessagesToBuffer = false;
}


CAOE_UI::~CAOE_UI()
{
}

// Parent class methods implementation


// Display a message to the user
void CAOE_UI::DisplayMessage(std::wstring message) {
	if (this->sendMessagesToBuffer) {
		this->messagesBuffer += message;
	} else {
		MessageBox(0, message.c_str(), _T("Customize AOE"), MB_ICONINFORMATION);
	}
}


// Returns true if the user answered Yes to the question
bool CAOE_UI::AskForConfirmation(std::wstring question) {
	// GetActiveWindow() makes the message modal. User MUST first answer the question before doing anything in the UI.
	int answer = MessageBox(GetActiveWindow(), question.c_str(), _T("Customize AOE"), MB_YESNO);
	return (answer == IDYES);
}


// Unused: the main UI class runs itself what is needed...
bool CAOE_UI::Execute() {
	//CAOE_MainForm *mainForm = new CAOE_MainForm("Customize AOE", wxPoint(50, 50), wxSize(450, 340));
	//mainForm->SetAPI(&this->e_api);
	//mainForm->Show(true);
	return true;
}


