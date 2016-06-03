#include "InstallManager.h"

// Manage file operations to install customROR (and ROR_API + wndmode.dll)
// shortMessage and logs are OUT parameters.
// Returns true if successful
bool installCustomRORFiles(std::wstring sourceDirectory, std::wstring targetExeFullPath, bool overwriteFiles,
	std::wstring &shortMessage, std::wstring &logs) {
	shortMessage = _T("");
	logs = _T("");
	if (targetExeFullPath.empty()) {
		shortMessage = _T("Missing target EXE full path.");
		return false;
	}
	if (sourceDirectory.empty()) {
		shortMessage = _T("Missing source files directory"); 
		return false;
	}

	std::wstring ROR_filename = targetExeFullPath;
	std::wstring destDirName = extractDirectory(ROR_filename);
	std::wstring destCustomROR_DLL = destDirName + _T("\\CustomROR.dll");

	// Compares src and dest
	if (sourceDirectory.compare(destDirName.c_str()) == 0) {
		shortMessage = _T("Source and target are identical, did not proceed to installation.");
		return false;
	}

	// Build all file names
	std::wstring destCustomRorDir = destDirName + _T("\\CustomROR");
	std::wstring srcCustomROR_DLL = sourceDirectory + _T("\\customROR.dll");
	std::wstring srcCustomROR_XML = sourceDirectory + _T("\\CustomROR.xml");
	std::wstring srcCustomROR_civs_XML = sourceDirectory + _T("\\CustomROR_civs.xml");
	std::wstring srcRORAPI_DLL = sourceDirectory + _T("\\ROR_API.dll");
	std::wstring srcRORAPI_CONF = sourceDirectory + _T("\\ROR_API.conf");
	std::wstring srcWNDMODE_DLL = sourceDirectory + _T("\\wndmode.dll");
	std::wstring srcCustomROR_DRS = sourceDirectory + _T("\\CustomROR.drs");
	std::wstring destCustomROR_XML = destDirName + _T("\\CustomROR.xml");
	std::wstring destCustomROR_civs_XML = destDirName + _T("\\CustomROR_civs.xml");
	std::wstring destRORAPI_DLL = destDirName + _T("\\ROR_API.dll");
	std::wstring destRORAPI_CONF = destDirName + _T("\\ROR_API.conf");
	std::wstring destWNDMODE_DLL = destDirName + _T("\\wndmode.dll");
	std::wstring destCustomROR_DRS = destDirName + _T("\\CustomROR\\CustomROR.drs");
	// Get current files status (existing ?)
	bool hasSrcCustomROR_XML = CheckFileExistence(srcCustomROR_XML);
	bool hasSrcCustomROR_civs_XML = CheckFileExistence(srcCustomROR_civs_XML);
	bool hasSrcRORAPI_DLL = CheckFileExistence(srcRORAPI_DLL);
	bool hasSrcRORAPI_CONF = CheckFileExistence(srcRORAPI_CONF);
	bool hasSrcWNDMODE_DLL = CheckFileExistence(srcWNDMODE_DLL);
	bool hasSrcCustomRor_DRS = CheckFileExistence(srcCustomROR_DRS);
	bool hasDestCustomROR_DLL = CheckFileExistence(destCustomROR_DLL);
	bool hasDestCustomROR_XML = CheckFileExistence(destCustomROR_XML);
	bool hasDestCustomROR_civs_XML = CheckFileExistence(destCustomROR_civs_XML);
	bool hasDestRORAPI_DLL = CheckFileExistence(destRORAPI_DLL);
	bool hasDestRORAPI_CONF = CheckFileExistence(destRORAPI_CONF);
	bool hasDestWNDMODE_DLL = CheckFileExistence(destWNDMODE_DLL);
	bool hasDestCustomRor_DRS = CheckFileExistence(destCustomROR_DRS);
	bool hasMissingFiles = false;
	std::wstring message = _T("Missing files in source directory:");
	if (!hasDestCustomROR_XML && !hasSrcCustomROR_XML) { message += _T(" CustomROR.xml"); hasMissingFiles = true; }
	if (!hasDestCustomROR_civs_XML && !hasSrcCustomROR_civs_XML) { message += _T(" CustomROR_civs.xml"); hasMissingFiles = true; }
	if (!hasDestRORAPI_DLL && !hasSrcRORAPI_DLL) { message += _T(" ROR_API.dll"); hasMissingFiles = true; }
	if (!hasDestRORAPI_CONF && !hasSrcRORAPI_CONF) { message += _T(" ROR_API.conf"); hasMissingFiles = true; }
	if (!hasDestWNDMODE_DLL && !hasSrcWNDMODE_DLL) { message += _T(" wndmode.dll"); hasMissingFiles = true; }
	if (!hasDestCustomRor_DRS && !hasSrcCustomRor_DRS) { message += _T(" CustomROR.drs"); hasMissingFiles = true; }
	if (hasMissingFiles) {
		shortMessage = message;
		return false;
	}

	logs += _T("Starting CustomROR installation...\n");
	logs += _T("Creating CustomROR directory...\n");
	CreateDirectory(destCustomRorDir.c_str(), NULL); // Returns 0 if failed
	logs += _T("Copying files...\n");
	// Warning for CopyFile: last param is failIfExists (opposite of overwrite). Return value = is_success (0=failed)
	bool copyFailed = false;
	BOOL singleCopySuccess;
	std::wstring copyErrors = _T("");
	// Copy customROR.dll
	singleCopySuccess = CopyFile(srcCustomROR_DLL.c_str(), destCustomROR_DLL.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = (overwriteFiles || !hasDestCustomROR_DLL); // Ignore copy errors if target exists AND we chose not to overwrite
	} else {
		message = _T("Copied ") + destCustomROR_DLL + _T("\n");
		logs += message;
	}
	// Copy customROR XML files
	if (hasDestCustomROR_XML && overwriteFiles) {
		// Try to backup old XML file
		std::wstring backupFilename = destCustomROR_XML + _T(".old");
		singleCopySuccess = CopyFile(destCustomROR_XML.c_str(), backupFilename.c_str(), false);
		if (singleCopySuccess) {
			message = _T("Created backup file ") + backupFilename + _T("\n");
			logs += message;
		}
	}
	singleCopySuccess = CopyFile(srcCustomROR_XML.c_str(), destCustomROR_XML.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestCustomROR_DLL);
	} else {
		message = _T("Copied ") + destCustomROR_XML + _T("\n");
		logs += message;
	}

	if (hasDestCustomROR_civs_XML && overwriteFiles) {
		// Try to backup old XML file
		std::wstring backupFilename = destCustomROR_civs_XML + _T(".old");
		singleCopySuccess = CopyFile(destCustomROR_civs_XML.c_str(), backupFilename.c_str(), false);
		if (singleCopySuccess) {
			message = _T("Created backup file ") + backupFilename + _T("\n");
			logs += message;
		}
	}
	singleCopySuccess = CopyFile(srcCustomROR_civs_XML.c_str(), destCustomROR_civs_XML.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestCustomROR_civs_XML);
	} else {
		message = _T("Copied ") + destCustomROR_civs_XML + _T("\n");
		logs += message;
	}

	// Copy ROR_API files
	singleCopySuccess = CopyFile(srcRORAPI_DLL.c_str(), destRORAPI_DLL.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestRORAPI_DLL);
	} else {
		message = _T("Copied ") + destRORAPI_DLL + _T("\n");
		logs += message;
	}
	singleCopySuccess = CopyFile(srcRORAPI_CONF.c_str(), destRORAPI_CONF.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestRORAPI_CONF);
	} else {
		message = _T("Copied ") + destRORAPI_CONF + _T("\n");
		logs += message;
	}
	// Copy wndmode dll file
	singleCopySuccess = CopyFile(srcWNDMODE_DLL.c_str(), destWNDMODE_DLL.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestWNDMODE_DLL);
	} else {
		message = _T("Copied ") + destWNDMODE_DLL + _T("\n");
		logs += message;
	}
	// Copy DRS file
	singleCopySuccess = CopyFile(srcCustomROR_DRS.c_str(), destCustomROR_DRS.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestCustomRor_DRS);
	} else {
		message = _T("Copied ") + destCustomROR_DRS + _T("\n");
		logs += message;
	}
	if (copyFailed) {
		copyErrors += _T("Some files could not be copied to destination directory. Make sure CustomizeAOE has admin rights and that destination files/directory are not locked.\n");
		shortMessage = copyErrors;
		logs += _T("CustomROR installation ended with errors/warnings.\n");
		return false;
	}

	logs += _T("CustomROR files have been successfully copied.\n");
	shortMessage = _T("CustomROR files have been successfully copied.");
	return true;
}


// Manage file operations to install a custom resolution
// logs is an OUT parameter.
// Returns true if successful
bool installResolutionFiles(std::wstring data_DRS_fullPath, std::wstring data2_DRS_fullPath, std::wstring targetExeFullPath,
	std::wstring &logs) {
	logs = _T("");
	std::wstring destDirName = extractDirectory(targetExeFullPath);

	bool overWriteFiles = true; // Overwrite files because they are "custom" files, this can't do any harm to game installation
	bool copyFailed = false;
	BOOL singleCopySuccess;
	std::wstring target = destDirName + _T("\\data\\") + _T(CST_INTERFAC_DRS_CUSTOM_FILE_NAME);
	if (data_DRS_fullPath == target) {
		logs += _T("Warning: source and target files are identical (data).\n");
		singleCopySuccess = true;
	} else {
		// Warning for CopyFile: last param is failIfExists (opposite of overwrite). Return value = is_success (0=failed)
		singleCopySuccess = CopyFile(data_DRS_fullPath.c_str(), target.c_str(), !overWriteFiles);
	}
	if (!singleCopySuccess) {
		copyFailed = overWriteFiles; // Ignore copy errors if we chose not to overwrite
		logs += overWriteFiles ? _T("File could not be copied (data). Check it is not write protected\n") : _T("File was not copied because it already exists (data).\n");
	} else {
		logs += _T("Created ") + target + _T("\n");
	}

	target = destDirName + _T("\\data2\\") + _T(CST_INTERFAC_DRS_CUSTOM_FILE_NAME);
	if (data2_DRS_fullPath == target) {
		logs += _T("Warning: source and target files are identical (data2).\n");
		singleCopySuccess = true;
	} else {
		singleCopySuccess = CopyFile(data2_DRS_fullPath.c_str(), target.c_str(), !overWriteFiles);
	}
	if (!singleCopySuccess) {
		copyFailed = copyFailed || overWriteFiles; // Ignore copy errors if we chose not to overwrite
		logs += overWriteFiles ? _T("File could not be copied (data2). Check it is not write protected.\n") : _T("File was not copied because it already exists (data2).\n");
	} else {
		logs += _T("Created ") + target + _T("\n");
	}

	if (copyFailed) {
		logs += _T("Could not create all necessary files. Abort.\n");
		return false;
	}
	return true;
}

