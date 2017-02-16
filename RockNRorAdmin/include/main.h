
#include <fstream>
#include "CAOE_Console.h"
//#include "CAOE_UI.h"
#include "WxMainForm.h"


//#define _CRT_SECURE_NO_WARNINGS 1
//#pragma warning(push)
//#pragma warning(disable:4996) // allow use of deprecated like strcpy

//#include <wx/wxprec.h>
//#ifndef WX_PRECOMP
#include <wx/wx.h>
//#endif

namespace CUSTOMIZE_AOE {
	enum RUN_MODE { RM_NONE = -1, RM_GUI, RM_CONSOLE, RM_INPUT };

	class CAOEApp : public wxApp
	{
	public:
		virtual bool OnInit();
		static int returnValue;
	protected:
		RUN_MODE runMode;
		//CAOE_UI objUI;
		std::wstring inputFile;
		bool ParseArguments();
		int RunUI();
		int RunSelectedMode();
		int OnRun();

		bool pauseBeforeExit;
	};


	static void usage();

}
