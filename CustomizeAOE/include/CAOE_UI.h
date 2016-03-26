#pragma once

#include "EmpiresX_API.h"
#include "CustomizeAOE_base.h"
//#include "mainWidget.h"

// Note: use __super::mymethod(...) to call parent class implementation, if needed

class CAOE_UI: public CustomizeAOE_base
{
public:
	CAOE_UI();
	~CAOE_UI();
	bool sendMessagesToBuffer; // If yes, messages are written to buffer. Otherwise they are shown as MessageBoxes.

	bool Execute();
	void FlushMessages() { this->messagesBuffer.clear(); }
	EmpiresX_API *GetAPI() { return &this->e_api; }
	std::wstring GetMessages() { return this->messagesBuffer; }
	// Make some base's protected methods public
	bool OpenEmpiresX(bool autoFixFile = true, bool noPromptBeforeFix = false) { return __super::OpenEmpiresX(autoFixFile, noPromptBeforeFix); };
	
protected:
	std::wstring messagesBuffer;

	bool AskForConfirmation(std::wstring question);
	void DisplayMessage(std::wstring message);

private:
};

