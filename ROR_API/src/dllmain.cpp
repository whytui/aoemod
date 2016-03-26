
#include "../include/ConfigManager.h"

// Global variable
ConfigManager global_conf;


/*The entry-point function should perform only simple initialization or termination tasks.
It must not call the LoadLibrary or LoadLibraryEx function (or a function that calls these functions),
because this may create dependency loops in the DLL load order.
This can result in a DLL being used before the system has executed its initialization code.
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: 
		global_conf.ReadConfigFromFile();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

