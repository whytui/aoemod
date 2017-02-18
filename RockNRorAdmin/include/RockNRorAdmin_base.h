#pragma once

#include "EmpiresX_API.h"

/*
Generic class that relies on EmpiresX_API.
Inherit from this class to create your own API/Program.
*/
class RockNRorAdmin_base
{
// We define virtual methods...
// ...so that calls from parent class methods will call child class' implementation
public:
	RockNRorAdmin_base();
	~RockNRorAdmin_base();

	// Main entry point for the program. To be defined in child class. Returns true if successful.
	virtual bool Execute() = 0; // abstract method
	virtual void SetInteractive(bool isInteractive) { this->interactive = isInteractive; }

protected:
	EmpiresX_API e_api;
	bool interactive; // Set it to false if there is no interaction with user (batch mode)

	virtual bool OpenEmpiresX(bool autoFixFile = true, bool noPromptBeforeFix = false);
	virtual bool CheckAndRunAutoFixes(bool noPromptBeforeFix = false);

	// Ask user for a confirmation. To be defined in child class.
	virtual bool AskForConfirmation(std::wstring question) = 0; // abstract method - needs a child class for implementation 

	// Display a message
	virtual void DisplayMessage(std::wstring message) {};

private:

};

