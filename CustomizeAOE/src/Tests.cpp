#include "Tests.h"



// Returns true if OK
bool openfile(EmpiresX_API &e_api) {
	if (!e_api.OpenEmpiresXFile()) {
		return false;
	}
	wprintf(_T("File version=%s\n"), e_api.GetFileVersion().c_str());
	e_api.CancelPendingChanges();
	int techFixesStatus = e_api.HasMissingTechFixes();
	if (techFixesStatus < 0) {
		printf("Ouch, ERROR with tech fixes status...\n");
		return false;
	}
	printf("Tech fixes are all installed: %s\n", (techFixesStatus == 0) ? "true" : "false");
	bool hasObsolete = e_api.HasObsoleteSequencesInstalled();
	printf("Is some obsolete sequence installed: %s\n", hasObsolete ? "true" : "false");

	if (hasObsolete || (techFixesStatus > 0)) {
		printf("\nNot fixed bugs and obsolete changes may provoke errors and incompatibility issues.\n");
		printf("Do you want to fix the bugs and remove obsolete changes (Y/N) ? ");
		std::wstring s_input;
		std::getline(std::wcin, s_input);
		towlower(s_input);
		if (s_input == _T("y")) {
			if (techFixesStatus > 0) {
				e_api.SetAllTechFixes();
			}
			if (hasObsolete) {
				e_api.RemoveObsoletes();
			}
			if (e_api.HasPendingChanges()) {
				if (e_api.WriteChangesToFile() < 0) {
					printf("Ouch, ERROR, the file has been closed, warning. Open it again if needed.\n");
					e_api.OpenEmpiresXFile();
					return false;
				}
			}
		}
	}
	return true;
}


void console() {
	EmpiresX_API e_api;
	std::wstring s_input;
	wprintf(_T("Customize AOE %s. Type <help> for command list.\n"), widen(VER_FILE_VERSION_STR).c_str());
	wprintf(_T("Warning: this console is stupid, do not write extra chars in commands (not even spaces).\n"));
	bool go_on = true;
	bool known_cmd = false;
	bool open_file;
	while (go_on) {
		wprintf(_T(" > "));
		std::getline(std::wcin, s_input);
		towlower(s_input);
		known_cmd = false;
		open_file = false;
		if ((s_input == _T("exit")) || (s_input == _T("quit"))) { go_on = false; known_cmd = true; }
		if (s_input == _T("help")) {
			known_cmd = true;
			wprintf_s(_T("Commands are:\n"));
			wprintf_s(_T("exit / quit       : exit the program. Pending changes will be lost.\n"));
			wprintf_s(_T("open              : select a file manually and open it\n"));
			wprintf_s(_T("reopen            : open a file if you already selected one (and if you closed it)\n"));
			wprintf_s(_T("open reg          : get file name from registry and open it\n"));
			wprintf_s(_T("open txt          : type a file name and open it\n"));
			wprintf_s(_T("close             : close current file\n"));
			wprintf_s(_T("filename          : display current file name\n"));
			wprintf_s(_T("run               : run current file\n"));
			wprintf_s(_T("cancel            : cancel all pending changes\n"));
			wprintf_s(_T("pending           : show pending changes\n"));
			wprintf_s(_T("api [on|off]      : turns on/off ROR API required EXE changes\n"));
			wprintf_s(_T("reset all         : reset all changes to defaults\n"));
			wprintf_s(_T("auto              : set all recommended changes\n"));
			wprintf_s(_T("save              : write pending changes to file\n"));
			wprintf_s(_T("dump all          : display information for ALL known sequences\n"));
			wprintf_s(_T("dump names        : display names for ALL known sequences\n"));
			wprintf_s(_T("dump non default  : display sequences that are currently non-default in file\n"));
			wprintf_s(_T("selected units    : set a new value for maximum selected units\n"));
			wprintf_s(_T("windowed [on|off] : enable/disable windowed mode.\n"));
			wprintf_s(_T("manage ai [on|off]: enable/disable \"manage ai\" features.\n"));
			wprintf_s(_T("resolution        : change game resolution (type x/y coordinates).\n"));
			wprintf_s(_T("set               : set a specific option (see names with <dump names>).\n"));
		}
		try {
			if (s_input == _T("api off")) { e_api.SetROR_API(false); e_api.SetAllROR_API_optionals(false); known_cmd = true; }
			if (s_input == _T("api on")) {
				e_api.SetROR_API(true);
				e_api.SetAllROR_API_optionals(true); known_cmd = true;
				wprintf(_T("Warning: if you enable windowed mode in ROR_API, you must also activate the changes here or the game will crash.\n"));
			}
			if (s_input == _T("reset all")) {
				known_cmd = true;
				e_api.ResetToDefault(BC_ROR_API);
				e_api.ResetToDefault(BC_RESOLUTION);
				e_api.ResetToDefault(BC_OPTIONS);
				e_api.ResetToDefault(BC_SELECTED_UNITS);
				e_api.ResetToDefault(BC_WINDOWED_MODE);
				e_api.ResetToDefault(BC_OBSOLETES);
				e_api.ResetToDefault(BC_VEG_WINDOWED_MODE);
			}
			if (s_input == _T("auto")) { known_cmd = true; e_api.SetSuggestedOptions(); }
			if (s_input == _T("windowed on")) {
				known_cmd = true;
				if (!e_api.SetWindowedMode(true)) {
					wprintf_s(_T("The action could not be performed.\n"));
				}
			}
			if (s_input == _T("windowed off")) {
				known_cmd = true;
				if (!e_api.SetWindowedMode(false)) {
					wprintf_s(_T("The action could not be performed.\n"));
				}
			}
			if (s_input == _T("manage ai on")) {
				known_cmd = true;
				if (!e_api.SetManageAI(true)) {
					wprintf_s(_T("The action could not be performed.\n"));
				}
			}
			if (s_input == _T("manage ai off")) {
				known_cmd = true;
				if (!e_api.SetManageAI(false)) {
					wprintf_s(_T("The action could not be performed.\n"));
				}
			}
			if (s_input == _T("selected")) {
				known_cmd = true;
				int n;
				wprintf_s(_T("Please type new value and <return>:\n"));
				std::getline(std::wcin, s_input);
				try {
					n = stoi(s_input);
					if ((n <= 0) || (n > 500)) {
						throw std::invalid_argument("out of range");
					} else {
						e_api.SetMaxSelectedUnits(n);
					}
				}
				catch (std::invalid_argument /* &e*/) {
					wprintf_s(_T("Bad value.\n"));
				}
			}
			if (s_input == _T("resolution")) {
				known_cmd = true;
				int x, y;
				try {
					wprintf_s(_T("Please type horizontal size and <return>:\n"));
					std::getline(std::wcin, s_input);
					x = stoi(s_input);
					if ((x <= 0) || (x > 5000)) {
						throw std::invalid_argument("out of range");
					}
					wprintf_s(_T("Please type vertical size and <return>:\n"));
					std::getline(std::wcin, s_input);
					y = stoi(s_input);
					if ((y <= 0) || (y > 5000)) {
						throw std::invalid_argument("out of range");
					}
					e_api.SetResolution(x, y, true);
				}
				catch (std::invalid_argument /* &e*/) {
					wprintf_s(_T("Bad value.\n"));
				}
			}

			if (s_input == _T("set")) {
				known_cmd = true;
				// ask for name
				wprintf_s(_T("Type sequence name then <return>:\n"));
				std::wstring seqName;
				std::getline(std::wcin, seqName);

				bool hasVar;
				BINSEQ_CATEGORIES category = e_api.FindCategory(seqName);
				if (category == BC_NONE) { seqName = _T(""); }
				int userChoice = -1;
				try {
					hasVar = (e_api.OptionHasVariable(category, seqName) != SVT_NO_VARIABLE);
				}
				catch (std::exception) {
					wprintf_s(_T("Sequence does not exist.\n"));
					seqName = _T("");
					hasVar = false;
				}

				// ask var value
				if (!seqName.empty() && hasVar) {
					userChoice = 0; // only 1 sequence
					wprintf_s(_T("Type variable value then <return>:\n"));
					std::getline(std::wcin, s_input);
					if (!e_api.SetSeqSelection(category, seqName, 0, s_input)) {
						wprintf_s(_T("Something went wrong.\n"));
					}
				}

				// ask sequence to set ON/OFF
				if (!seqName.empty() && !hasVar) {
					wprintf_s(_T("Type ON or OFF (case insensitive) then <return>:\n"));
					std::getline(std::wcin, s_input);
					towlower(s_input);
					if (s_input == _T("on")) {
						userChoice = e_api.GetSeqIndexFromFuncMeaning(category, seqName, FM_ON);
					}
					if (s_input == _T("off")) {
						userChoice = e_api.GetSeqIndexFromFuncMeaning(category, seqName, FM_OFF);
					}
					e_api.SetSeqSelection(category, seqName, userChoice, _T(""));
				}
			}

			if (s_input == _T("save")) {
				known_cmd = true;
				if (e_api.WriteChangesToFile() < 0) {
					printf("ERROR while saving, reopening the file.\n");
					e_api.OpenEmpiresXFile();
				}
			}
			if (s_input == _T("pending")) {
				known_cmd = true;
				e_api.DumpPendings(_T("\n"));
			}
			if (s_input == _T("dump all")) {
				known_cmd = true;
				e_api.DumpAllInfo(_T("\n"));
			}
			if (s_input == _T("dump names")) {
				known_cmd = true;
				e_api.DumpAllNames(_T("\n"));
			}
			if (s_input == _T("dump non default")) {
				known_cmd = true;
				e_api.DumpNotDefault(_T("\n"));
			}
			if (s_input == _T("open")) {
				known_cmd = true;
				if (!e_api.SetFileNameManually()) {
					wprintf_s(_T("File doesn't exists: %s.\n"), s_input, 250);
				} else {
					open_file = true;
				}
			}
			if (s_input == _T("reopen")) {
				known_cmd = true;
				open_file = true;
			}
			if (s_input == _T("open reg")) {
				known_cmd = true;
				if (!e_api.SetFileNameFromRegistry()) {
					wprintf_s(_T("File doesn't exists: %s.\n"), s_input, 250);
				} else {
					open_file = true;
				}
			}
			if (s_input == _T("open txt")) {
				known_cmd = true;
				wprintf_s(_T("Please write file name and type <return>:\n"));
				std::getline(std::wcin, s_input);
				std::wstring noquotes;
				if (s_input.at(0) == _T('\"')) {
					noquotes = s_input.substr(1, s_input.length() - 2);
				} else {
					noquotes = s_input;
				}
				if (!e_api.SetFileName(noquotes)) {
					wprintf_s(_T("File doesn't exists: %s.\n"), noquotes.c_str(), 250);
				} else {
					open_file = true;
				}
			}
			if (s_input == _T("close")) {
				known_cmd = true;
				if (!e_api.CloseEmpiresXFile()) {
					wprintf_s(_T("Error while closing the file.\n"));
				};
			}
			if (s_input == _T("cancel")) {
				known_cmd = true;
				if (!e_api.CancelPendingChanges()) {
					wprintf_s(_T("Error while cancelling changes.\n"));
				};
			}
			if (s_input == _T("run")) {
				known_cmd = true;
				std::string cmd = "\"";
				cmd += narrow(e_api.GetFileName());
				cmd += "\"";
				wprintf_s(_T("Please wait for the game process to end...\n"));
				system(cmd.c_str());
			}
			if (s_input == _T("filename")) {
				known_cmd = true;
				wprintf_s(_T("File name=%s\n"), e_api.GetFileName().c_str(), 250);
			}

			if (open_file) {
				try {
					openfile(e_api);
				}
				catch (std::exception &e) {
					printf_s("Error while opening file: %s\n", e.what(), 250);
				}
			}

		}
		catch (std::exception &e) {
			printf_s("An error occured. %s\n", e.what(), 500);
		}
		if (!known_cmd && s_input != _T("")) { wprintf_s(_T("Unknown command. Type <help> if needed.\n")); }
	}
}


void test_api() {
	EmpiresX_API e_api;
	e_api.SetFileNameManually();
	e_api.OpenEmpiresXFile();
	e_api.DumpAllInfo(_T("\n"));
	int techFixesStatus = e_api.HasMissingTechFixes();
	if (techFixesStatus < 0) {
		printf("Ouch, ERROR with tech fixes status...\n");
	}
	printf("Tech fixes are all installed: %s\n", (techFixesStatus == 0) ? "true" : "false");
	bool test2 = e_api.HasObsoleteSequencesInstalled();
	printf("Is some obsolete sequence installed: %s\n", test2 ? "true" : "false");
	if (techFixesStatus > 0) {
		e_api.SetAllTechFixes();
	}
	if (e_api.WriteChangesToFile() < 0) {
		printf("Ouch, ERROR, the file has been closed, warning. Open it again...\n");
		e_api.OpenEmpiresXFile();
	}
	if (test2) {
		e_api.RemoveObsoletes();
	}
	if (e_api.WriteChangesToFile() < 0) {
		printf("Ouch, ERROR, the file has been closed, warning. Open it again if needed.\n");
		e_api.OpenEmpiresXFile();
	}

	int user_input = -1;
	std::wstring s_input;
	while (user_input != 0) {
		wprintf(_T("0) Quit\n1) Reset to default\n2) Enable ROR API\n3) Disable ROR API\n4) Suggested config\n5) Save\n"));
		wprintf(_T(" => "));
		std::getline(std::wcin, s_input);
		try {
			user_input = stoi(s_input);
		}
		catch (std::invalid_argument /* &e*/) {
			user_input = -1;
		}
		switch (user_input) {
		case 1:
			e_api.ResetToDefault(BC_ROR_API);
			break;
		case 2:
			e_api.SetROR_API(true);
			e_api.SetAllROR_API_optionals(true);
			break;
		case 3:
			e_api.SetAllROR_API_optionals(false);
			e_api.SetROR_API(false);
			break;
		case 4:
			e_api.SetSuggestedOptions();
			break;
		case 5:
			if (e_api.WriteChangesToFile() < 0) {
				printf("Ouch, ERROR, the file has been closed, warning. Open it again if needed.\n");
				e_api.OpenEmpiresXFile();
			}
			break;
		}
	}
}


void tmp_test() {
	console();
	return;
	
	wprintf(_T("0) Quit\n1) From registry\n2) Select file\n3) Type full path\n4) Hardcoded\n"));
	int user_input = -1;
	std::wstring s;
	FileSelector *fs = new FileSelector();
	while ((user_input != 0) && !fs->IsFileValid()) {
		wprintf(_T(" => "));
		std::getline(std::wcin, s);
		user_input = stoi(s);
		switch (user_input) {
		case 1: fs->ReadRegistry();
			break;
		case 2: fs->AskFileName();
			break;
		case 3:
			wprintf(_T("Full path of the EXE ? "));
			std::getline(std::wcin, s);
			if ((s.length() > 2) && (s.at(0) == '\"')) { s = s.substr(1, s.length() - 2); }
			fs->SetFileName(s);
			break;
		case 4: //fs->SetFileName(_T("C:\\FCD 1.0c EMPIRESX.EXE"));
			fs->SetFileName(_T("E:\\Jeux\\Age of Empires\\FCD 1.0c EMPIRESX.EXE"));
			break;
		}
	}

	if (fs->IsFileValid()) {
		wprintf(_T("File = %ls\n"), fs->getFileName().c_str(), MAX_PATH);
		EmpiresXFileManager *aoeManager = new EmpiresXFileManager();
		if (aoeManager->OpenCheckExe(fs->getFileName()) == AOE_INVALID_FILE) {
			delete aoeManager;
			delete fs;
			throw std::exception("Bad file version");
		};

		int var_int = 0;
		short int var_int2 = 0;
		try {
			var_int = aoeManager->GetUserSelection(BC_RESOLUTION, _T("HSize1")); // -1 parce que le user a rien fait
		}
		catch (std::exception &e) {
			printf(e.what());
		}

		BINSEQ_CATEGORIES conflict_category = BC_NONE;
		int conflict_index = -1;
		//GetWindowRect
		bool hasConflict = aoeManager->FindConflictingSequenceDef(BC_VEG_WINDOWED_MODE, _T("Loadconfig"), conflict_category, conflict_index);
		if (hasConflict) {
			wprintf_s(_T("Conflict with %d - %d"), conflict_category, conflict_index);
		}

		var_int = aoeManager->GetFileSelection(BC_RESOLUTION, _T("HSize2"));
		aoeManager->GetFileVarValue(BC_RESOLUTION, _T("HSize2"), &var_int2);

		//aoeManager->SetUserSelection(BC_RESOLUTION, _T("HSize2"), 0);
		aoeManager->SetUserSelectionToDefault(BC_RESOLUTION, _T("HSize2")); // selection to 0 + var to its default value
		aoeManager->GetUserVarValue(BC_RESOLUTION, _T("HSize2"), &var_int2);
		var_int2 = 42;
		aoeManager->SetUserVarValue(BC_RESOLUTION, _T("HSize2"), &var_int2);
		aoeManager->GetUserVarValue(BC_RESOLUTION, _T("HSize2"), &var_int2);
		aoeManager->SetUserVarValueFromDefault(BC_RESOLUTION, _T("HSize2"));
		aoeManager->GetUserVarValue(BC_RESOLUTION, _T("HSize2"), &var_int2);

		aoeManager->SetUserSelection(BC_RESOLUTION, _T("HSize2"), 0);
		var_int2 = 1234;
		if (!aoeManager->SwitchToWriteMode()) {
			delete aoeManager;
			aoeManager = NULL;
			delete fs;
			return;
		};
		aoeManager->SetUserVarValue(BC_RESOLUTION, _T("HSize2"), &var_int2);
		int res = aoeManager->SetFileSelection(BC_RESOLUTION, _T("HSize2"));

		delete aoeManager;
		aoeManager = NULL;
	}
	delete fs;

}


void old1() {
	AOE_binData machin;
	FileSelector *fs = new FileSelector();
	fs->ReadRegistry();

	if (fs->IsFileValid()) {
		wprintf(_T("File = %ls\n"), fs->getFileName().c_str(), MAX_PATH);
		EmpiresXFileManager *aoeManager = new EmpiresXFileManager();
		if (aoeManager->OpenCheckExe(fs->getFileName()) == AOE_INVALID_FILE) {
			delete aoeManager;
			delete fs;
			throw std::exception("Bad file version");
		};

	if (aoeManager->OpenCheckExe(fs->getFileName()) == AOE_INVALID_FILE) {
		delete aoeManager;
		delete fs;
		throw std::exception("Bad file version");
	};


	BinarySeqDefinition *bsd = new BinarySeqDefinition(5, 1);
	bsd->SetSeqOffset(0xf4edd);
	bsd->SetVarType(0, SVT_INT_4B);
	bsd->SetVarRelativeOffset(0, 1);
	//bsd->WriteSequence(0, "abcde");
	unsigned char buf[] = { 0xb8, 0xd0, 0x07, 00, 00 };
	bsd->WriteSequence(0, buf);
	std::wcout << bsd->DumpSeqBuffer(0, _T(", ")) << endl;
	unsigned long a;
	long b;
	bsd->GetDefVarValue(0, &a);// from def => toujours 2000
	bsd->GetDefVarValue(0, &b);
	std::wcout << _T("a=") << a << endl;
	std::wcout << _T("b=") << b << endl;

	FileEditor *fe = new FileEditor(aoeManager->_GetFile());
	int res = fe->CheckSeqConsistency(bsd);
	fe->GetVarValue(bsd, 0, &a);
	std::wcout << _T("res=") << res << endl;
	a = 150;
	/*if (!aoeManager->SwitchToWriteMode()) {
	delete fe;
	delete bsd;
	delete aoeManager;
	delete fs;
	throw std::exception("arf c ballot");
	}
	fe->SetFile(aoeManager->_GetFile());
	fe->SetVarValue(bsd, 0, &a);
	if (!aoeManager->SwitchToReadMode()) {
	delete fe;
	delete bsd;
	delete aoeManager;
	delete fs;
	throw std::exception("arf c ballot");
	}*/
	fe->SetFile(aoeManager->_GetFile());

	/*BinarySeqDefinition *bsd2 = new BinarySeqDefinition(6, 2, 0xc1c4);
	bsd2->SetVarType(0, SVT_NO_VARIABLE);
	bsd2->SetVarType(1, SVT_NO_VARIABLE);
	unsigned char buf2[] = { 0x74, 0x50, 0x85, 0xC0, 0x75, 0x63 };
	unsigned char buf3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	if (sizeof(buf2) != bsd2->GetSeqSize()) { throw std::exception("bouh ! pb size"); };
	bsd2->WriteSequence(0, buf2);
	bsd2->WriteSequence(1, buf3);*/
	BinarySeqDefinition *bsd2;

	machin.resolution_10c.SetUserSelection(0, 0);
	short int si;
	machin.resolution_10c.GetUserVarValue(0, &si);
	si = 321;
	machin.resolution_10c.SetUserVarValue(0, &si);
	machin.resolution_10c.GetUserVarValue(0, &si);
	machin.resolution_10c.SetUserVarValue_default(0);
	machin.resolution_10c.GetUserVarValue(0, &si);

	for (int cpt = 0; cpt < machin.resolution_10c.GetCount(); cpt++) {
		bsd2 = machin.resolution_10c.GetBinSeqDefinition(cpt);
		res = fe->CheckSeqConsistency(bsd2);
		char var1;
		short int var2;
		long int var4;
		float varFloat;
		if (res > -1) {
			switch (bsd2->GetVarType(res)) {
			case SVT_INT_1B: fe->GetVarValue(bsd2, res, &var1); break;
			case SVT_INT_2B: fe->GetVarValue(bsd2, res, &var2); break;
			case SVT_INT_4B: fe->GetVarValue(bsd2, res, &var4); break;
			case SVT_FLOAT_4B: fe->GetVarValue(bsd2, res, &varFloat); break;
			default:break;
			}
		}
		//std::cout << machin.options_10c.GetBinSeqDefinition(cpt)->GetSeqName().c_str() << endl;
		wprintf(_T("- %50s - match=%d"), bsd2->GetSeqName().c_str(), res);
		switch (bsd2->GetVarType(res)) {
		case SVT_INT_2B: wprintf(_T("\t%d"), (long int)var2); break;
		case SVT_INT_4B: wprintf(_T("\t%d"), var4); break;
		case SVT_INT_1B: wprintf(_T("\t%d"), (long int)var1); break;
		case SVT_FLOAT_4B: wprintf(_T("\t%f"), varFloat); break;
		default: break;
		}
		wprintf(_T("\n"));
		//printf(" value=%d", res);
		//printf("\n");
		//if 
		/*if (!aoeManager->SwitchToWriteMode()) {
		delete fe;
		delete bsd;
		delete bsd2;
		delete aoeManager;
		delete fs;
		throw std::exception("arf c ballot");
		}*/
	}
	//fe->SetFile(aoeManager->_GetFile());
	//fe->GetVarValue(bsd2, 0, &a);
	//fe->WriteFromSequence(bsd2, 0);


	delete fe;
	fe = NULL;
	delete bsd;
	bsd = NULL;
	//delete bsd2;
	bsd2 = NULL;

	delete aoeManager;
	aoeManager = NULL;
		}
		delete fs;
}

