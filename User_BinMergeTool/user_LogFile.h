#pragma once

class User_RESULT_LOG {
public:
	CString LogFileName = _T("");
	CString LogFileNameVersion = _T("");
	CString LogFileNameUpdate = _T("");
	CString LogFileNameReset = _T("");
	CString LogFileNameBB = _T("");
	CString LogFileContent = _T("");
	CString DebugLogFileName = _T("");
	void CreateFileTxt(CString FolderPath, CString FileName, CString* FileContent);

};

extern User_RESULT_LOG user_log;
