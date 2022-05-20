#include "pch.h"

#include "user_log_saving.h"
#include <direct.h>

User_Log_Save user_log_save;

User_Log_Save::User_Log_Save()
{
	//user_full_log = NULL;
	iTestItemCount = 0;

	TCHAR pc_name[255];
	unsigned long strlen;
	strlen = 255;
	GetComputerName(pc_name, &strlen);

	csLogTitle = "";


}

/*
User_Log_Save::User_Log_Save(User_Table_Log* user_log, int iTestCount)		//
{
	user_full_log = user_log;
	iTestItemCount = iTestCount;

	TCHAR pc_name[255];
	unsigned long strlen;
	strlen = 255;
	GetComputerName(pc_name, &strlen);
	csFixture = pc_name;

	csLogTitle = "";
	csTestConfig = "TC1";
	csDock_PID_VID = "NA";
	csDock_SN = "SN001";
	csSystem_Config = "NA";
	csTotalResult = "NA";
	csErrorMsg = "";
	//csErrorCode = "";

	csDeviceCheck_Result = "NA";
	csEthernetSpeed_Result = "NA";
	csUSBSpeed_Result = "NA";
	csDisplay_Result = "NA";
	csMonitorLightup_Time = "NA";
	csBlinkScreen_Result = "NA";
	csDisplay_Resolution_Result = "NA";
	csDisplay_Resolution_Content = "NA";
}
*/

User_Log_Save::~User_Log_Save()
{
	//user_full_log = NULL;
}

int User_Log_Save::doSaveCSVLog(CString log_name, CString IOT_iVersion, CString NoIOT_iVersion)
{
	CString csMsg;

	csMsg = "";


	//CTime t;
	//CString csTime, csTime_Log;
	//t = CTime::GetCurrentTime();
	//csTime = t.Format("_%Y%m%d%H%M%S");
	//csTime_Log = t.Format("%Y%m%d"); //YYYMMDD
	
	//CString dir_folder;
	//dir_folder.Format(_T("Alan"));
	int imkdir = _wmkdir(_T("./Log/"));

	csLogTitle.Format(_T("\
iVersion,Merged File Name,DMC&PD,\
TBT4,DP,USB 3,USB 2,Audio,Lan I225, MCU , Security,\
GD MCU, Extra USB3, Extra DP,\
\n"));

	CString csLogFile;
	CString csWriteContent;
	CString csFinalResult;

	//csLogFile.Format(_T("Log/%s_%s_%s_%s.csv"), csDock_SN.GetString(), csFixture.GetString(), csFinalResult.GetString(), csTime_Log.GetString());
	//csLogFile.Format(_T("Log/%s/%s_%s_%s_%s.csv"), log_folder.GetString(), csDock_SN.GetString(), csFixture.GetString(), csFinalResult.GetString(), csTime_Log.GetString());
	csLogFile.Format(_T("Log/%s.csv"), log_name);

	CStdioFile cf1;


	int open_cnt_out = 3;	//設Retry x time
	do
	{
		if (!cf1.Open(csLogFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeUnicode))
		{
			csMsg.Format(_T("Fail to Open Log File: [%s.csv]\n"), log_name);
			AfxMessageBox(csMsg);

			//cf1.Close();		//Open 失敗不用close
		}
		else
		{
			break;	
		}

		open_cnt_out--;
		if (!open_cnt_out)	//一直開失敗，跳出
		{
			return 1;
		}
	} while (open_cnt_out);

	//cf1.WriteString(csLogTitle);

	//Check the file is empty or not
	CString csReadStr;
	cf1.ReadString(csReadStr);

	cf1.SeekToBegin();

	if (csReadStr == "")
	{
		cf1.WriteString(csLogTitle);
		cf1.SeekToEnd();
	}
	else
	{
		cf1.SeekToEnd();
	}

	//Get all Test Result
	//doGetResults();

	
	csWriteContent.Format(_T("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n"), \
		IOT_iVersion, m_FileName_MergedFile_IOT, m_FileName_DMC_IOT, m_FileName_TBT4_IOT, m_FileName_DP, m_FileName_USB3_IOT, m_FileName_USB2_HUB, m_FileName_USB_AUDIO, m_FileName_LAN_I225, \
		m_FileName_MCU, m_FileName_Security, m_FileName_GD_MCU, m_FileName_Extra_USB3_IOT, m_FileName_Extra_DP\
	);
	
	csWriteContent.Format(_T("%s%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n"), \
		csWriteContent, NoIOT_iVersion, m_FileName_MergedFile_NoIOT, m_FileName_DMC_NoIOT, m_FileName_TBT4_NoIOT, m_FileName_DP, m_FileName_USB3_NoIOT, m_FileName_USB2_HUB, m_FileName_USB_AUDIO, m_FileName_LAN_I225, \
		m_FileName_MCU, m_FileName_Security, m_FileName_GD_MCU, m_FileName_Extra_USB3_NoIOT, m_FileName_Extra_DP\
	);

	cf1.WriteString(csWriteContent);

	cf1.Close();

	return 1;
}

int User_Log_Save::doSaveCSVLog_MergedEXE(CString log_name) {
	CString csMsg;

	csMsg = "";

	int imkdir = _wmkdir(_T("./Log/"));

	csLogTitle_MergedEXE.Format(_T("\
Execution Name,Merged File Name IOT,Merged File Name NoIOT, \
\n"));

	CString csLogFile;
	CString csWriteContent;
	CString csFinalResult;

	csLogFile.Format(_T("Log/%s.csv"), log_name);

	CStdioFile cf1;

	int open_cnt_out = 3;	//設Retry x time
	do
	{
		if (!cf1.Open(csLogFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeUnicode))
		{
			csMsg.Format(_T("Fail to Open Log File: [%s.csv]\n"), log_name);
			AfxMessageBox(csMsg);

			//cf1.Close();		//Open 失敗不用close

		}
		else
		{
			break;
		}

		open_cnt_out--;
		if (!open_cnt_out)	//一直開失敗，跳出
		{
			return 1;
		}
	} while (open_cnt_out);



	CString csReadStr;
	cf1.ReadString(csReadStr);

	cf1.SeekToBegin();

	if (csReadStr == "")
	{
		cf1.WriteString(csLogTitle_MergedEXE);
		cf1.SeekToEnd();
	}
	else
	{
		cf1.SeekToEnd();
	}


	csWriteContent.Format(_T("%s,%s,%s\n"), \
		m_FileName_Execution_Name, m_FileName_MergedFile_IOT, m_FileName_MergedFile_NoIOT \
	);

	cf1.WriteString(csWriteContent);

	cf1.Close();

	return 1;


}