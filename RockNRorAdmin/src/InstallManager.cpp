#include "InstallManager.h"

// Manage file operations to install RockNRor (and ROR_API + wndmode.dll)
// shortMessage and logs are OUT parameters.
// Returns true if successful
bool installRockNRorFiles(std::wstring sourceDirectory, std::wstring targetExeFullPath, bool overwriteFiles,
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

	// Compares src and dest
	if (sourceDirectory.compare(destDirName.c_str()) == 0) {
		shortMessage = _T("Source and target are identical, did not proceed to installation.");
		return false;
	}

	// Build all file names
	//std::wstring destRockNRorDir = destDirName + _T("\\RockNRor");
	std::wstring srcRORAPI_DLL = sourceDirectory + _T("\\ROR_API.dll");
	std::wstring srcRORAPI_CONF = sourceDirectory + _T("\\ROR_API.conf");
	std::wstring srcWNDMODE_DLL = sourceDirectory + _T("\\wndmode.dll");
	std::wstring srcRockNRor_dir = sourceDirectory + _T("\\RockNRor\\");
	std::wstring srcRockNRor_DLL = sourceDirectory + _T("\\RockNRor\\RockNRor.dll");
	std::wstring destRORAPI_DLL = destDirName + _T("\\ROR_API.dll");
	std::wstring destRORAPI_CONF = destDirName + _T("\\ROR_API.conf");
	std::wstring destWNDMODE_DLL = destDirName + _T("\\wndmode.dll");
	std::wstring destRockNRor_dir = destDirName + _T("\\RockNRor\\");
	std::wstring destRockNRor_DLL = destDirName + _T("\\RockNRor\\RockNRor.dll");
	// Get current files status (existing ?)
	bool hasSrcRORAPI_DLL = CheckFileExistence(srcRORAPI_DLL);
	bool hasSrcRORAPI_CONF = CheckFileExistence(srcRORAPI_CONF);
	bool hasSrcWNDMODE_DLL = CheckFileExistence(srcWNDMODE_DLL);
	bool hasSrcRockNRor_dir = DirectoryExists(srcRockNRor_dir);
	bool hasSrcRockNRor_DLL = CheckFileExistence(srcRockNRor_DLL);
	bool hasDestRORAPI_DLL = CheckFileExistence(destRORAPI_DLL);
	bool hasDestRORAPI_CONF = CheckFileExistence(destRORAPI_CONF);
	bool hasDestWNDMODE_DLL = CheckFileExistence(destWNDMODE_DLL);
	bool hasDestRockNRor_dir = DirectoryExists(destRockNRor_dir);
	bool hasDestRockNRor_DLL = CheckFileExistence(destRockNRor_DLL);
	bool hasMissingFiles = false;
	std::wstring message = _T("Missing files in source directory:");
	if (!hasDestRORAPI_DLL && !hasSrcRORAPI_DLL) { message += _T(" ROR_API.dll"); hasMissingFiles = true; }
	if (!hasDestRORAPI_CONF && !hasSrcRORAPI_CONF) { message += _T(" ROR_API.conf"); hasMissingFiles = true; }
	if (!hasDestWNDMODE_DLL && !hasSrcWNDMODE_DLL) { message += _T(" wndmode.dll"); hasMissingFiles = true; }
	if (!hasDestRockNRor_dir && !hasSrcRockNRor_dir) { message += _T(" RockNRor\\"); hasMissingFiles = true; }
	if (!hasSrcRockNRor_DLL && !hasDestRockNRor_DLL) { message += _T(" RockNRor\\RockNRor.dll"); hasMissingFiles = true; }
	if (hasMissingFiles) {
		shortMessage = message;
		return false;
	}

	logs += _T("Starting RockNRor installation...\n");
	logs += _T("Creating RockNRor directory...\n");
	CreateDirectory(destRockNRor_dir.c_str(), NULL); // Returns 0 if failed
	hasDestRockNRor_dir = DirectoryExists(destRockNRor_dir);
	logs += _T("Copying files...\n");
	// Warning for CopyFile: last param is failIfExists (opposite of overwrite). Return value = is_success (0=failed)
	bool copyFailed = false;
	BOOL singleCopySuccess;
	std::wstring copyErrors = _T("");

	// Copy ROR_API files
	if (overwriteFiles) {
		RemoveReadOnlyFlag(destRORAPI_DLL.c_str()); // CopyFile fails if target file exists and is readonly => remove the flag
	}
	singleCopySuccess = CopyFile(srcRORAPI_DLL.c_str(), destRORAPI_DLL.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestRORAPI_DLL);
	} else {
		message = _T("Copied ") + destRORAPI_DLL + _T("\n");
		logs += message;
	}
	if (overwriteFiles) {
		RemoveReadOnlyFlag(destRORAPI_CONF.c_str()); // CopyFile fails if target file exists and is readonly => remove the flag
	}
	singleCopySuccess = CopyFile(srcRORAPI_CONF.c_str(), destRORAPI_CONF.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestRORAPI_CONF);
	} else {
		message = _T("Copied ") + destRORAPI_CONF + _T("\n");
		logs += message;
	}
	// Copy wndmode dll file
	if (overwriteFiles) {
		RemoveReadOnlyFlag(destWNDMODE_DLL.c_str()); // CopyFile fails if target file exists and is readonly => remove the flag
	}
	singleCopySuccess = CopyFile(srcWNDMODE_DLL.c_str(), destWNDMODE_DLL.c_str(), !overwriteFiles);
	if (!singleCopySuccess) {
		copyFailed = copyFailed || (overwriteFiles || !hasDestWNDMODE_DLL);
	} else {
		message = _T("Copied ") + destWNDMODE_DLL + _T("\n");
		logs += message;
	}
	// Copy RockNRor directory and underlying files
	if (!hasDestRockNRor_dir) {
		if (CreateDirectory(destRockNRor_dir.c_str(), NULL) != 0) {
			message += _T("Created directory ") + destRockNRor_dir + _T("\n");
			logs += message;
		} else {
			copyFailed = true;
		}
	}
	std::vector<std::wstring> files = GetFileListFromFolder(srcRockNRor_dir);
	for each (std::wstring filename in files)
	{
		if (overwriteFiles || !CheckFileExistence(filename)) {
			std::wstring sourceFile = srcRockNRor_dir + std::wstring(_T("\\")) + filename;
			std::wstring targetFile = destRockNRor_dir + std::wstring(_T("\\")) + filename;
			if (overwriteFiles) {
				RemoveReadOnlyFlag(targetFile.c_str()); // CopyFile fails if target file exists and is readonly => remove the flag
			}
			singleCopySuccess = CopyFile(sourceFile.c_str(), targetFile.c_str(), !overwriteFiles);
			if (!singleCopySuccess) {
				copyFailed = true;
				message = _T("Failed to write ") + targetFile + _T("\n");
				logs += message;
			} else {
				message = _T("Copied ") + targetFile + _T("\n");
				logs += message;
			}
		}
	}
	if (copyFailed) {
		copyErrors += _T("Some files could not be copied to destination directory. Make sure RockNRorAdmin has admin rights and that destination files/directory are not locked.\n");
		shortMessage = copyErrors;
		logs += _T("RockNRor installation ended with errors/warnings.\n");
		return false;
	}

	logs += _T("RockNRor files have been successfully copied.\n");
	shortMessage = _T("RockNRor files have been successfully copied.");
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
		if (overWriteFiles) {
			RemoveReadOnlyFlag(target.c_str()); // CopyFile fails if target file exists and is readonly => remove the flag
		}
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
		if (overWriteFiles) {
			RemoveReadOnlyFlag(target.c_str()); // CopyFile fails if target file exists and is readonly => remove the flag
		}
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

