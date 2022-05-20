#pragma once


//#include"user_gb_Struct_share.h"

//struct User_Table_Log
//{
//
//	CString Index;
//	CString Name;
//	//CString Path;
//	CString Category;
//	CString Result;
//	CString Argument;
//	//CString ErrorMessage;
//
//};

class User_Log_Save
{
public:
	//struct User_Table_Log
	//{

	//	CString Index;
	//	CString Name;
	//	//CString Path;
	//	CString Category;
	//	CString Result;
	//	CString Argument;
	//	//CString ErrorMessage;

	//};

	User_Log_Save();
//	User_Log_Save(User_Table_Log* user_log, int iTestItemCount);
	~User_Log_Save();

//	User_Table_Log* user_full_log;
	int iTestItemCount;

	int doSaveCSVLog(CString log_name, CString IOT_iVersion, CString NoIOT_iVersion);

	int doSaveCSVLog_MergedEXE(CString log_name);

	//Test Config,Fixture,Dock PID/VID,Dock SN,System Config,Time when Complete,Total Result,Error Message
	CString csLogTitle;
	CString csLogTitle_MergedEXE;
	//CString csTestConfig;
	//CString csFixture;
	//CString csDock_PID_VID;
	//CString csDock_SN;
	//CString csSystem_Config;
	//CString csTotalResult;
	//CString csErrorMsg;
	//CString csErrorCode;

	//CString csDeviceCheck_Result;
	//CString csDeviceCheck_Content;
	//CString csEthernetSpeed_Result;
	//CString csEthernetSpeed_Content;
	//int csUSBSpeed_Ini_Flag;
	//CString csUSBSpeed_Result;
	//CString csUSBSpeed_Content;
	//CString csDisplay_Result;
	//CString csDisplay_Content;
	//CString csDisplay_Time;
	//CString csMonitorLightup_Time;
	//CString csBlinkScreen_Result;
	//CString csPowerCheck_Result;
	//CString csWOL_Result;
	//CString csTestTime_cnt;
	//CString csTestLoop_cnt;
	//CString csDisplay_Resolution_Result;
	//CString csDisplay_Resolution_Content;

	//int system_error_flag = 0;
	//m_FileName_DMC
	CString m_FileName_PD, m_FileName_DP, m_FileName_USB2_HUB, m_FileName_USB_AUDIO, m_FileName_LAN_I225, m_FileName_MCU, m_FileName_Exe, m_FileName_Security;
	CString m_FileName_TBT4_IOT, m_FileName_TBT4_NoIOT;
	CString m_FileName_USB3_IOT, m_FileName_USB3_NoIOT;
	CString m_FileName_DMC_IOT, m_FileName_DMC_NoIOT;
	CString  m_FileName_MergedFile_IOT, m_FileName_MergedFile_NoIOT, m_FileName_Execution_Name;
	CString m_FileName_Extra_USB3_IOT, m_FileName_Extra_USB3_NoIOT;
	CString m_FileName_Extra_DP;
	CString m_FileName_GD_MCU;

	//Victor 20220520
	CString m_FileName_RTK_MST;

private:
//	void doGetResults();

};

extern User_Log_Save user_log_save;