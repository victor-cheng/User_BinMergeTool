//#include "stdafx.h"
#include "pch.h"

#include "user_FileManager.h"

#include "tchar.h"
#include  "locale.h"

#include <fstream>
#include <iostream>
using namespace std;


#define MAX_FOLDER_DEPTH 20

int CreateFolder(CString folder_path) {
	CString FolderPath[MAX_FOLDER_DEPTH];
	CString DefaultOutPath;
	int Segment[MAX_FOLDER_DEPTH + 1];
	DefaultOutPath = folder_path;
	int Cheak_result;
	BOOL DiskLableNone = 0;

	// segment folder name
	for (int i = 0; i < MAX_FOLDER_DEPTH; i++) {
		Cheak_result = DefaultOutPath.Find(_T("\\"), 0);
		Segment[i] = Cheak_result;
		if (Cheak_result == 0) {
			DiskLableNone = 1;
		}
		else if (Cheak_result == -1) {
			break;
		}
		FolderPath[i].Format(_T("%s"), DefaultOutPath.Mid(0, Segment[i]));
		DefaultOutPath.Format(_T("%s"), DefaultOutPath.Mid(Cheak_result + 1));
	}

	// reassemble folder path
	int index = 0;
	CString FolderPathMake = _T("");
	if (!DiskLableNone) {		// default path no disk lable
		Cheak_result = FolderPath[0].Find(_T(":"), 0);
		if (Cheak_result != 1) {
			return 1;				// error
		}
		FolderPathMake = FolderPath[index];
	}

	index++;
	while (Segment[index] != -1) {
		FolderPathMake += _T("\\") + FolderPath[index];
		if (!PathIsDirectory(FolderPathMake)) {				// verify folder exist 
			if (!CreateDirectory(FolderPathMake, NULL)) {	// create new folder
				return 1;			//error
			}
		}
		index++;
	}


	return 0;
}

void CreateFileTxt(CString FolderPath, CString FileName, CString* FileContent) {
#if 0
	//CString str = _T("line 1\n");
	//CString str1 = _T("line 2\n");
	//CString str2 = _T("line 3\n");

	//	CopyFile(_T("test.txt"), _T("test.tmp"), 0);

	CStdioFile f(FolderPath + FileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite);
	f.SeekToEnd();
	//f.WriteString(str);
	//f.WriteString(str1);
	//f.WriteString(str2);
	f.WriteString(*FileContent);
	f.Flush();
	f.Close();
#endif 
#if  0

	char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");//設定<ctpye.h>中字元處理方式

	CStdioFile f(FolderPath + FileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::modeReadWrite);
	f.SeekToEnd();
	//f.WriteString(str);
	//f.WriteString(str1);
	//f.WriteString(str2);

	f.WriteString(*FileContent);
	f.Flush();
	f.Close();

	setlocale(LC_CTYPE, old_locale);
	free(old_locale);//還原區域設定
#endif

#if 1			//Alan try Unicode
	FILE* fStream;
	errno_t e = _tfopen_s(&fStream, FolderPath + FileName,
		_T("wt,ccs=UTF-8"));
	if (e != 0) return; // failed..
	CStdioFile f(fStream);  // open the file from this stream


	f.WriteString(*FileContent);
	f.Flush();
	f.Close();
#endif
}


void GetSystemTime(void) {
	CTime t = CTime::GetCurrentTime();
	int d = t.GetDay(); //?得几?
	int y = t.GetYear(); //?取年份
	int m = t.GetMonth(); //?取?前月份
	int h = t.GetHour(); //?取?前?几?
	int mm = t.GetMinute(); //?取分?
	int s = t.GetSecond(); //?取秒
	int w = t.GetDayOfWeek(); //?取星期几，注意1?星期天，7?星期六

	CTime tm = CTime::GetCurrentTime();
	CString str;
	str.Format(_T("%s  %2d:%2d:%2d"), tm.Format("%Y-%m-%d"), h, mm, s);
	AfxMessageBox(str);
}


CString Read_file(CString file_path) {
#if 0
	CString strLine, strTemp;

	//区域设定
	//char* old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	//setlocale(LC_CTYPE, "chs");

	CStdioFile fr(file_path, CFile::modeRead);
	while (fr.ReadString(strTemp))
	{
		strLine += strTemp + _T("\n");
		//strLine += strTemp;
	}
	fr.Close();


	//setlocale(LC_CTYPE, old_locale);
	//free(old_locale);//还原区域设定

	return strLine;
#endif
#if 1
	FILE* fStream = NULL;
	CString strTemp, strLine;

	fStream = NULL;
	errno_t e = _tfopen_s(&fStream,
		file_path, _T("rt,ccs=UTF-8")); // or ccs=UTF-8
	if (e != 0) return 0; // failed..CString sRead;

	CStdioFile mFileRead(fStream);
	//	try
	//	{
	//
	//		mFileRead.ReadString(strTemp);
	////		m_ctlDisplay.SetWindowText(strTemp);
	//	}
	//	catch (CException* e)
	//	{
	//		e->ReportError();
	//	}
	while (mFileRead.ReadString(strTemp))
	{
		strLine += strTemp + _T("\n");
		//strLine += strTemp;
	}
	mFileRead.Close();

	return strLine;
#endif
}
std::string user_CT2A(CString C_str) {

	CT2A temp(C_str);
	const char* pszA = temp;
	std::string strA(pszA);
	return strA;
}


void FileOpen_Read(void) {

	// 1. 開檔
	//ifstream fin("haha.jpg", ifstream::in | ifstream::binary);
	//fstream fin("haha.jpg", ios::binary | ios::in);
	fstream fin("Device_List.txt", ios::binary | ios::in);
	
	// 2. Seek 檔案頭到尾
	fin.seekg(0, ios::end);
	// 3. tell 位置 
	long long length = fin.tellg();
	// 4. 關檔
	fin.close();

	fstream fin_2("Device_List.txt", ios::binary | ios::in);


	char* p_buff = new char[length];

	//for (size_t i = 0; i < length; i++)
	//{
	//	fin_2 >> (p_buff + i);
	//}
	char ch;
	int i = 0;
	while (fin_2)
	{
		fin_2.get(ch);
		p_buff[i] = ch;
		//fin_3.put(p_buff[i]);
		i++;
	}
	fin_2.close();

	//fin >> p_buff;
	//p_buff[0] = '3';
	//p_buff[1] = '2';
	//p_buff[2] = '1';
	//p_buff[3] = '5';
	//fstream fin_2("alantoKon.jpg", ios::binary | ios::out);
	//fstream fin_3("alantoKon.txt", ios::binary | ios::out);
	fstream fin_3("alantoKon.txt", ios::binary | ios::out);
	for (size_t i = 0; i < length; i++)
	{
		//fin_3 << *(p_buff+i);
		fin_3.put(*(p_buff + i));
	
	
	}

	//for (int i = 0; i < 10; i++){
	//	fin_2 << i << " ";
	//}

	//fin_2 << p_buff;

	fin_3.close();
	//fin_2.close();

	//FILE* pFile;
	//char buffer[1024];

	//pFile = fopen(temp_name, "r");
	//if (NULL == pFile) {
	//	printf("Open failure");
	//	return 0;
	//}
	//else {
	//	fread(buffer, 1024, 1, pFile);
	//	printf("%s", buffer);
	//}

	//fclose(pFile);
	//return 0;
}