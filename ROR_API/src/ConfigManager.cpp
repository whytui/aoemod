#include "../include/ConfigManager.h"

#define BUFFER_SIZE 255

ConfigManager::ConfigManager() {
	Init();
}

ConfigManager::~ConfigManager() {
	free(this->DLL_names_first);
	this->DLL_names_first = NULL;
	if (this->hModules) {
		free(this->hModules);
	}
	this->hModules = NULL;
}

void ConfigManager::Init() {
	this->currentCalls = 0;
	this->DLL_names_first = new StringList();
	this->DLL_names_current = this->DLL_names_first;
	this->hModules = NULL;
	this->modulesCount = 0;
}


// Returns 0 if OK
int ConfigManager::ReadConfigFromFile() {
#ifdef _DEBUG
	MessageBox(0, L"Hi there ! You are using DEBUG version of RoR API.", L"RoR API", 0);
#endif

	if (!this->DLL_names_first) { return 1; }
	char bufferRead[BUFFER_SIZE];
	bufferRead[BUFFER_SIZE - 1] = 0; // Just a security
	FILE *configFile;
	errno_t errNo = fopen_s(&configFile, CONF_FILE_NAME, "rb");
	if (errNo || !configFile) { return 1; }
	try {
		fseek(configFile, 0, SEEK_SET); // begin of file

		while (fscanf_s(configFile, "%s\n", bufferRead, BUFFER_SIZE - 1) > 0) {
			bool ignore_line;
			int len;
			for (len = 0; bufferRead[len] != 0; len++);
			if ((len >= 3) && (bufferRead[len - 3] == 'D')) { bufferRead[len - 3] = 'd'; }
			if ((len >= 2) && (bufferRead[len - 2] == 'L')) { bufferRead[len - 2] = 'l'; }
			if ((len >= 1) && (bufferRead[len - 1] == 'L')) { bufferRead[len - 1] = 'l'; }
			ignore_line = ((len < 4) || (bufferRead[0] == '#') || (bufferRead[len - 4] != '.') || (bufferRead[len - 3] != 'd') || (bufferRead[len - 2] != 'l') || (bufferRead[len - 1] != 'l'));
			if (!ignore_line) {
				// Add dll to list
				this->DLL_names_current->AddAfter(bufferRead, len);
				this->DLL_names_current = this->DLL_names_current->next;
			}
		}
	}
	catch (...) {
		fclose(configFile);
		return 2;
	}
	fclose(configFile);
	return 0;
}


// Load all DLL libraries from names we read and stored in this->DLL_names_first string list.
// this->modulesCount is updated with the size of this->hModules
// Some values in this->hModules[...] may be NULL (if DLL couldn't be loaded)
void ConfigManager::LoadModules() {
	if (!this->DLL_names_first) return;
	for (StringList *cur = this->DLL_names_first; cur; cur = cur->next) {
		if (cur->string) { this->modulesCount++; }
	}
	this->hModules = (HMODULE*) malloc(this->modulesCount * sizeof(HMODULE));
	memset(this->hModules, 0, this->modulesCount * sizeof(HMODULE));
	int iModule = 0;
	for (StringList *cur = this->DLL_names_first; cur; cur = cur->next) {
		if (cur->string) {
			this->hModules[iModule] = LoadLibraryA(cur->string);
#ifdef _DEBUG
			if (this->hModules[iModule]) {
				char s[300];
				sprintf_s(s, "Loaded module: %s ; hModule=%d", cur->string, this->hModules[iModule]);
				MessageBoxA(0, s, "ROR API", MB_ICONINFORMATION);
			}
#endif
			iModule++;
		}
	}
}
