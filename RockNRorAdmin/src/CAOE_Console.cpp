#include "CAOE_Console.h"


CAOE_Console::CAOE_Console()
{
	this->winput = NULL;
	this->echo = false;
	this->hasError = false;
}


CAOE_Console::CAOE_Console(std::wistream *inputStream) {
	this->winput = inputStream;
	this->echo = false;
	this->hasError = false;
}


CAOE_Console::~CAOE_Console()
{
}

// Parent class methods implementation


// Display a message to the console
void CAOE_Console::DisplayMessage(std::wstring message) {
	wprintf(message.c_str());
}


// Returns true if the user answered Y or y to the question
bool CAOE_Console::AskForConfirmation(std::wstring question) {
	std::wstring s_input;
	this->DisplayMessage(question);
	std::getline(*this->winput, s_input);
	towlower(s_input);
	return (s_input == _T("y"));
}


// Program entry point
bool CAOE_Console::Execute() {
	wprintf(_T("RockNRor Admin %s. Type <help> for command list.\n"), widen(VER_FILE_VERSION_STR).c_str());
	wprintf(_T("Warning: this console is stupid, do not write extra chars in commands (not even spaces).\n"));
	return this->MainConsoleLoop();
}


// Returns true if at least 1 command ended with errors. Useful for batch mode.
bool CAOE_Console::HasError() {
	return this->hasError;
}


// Useful macro to show an error message for commands that require EXE file to be opened.
#define FAIL_IF_NO_EXE(exefilevar) if (exefilevar.empty()) { \
this->hasError = true; \
wprintf_s(_T("You need to select empiresx EXE first.\n")); \
return true; \
}


// --------------------------------------------
// Own methods

void CAOE_Console::ShowCommands() {
	wprintf_s(_T("Commands are:\n"));
	wprintf_s(_T("exit / quit        : exit the program. Pending changes will be lost.\n"));
	wprintf_s(_T("#anything          : lines with leading # are ignored (comments)\n"));
	wprintf_s(_T("prompt             : to write some text in output (useful in scripts)\n"));
	wprintf_s(_T("echo [on|off]      : turns on/off echoing typed commands (useful in scripts)\n"));
	wprintf_s(_T("@anycommand        : disables echo (if enabled) just for this command (similar to DOS)\n"));
	wprintf_s(_T("open               : select a file manually and open it\n"));
	wprintf_s(_T("reopen             : open a file if you already selected one (and if you closed it)\n"));
	wprintf_s(_T("open reg           : get file name from registry and open it\n"));
	wprintf_s(_T("open txt           : type a file name and open it\n"));
	wprintf_s(_T("close              : close current file\n"));
	wprintf_s(_T("filename           : display current file name\n"));
	wprintf_s(_T("run                : run current file\n"));
	wprintf_s(_T("cancel             : cancel all pending changes\n"));
	wprintf_s(_T("pending            : show pending changes\n"));
	wprintf_s(_T("autofix            : fix (+save) technical bugs (done by \"open...\" commands except in batch mode). This LOSES all pending changes.\n"));
	wprintf_s(_T("reset all          : reset all changes to defaults\n"));
	wprintf_s(_T("auto               : set all recommended changes\n"));
	wprintf_s(_T("save               : write pending changes to file\n"));
	wprintf_s(_T("dump all           : display information for ALL known sequences\n"));
	wprintf_s(_T("dump all w obs     : display information for ALL known sequences including OBSOLETE.\n"));
	wprintf_s(_T("dump names         : display names for ALL known sequences\n"));
	wprintf_s(_T("dump non default   : display sequences that are currently non-default in file\n"));
	wprintf_s(_T("selected units     : set a new value for maximum selected units\n"));
	wprintf_s(_T("windowed [on|off]  : enable/disable windowed mode.\n"));
	wprintf_s(_T("manage ai [on|off] : enable/disable \"manage ai\" features.\n"));
	wprintf_s(_T("audiovideo [on|off]: enable/disable \"mp3\" music file format instead of midi (.mid).\n"));
	wprintf_s(_T("resolution         : change game resolution (type x/y coordinates).\n"));
	wprintf_s(_T("api [on|off]       : turns on/off ROR API required EXE changes\n"));
	wprintf_s(_T("set                : set a specific option (see names with <dump names>).\n"));
	wprintf_s(_T("install rocknror   : copy ROR_API and RockNRor files to AOE installation folder.\n"));
	// Note: copy itf drs is for use with RockNRor (copies into itf_custom.drs and preserves interfac.drs files)
	wprintf_s(_T("copy itf drs       : copy custom interfac.drs files to AOE installation folder (for custom resolutions).\n"));
	wprintf_s(_T("install color fix  : patch windows registry to fix directdraw color bug.\n"));
}


// Executes the provided command (wstring).
// Displays a message if command is unknown.
// Returns false if the command asks to exit the console
bool CAOE_Console::ExecuteCommand(std::wstring cmd) {
	std::wstring s_input;
	bool open_file = false;
	if ((cmd == _T("exit")) || (cmd == _T("quit"))) {
		return false; // indicate to caller we have to leave ;)
	}
	if (cmd.empty()) { return true; }
	if (cmd.at(0) == _T('#')) { return true; } // # prefix allows comments (such lines are ignored).
	if ((cmd == _T("help")) || (cmd == _T("?"))) {
		ShowCommands();
		return true;
	}
	if (cmd == _T("prompt")) {
		//wprintf_s(_T("Please type the text that will be written to output:\n"));
		std::getline(*this->winput, s_input);
		wprintf_s(_T("%s\n"), s_input.c_str());
		return true;
	}
	if (cmd == _T("echo on")) {
		this->echo = true;
		return true;
	}
	if (cmd == _T("echo off")) {
		this->echo = false;
		return true;
	}
	if (cmd == _T("api off")) {
		this->e_api.SetROR_API(false);
		if (!this->e_api.SetAllROR_API_optionals(false)) { this->hasError = true; }
		return true;
	}
	if (cmd == _T("api on")) {
		this->e_api.SetROR_API(true);
		//if (!this->e_api.SetAllROR_API_optionals(true)) { this->hasError = true; } // Do not install optionals, RockNRor can install them dynamically
		wprintf(_T("Warning: if you enable windowed mode in ROR_API, you must also activate the changes here or the game will crash.\n"));
		return true;
	}
	if (cmd == _T("reset all")) {
		wprintf(_T("This will not revert tech fixes.\n"));
		if (!this->e_api.ResetToDefault(BC_ROR_API)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_RESOLUTION)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_OPTIONS)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_SELECTED_UNITS)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_WINDOWED_MODE)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_MANAGE_AI)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_AUDIO_VIDEO)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_OBSOLETES)) { this->hasError = true; }
		if (!this->e_api.ResetToDefault(BC_VEG_WINDOWED_MODE)) { this->hasError = true; }
		return true;
	}
	if (cmd == _T("auto")) { this->e_api.SetSuggestedOptions(); return true; }
	if (cmd == _T("autofix")) {
		if (!this->CheckAndRunAutoFixes(true)) { this->hasError = true; }
		return true;
	}
	
	if (cmd == _T("windowed on")) {
		if (!this->e_api.SetWindowedMode(true)) {
			wprintf_s(_T("The action could not be performed.\n"));
			this->hasError = true;
		}
		return true;
	}
	if (cmd == _T("windowed off")) {
		if (!this->e_api.SetWindowedMode(false)) {
			wprintf_s(_T("The action could not be performed.\n"));
			this->hasError = true;
		}
		return true;
	}
	if (cmd == _T("manage ai on")) {
		if (!this->e_api.SetManageAI(true)) {
			wprintf_s(_T("The action could not be performed.\n"));
			this->hasError = true;
		}
		return true;
	}
	if (cmd == _T("manage ai off")) {
		if (!this->e_api.SetManageAI(false)) {
			wprintf_s(_T("The action could not be performed.\n"));
			this->hasError = true;
		}
		return true;
	}
	if (cmd == _T("audiovideo on")) {
		if (!this->e_api.SetAllAudioVideoOptions(true)) {
			wprintf_s(_T("The action could not be performed.\n"));
			this->hasError = true;
		}
		return true;
	}
	if (cmd == _T("audiovideo off")) {
		if (!this->e_api.SetAllAudioVideoOptions(false)) {
			wprintf_s(_T("The action could not be performed.\n"));
			this->hasError = true;
		}
		return true;
	}

	if (cmd == _T("selected units")) {
		int n;
		wprintf_s(_T("Please type new value and <return>:\n"));
		std::getline(*this->winput, s_input);
		try {
			n = stoi(s_input);
			if ((n <= 0) || (n > 500)) {
				this->hasError = true;
				throw std::out_of_range("Number is out of range");
			} else {
				if (!this->e_api.SetMaxSelectedUnits(n)) { this->hasError = true; }
			}
		}
		catch (std::invalid_argument /* &e*/) {
			wprintf_s(_T("Bad value.\n"));
			this->hasError = true;
		}
		catch (std::out_of_range /* &e*/) {
			wprintf_s(_T("Bad value.\n"));
			this->hasError = true;
		}
		return true;
	}

	if (cmd == _T("resolution")) {
		int x, y;
		try {
			wprintf_s(_T("Please type horizontal size and <return>:\n"));
			std::getline(*this->winput, s_input);
			x = stoi(s_input);
			if ((x <= 0) || (x > 5000)) {
				this->hasError = true;
				throw std::invalid_argument("out of range");
			}
			wprintf_s(_T("Please type vertical size and <return>:\n"));
			std::getline(*this->winput, s_input);
			y = stoi(s_input);
			if ((y <= 0) || (y > 5000)) {
				this->hasError = true;
				throw std::invalid_argument("out of range");
			}
			if (!this->e_api.SetResolution(x, y, true)) { this->hasError = true; }
		}
		catch (std::invalid_argument /* &e*/) {
			this->hasError = true;
			wprintf_s(_T("Bad value.\n"));
		}
		return true;
	}

	if (cmd == _T("set")) {
		// ask for name
		wprintf_s(_T("Type sequence name then <return>:\n"));
		std::wstring seqName;
		std::getline(*this->winput, seqName);

		bool hasVar;
		BINSEQ_CATEGORIES category = this->e_api.FindCategory(seqName);
		if (category == BC_NONE) { seqName = _T(""); }
		int userChoice = -1;
		try {
			hasVar = (this->e_api.OptionHasVariable(category, seqName) != SVT_NO_VARIABLE);
		}
		catch (std::exception) {
			wprintf_s(_T("Sequence does not exist.\n"));
			seqName = _T("");
			hasVar = false;
			this->hasError = true;
		}

		// ask var value
		if (!seqName.empty() && hasVar) {
			userChoice = 0; // only 1 sequence
			wprintf_s(_T("Type variable value then <return>:\n"));
			std::getline(*this->winput, s_input);
			if (!this->e_api.SetSeqSelection(category, seqName, 0, s_input)) {
				this->hasError = true;
				wprintf_s(_T("Something went wrong.\n"));
			}
		}

		// ask sequence to set ON/OFF
		if (!seqName.empty() && !hasVar) {
			wprintf_s(_T("Type ON or OFF (case insensitive) then <return>:\n"));
			std::getline(*this->winput, s_input);
			towlower(s_input);
			if (s_input == _T("on")) {
				userChoice = this->e_api.GetSeqIndexFromFuncMeaning(category, seqName, FM_ON);
			}
			if (s_input == _T("off")) {
				userChoice = this->e_api.GetSeqIndexFromFuncMeaning(category, seqName, FM_OFF);
			}
			if (!this->e_api.SetSeqSelection(category, seqName, userChoice, _T(""))) { this->hasError = true; }
		}
		return true;
	}

	if (cmd == _T("save")) {
		FAIL_IF_NO_EXE(e_api.GetFileName());
		if (this->e_api.WriteChangesToFile() < 0) {
			this->hasError = true;
			printf("ERROR while saving, reopening the file.\n");
			this->e_api.OpenEmpiresXFile();
		}
		return true;
	}
	if (cmd == _T("pending")) {
		this->e_api.DumpPendings(_T("\n"));
		return true;
	}
	if (cmd == _T("dump all")) {
		this->e_api.DumpAllInfo(_T("\n"), false);
		return true;
	}
	if (cmd == _T("dump all w obs")) {
		this->e_api.DumpAllInfo(_T("\n"), true);
		return true;
	}
	if (cmd == _T("dump names")) {
		this->e_api.DumpAllNames(_T("\n"));
		return true;
	}
	if (cmd == _T("dump non default")) {
		this->e_api.DumpNotDefault(_T("\n"));
		return true;
	}
	if (cmd == _T("open")) {
		if (!this->e_api.SetFileNameManually()) {
			this->hasError = true;
			wprintf_s(_T("File doesn't exists: %s.\n"), s_input, 250);
			return true;
		} else {
			open_file = true;// Do not return (so we actually open the file)
		}
	}
	if (cmd == _T("reopen")) {
		open_file = true;
	}
	if (cmd == _T("open reg")) {
		if (!e_api.SetFileNameFromRegistry()) {
			this->hasError = true;
			wprintf_s(_T("File doesn't exists: %s.\n"), s_input, 250);
			return true;
		} else {
			open_file = true;
			// Do not return (so we actually open the file)
		}
	}
	if (cmd == _T("open txt")) {
		wprintf_s(_T("Please write file name and type <return>:\n"));
		std::getline(*this->winput, s_input);
		std::wstring noquotes;
		if (!s_input.empty() && (s_input.at(0) == _T('\"'))) {
			noquotes = s_input.substr(1, s_input.length() - 2);
		} else {
			noquotes = s_input;
		}
		if (!e_api.SetFileName(noquotes)) {
			this->hasError = true;
			wprintf_s(_T("File doesn't exists: %s.\n"), noquotes.c_str(), 250);
		} else {
			open_file = true;
		}
		// Do not return
	}
	if (cmd == _T("close")) {
		if (!e_api.CloseEmpiresXFile()) {
			this->hasError = true;
			wprintf_s(_T("Error while closing the file.\n"));
		};
		return true;
	}
	if (cmd == _T("cancel")) {
		if (!e_api.CancelPendingChanges()) {
			this->hasError = true;
			wprintf_s(_T("Error while cancelling changes.\n"));
		};
		return true;
	}
	if (cmd == _T("run")) {
		std::wstring filename = e_api.GetFileName();
		FAIL_IF_NO_EXE(filename);
		std::wstring dirname = filename.substr(0, filename.find_last_of(_T("/\\")));
		SetCurrentDirectory(dirname.c_str());
		std::string syscmd = "\"";
		syscmd += narrow(filename);
		syscmd += "\"";
		wprintf_s(_T("Please wait for the game process to end...\n"));
		system(syscmd.c_str());
		return true;
	}
	if (cmd == _T("filename")) {
		wprintf_s(_T("File name=%s\n"), e_api.GetFileName().c_str(), 250);
		return true;
	}
	if (cmd == _T("install rocknror")) {
		std::wstring filename = e_api.GetFileName();
		FAIL_IF_NO_EXE(filename);
		wprintf_s(_T("Please write RockNRorAdmin files source (root) directory full path and type <return>:\n"));
		wprintf_s(_T("(this folder should contain various files/folders, including ROR_API.dll file and RockNRor\\ subdirectory)\n"));
		std::getline(*this->winput, s_input);
		std::wstring noquotes;
		if (!s_input.empty() && (s_input.at(0) == _T('\"'))) {
			noquotes = s_input.substr(1, s_input.length() - 2);
		} else {
			noquotes = s_input;
		}
		std::wstring shortMessage = _T("");
		std::wstring installLogs = _T("");
		bool success = installCustomRORFiles(noquotes, filename, true, shortMessage, installLogs);
		if (!success) { this->hasError = true; }
		wprintf_s(_T("%s\n"), installLogs.c_str());
		wprintf_s(_T("Installation status summary:\n%s\n"), shortMessage.c_str());
		wprintf_s(success ? _T("Success.\n") : _T("Failed.\n"));
		return true;
	}
	if (cmd == _T("copy itf drs")) {
		std::wstring filename = e_api.GetFileName();
		FAIL_IF_NO_EXE(filename);
		wprintf_s(_T("Please type source 'interfac.drs' (for data dir) full path and type <return>:\n"));
		std::getline(*this->winput, s_input);
		std::wstring noquotes;
		if (!s_input.empty() && (s_input.at(0) == _T('\"'))) {
			noquotes = s_input.substr(1, s_input.length() - 2);
		} else {
			noquotes = s_input;
		}
		std::wstring dataDrsFullPath = noquotes;
		wprintf_s(_T("Please type source 'interfac.drs' (for data2 dir) full path and type <return>:\n"));
		std::getline(*this->winput, s_input);
		if (!s_input.empty() && (s_input.at(0) == _T('\"'))) {
			noquotes = s_input.substr(1, s_input.length() - 2);
		} else {
			noquotes = s_input;
		}
		std::wstring data2DrsFullPath = noquotes;
		std::wstring installLogs = _T("");
		bool success = installResolutionFiles(dataDrsFullPath, data2DrsFullPath, filename, installLogs);
		if (!success) { this->hasError = true; }
		wprintf_s(_T("%s\n"), installLogs.c_str());
		wprintf_s(success ? _T("Success.\n") : _T("Failed.\n"));
		return true;
	}
	if (cmd == _T("install color fix")) {
		std::wstring filename = e_api.GetFileName();
		FAIL_IF_NO_EXE(filename);
		if (FixRegistryForDDrawColorBug(e_api.GetFileName())) {
			wprintf_s(_T("Successfully installed directdraw color bug fix.\n"));
		} else {
			this->hasError = true;
			wprintf_s(_T("An error occurred, color fix was not installed. Maybe you are missing admin rights or some registry values are missing.\n"));
		}
		return true;
	}

	if (open_file) {
		try {
			if (!this->OpenEmpiresX(true, false)) { this->hasError = true; }
		}
		catch (std::exception &e) {
			this->hasError = true;
			printf_s("Error while opening file: %s\n", e.what(), 250);
		}
		return true;
	}

	// If cmd does not correspond to a known command:
	if (cmd != _T("")) { wprintf_s(_T("Unknown command. Type <help> if needed.\n")); }
	return true;
}


bool CAOE_Console::MainConsoleLoop() {
	std::wstring s_input;
	bool go_on = true;
	bool silentCommand = false;
	while (go_on && !this->winput->eof()) {
		if (!silentCommand) {
			wprintf(_T(" > "));
		}
		silentCommand = false;
		std::getline(*this->winput, s_input);
		towlower(s_input);
		// "@" prefix mutes "this" command's echo (ONLY this command)
		if (!s_input.empty() && (s_input.at(0) == _T('@'))) {
			s_input = s_input.substr(1); // Remove leading '@'
			silentCommand = true;
		}

		if (this->echo && !silentCommand) { this->DisplayMessage(s_input + _T("\n")); }
		try {
			go_on = ExecuteCommand(s_input);
		}
		catch (std::exception &e) {
			printf_s("An error occured: %s\n", e.what(), 500);
		}
	}
	return !this->hasError;
}

