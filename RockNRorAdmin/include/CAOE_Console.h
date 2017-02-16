#pragma once

#include <iostream>
#include "mystrings.h"
#include "FileSelector.h"
#include "EmpiresXFileManager.h"
#include "../../AOE_DataPatcher/include/BinarySeqDefinition.h"
#include "FileEditor.h"
#include "../../AOE_DataPatcher/include/AOE_binData.h"
#include "EmpiresX_API.h"
#include "Version.h"
#include "CustomizeAOE_base.h"
#include "InstallManager.h"
#include "ddraw_reg_colorFix.h"

// Note: use __super::mymethod(...) to call parent class implementation, if needed

class CAOE_Console: public CustomizeAOE_base
{
public:
	CAOE_Console();
	CAOE_Console(std::wistream *inputStream);
	~CAOE_Console();

	void AssignInputStream(std::wistream *inputStream) { this->winput = inputStream; }
	void SetEcho(bool value) { this->echo = value; }
	bool Execute();
	// Returns true if at least 1 command ended with errors. Useful for batch mode.
	bool HasError();

protected:
	bool AskForConfirmation(std::wstring question);
	void DisplayMessage(std::wstring message);

private:
	std::wistream *winput;
	bool echo;
	bool hasError; // For batch mode
	
	void ShowCommands();
	bool MainConsoleLoop();
	bool ExecuteCommand(std::wstring cmd);
};

