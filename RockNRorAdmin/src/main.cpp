// application entry point
//

#include "main.h"

using namespace CUSTOMIZE_AOE;

IMPLEMENT_APP_NO_MAIN(CAOEApp)

int CAOEApp::returnValue = 0;

static void CUSTOMIZE_AOE::usage() {
	printf("You can call this program with...\n");
	printf("- No argument\n");
	printf("- 1 argument: [-GUI|-console]\n");
	printf("- 2 arguments: -input <filename>\n");
	printf("- Other optional arguments:\n");
	printf("  -pause = Do not close window automatically in console/input modes\n");
}


int CAOEApp::RunUI() {
	try {
		WxMainForm *objUI = new WxMainForm("RockNRor Admin", wxPoint(50, 50), wxSize(600, 400));
		objUI->Show(true);
		return 0;
	}
	catch (...) {
		return -1;
	}
}


bool CAOEApp::ParseArguments() {
	int argCount = wxGetApp().argc;
	wxArrayString args = wxGetApp().argv.GetArguments();
	this->runMode = RM_NONE;
	if (argCount == 1) { this->runMode = RM_GUI; } // Default value (no command-line argument)
	for (wxArrayString::iterator it = args.begin(), en = args.end(); it != en; ++it) {
		if (*it == _T("-console")) {
			this->runMode = RM_CONSOLE;
		}
		if (*it == _T("-GUI")) {
			this->runMode = RM_GUI;
		}
		if (*it == _T("-input")) {
			if ((it + 1) != en) {
				it++;
				this->runMode = RM_INPUT;
				this->inputFile = *it;
			} else {
				this->runMode = RM_NONE;
			}
		}
		if (*it == _T("-pause")) {
			this->pauseBeforeExit = true;
		}
	}

	return true;
}


bool CAOEApp::OnInit()
{
	this->pauseBeforeExit = false;
	this->inputFile = _T("");
	this->ParseArguments();
	this->returnValue = this->RunSelectedMode();

	return true;
}


int CAOEApp::RunSelectedMode()
{
	if (this->runMode == RM_NONE) {
		this->pauseBeforeExit = true;
		usage();
		return 1;
	}

	CAOE_Console objConsole;
	std::wifstream stream;

	switch (this->runMode) {
	case RM_GUI:
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		return RunUI();
	case RM_CONSOLE:
		ShowWindow(GetConsoleWindow(), SW_NORMAL);
		objConsole.AssignInputStream(&std::wcin);
		break;
	case RM_INPUT:
		ShowWindow(GetConsoleWindow(), SW_NORMAL);
		stream.open(this->inputFile, std::ios_base::in);
		if (!stream.is_open()) {
			wprintf_s(_T("Could not open input file %s.\n"), this->inputFile.c_str());
			return 2; // Probably an invalid file name
		}
		objConsole.AssignInputStream(&stream);
		objConsole.SetEcho(true);
		objConsole.SetInteractive(false);
		break;
	default:
		printf("Bad mode - internal error.\n");
		this->pauseBeforeExit = true;
		return -2;
	}

	// Console mode
	try {
		objConsole.Execute();
		if ((this->runMode == RUN_MODE::RM_INPUT) && objConsole.HasError()) {
			wprintf_s(_T("Batch mode ended with ERRORS.\n"));
			return 1;
		}
	}
	catch (std::exception &e) {
		printf("Critical ERROR. Message: %s\n", e.what());
	}
	catch (...) {
		printf("Critical ERROR. The program is not supposed to raise an exception there :(\n");
		return -1;
	}

	return 0;
}


int CAOEApp::OnRun() {
	if (this->runMode == RM_GUI) {
		wxApp::OnRun();
	}
	if (this->pauseBeforeExit) {
		system("pause");
	}
	return 0;
}

// Source: http://comp.soft-sys.wxwindows.narkive.com/ccWVHxF3/windows-gui-application-with-console
int main(int argc, char *argv[])
{
	HINSTANCE inst;
	inst = (HINSTANCE)GetModuleHandle(NULL);

	// use the wxEntry method as defined in wx/msw/app.h, I've included this declaration as a comment below
	wxEntry(inst, NULL, NULL, SW_HIDE);

	// this return is never reached when closing the console,
	// only when closing the wxFrame, hence the wxApp
	return CAOEApp::returnValue;
}

