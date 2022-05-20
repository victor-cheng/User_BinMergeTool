
//#include "stdafx.h"
#include "pch.h"

#include "user_LogFile.h"

User_RESULT_LOG user_log;


void User_RESULT_LOG::CreateFileTxt(CString FolderPath, CString FileName, CString* FileContent) {
	CStdioFile f(FolderPath + _T("\\") + FileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite);
	f.SeekToEnd();
	f.WriteString(*FileContent);
	f.Flush();
	f.Close();
}
