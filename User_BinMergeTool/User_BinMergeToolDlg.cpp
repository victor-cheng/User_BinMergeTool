
// User_BinMergeToolDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "User_BinMergeTool.h"
#include "User_BinMergeToolDlg.h"
#include "afxdialogex.h"

#include "user_LogFile.h"

#include "user_log_saving.h"

#include <vector>

using namespace std;



//#define Reduction_Mode




#define HeadFile_Size 512
#define CheckFile_Size 512

#define MCU_Key 0x41697356	//AisV
#define APP_Key 0xC6DA13AA

#define constant_C 0x11707346
#define constant_CC 0x21264672



#define IOT_PAGE
#define NoIOT_PAGE

// CUserBinMergeToolDlg 對話方塊
CString m_IspOriginalPath;
User_DisplayInfo user_DisplayInfo;

//Alan,20220208 , How to use vector
vector<BYTE> intToBytes(int paramInt)
{
	vector<BYTE> arrayOfByte(4);
	for (int i = 0; i < 4; i++)
		arrayOfByte[3 - i] = (paramInt >> (i * 8));
	return arrayOfByte;
}

void Int2BytesArray(BYTE* Array_pnt, int Array_Size,int paramInt) 
{
	for (size_t i = 0; i < Array_Size; i++)
	{
		//*(Array_pnt + (Array_Size - 1) - i) = ((paramInt >> (i * 8)) & 0xff);	//Big Endian
		*(Array_pnt + i) = ((paramInt >> (i * 8)) & 0xff);		//Little Endian
	}

	return;
}

int BytesArray2Int(BYTE* Array_pnt, int Array_Size)
{
	int Result = 0;
	for (size_t i = 0; i < Array_Size; i++)
	{
		Result += *(Array_pnt + i) << (i * 8);
	}

	return Result;
}

CUserBinMergeToolDlg::CUserBinMergeToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USER_BINMERGETOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUserBinMergeToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RICHEDIT21, user_DisplayInfo.m_CtrlLogMsg);
}

BEGIN_MESSAGE_MAP(CUserBinMergeToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PATH_DMC_IOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathDmcIOT)
	ON_BN_CLICKED(IDC_BUTTON_PATH_DMC_NOIOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathDmcNoIOT)
	ON_BN_CLICKED(IDC_BUTTON_PATH_TBT4, &CUserBinMergeToolDlg::OnBnClickedButtonPathTbt4)
	ON_BN_CLICKED(IDC_BUTTON_PATH_DP_IOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathDp)
	ON_BN_CLICKED(IDC_BUTTON_PATH_USB3_IOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathUsbHub1)
	ON_BN_CLICKED(IDC_BUTTON_PATH_USB2_HUB, &CUserBinMergeToolDlg::OnBnClickedButtonPathUsb2Hub)
	ON_BN_CLICKED(IDC_BUTTON_PATH_USB_AUDIO, &CUserBinMergeToolDlg::OnBnClickedButtonPathUsbAudio)
	ON_BN_CLICKED(IDC_BUTTON_PATH_LAN_I225, &CUserBinMergeToolDlg::OnBnClickedButtonPathLanI225)
	ON_BN_CLICKED(IDC_BUTTON_PATH_MCU, &CUserBinMergeToolDlg::OnBnClickedButtonPathMcu)
	ON_BN_CLICKED(IDC_BUTTON_PATH_EXE, &CUserBinMergeToolDlg::OnBnClickedButtonPathExe)
	ON_BN_CLICKED(IDC_BUTTON_COMBINE, &CUserBinMergeToolDlg::OnBnClickedButtonCombine)
	ON_BN_CLICKED(IDOK, &CUserBinMergeToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUserBinMergeToolDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_BIN_TO_EXE, &CUserBinMergeToolDlg::OnBnClickedButtonBinToExe)
	ON_BN_CLICKED(IDC_BUTTON_PATH_COMBINED_BIN, &CUserBinMergeToolDlg::OnBnClickedButtonPathCombinedBin)
	ON_BN_CLICKED(IDC_BUTTON_PATH_CODEVER_TAG, &CUserBinMergeToolDlg::OnBnClickedButtonPathCodeverTag)
	ON_BN_CLICKED(IDC_BUTTON_PATH_SECURITY, &CUserBinMergeToolDlg::OnBnClickedButtonPathSecurity)
	ON_EN_CHANGE(IDC_EDIT_HEAD_VERSION_IOT, &CUserBinMergeToolDlg::OnEnChangeEditHeadVersion)
	ON_BN_CLICKED(IDC_BUTTON_PATH_COMBINED_BIN_NOIOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathCombinedBinNoiot)
	ON_BN_CLICKED(IDC_BUTTON_PATH_USB3_NoIOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathUsb3HubNoiot)
	ON_BN_CLICKED(IDC_BUTTON_PATH_TBT4_NoIOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathTbt4Noiot)
	ON_BN_CLICKED(IDC_BUTTON_PATH_GD_MCU, &CUserBinMergeToolDlg::OnBnClickedButtonPathGdMcu)
	ON_BN_CLICKED(IDC_BUTTON_PATH_EXTRA_USB3_IOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathExtraUsb3Iot)
	ON_BN_CLICKED(IDC_BUTTON_PATH_EXTRA_USB3_NoIOT, &CUserBinMergeToolDlg::OnBnClickedButtonPathExtraUsb3Noiot)
	ON_BN_CLICKED(IDC_BUTTON_PATH_EXTRA_DP, &CUserBinMergeToolDlg::OnBnClickedButtonPathExtraDp)

	//Victor 20220520
	ON_BN_CLICKED(IDC_BUTTON_PATH_RTK_MST, &CUserBinMergeToolDlg::OnBnClickedButtonPathRTKMST)
END_MESSAGE_MAP()


// CUserBinMergeToolDlg 訊息處理常式

BOOL CUserBinMergeToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	m_IspOriginalPath.Format(_T("%s"), szFilePath);
	WORD linesfind = m_IspOriginalPath.ReverseFind('\\');
	m_IspOriginalPath = m_IspOriginalPath.Left(linesfind);

	SetCurrentDirectory(m_IspOriginalPath);

	//Set log file name
	CTime m_CurrentTime = CTime::GetCurrentTime();
	CString system_time;
	system_time.Format(_T("%s%02d%02d%02d"), m_CurrentTime.Format("%Y%m%d"), m_CurrentTime.GetHour(), m_CurrentTime.GetMinute(), m_CurrentTime.GetSecond());
	user_log.LogFileName.Format(_T("BinMerge_Log_%s.txt"), system_time);


	memset(&isp_headpage.Reserve_0, 0xff, sizeof(isp_headpage.Reserve_0));
	memset(&isp_headpage.Reserve_1, 0xff, sizeof(isp_headpage.Reserve_1));
	memset(&isp_headpage.Reserve_2, 0xff, sizeof(isp_headpage.Reserve_2));
	memset(&isp_headpage.Reserve_3, 0xff, sizeof(isp_headpage.Reserve_3));
	memset(&isp_headpage.Reserve_4, 0xff, sizeof(isp_headpage.Reserve_4));
	memset(&isp_headpage.Reserve_5, 0xff, sizeof(isp_headpage.Reserve_5));
	memset(&isp_headpage.SetCheckReady, 0xff, sizeof(isp_headpage.SetCheckReady));	//Alan, 20220314
	


	memset(&isp_headpage_NoIOT.Reserve_0, 0xff, sizeof(isp_headpage_NoIOT.Reserve_0));
	memset(&isp_headpage_NoIOT.Reserve_1, 0xff, sizeof(isp_headpage_NoIOT.Reserve_1));
	memset(&isp_headpage_NoIOT.Reserve_2, 0xff, sizeof(isp_headpage_NoIOT.Reserve_2));
	memset(&isp_headpage_NoIOT.Reserve_3, 0xff, sizeof(isp_headpage_NoIOT.Reserve_3));
	memset(&isp_headpage_NoIOT.Reserve_4, 0xff, sizeof(isp_headpage_NoIOT.Reserve_4));
	memset(&isp_headpage_NoIOT.Reserve_5, 0xff, sizeof(isp_headpage_NoIOT.Reserve_5));
	memset(&isp_headpage_NoIOT.SetCheckReady, 0xff, sizeof(isp_headpage_NoIOT.SetCheckReady));	//Alan, 20220314

	Initial_Read();
	Initial_Value_Set();

	Initial_DMC_Value(Initial_Selection);
	Initial_DMC_NoIOT_Value(Initial_Selection);
	Initial_TBT4_Value(Initial_Selection);
	Initial_TBT4_Value_NoIOT(Initial_Selection);
	Initial_DP_Value(Initial_Selection);
	Initial_USB3_IOT_Value(Initial_Selection);
	Initial_USB3_NoIOT_Value(Initial_Selection);
	Initial_USB2_HUB_Value(Initial_Selection);
	Initial_Audio_Value(Initial_Selection);
	Initial_LanI225_Value(Initial_Selection);
	Initial_MCU_Value(Initial_Selection);
	Initial_PathSecurity(Initial_Selection);

	//Alan, 20220207
	Initial_Extra_DP_Value(Initial_Selection);
	Initial_Extra_USB3_IOT_Value(Initial_Selection);
	Initial_Extra_USB3_NoIOT_Value(Initial_Selection);
	Initial_GD_MCU_Value(Initial_Selection);
	
	//Victor 20220520
	Initial_RTK_MST_Value(Initial_Selection);

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CUserBinMergeToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CUserBinMergeToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUserBinMergeToolDlg::OnBnClickedButtonPathDmcIOT()
{
	Initial_DMC_Value(Buttom_Selection);

}
void CUserBinMergeToolDlg::Initial_DMC_Value(BYTE action)
{

	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_DMC_IOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_DMC_IOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_DMC_IOT))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_DMC_IOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_DMC_IOT)->SetWindowText(file_name);

	user_log_save.m_FileName_DMC_IOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_DMC_IOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//m_Head_DMC.lenth.integer = file_size;
	isp_headpage.isp_length.PD._dword = file_size;	//兩顆chip，放一個就好
	//isp_headpage_NoIOT.isp_length.PD._dword = file_size;	//兩顆chip，放一個就好

	isp_headpage.isp_length.DMC._dword = 0;	
	//isp_headpage_NoIOT.isp_length.DMC._dword = 0;

	message.Format(_T("DMC lenth:0x%x\r\n"), isp_headpage.isp_length.PD._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
	//m_Head_PD.lenth.integer = 0;

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_DMC_IOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	BYTE shift_buff[2];

	shift_buff[0] = temp_buff[0xEB] & 0x0f;
	shift_buff[1] = (temp_buff[0xEB] & 0xf0) >> 4;

	isp_headpage.isp_version_in_mcu.PD[0] = temp_buff[0xEA];	//Alan,20201228, restruct version format
	isp_headpage.isp_version_in_mcu.PD[1] = shift_buff[0];
	isp_headpage.isp_version_in_mcu.PD[2] = shift_buff[1];

	//isp_headpage_NoIOT.isp_version_in_mcu.PD[0] = temp_buff[0xEA];	//Alan,20201228, restruct version format
	//isp_headpage_NoIOT.isp_version_in_mcu.PD[1] = shift_buff[0];
	//isp_headpage_NoIOT.isp_version_in_mcu.PD[2] = shift_buff[1];
	
	shift_buff[0] = temp_buff[0x37] & 0x0f;
	shift_buff[1] = (temp_buff[0x37] & 0xf0) >> 4;
	
	isp_headpage.isp_version_in_mcu.DMC[0] = temp_buff[0x36];	//Alan,20201228, restruct version format
	isp_headpage.isp_version_in_mcu.DMC[1] = shift_buff[0];
	isp_headpage.isp_version_in_mcu.DMC[2] = shift_buff[1];
	//isp_headpage_NoIOT.isp_version_in_mcu.DMC[0] = temp_buff[0x36];	//Alan,20201228, restruct version format
	//isp_headpage_NoIOT.isp_version_in_mcu.DMC[1] = shift_buff[0];
	//isp_headpage_NoIOT.isp_version_in_mcu.DMC[2] = shift_buff[1];

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%x%x%x"), isp_headpage.isp_version_in_mcu.DMC[2], isp_headpage.isp_version_in_mcu.DMC[1], isp_headpage.isp_version_in_mcu.DMC[0]);
	GetDlgItem(IDC_EDIT_VSERION_DMC)->SetWindowText(message);

	message.Format(_T("%x%x%x"), isp_headpage.isp_version_in_mcu.PD[2], isp_headpage.isp_version_in_mcu.PD[1], isp_headpage.isp_version_in_mcu.PD[0]);
	GetDlgItem(IDC_EDIT_VSERION_PD)->SetWindowText(message);


	//m_Head_DMC.checksum.integer = Make_CheckSum(m_PathFile_DMC);
	//message.Format(_T("DMC checksum:0x%x\r\n"), m_Head_DMC.checksum.integer);
	isp_headpage.isp_checksum.PD._word = Make_CheckSum(m_PathFile_DMC_IOT);
	//isp_headpage_NoIOT.isp_checksum.PD._word = isp_headpage.isp_checksum.PD._word;
	message.Format(_T("DMC checksum:0x%x\r\n"), isp_headpage.isp_checksum.PD._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}



void CUserBinMergeToolDlg::OnBnClickedButtonPathDmcNoIOT()
{
	Initial_DMC_NoIOT_Value(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_DMC_NoIOT_Value(BYTE action) {
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_DMC_NoIOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_DMC_NOIOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_DMC_NoIOT))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_DMC_NoIOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_DMC_NOIOT)->SetWindowText(file_name);

	user_log_save.m_FileName_DMC_NoIOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_DMC_NoIOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//m_Head_DMC.lenth.integer = file_size;
	//isp_headpage.isp_length.PD._dword = file_size;	//兩顆chip，放一個就好
	isp_headpage_NoIOT.isp_length.PD._dword = file_size;	//兩顆chip，放一個就好

	//isp_headpage.isp_length.DMC._dword = 0;
	isp_headpage_NoIOT.isp_length.DMC._dword = 0;

	message.Format(_T("DMC lenth:0x%x\r\n"), isp_headpage_NoIOT.isp_length.PD._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
	//m_Head_PD.lenth.integer = 0;

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_DMC_NoIOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	BYTE shift_buff[2];

	shift_buff[0] = temp_buff[0xEB] & 0x0f;
	shift_buff[1] = (temp_buff[0xEB] & 0xf0) >> 4;

	//isp_headpage.isp_version_in_mcu.PD[0] = temp_buff[0xEA];	//Alan,20201228, restruct version format
	//isp_headpage.isp_version_in_mcu.PD[1] = shift_buff[0];
	//isp_headpage.isp_version_in_mcu.PD[2] = shift_buff[1];

	isp_headpage_NoIOT.isp_version_in_mcu.PD[0] = temp_buff[0xEA];	//Alan,20201228, restruct version format
	isp_headpage_NoIOT.isp_version_in_mcu.PD[1] = shift_buff[0];
	isp_headpage_NoIOT.isp_version_in_mcu.PD[2] = shift_buff[1];

	shift_buff[0] = temp_buff[0x37] & 0x0f;
	shift_buff[1] = (temp_buff[0x37] & 0xf0) >> 4;

	//isp_headpage.isp_version_in_mcu.DMC[0] = temp_buff[0x36];	//Alan,20201228, restruct version format
	//isp_headpage.isp_version_in_mcu.DMC[1] = shift_buff[0];
	//isp_headpage.isp_version_in_mcu.DMC[2] = shift_buff[1];
	isp_headpage_NoIOT.isp_version_in_mcu.DMC[0] = temp_buff[0x36];	//Alan,20201228, restruct version format
	isp_headpage_NoIOT.isp_version_in_mcu.DMC[1] = shift_buff[0];
	isp_headpage_NoIOT.isp_version_in_mcu.DMC[2] = shift_buff[1];

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%x%x%x"), isp_headpage_NoIOT.isp_version_in_mcu.DMC[2], isp_headpage_NoIOT.isp_version_in_mcu.DMC[1], isp_headpage_NoIOT.isp_version_in_mcu.DMC[0]);
	GetDlgItem(IDC_EDIT_VSERION_DMC_NoIOT)->SetWindowText(message);

	message.Format(_T("%x%x%x"), isp_headpage_NoIOT.isp_version_in_mcu.PD[2], isp_headpage_NoIOT.isp_version_in_mcu.PD[1], isp_headpage_NoIOT.isp_version_in_mcu.PD[0]);
	GetDlgItem(IDC_EDIT_VSERION_PD_NoIOT)->SetWindowText(message);

	//m_Head_DMC.checksum.integer = Make_CheckSum(m_PathFile_DMC);
	//message.Format(_T("DMC checksum:0x%x\r\n"), m_Head_DMC.checksum.integer);
	isp_headpage_NoIOT.isp_checksum.PD._word = Make_CheckSum(m_PathFile_DMC_NoIOT);
	message.Format(_T("DMC checksum:0x%x\r\n"), isp_headpage_NoIOT.isp_checksum.PD._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathTbt4()
{
	Initial_TBT4_Value(Buttom_Selection);

}
void CUserBinMergeToolDlg::Initial_TBT4_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_TBT4_IOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_TBT4)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_TBT4_IOT))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_TBT4_IOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_TBT4)->SetWindowText(file_name);

	user_log_save.m_FileName_TBT4_IOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_TBT4_IOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.TBT4._dword = file_size;

	message.Format(_T("TBT4 length:0x%x\r\n"), isp_headpage.isp_length.TBT4._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1


	fstream m_temp;
	m_temp.open(m_PathFile_TBT4_IOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	isp_headpage.isp_version_in_mcu.TBT4[0] = temp_buff[0x4224];	//Model Reversion	//Alan,20201228, restruct version format
	isp_headpage.isp_version_in_mcu.TBT4[1] = temp_buff[0x4225];	//NVM Reversion
	isp_headpage.isp_version_in_mcu.TBT4[2] = temp_buff[0x4009];	//pre version
	isp_headpage.isp_version_in_mcu.TBT4[3] = temp_buff[0x400A];	//primary version

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%x%x%x%x"), isp_headpage.isp_version_in_mcu.TBT4[3], isp_headpage.isp_version_in_mcu.TBT4[2], isp_headpage.isp_version_in_mcu.TBT4[1], isp_headpage.isp_version_in_mcu.TBT4[0]);
	GetDlgItem(IDC_EDIT_VSERION_TBT4)->SetWindowText(message);

	isp_headpage.isp_checksum.TBT4._word = Make_CheckSum(m_PathFile_TBT4_IOT);
	message.Format(_T("TBT4 checksum:0x%x\r\n"), isp_headpage.isp_checksum.TBT4._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathTbt4Noiot()
{
	Initial_TBT4_Value_NoIOT(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_TBT4_Value_NoIOT(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_TBT4_NoIOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_TBT4_NoIOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_TBT4_NoIOT))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_TBT4_NoIOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_TBT4_NoIOT)->SetWindowText(file_name);

	user_log_save.m_FileName_TBT4_NoIOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_TBT4_NoIOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage_NoIOT.isp_length.TBT4._dword = file_size;

	message.Format(_T("TBT4 length:0x%x\r\n"), isp_headpage_NoIOT.isp_length.TBT4._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1


	fstream m_temp;
	m_temp.open(m_PathFile_TBT4_NoIOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	isp_headpage_NoIOT.isp_version_in_mcu.TBT4[0] = temp_buff[0x4224];	//Model Reversion	//Alan,2021037
	isp_headpage_NoIOT.isp_version_in_mcu.TBT4[1] = temp_buff[0x4225];	//NVM Reversion
	isp_headpage_NoIOT.isp_version_in_mcu.TBT4[2] = temp_buff[0x4009];	//pre version
	isp_headpage_NoIOT.isp_version_in_mcu.TBT4[3] = temp_buff[0x400A];	//primary version

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%x%x%x%x"), isp_headpage_NoIOT.isp_version_in_mcu.TBT4[3], isp_headpage_NoIOT.isp_version_in_mcu.TBT4[2], isp_headpage_NoIOT.isp_version_in_mcu.TBT4[1], isp_headpage_NoIOT.isp_version_in_mcu.TBT4[0]);
	GetDlgItem(IDC_EDIT_VSERION_TBT4_NoIOT)->SetWindowText(message);

	isp_headpage_NoIOT.isp_checksum.TBT4._word = Make_CheckSum(m_PathFile_TBT4_NoIOT);
	message.Format(_T("No IOT TBT4 checksum:0x%x\r\n"), isp_headpage_NoIOT.isp_checksum.TBT4._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathDp()
{
	Initial_DP_Value(Buttom_Selection);

}
void CUserBinMergeToolDlg::Initial_DP_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_DP = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_DP_IOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_DP))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_DP.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_DP_IOT)->SetWindowText(file_name);

	user_log_save.m_FileName_DP.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_DP);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.DP6x._dword = file_size;	//兩顆chip，放一個就好
	isp_headpage_NoIOT.isp_length.DP6x._dword = file_size;	//兩顆chip，放一個就好


	message.Format(_T("DP length:0x%x\r\n"), isp_headpage.isp_length.DP6x._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_DP, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	if (temp_buff[0x10D] != 0)		//Check 5xxx test version or not
	{

		isp_headpage.isp_version_in_mcu.DP5x[0] = temp_buff[0x10D];	//Alan,20201228, restruct version format
		isp_headpage.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		isp_headpage.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0] = temp_buff[0x10D];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		isp_headpage_NoIOT.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];
		
	}
	else
	{

		isp_headpage.isp_version_in_mcu.DP5x[0] = temp_buff[0x18402];	//Alan,20201228, restruct version format
		isp_headpage.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		isp_headpage.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0] = temp_buff[0x18402];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		isp_headpage_NoIOT.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];
	}

	if (temp_buff[0x10020D] != 0)		//Check 6xxx test version or not
	{
		isp_headpage.isp_version_in_mcu.DP6x[0] = temp_buff[0x10020D];	//Alan,20201228, restruct version format
		isp_headpage.isp_version_in_mcu.DP6x[1] = temp_buff[0x154001];
		isp_headpage.isp_version_in_mcu.DP6x[2] = temp_buff[0x154000];

		isp_headpage_NoIOT.isp_version_in_mcu.DP6x[0] = temp_buff[0x10020D];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.isp_version_in_mcu.DP6x[1] = temp_buff[0x154001];
		isp_headpage_NoIOT.isp_version_in_mcu.DP6x[2] = temp_buff[0x154000];
	}
	else
	{

		isp_headpage.isp_version_in_mcu.DP6x[0] = temp_buff[0x154002];	//Alan,20201228, restruct version format
		isp_headpage.isp_version_in_mcu.DP6x[1] = temp_buff[0x154001];
		isp_headpage.isp_version_in_mcu.DP6x[2] = temp_buff[0x154000];

		isp_headpage_NoIOT.isp_version_in_mcu.DP6x[0] = temp_buff[0x154002];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.isp_version_in_mcu.DP6x[1] = temp_buff[0x154001];
		isp_headpage_NoIOT.isp_version_in_mcu.DP6x[2] = temp_buff[0x154000];
	}

	m_temp.close();
	delete[] temp_buff;
#endif
	message.Format(_T("%02d%02d%03d"), isp_headpage.isp_version_in_mcu.DP5x[2], isp_headpage.isp_version_in_mcu.DP5x[1], isp_headpage.isp_version_in_mcu.DP5x[0]);

	GetDlgItem(IDC_EDIT_VSERION_DP_5XXX_IOT)->SetWindowText(message);


	message.Format(_T("%02d%02d%03d"), isp_headpage.isp_version_in_mcu.DP6x[2], isp_headpage.isp_version_in_mcu.DP6x[1], isp_headpage.isp_version_in_mcu.DP6x[0]);
	GetDlgItem(IDC_EDIT_VSERION_DP_6XXX_IOT)->SetWindowText(message);


	isp_headpage.isp_checksum.DP6x._word = Make_CheckSum(m_PathFile_DP);
	isp_headpage_NoIOT.isp_checksum.DP6x._word = isp_headpage.isp_checksum.DP6x._word;
	message.Format(_T("DP checksum:0x%x\r\n"), isp_headpage.isp_checksum.DP6x._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}



void CUserBinMergeToolDlg::OnBnClickedButtonPathUsbHub1()
{
	Initial_USB3_IOT_Value(Buttom_Selection);
}
void CUserBinMergeToolDlg::Initial_USB3_IOT_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_USB_IOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_USB3_IOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_USB_IOT))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_USB_IOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_USB3_IOT)->SetWindowText(file_name);

	user_log_save.m_FileName_USB3_IOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_USB_IOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.USB3._dword = file_size;

	message.Format(_T("USB_Hub1 length:0x%x\r\n"), isp_headpage.isp_length.USB3._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_USB_IOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	for (int i = 0; i < file_size; i++)
	{
		int j = 0;
		if ((BYTE)temp_buff[i] == 0xEF) {
			j = i + 1;
			if ((BYTE)temp_buff[j] == 0x17) {
				j++;
				if ((BYTE)temp_buff[j] == 0xB6) {
					j++;
					if ((BYTE)temp_buff[j] == 0x30) {
						j++;

						isp_headpage.isp_version_in_mcu.USB3[0] = temp_buff[j] + 3;	//Alan,20201228, restruct version format
						j++;

						isp_headpage.isp_version_in_mcu.USB3[1] = temp_buff[j];	//Alan,20201228, restruct version format
						break;
					}
				}


			}
		}

	}

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%02x%02x"), isp_headpage.isp_version_in_mcu.USB3[1], isp_headpage.isp_version_in_mcu.USB3[0]);
	GetDlgItem(IDC_EDIT_VSERION_USB3_IOT)->SetWindowText(message);

	isp_headpage.isp_checksum.USB3._word = Make_CheckSum(m_PathFile_USB_IOT);
	message.Format(_T("USB3.0 checksum:0x%x\r\n"), isp_headpage.isp_checksum.USB3._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}



void CUserBinMergeToolDlg::OnBnClickedButtonPathUsb2Hub()
{
	Initial_USB2_HUB_Value(Buttom_Selection);
}
void CUserBinMergeToolDlg::Initial_USB2_HUB_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_USB2_HUB = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_USB2_HUB)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_USB2_HUB))
		{
			return;
		}
	}



	if (!PathFileExists(m_PathFile_USB2_HUB))
	{
		return;
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_USB2_HUB.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_USB2_HUB)->SetWindowText(file_name);

	user_log_save.m_FileName_USB2_HUB.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_USB2_HUB);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.USB2._dword = file_size;
	isp_headpage_NoIOT.isp_length.USB2._dword = file_size;

	message.Format(_T("USB2_Hub length:0x%x\r\n"), isp_headpage.isp_length.USB2._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_USB2_HUB, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}


	for (int i = 0; i < file_size; i++)
	{
		int j = 0;
		if ((BYTE)temp_buff[i] == 0x47) {
			j = i + 1;
			//BYTE teno = temp_buff[j];
			if ((BYTE)temp_buff[j] == 0x80) {


				j++;
				isp_headpage.isp_version_in_mcu.USB2[4] = temp_buff[j];	//Alan,20201228, restruct version format
				isp_headpage_NoIOT.isp_version_in_mcu.USB2[4] = temp_buff[j];	//Alan,20210317, NoIOT format
				j += 8;
				isp_headpage.isp_version_in_mcu.USB2[3] = temp_buff[j];	//Alan,20201228, restruct version format
				isp_headpage_NoIOT.isp_version_in_mcu.USB2[3] = temp_buff[j];	//Alan,20210317, NoIOT format
				j += 8;
				isp_headpage.isp_version_in_mcu.USB2[2] = temp_buff[j];	//Alan,20201228, restruct version format
				isp_headpage_NoIOT.isp_version_in_mcu.USB2[2] = temp_buff[j];	//Alan,20210317, NoIOT format
				j += 8;
				isp_headpage.isp_version_in_mcu.USB2[1] = temp_buff[j];	//Alan,20201228, restruct version format
				isp_headpage_NoIOT.isp_version_in_mcu.USB2[1] = temp_buff[j];	//Alan,20210317, NoIOT format
				j += 8;
				isp_headpage.isp_version_in_mcu.USB2[0] = temp_buff[j];	//Alan,20201228, restruct version format
				isp_headpage_NoIOT.isp_version_in_mcu.USB2[0] = temp_buff[j];	//Alan,20210317, NoIOT format
				break;
			}
		}

	}


	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%c%c%c%c%c"), isp_headpage.isp_version_in_mcu.USB2[4], isp_headpage.isp_version_in_mcu.USB2[3], isp_headpage.isp_version_in_mcu.USB2[2], isp_headpage.isp_version_in_mcu.USB2[1], isp_headpage.isp_version_in_mcu.USB2[0]);

	GetDlgItem(IDC_EDIT_VSERION_USB2_HUB)->SetWindowText(message);

	message.Format(_T("%dkB"), (file_size / 1024));
	GetDlgItem(IDC_STATIC_U2_LENGTH_ORIGINAL)->SetWindowText(message);
	//message.Format(_T("%dkB"), (Reduction_File(Chip_U2_Hub, m_PathFile_USB2_HUB) / 1024));
	//GetDlgItem(IDC_STATIC_U2_LENGTH_REDUCTION)->SetWindowText(message);

	isp_headpage.isp_checksum.USB2._word = Make_CheckSum(m_PathFile_USB2_HUB);
	isp_headpage_NoIOT.isp_checksum.USB2._word = isp_headpage.isp_checksum.USB2._word;
	message.Format(_T("USB2 checksum:0x%x\r\n"), isp_headpage.isp_checksum.USB2._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}



void CUserBinMergeToolDlg::OnBnClickedButtonPathUsbAudio()
{
	Initial_Audio_Value(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_Audio_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_USB_AUDIO = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_USB_AUDIO)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_USB_AUDIO))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_USB_AUDIO.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_USB_AUDIO)->SetWindowText(file_name);

	user_log_save.m_FileName_USB_AUDIO.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_USB_AUDIO);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.AUDIO._dword = file_size;
	isp_headpage_NoIOT.isp_length.AUDIO._dword = file_size;


	message.Format(_T("Audio length:0x%x\r\n"), isp_headpage.isp_length.AUDIO._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_USB_AUDIO, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	isp_headpage.isp_version_in_mcu.AUDIO[0] = temp_buff[0x24];	//Alan,20201228, restruct version format
	isp_headpage.isp_version_in_mcu.AUDIO[1] = temp_buff[0x23];
	isp_headpage.isp_version_in_mcu.AUDIO[2] = temp_buff[0x22];

	isp_headpage_NoIOT.isp_version_in_mcu.AUDIO[0] = temp_buff[0x24];	//Alan,20210317, NoIOT
	isp_headpage_NoIOT.isp_version_in_mcu.AUDIO[1] = temp_buff[0x23];
	isp_headpage_NoIOT.isp_version_in_mcu.AUDIO[2] = temp_buff[0x22];
	
	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%x%x%x"), isp_headpage.isp_version_in_mcu.AUDIO[2], isp_headpage.isp_version_in_mcu.AUDIO[1], isp_headpage.isp_version_in_mcu.AUDIO[0]);
	GetDlgItem(IDC_EDIT_VSERION_USB_AUDIO)->SetWindowText(message);

	message.Format(_T("%dkB"), (file_size / 1024));
	GetDlgItem(IDC_STATIC_AUDIO_LENGTH_ORIGINAL)->SetWindowText(message);
	//message.Format(_T("%dkB"), (Reduction_File(Chip_Audio, m_PathFile_USB_AUDIO) / 1024));
	//GetDlgItem(IDC_STATIC_AUDIO_LENGTH_REDUCTION5)->SetWindowText(message);

	isp_headpage.isp_checksum.AUDIO._word = Make_CheckSum(m_PathFile_USB_AUDIO);
	isp_headpage_NoIOT.isp_checksum.AUDIO._word = isp_headpage.isp_checksum.AUDIO._word;
	message.Format(_T("Audio checksum:0x%x\r\n"), isp_headpage.isp_checksum.AUDIO._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}


void CUserBinMergeToolDlg::OnBnClickedButtonPathLanI225()
{
	Initial_LanI225_Value(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_LanI225_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_LAN_I225 = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_LAN_I225)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_LAN_I225))
		{
			return;
		}
	}


	CString message;
	message.Format(_T("%s"), m_PathFile_LAN_I225.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_LAN_I225)->SetWindowText(file_name);

	user_log_save.m_FileName_LAN_I225.Format(_T("%s"), file_name);


	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_LAN_I225);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.I255._dword = file_size;
	isp_headpage_NoIOT.isp_length.I255._dword = file_size;


	message.Format(_T("LanI225 length:0x%x\r\n"), isp_headpage.isp_length.I255._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1


	fstream m_temp;
	m_temp.open(m_PathFile_LAN_I225, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	isp_headpage.isp_version_in_mcu.I255[1] = temp_buff[0xA];	//Alan,20201228, restruct version format
	isp_headpage.isp_version_in_mcu.I255[2] = temp_buff[0xB];
	isp_headpage.isp_version_in_mcu.I255[0] = temp_buff[0xC];

	isp_headpage_NoIOT.isp_version_in_mcu.I255[1] = temp_buff[0xA];	//Alan,20210317, No IOT
	isp_headpage_NoIOT.isp_version_in_mcu.I255[2] = temp_buff[0xB];
	isp_headpage_NoIOT.isp_version_in_mcu.I255[0] = temp_buff[0xC];
	
	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%x%x%x"), isp_headpage.isp_version_in_mcu.I255[2], isp_headpage.isp_version_in_mcu.I255[1], isp_headpage.isp_version_in_mcu.I255[0]);
	GetDlgItem(IDC_EDIT_VSERION_LAN_I225)->SetWindowText(message);

	message.Format(_T("%dkB"), (file_size / 1024));
	GetDlgItem(IDC_STATIC_LAN_LENGTH_ORIGINAL)->SetWindowText(message);
	//message.Format(_T("%dkB"), (Reduction_File(Chip_LAN_I225, m_PathFile_LAN_I225) / 1024));
	//GetDlgItem(IDC_STATIC_LAN_LENGTH_REDUCTION)->SetWindowText(message);

	isp_headpage.isp_checksum.I255._word = Make_CheckSum(m_PathFile_LAN_I225);
	isp_headpage_NoIOT.isp_checksum.I255._word = isp_headpage.isp_checksum.I255._word;
	message.Format(_T("I225 checksum:0x%x\r\n"), isp_headpage.isp_checksum.I255._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}


void CUserBinMergeToolDlg::OnBnClickedButtonPathMcu()
{
	Initial_MCU_Value(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_MCU_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_MCU = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_MCU)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_MCU))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_MCU);
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_MCU)->SetWindowText(file_name);

	user_log_save.m_FileName_MCU.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_MCU);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage.isp_length.MCU._dword = file_size;
	isp_headpage_NoIOT.isp_length.MCU._dword = file_size;


	message.Format(_T("MCU length:0x%x\r\n"), isp_headpage.isp_length.MCU._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_MCU, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	isp_headpage.isp_version_in_mcu.MCU[1] = temp_buff[0x1000];	//Alan,20201228, restruct version format
	isp_headpage.isp_version_in_mcu.MCU[0] = temp_buff[0x1001];

	isp_headpage_NoIOT.isp_version_in_mcu.MCU[1] = temp_buff[0x1000];	//Alan,20210317
	isp_headpage_NoIOT.isp_version_in_mcu.MCU[0] = temp_buff[0x1001];
	
	m_temp.close();
	delete[] temp_buff;
#endif


	message.Format(_T("%02x%02x"), isp_headpage.isp_version_in_mcu.MCU[1], isp_headpage.isp_version_in_mcu.MCU[0]);
	GetDlgItem(IDC_EDIT_VSERION_MCU)->SetWindowText(message);

	message.Format(_T("%dkB"), (file_size / 1024));
	GetDlgItem(IDC_STATIC_MCU_LENGTH_ORIGINAL)->SetWindowText(message);
	//message.Format(_T("%dkB"), (Reduction_File(Chip_MCU, m_PathFile_MCU) / 1024));
	//GetDlgItem(IDC_STATIC_MCU_LENGTH_REDUCTION)->SetWindowText(message);

	isp_headpage.isp_checksum.MCU._word = Make_CheckSum(m_PathFile_MCU);
	isp_headpage_NoIOT.isp_checksum.MCU._word = isp_headpage.isp_checksum.MCU._word;
	message.Format(_T("MCU checksum:0x%x\r\n"), isp_headpage.isp_checksum.MCU._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathExe()
{
	CString m_pathname;
	//CString Ini_File_Path = _T("\\Test_Case\\");

	CFileDialog fdlg(false, CString(_T(".exe")), m_IspOriginalPath + _T("\\"), NULL, _T("Exe Files|*.exe||"));	//建立單選型檔案對話盒物件
	if (fdlg.DoModal() != IDOK) {
		return;
	}
	m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

	m_PathFile_Exe = m_pathname;

	CString message;
	message.Format(_T("%s"), m_PathFile_Exe.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_EXE)->SetWindowText(file_name);
	GetDlgItem(IDC_EDIT_PATH_EXE)->EnableWindow(false);


}


void CUserBinMergeToolDlg::OnBnClickedButtonCombine()
{	
#if 0
	CString CStr_temp;
	BYTE Temp_Array[4];

	int Temp_value;

	Int2BytesArray(Temp_Array, sizeof(isp_headpage.GD_MCU.Length), isp_headpage.GD_MCU.Length._dword);

	CStr_temp.Format(_T("%x %x %x %x"), Temp_Array[3], Temp_Array[2], Temp_Array[1], Temp_Array[0]);
	user_DisplayInfo.AppendToLogAndScroll(CStr_temp, GREEN);
	
	Temp_value = BytesArray2Int(Temp_Array, sizeof(Temp_Array));
	
	CStr_temp.Format(_T("%d"), Temp_value);
	user_DisplayInfo.AppendToLogAndScroll(CStr_temp, GREEN);
	return;
#endif // 1

#if 0
	CString _message;

	_message.Format(_T("IspExtendTempChip_t :%d\n"), sizeof(IspExtendChip_t));
	user_DisplayInfo.AppendToLogAndScroll(_message, RED);
	return;
	_message.Format(_T("GD_MCU Version:%d\n"), sizeof(isp_headpage.GD_MCU.Version));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("GD_MCU Offset:%d\n"), sizeof(isp_headpage.GD_MCU.Offset));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("GD_MCU Length:%d\n"), sizeof(isp_headpage.GD_MCU.Length));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("GD_MCU Checksum:%d\n"), sizeof(isp_headpage.GD_MCU.Checksum));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("GD_MCU Sum:%d\n"), sizeof(isp_headpage.GD_MCU));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	return;
	_message.Format(_T("isp_version_in_mcu:%d\n"), sizeof(isp_headpage.isp_version_in_mcu));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Reserve_0:%d\n"), sizeof(isp_headpage.Reserve_0));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("isp_offset:%d\n"), sizeof(isp_headpage.isp_offset));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Reserve_1:%d\n"), sizeof(isp_headpage.Reserve_1));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("isp_length:%d\n"), sizeof(isp_headpage.isp_length));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Reserve_2:%d\n"), sizeof(isp_headpage.Reserve_2));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("isp_checksum:%d\n"), sizeof(isp_headpage.isp_checksum));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Reserve_3:%d\n"), sizeof(isp_headpage.Reserve_3));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("CompareKeyFlag:%d\n"), sizeof(isp_headpage.CompareKeyFlag));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Reserve_4:%d\n"), sizeof(isp_headpage.Reserve_4));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("GD_MCU:%d\n"), sizeof(isp_headpage.GD_MCU));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Extra_USB3:%d\n"), sizeof(isp_headpage.Extra_USB3));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Extra_DP:%d\n"), sizeof(isp_headpage.Extra_DP));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	_message.Format(_T("Reserve_5:%d\n"), sizeof(isp_headpage.Reserve_5));
	user_DisplayInfo.AppendToLogAndScroll(_message, BLUE);

	return;
#endif

#if 0
	//Temp calculate
	DWORD result_MCU, result_App;
	DWORD instant_value;
//#define constant_C 0x11707346
//#define constant_CC 0x21264672

	//instant_value = (MCU_Key + constant_C)*2 + constant_CC;

	result_MCU = MCU_Key + constant_C;
	result_App = (APP_Key - constant_CC) / 2;


	if (result_MCU == result_App)
	{
		AfxMessageBox(_T("Verified pass."), MB_ICONEXCLAMATION);
		return;
	}

	return;
#endif


	GetDlgItem(IDC_EDIT_COMPANY_NAME)->GetWindowText(m_Company_Name);
	GetDlgItem(IDC_EDIT_TAG_VALUE)->GetWindowText(m_Edge_Tag);

	GetDlgItem(IDC_EDIT_HEAD_VERSION_IOT)->GetWindowText(m_Head_Version_IOT);
	if (m_Head_Version_IOT == _T(""))
	{
		AfxMessageBox(_T("No IOT Version value."), MB_ICONEXCLAMATION);
		return;
	}
	if (m_Head_Version_IOT.GetLength() != 4)
	{
		AfxMessageBox(_T("Must insert 4 numbers at iVersion."), MB_ICONEXCLAMATION);
		return;
	}

	GetDlgItem(IDC_EDIT_HEAD_VERSION_NOIOT)->GetWindowText(m_Head_Version_NoIOT);
	if (m_Head_Version_NoIOT == _T(""))
	{
		AfxMessageBox(_T("No No-IOT Version value."), MB_ICONEXCLAMATION);
		return;
	}
	if (m_Head_Version_NoIOT.GetLength() != 4)
	{
		AfxMessageBox(_T("Must insert 4 numbers at iVersion."), MB_ICONEXCLAMATION);
		return;
	}

	if (m_Edge_Tag == _T(""))
	{
		AfxMessageBox(_T("No Tag value."), MB_ICONEXCLAMATION);
		return;
	}
	

	//後續要把version放在m_Version裡
	CString m_pathname;
	CString message, message_IOT, message_NoIOT;

	CFileDialog fdlg(false, CString(_T(".bin")), m_IspOriginalPath + _T("\\"), NULL, _T("Bin Files|*.bin||"));	//建立單選型檔案對話盒物件
	if (fdlg.DoModal() != IDOK) {
		return;
	}
	m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

	CString m_pathname_temp;
	int Check_result = m_pathname.Find(_T(".bin"), 0);
	m_pathname_temp = m_pathname.Mid(0, Check_result);

	m_Combine_path_IOT = m_pathname_temp + _T("_") + m_Head_Version_IOT + _T("_IOT.bin");

	m_Combine_path_NoIOT = m_pathname_temp + _T("_") + m_Head_Version_NoIOT + _T("_NoIOT.bin");

	if (PathFileExists(m_Combine_path_IOT))
	{
		DeleteFile(m_Combine_path_IOT);
	}
	if (PathFileExists(m_Combine_path_NoIOT))
	{
		DeleteFile(m_Combine_path_NoIOT);
	}

	CString file_name;
	message.Format(_T("%s"), m_Combine_path_IOT);
	int Check_Result = message.ReverseFind(_T('\\'));
	message_IOT = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_COMBINED_BIN_IOT)->SetWindowText(message_IOT.GetString());

	user_log_save.m_FileName_MergedFile_IOT = message_IOT;


	message.Format(_T("%s"), m_Combine_path_NoIOT);
	Check_Result = message.ReverseFind(_T('\\'));
	message_NoIOT = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_COMBINED_BIN_NOIOT)->SetWindowText(message_NoIOT.GetString());

	user_log_save.m_FileName_MergedFile_NoIOT = message_NoIOT;



	Create_HeaderFile();



	CButton* m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_REDUCTION);
	int m_Reduction_flag = m_ctlCheck->GetCheck();

#ifdef IOT_PAGE

		user_CombineFile(m_HeadFile_Path_IOT, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_DMC_IOT)) {
			AfxMessageBox(_T("DMC file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_DMC_IOT, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_TBT4_IOT)) {
			AfxMessageBox(_T("TBT4 file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_TBT4_IOT, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_DP)) {
			AfxMessageBox(_T("DP file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_DP, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_USB_IOT)) {
			AfxMessageBox(_T("USB3 IOT file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_USB_IOT, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_USB2_HUB)) {
			AfxMessageBox(_T("USB 2.0 Hub file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_USB2_HUB, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_USB_AUDIO)) {
			AfxMessageBox(_T("Audio file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_USB_AUDIO, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_LAN_I225)) {
			AfxMessageBox(_T("Lan I225 file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_LAN_I225, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_MCU)) {
			AfxMessageBox(_T("MCU file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_MCU, m_Combine_path_IOT);

		//*****Alan, 20220207*************
		if (!PathFileExists(m_PathFile_GD_MCU)) {
			AfxMessageBox(_T("GD MCU file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_GD_MCU, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_Extra_USB_IOT)) {
			AfxMessageBox(_T("Extra USB3 IOT file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_Extra_USB_IOT, m_Combine_path_IOT);

		if (!PathFileExists(m_PathFile_Extra_DP)) {
			AfxMessageBox(_T("Extra DP file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_Extra_DP, m_Combine_path_IOT);
		//*******************************

		//Victor 0520
		if (!PathFileExists(m_PathFile_RTK_MST)) {
			AfxMessageBox(_T("RTK MST file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_RTK_MST, m_Combine_path_IOT);


		if (!PathFileExists(m_PathFile_Security)) {
			AfxMessageBox(_T("Security file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_IOT);
		user_CombineFile(m_PathFile_Security, m_Combine_path_IOT);

		gb_Check_sum._dword = Make_Security_CheckSum(m_Combine_path_IOT);
		message.Format(_T("%04x"), gb_Check_sum._dword);
		GetDlgItem(IDC_STATIC_VALUE_CHECKSUM)->SetWindowText(message);
		message.Format(_T("Security Value : %04x\r\n"), gb_Check_sum._dword);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);


		//Build CheckSum file
		Create_CheckSum_File();
		user_CombineFile(m_ChecksumFile_Path, m_Combine_path_IOT);

#endif

#ifdef NoIOT_PAGE

		user_CombineFile(m_HeadFile_Path_NoIOT, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_DMC_NoIOT)) {
			AfxMessageBox(_T("DMC file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_DMC_NoIOT, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_TBT4_NoIOT)) {
			AfxMessageBox(_T("TBT4 file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_TBT4_NoIOT, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_DP)) {
			AfxMessageBox(_T("DP file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_DP, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_USB_NoIOT)) {
			AfxMessageBox(_T("USB3 IOT file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_USB_NoIOT, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_USB2_HUB)) {
			AfxMessageBox(_T("USB 2.0 Hub file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_USB2_HUB, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_USB_AUDIO)) {
			AfxMessageBox(_T("Audio file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_USB_AUDIO, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_LAN_I225)) {
			AfxMessageBox(_T("Lan I225 file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_LAN_I225, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_MCU)) {
			AfxMessageBox(_T("MCU file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_MCU, m_Combine_path_NoIOT);

		//*****Alan, 20220207*************
		if (!PathFileExists(m_PathFile_GD_MCU)) {
			AfxMessageBox(_T("GD MCU file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_GD_MCU, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_Extra_USB_NoIOT)) {
			AfxMessageBox(_T("Extra USB3 No IOT file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_Extra_USB_NoIOT, m_Combine_path_NoIOT);

		if (!PathFileExists(m_PathFile_Extra_DP)) {
			AfxMessageBox(_T("Extra DP file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_Extra_DP, m_Combine_path_NoIOT);
		//*******************************

		//Victor 20220520
		if (!PathFileExists(m_PathFile_RTK_MST)) {
			AfxMessageBox(_T("Extra RTK MST not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_RTK_MST, m_Combine_path_NoIOT);


		if (!PathFileExists(m_PathFile_Security)) {
			AfxMessageBox(_T("Security file not exist."), MB_ICONEXCLAMATION);
			return;
		}

		user_Adding_Tag(m_Combine_path_NoIOT);
		user_CombineFile(m_PathFile_Security, m_Combine_path_NoIOT);

		gb_Check_sum._dword = Make_Security_CheckSum(m_Combine_path_NoIOT);
		message.Format(_T("%04x"), gb_Check_sum._dword);
		GetDlgItem(IDC_STATIC_VALUE_CHECKSUM)->SetWindowText(message);
		message.Format(_T("Security Value : %04x\r\n"), gb_Check_sum._dword);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);


		//Build CheckSum file
		Create_CheckSum_File();
		user_CombineFile(m_ChecksumFile_Path, m_Combine_path_NoIOT);

#endif

		user_log_save.doSaveCSVLog(_T("Combined Version History"), m_Head_Version_IOT, m_Head_Version_NoIOT);

	//message.Format(_T("%s"), m_Combine_path_IOT);
	//Check_Result = message.ReverseFind(_T('\\'));
	//message_IOT = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	//message.Format(_T("%s"), m_Combine_path_NoIOT);
	//Check_Result = message.ReverseFind(_T('\\'));
	//message_NoIOT = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	message.Format(_T("%s & %s created!"), message_IOT, message_NoIOT);
	AfxMessageBox(message, MB_ICONEXCLAMATION);

	   

	GetDlgItem(IDC_BUTTON_COMBINE)->EnableWindow(false);


}

int CUserBinMergeToolDlg::user_Get_File_Size(CString file_path) {
	CString message;

	fstream m_temp(file_path, ios::binary | ios::in);
	if (!m_temp)
	{

		message.Format(_T("Cound not open %s in get length"), file_path);
		AfxMessageBox(message, MB_ICONEXCLAMATION);

		m_temp.close();
		return 0;
	}

	m_temp.seekg(0, ios::end);
	// 3. tell 位置 
	int  length = m_temp.tellg();
	// 4. 關檔
	m_temp.close();

	if (!length)
	{
		message.Format(_T("Length is 0"), file_path);
		AfxMessageBox(message, MB_ICONEXCLAMATION);
	}

	return length;
}

void CUserBinMergeToolDlg::OnBnClickedOk()
{
	Initial_Write();

	CDialogEx::OnOK();
}


void CUserBinMergeToolDlg::OnBnClickedCancel()
{
	Initial_Write();

	CDialogEx::OnCancel();
}

void CUserBinMergeToolDlg::Initial_Write(void) {
	CString message;

	CString PathINI = m_IspOriginalPath + _T("\\Temp_Configure.ini");

	GetDlgItem(IDC_EDIT_COMPANY_NAME)->GetWindowText(m_Company_Name);
	WritePrivateProfileString(_T("Head config"), _T("Company_Name"), m_Company_Name, PathINI);
	GetDlgItem(IDC_EDIT_HEAD_VERSION_IOT)->GetWindowText(m_Head_Version_IOT);
	WritePrivateProfileString(_T("Head config"), _T("Head_Version_IOT"), m_Head_Version_IOT, PathINI);
	GetDlgItem(IDC_EDIT_HEAD_VERSION_NOIOT)->GetWindowText(m_Head_Version_NoIOT);
	WritePrivateProfileString(_T("Head config"), _T("Head_Version_NoIOT"), m_Head_Version_NoIOT, PathINI);
	GetDlgItem(IDC_EDIT_TAG_VALUE)->GetWindowText(m_Edge_Tag);
	WritePrivateProfileString(_T("Head config"), _T("Edge_Tag"), m_Edge_Tag, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_DMC"), m_PathFile_DMC_IOT, PathINI);
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_DMC_NoIOT"), m_PathFile_DMC_NoIOT, PathINI);
	
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_TBT4_IOT"), m_PathFile_TBT4_IOT, PathINI);
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_TBT4_NoIOT"), m_PathFile_TBT4_NoIOT, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_DP"), m_PathFile_DP, PathINI);



	WritePrivateProfileString(_T("Bin config"), _T("PathFile_USB3_IOT"), m_PathFile_USB_IOT, PathINI);
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_USB3_NoIOT"), m_PathFile_USB_NoIOT, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_USB2_HUB"), m_PathFile_USB2_HUB, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_USB_AUDIO"), m_PathFile_USB_AUDIO, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_LAN_I225"), m_PathFile_LAN_I225, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_MCU"), m_PathFile_MCU, PathINI);

	WritePrivateProfileString(_T("Bin config"), _T("PathFile_GD_MCU"), m_PathFile_GD_MCU, PathINI);	//Alan,20220208
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_EXTRA_USB3_IOT"), m_PathFile_Extra_USB_IOT, PathINI);	//Alan,20220208
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_EXTRA_USB3_NoIOT"), m_PathFile_Extra_USB_NoIOT, PathINI);	//Alan,20220208
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_EXTRA_DP"), m_PathFile_Extra_DP, PathINI);	//Alan,20220208

	//Victor 20220520
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_RTK_MST"), m_PathFile_RTK_MST, PathINI);


	WritePrivateProfileString(_T("Bin config"), _T("PathFile_Exe"), m_PathFile_Exe, PathINI);

	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_DMC"), m_Reduction_PathFile[Chip_DMC], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_TBT4"), m_Reduction_PathFile[Chip_TBT4], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_DP"), m_Reduction_PathFile[Chip_DP], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_USB_HUB1"), m_Reduction_PathFile[Chip_USB_Hub1], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_USB2_HUB"), m_Reduction_PathFile[Chip_U2_Hub], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_USB_AUDIO"), m_Reduction_PathFile[Chip_Audio], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_LAN_I225"), m_Reduction_PathFile[Chip_LAN_I225], PathINI);
	WritePrivateProfileString(_T("Reducted Bin config"), _T("PathFile_MCU"), m_Reduction_PathFile[Chip_MCU], PathINI);

	WritePrivateProfileString(_T("Other config"), _T("PathFile_CodeVer"), m_CodeVer_Path, PathINI);
	WritePrivateProfileString(_T("Bin config"), _T("PathFile_Security"), m_PathFile_Security, PathINI);
	
}


void CUserBinMergeToolDlg::Initial_Read(void)
{
#define MAX_FILE_NAME_BUFFER 20480
	CString FilePath_temp;
	int Check_Result = 0;

	CString PathINI = m_IspOriginalPath + _T("\\Temp_Configure.ini");

	if (!PathFileExists(PathINI))
	{
		return;
	}

	GetPrivateProfileString(_T("Head config"), _T("Company_Name"), _T("USI"), m_Company_Name.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	GetPrivateProfileString(_T("Head config"), _T("Head_Version_IOT"), _T("0"), m_Head_Version_IOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	GetPrivateProfileString(_T("Head config"), _T("Head_Version_NoIOT"), _T("0"), m_Head_Version_NoIOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	GetPrivateProfileString(_T("Head config"), _T("Edge_Tag"), _T("<Default>"), m_Edge_Tag.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	m_Edge_Tag.Format(_T("%s"), m_Edge_Tag.GetString());	//Initialization, or there is error about "GetLenth()"

	GetPrivateProfileString(_T("Bin config"), _T("PathFile_DMC"), _T(""), m_PathFile_DMC_IOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_DMC_IOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_DMC_IOT = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_DMC_NoIOT"), _T(""), m_PathFile_DMC_NoIOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_DMC_NoIOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_DMC_NoIOT = FilePath_temp;
	}
	
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_TBT4_IOT"), _T(""), m_PathFile_TBT4_IOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_TBT4_IOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_TBT4_IOT = FilePath_temp;
	}

	GetPrivateProfileString(_T("Bin config"), _T("PathFile_TBT4_NoIOT"), _T(""), m_PathFile_TBT4_NoIOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_TBT4_NoIOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_TBT4_NoIOT = FilePath_temp;
	}
	
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_DP"), _T(""), m_PathFile_DP.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_DP));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_DP = FilePath_temp;
	}

	GetPrivateProfileString(_T("Bin config"), _T("PathFile_USB3_IOT"), _T(""), m_PathFile_USB_IOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_USB_IOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_USB_IOT = FilePath_temp;
	}

	GetPrivateProfileString(_T("Bin config"), _T("PathFile_USB3_NoIOT"), _T(""), m_PathFile_USB_NoIOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_USB_NoIOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_USB_NoIOT = FilePath_temp;
	}

	GetPrivateProfileString(_T("Bin config"), _T("PathFile_USB2_HUB"), _T(""), m_PathFile_USB2_HUB.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_USB2_HUB));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_USB2_HUB = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_USB_AUDIO"), _T(""), m_PathFile_USB_AUDIO.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_USB_AUDIO));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_USB_AUDIO = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_LAN_I225"), _T(""), m_PathFile_LAN_I225.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_LAN_I225));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_LAN_I225 = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_MCU"), _T(""), m_PathFile_MCU.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_MCU));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_MCU = FilePath_temp;
	}
	
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_GD_MCU"), _T(""), m_PathFile_GD_MCU.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_GD_MCU));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_GD_MCU = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_EXTRA_USB3_IOT"), _T(""), m_PathFile_Extra_USB_IOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_Extra_USB_IOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_Extra_USB_IOT = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_EXTRA_USB3_NoIOT"), _T(""), m_PathFile_Extra_USB_NoIOT.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_Extra_USB_NoIOT));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_Extra_USB_NoIOT = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_EXTRA_DP"), _T(""), m_PathFile_Extra_DP.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_Extra_DP));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_Extra_DP = FilePath_temp;
	}

	//Victor 20220520
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_RTK_MST"), _T(""), m_PathFile_RTK_MST.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_RTK_MST));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_RTK_MST = FilePath_temp;
	}

	GetPrivateProfileString(_T("Bin config"), _T("PathFile_Exe"), _T(""), m_PathFile_Exe.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_Exe));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_Exe = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_DMC"), _T(""), m_Reduction_PathFile[Chip_DMC].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_DMC]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_DMC] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_TBT4"), _T(""), m_Reduction_PathFile[Chip_TBT4].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_TBT4]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_TBT4] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_DP"), _T(""), m_Reduction_PathFile[Chip_DP].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_DP]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_DP] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_USB_HUB1"), _T(""), m_Reduction_PathFile[Chip_USB_Hub1].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_USB_Hub1]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_USB_Hub1] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_USB2_HUB"), _T(""), m_Reduction_PathFile[Chip_U2_Hub].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_U2_Hub]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_U2_Hub] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_USB_AUDIO"), _T(""), m_Reduction_PathFile[Chip_Audio].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_Audio]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_Audio] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_LAN_I225"), _T(""), m_Reduction_PathFile[Chip_LAN_I225].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_LAN_I225]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_LAN_I225] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Reducted Bin config"), _T("PathFile_MCU"), _T(""), m_Reduction_PathFile[Chip_MCU].GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_Reduction_PathFile[Chip_MCU]));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_Reduction_PathFile[Chip_MCU] = FilePath_temp;
	}
	GetPrivateProfileString(_T("Other config"), _T("PathFile_CodeVer"), _T(""), m_CodeVer_Path.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_CodeVer_Path));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_CodeVer_Path = FilePath_temp;
	}
	GetPrivateProfileString(_T("Bin config"), _T("PathFile_Security"), _T(""), m_PathFile_Security.GetBuffer(MAX_FILE_NAME_BUFFER), MAX_FILE_NAME_BUFFER, PathINI);
	FilePath_temp.Format(_T("%s"), SetRelative_Path(m_PathFile_Security));	//有相對路徑的檔案，改成相對路徑
	if (PathFileExists(FilePath_temp)) {
		m_PathFile_Security = FilePath_temp;
	}
}

void CUserBinMergeToolDlg::Initial_Value_Set(void)
{
	CString message, file_name;
	int Check_Result;
	int temp_value;

	GetDlgItem(IDC_EDIT_COMPANY_NAME)->SetWindowText(m_Company_Name);
	GetDlgItem(IDC_EDIT_HEAD_VERSION_IOT)->SetWindowText(m_Head_Version_IOT.GetString());
	GetDlgItem(IDC_EDIT_HEAD_VERSION_NOIOT)->SetWindowText(m_Head_Version_NoIOT.GetString());
	GetDlgItem(IDC_EDIT_TAG_VALUE)->SetWindowText(m_Edge_Tag);

	message.Format(_T("%s"), m_PathFile_DMC_IOT.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_DMC_NOIOT)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_DMC_NoIOT.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_DMC_IOT)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_TBT4_IOT.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_TBT4)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_TBT4_NoIOT.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_TBT4_NoIOT)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_DP.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_DP_IOT)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_USB_IOT.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_USB3_IOT)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_USB_NoIOT.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_TBT4_NoIOT)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_USB2_HUB.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_USB2_HUB)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_USB_AUDIO.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_USB_AUDIO)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_LAN_I225.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_LAN_I225)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_MCU.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_MCU)->SetWindowText(file_name);

	message.Format(_T("%s"), m_PathFile_Exe.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_EXE)->SetWindowText(file_name);


	//GetDlgItem(IDC_EDIT_VSERION_DMC)->SetWindowText(m_Version_DMC);
	//GetDlgItem(IDC_EDIT_VSERION_PD)->SetWindowText(m_Version_PD);
	//GetDlgItem(IDC_EDIT_VSERION_TBT4)->SetWindowText(m_Version_TBT4);
	//GetDlgItem(IDC_EDIT_VSERION_DP_5XXX_IOT)->SetWindowText(m_Version_DP_5xxx);
	//GetDlgItem(IDC_EDIT_VSERION_DP_6XXX_IOT)->SetWindowText(m_Version_DP_6xxx);
	//GetDlgItem(IDC_EDIT_VSERION_USB2_HUB)->SetWindowText(m_Version_USB2_HUB);
	//GetDlgItem(IDC_EDIT_VSERION_USB_AUDIO)->SetWindowText(m_Version_USB_AUDIO);
	//GetDlgItem(IDC_EDIT_VSERION_LAN_I225)->SetWindowText(m_Version_LAN_I225);
	//GetDlgItem(IDC_EDIT_VSERION_MCU)->SetWindowText(m_Version_MCU);

	message.Format(_T("%s"), m_PathFile_Security.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_PATH_SECURITY)->SetWindowText(file_name);

	message.Format(_T("%s"), m_CodeVer_Path.GetString());
	Check_Result = message.ReverseFind(_T('\\'));
	file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	GetDlgItem(IDC_EDIT_CODEVER_TAG)->SetWindowText(file_name);
	
}

void CUserBinMergeToolDlg::user_CreateFile(CString target_path, char* content, int buff_lenth) {
	fstream m_temp;
	do
	{
		m_temp.open(target_path, ios::binary | ios::out | ios::app);

		if (!m_temp)
		{
			CString message;
			message.Format(_T("Cound not open %s in createfile"), target_path);
			//AfxMessageBox(message, MB_ICONEXCLAMATION);
			user_DisplayInfo.AppendToLogAndScroll(message, RED);
		}
	} while (!m_temp);

	//fstream m_temp(m_Combine_path, ios::binary | ios::out | ios::app);

	for (size_t i = 0; i < buff_lenth; i++)
	{
		m_temp.put(*(content + i));
	}

	m_temp.close();
}

void CUserBinMergeToolDlg::user_CombineFile(CString file_path_input, CString file_path_output) {

	char* temp_buff;
	char temp_char;
	int file_size = 0;
	file_size = user_Get_File_Size(file_path_input);

	fstream m_temp(file_path_input, ios::binary | ios::in);
	//m_temp.open(file_path_input, ios::binary | ios::in);
	if (!m_temp)
	{
		CString message;
		message.Format(_T("Cound not open %s"), file_path_input);
		AfxMessageBox(message, MB_ICONEXCLAMATION);
	}

	temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));

	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_char);
		//if (!m_temp) {
		//	file_end_flag = 1;
		//	socket_size = file_size - content_pointer;
		//	break;
		//}
		temp_buff[i] = temp_char;
	}

	m_temp.close();

	user_CreateFile(file_path_output, temp_buff, file_size);

	delete[] temp_buff;


}

string CUserBinMergeToolDlg::user_CT2A(CString C_str) {
	//char* temp_str_ASCII;

	//CT2A temp_str_ASCII(C_str);

	//return temp_str_ASCII.m_szBuffer;

	CT2A temp(C_str);
	const char* pszA = temp;
	std::string strA(pszA);
	return strA;
}

void CUserBinMergeToolDlg::user_Adding_Tag(CString target_path)
{
	char* tag_buff;

	GetDlgItem(IDC_EDIT_TAG_VALUE)->GetWindowText(m_Edge_Tag);

	int temp = m_Edge_Tag.GetLength();

	tag_buff = new char[m_Edge_Tag.GetLength() + 1];
	memset(tag_buff, 0, sizeof(tag_buff));

	strcpy_s(tag_buff, m_Edge_Tag.GetLength() + 1, user_CT2A(m_Edge_Tag).c_str());
	user_CreateFile(target_path, tag_buff, m_Edge_Tag.GetLength());

	delete[] tag_buff;
}


void CUserBinMergeToolDlg::Create_HeaderFile(void) {
	char head_buff[512];
	CString message;

	for (int i = 0; i < HeadFile_Size; i++) {
		head_buff[i] = 0xff;
	}

	Set_Offset_Value();

//***Create bcdversion******
	//----------------IOT head version----------------------------
	char temp_buff[6];
	strcpy_s(temp_buff, 6, user_CT2A(m_Head_Version_IOT).c_str());

	for (size_t i = 0; i < 4; i++)
	{
		if ((temp_buff[i] >= 0x30) && (temp_buff[i] <= 0x39))		//ASCii to value, 1~9
		{
			temp_buff[i] = temp_buff[i] - 0x30;
		}
		if ((temp_buff[i] >= 0x61) && (temp_buff[i] <= 0x66))		//ASCii to value, a~f
		{
			temp_buff[i] = temp_buff[i] - 0x57;
		}
		if ((temp_buff[i] >= 0x41) && (temp_buff[i] <= 0x46))		//ASCii to value, A~F
		{
			temp_buff[i] = temp_buff[i] - 0x37;
		}
	}
	isp_headpage.isp_version_in_mcu.bcdVersion[1] = (temp_buff[0] << 4) + temp_buff[1];
	isp_headpage.isp_version_in_mcu.bcdVersion[0] = (temp_buff[2] << 4) + temp_buff[3];


	//----------------No IOT head version----------------------------
	strcpy_s(temp_buff, 6, user_CT2A(m_Head_Version_NoIOT).c_str());

	for (size_t i = 0; i < 4; i++)
	{
		if ((temp_buff[i] >= 0x30) && (temp_buff[i] <= 0x39))		//ASCii to value, 1~9
		{
			temp_buff[i] = temp_buff[i] - 0x30;
		}
		if ((temp_buff[i] >= 0x61) && (temp_buff[i] <= 0x66))		//ASCii to value, a~f
		{
			temp_buff[i] = temp_buff[i] - 0x57;
		}
		if ((temp_buff[i] >= 0x41) && (temp_buff[i] <= 0x46))		//ASCii to value, A~F
		{
			temp_buff[i] = temp_buff[i] - 0x37;
		}
	}

	isp_headpage_NoIOT.isp_version_in_mcu.bcdVersion[1] = (temp_buff[0] << 4) + temp_buff[1];
	isp_headpage_NoIOT.isp_version_in_mcu.bcdVersion[0] = (temp_buff[2] << 4) + temp_buff[3];
	
//***************************

//****Create total length*******
	//Security offset + Security length +CheckSum file size
	isp_headpage.isp_length.Summary_Length._dword = isp_headpage.isp_offset.LVN_Security._dword+ isp_headpage.isp_length.LVN_Security._dword;
	isp_headpage_NoIOT.isp_length.Summary_Length._dword = isp_headpage_NoIOT.isp_offset.LVN_Security._dword + isp_headpage_NoIOT.isp_length.LVN_Security._dword;


	message.Format(_T("Totol Length : %d\r\n"), isp_headpage.isp_length.Summary_Length._dword);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
	
	message.Format(_T("NoIOT Totol Length : %d\r\n"), isp_headpage_NoIOT.isp_length.Summary_Length._dword);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
	//***************************

	isp_headpage.CompareKeyFlag[0] = 0x05;	//Set new security flag to 0x05
	isp_headpage_NoIOT.CompareKeyFlag[0] = 0x05;	//Set new security flag to 0x05

//***Check checkbox checked or not********
	CButton* m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_REDUCTION);
	int m_Reduction_flag = m_ctlCheck->GetCheck();
//****************************************


	//int i = 0;


//Little Ending to Big Ending, Version
	Eding_Swap(&isp_headpage.isp_version_in_mcu.DMC[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.PD[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.DP5x[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.DP6x[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.TBT4[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.USB3[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.USB2[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.AUDIO[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.I255[0], 5);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.MCU[0], 2);
	Eding_Swap(&isp_headpage.isp_version_in_mcu.bcdVersion[0], 2);
	//Alan,20220208
	Eding_Swap(&isp_headpage.GD_MCU.Version[0], 5);
	Eding_Swap(&isp_headpage.Extra_USB3.Version[0], 5);
	Eding_Swap(&isp_headpage.Extra_DP.Version_DP5[0], 5);
	Eding_Swap(&isp_headpage.Extra_DP.Version_DP6[0], 5);

	//Victor 20220520
	//seems TBD here

	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.DMC[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.PD[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.DP6x[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.TBT4[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.USB3[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.USB2[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.AUDIO[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.I255[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.MCU[0], 2);
	Eding_Swap(&isp_headpage_NoIOT.isp_version_in_mcu.bcdVersion[0], 2);
	//Alan,20220208
	Eding_Swap(&isp_headpage_NoIOT.GD_MCU.Version[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.Extra_USB3.Version[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.Extra_DP.Version_DP5[0], 5);
	Eding_Swap(&isp_headpage_NoIOT.Extra_DP.Version_DP6[0], 5);

	//Victor 20220520
	//seem TBD here

//Little Ending to Big Ending, Offset
	Eding_Swap(&isp_headpage.isp_offset.DMC._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.PD._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.DP5x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.DP6x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.TBT4._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.USB3._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.USB2._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.AUDIO._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.I255._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.MCU._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_offset.LVN_Security._byte[0], sizeof(Union_DWORD_t));
	//Alan,20220208
	Eding_Swap(&isp_headpage.GD_MCU.Offset[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.Extra_USB3.Offset[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.Extra_DP.Offset[0], sizeof(Union_DWORD_t));

	//Victor 20220520
	//TBD

	Eding_Swap(&isp_headpage_NoIOT.isp_offset.DMC._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.PD._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.DP5x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.DP6x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.TBT4._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.USB3._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.USB2._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.AUDIO._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.I255._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.MCU._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_offset.LVN_Security._byte[0], sizeof(Union_DWORD_t));
	//Alan,20220208
	Eding_Swap(&isp_headpage_NoIOT.GD_MCU.Offset[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.Extra_USB3.Offset[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.Extra_DP.Offset[0], sizeof(Union_DWORD_t));

	//Victor 20220520
	//TBD

//Little Ending to Big Ending, length
	Eding_Swap(&isp_headpage.isp_length.DMC._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.PD._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.DP5x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.DP6x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.TBT4._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.USB3._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.USB2._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.AUDIO._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.I255._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.MCU._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.LVN_Security._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.isp_length.Summary_Length._byte[0], sizeof(Union_DWORD_t));
	//Alan,20220208
	Eding_Swap(&isp_headpage.GD_MCU.Length[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.Extra_USB3.Length[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage.Extra_DP.Length[0], sizeof(Union_DWORD_t));

	//Victor 20220520
	//TBD

	Eding_Swap(&isp_headpage_NoIOT.isp_length.DMC._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.PD._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.DP5x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.DP6x._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.TBT4._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.USB3._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.USB2._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.AUDIO._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.I255._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.MCU._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.LVN_Security._byte[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_length.Summary_Length._byte[0], sizeof(Union_DWORD_t));
	//Alan,20220208
	Eding_Swap(&isp_headpage_NoIOT.GD_MCU.Length[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.Extra_USB3.Length[0], sizeof(Union_DWORD_t));
	Eding_Swap(&isp_headpage_NoIOT.Extra_DP.Length[0], sizeof(Union_DWORD_t));

	//Victor 20220520
	//TBD

//Little Ending to Big Ending, Checksum
	Eding_Swap(&isp_headpage.isp_checksum.DMC._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.PD._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.DP5x._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.DP6x._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.TBT4._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.USB3._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.USB2._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.AUDIO._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.I255._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.MCU._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.isp_checksum.LVN_Security._byte[0], sizeof(Union_WORD_t));
	//Alan,20220208
	Eding_Swap(&isp_headpage.GD_MCU.Checksum[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.Extra_USB3.Checksum[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage.Extra_DP.Checksum[0], sizeof(Union_WORD_t));

	//Victor 20220520
	//TBD

	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.DMC._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.PD._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.DP5x._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.DP6x._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.TBT4._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.USB3._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.USB2._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.AUDIO._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.I255._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.MCU._byte[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.isp_checksum.LVN_Security._byte[0], sizeof(Union_WORD_t));
	//Alan,20220208
	Eding_Swap(&isp_headpage_NoIOT.GD_MCU.Checksum[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.Extra_USB3.Checksum[0], sizeof(Union_WORD_t));
	Eding_Swap(&isp_headpage_NoIOT.Extra_DP.Checksum[0], sizeof(Union_WORD_t));

	//Victor 20220520
	Eding_Swap(&isp_headpage_NoIOT.RTK_MST.Checksum[0], sizeof(Union_WORD_t));

	temp_point = &isp_headpage;

	temp_point_NoIOT = &isp_headpage_NoIOT;





	//head_buff[509] = temp_buff[2];
	//head_buff[510] = temp_buff[3];
	//head_buff[511] = temp_buff[4];


	m_HeadFile_Path_IOT.Format(_T("%s\\head_IOT.bin"), m_IspOriginalPath.GetString());

	if (PathFileExists(m_HeadFile_Path_IOT)) {
		DeleteFile(m_HeadFile_Path_IOT);
	}

	user_CreateFile(m_HeadFile_Path_IOT, (char*)temp_point, HeadFile_Size);

	m_HeadFile_Path_NoIOT.Format(_T("%s\\headNo_IOT.bin"), m_IspOriginalPath.GetString());

	if (PathFileExists(m_HeadFile_Path_NoIOT)) {
		DeleteFile(m_HeadFile_Path_NoIOT);
	}

	user_CreateFile(m_HeadFile_Path_NoIOT, (char*)temp_point_NoIOT, HeadFile_Size);

}

void CUserBinMergeToolDlg::Set_Offset_Value(void) {
	CString message;

	int tag_length = 0;
	int offset_pointer = 0;
	int NoIOT_offset_pointer = 0;

	tag_length = m_Edge_Tag.GetLength();

	CButton * m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_REDUCTION);
	int m_Reduction_flag = m_ctlCheck->GetCheck();
#if 1
		offset_pointer = offset_pointer + HeadFile_Size + tag_length;
		isp_headpage.isp_offset.PD._dword = offset_pointer;	//DMC offset

		message.Format(_T("PD offset:0x%x\r\n"), isp_headpage.isp_offset.PD._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		offset_pointer = offset_pointer + isp_headpage.isp_length.PD._dword + tag_length;
		isp_headpage.isp_offset.TBT4._dword = offset_pointer;	//TBT offset
		
		message.Format(_T("TBT offset:0x%x\r\n"), isp_headpage.isp_offset.TBT4._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.TBT4._dword + tag_length;
		isp_headpage.isp_offset.DP6x._dword = offset_pointer;	//DP offset

		message.Format(_T("DP6xxx offset:0x%x\r\n"), isp_headpage.isp_offset.DP6x._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.DP6x._dword + tag_length;
		isp_headpage.isp_offset.USB3._dword = offset_pointer;	//USB Hub1 offset
		
		message.Format(_T("USB 3.0 offset:0x%x\r\n"), isp_headpage.isp_offset.USB3._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.USB3._dword + tag_length;
		isp_headpage.isp_offset.USB2._dword = offset_pointer;	//USB 2.0 Hub offset

		message.Format(_T("USB 2.0 Hub offset:0x%x\r\n"), isp_headpage.isp_offset.USB2._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.USB2._dword + tag_length;
		isp_headpage.isp_offset.AUDIO._dword = offset_pointer;	//Audio offset

		message.Format(_T("Audio offset:0x%x\r\n"), isp_headpage.isp_offset.AUDIO._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.AUDIO._dword + tag_length;
		isp_headpage.isp_offset.I255._dword = offset_pointer;	//LanI225 offset
		
		message.Format(_T("LanI225 offset:0x%x\r\n"), isp_headpage.isp_offset.I255._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.I255._dword + tag_length;
		isp_headpage.isp_offset.MCU._dword = offset_pointer;	//MCU offset
		

		message.Format(_T("MCU offset:0x%x\r\n"), isp_headpage.isp_offset.MCU._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
		
		offset_pointer = offset_pointer + isp_headpage.isp_length.MCU._dword + tag_length;
				
		//****Alan, 20220207****
		//isp_headpage.GD_MCU.Offset._dword = offset_pointer;	//GD MCU offset
		Int2BytesArray(isp_headpage.GD_MCU.Offset, sizeof(isp_headpage.GD_MCU.Offset), offset_pointer);

		//message.Format(_T("GD MCU offset:0x%x\r\n"), isp_headpage.GD_MCU.Offset._dword);
		message.Format(_T("GD MCU offset:0x%02x%02x%02x%02x\r\n"), isp_headpage.GD_MCU.Offset[3], isp_headpage.GD_MCU.Offset[2], isp_headpage.GD_MCU.Offset[1], isp_headpage.GD_MCU.Offset[0]);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);


		//offset_pointer = offset_pointer + isp_headpage.GD_MCU.Length._dword + tag_length;
		offset_pointer = offset_pointer + BytesArray2Int(isp_headpage.GD_MCU.Length, sizeof(isp_headpage.GD_MCU.Length)) + tag_length;

		//isp_headpage.Extra_USB3.Offset._dword = offset_pointer;	//Extra USB3 IOT offset
		Int2BytesArray(isp_headpage.Extra_USB3.Offset, sizeof(isp_headpage.Extra_USB3.Offset), offset_pointer);

		//message.Format(_T("Extra USB3 IOT offset:0x%x\r\n"), isp_headpage.Extra_USB3.Offset._dword);
		message.Format(_T("Extra USB3 IOT offset:0x%02x%02x%02x%02x\r\n"), isp_headpage.Extra_USB3.Offset[3], isp_headpage.Extra_USB3.Offset[2], isp_headpage.Extra_USB3.Offset[1], isp_headpage.Extra_USB3.Offset[0]);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		//offset_pointer = offset_pointer + isp_headpage.Extra_USB3.Length._dword + tag_length;
		offset_pointer = offset_pointer + BytesArray2Int(isp_headpage.Extra_USB3.Length, sizeof(isp_headpage.Extra_USB3.Length)) + tag_length;

		//isp_headpage.Extra_DP.Offset._dword = offset_pointer;	//Extra DP offset
		Int2BytesArray(isp_headpage.Extra_DP.Offset, sizeof(isp_headpage.Extra_DP.Offset), offset_pointer);

		//Victor 20220520
		//TBD

		//message.Format(_T("Extra DP offset:0x%x\r\n"), isp_headpage.Extra_DP.Offset._dword);
		message.Format(_T("Extra DP offset:0x%02x%02x%02x%02x\r\n"), isp_headpage.Extra_DP.Offset[3], isp_headpage.Extra_DP.Offset[2], isp_headpage.Extra_DP.Offset[1], isp_headpage.Extra_DP.Offset[0]);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		//offset_pointer = offset_pointer + isp_headpage.Extra_DP.Length._dword + tag_length;
		offset_pointer = offset_pointer + BytesArray2Int(isp_headpage.Extra_DP.Length, sizeof(isp_headpage.Extra_DP.Length)) + tag_length;
		//**********************

		isp_headpage.isp_offset.LVN_Security._dword = offset_pointer;	//LNV Security offset

		message.Format(_T("Security offset:0x%x\r\n"), isp_headpage.isp_offset.LVN_Security._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
#endif
#if 1
		NoIOT_offset_pointer = NoIOT_offset_pointer + HeadFile_Size + tag_length;
		isp_headpage_NoIOT.isp_offset.PD._dword = NoIOT_offset_pointer;	//DMC offset

		message.Format(_T("PD offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.PD._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.PD._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.TBT4._dword = NoIOT_offset_pointer;	//TBT offset

		message.Format(_T("TBT offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.TBT4._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.TBT4._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.DP6x._dword = NoIOT_offset_pointer;	//DP offset

		message.Format(_T("DP6xxx offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.DP6x._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.DP6x._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.USB3._dword = NoIOT_offset_pointer;	//USB Hub1 offset

		message.Format(_T("USB 3.0 offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.USB3._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.USB3._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.USB2._dword = NoIOT_offset_pointer;	//USB 2.0 Hub offset

		message.Format(_T("USB 2.0 Hub offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.USB2._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.USB2._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.AUDIO._dword = NoIOT_offset_pointer;	//Audio offset

		message.Format(_T("Audio offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.AUDIO._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.AUDIO._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.I255._dword = NoIOT_offset_pointer;	//LanI225 offset

		message.Format(_T("LanI225 offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.I255._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.I255._dword + tag_length;
		isp_headpage_NoIOT.isp_offset.MCU._dword = NoIOT_offset_pointer;	//LanI225 offset


		message.Format(_T("MCU offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.MCU._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		NoIOT_offset_pointer = NoIOT_offset_pointer + isp_headpage_NoIOT.isp_length.MCU._dword + tag_length;

		//****Alan, 20220207****
		//isp_headpage_NoIOT.GD_MCU.Offset._dword = offset_pointer;	//GD MCU offset
		Int2BytesArray(isp_headpage_NoIOT.GD_MCU.Offset, sizeof(isp_headpage_NoIOT.GD_MCU.Offset), NoIOT_offset_pointer);

		//message.Format(_T("GD MCU offset:0x%x\r\n"), isp_headpage_NoIOT.GD_MCU.Offset._dword);
		message.Format(_T("GD MCU offset:0x%02x%02x%02x%02x\r\n"), isp_headpage_NoIOT.GD_MCU.Offset[3], isp_headpage_NoIOT.GD_MCU.Offset[2], isp_headpage_NoIOT.GD_MCU.Offset[1], isp_headpage_NoIOT.GD_MCU.Offset[0]);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		//offset_pointer = offset_pointer + isp_headpage_NoIOT.GD_MCU.Length._dword + tag_length;
		NoIOT_offset_pointer = NoIOT_offset_pointer + BytesArray2Int(isp_headpage_NoIOT.GD_MCU.Length, sizeof(isp_headpage_NoIOT.GD_MCU.Length)) + tag_length;

		//isp_headpage_NoIOT.Extra_USB3.Offset._dword = offset_pointer;	//Extra USB3 NoIOT offset
		Int2BytesArray(isp_headpage_NoIOT.Extra_USB3.Offset, sizeof(isp_headpage_NoIOT.Extra_USB3.Offset), NoIOT_offset_pointer);

		//message.Format(_T("Extra USB3 offset:0x%x\r\n"), isp_headpage_NoIOT.Extra_USB3.Offset._dword);
		message.Format(_T("Extra USB3 IOT offset:0x%02x%02x%02x%02x\r\n"), isp_headpage_NoIOT.Extra_USB3.Offset[3], isp_headpage_NoIOT.Extra_USB3.Offset[2], isp_headpage_NoIOT.Extra_USB3.Offset[1], isp_headpage_NoIOT.Extra_USB3.Offset[0]);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		//offset_pointer = offset_pointer + isp_headpage_NoIOT.Extra_USB3.Length._dword + tag_length;
		NoIOT_offset_pointer = NoIOT_offset_pointer + BytesArray2Int(isp_headpage_NoIOT.Extra_USB3.Length, sizeof(isp_headpage_NoIOT.Extra_USB3.Length)) + tag_length;

		//isp_headpage_NoIOT.Extra_DP.Offset._dword = offset_pointer;	//Extra DP offset
		Int2BytesArray(isp_headpage_NoIOT.Extra_DP.Offset, sizeof(isp_headpage_NoIOT.Extra_DP.Offset), NoIOT_offset_pointer);

		//message.Format(_T("Extra DP offset:0x%x\r\n"), isp_headpage_NoIOT.Extra_DP.Offset._dword);
		message.Format(_T("Extra DP offset:0x%02x%02x%02x%02x\r\n"), isp_headpage_NoIOT.Extra_DP.Offset[3], isp_headpage_NoIOT.Extra_DP.Offset[2], isp_headpage_NoIOT.Extra_DP.Offset[1], isp_headpage_NoIOT.Extra_DP.Offset[0]);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

		//offset_pointer = offset_pointer + isp_headpage_NoIOT.Extra_DP.Length._dword + tag_length;
		NoIOT_offset_pointer = NoIOT_offset_pointer + BytesArray2Int(isp_headpage_NoIOT.Extra_DP.Length, sizeof(isp_headpage_NoIOT.Extra_DP.Length)) + tag_length;
		//**********************

		//Victor
		//TBD here

		isp_headpage_NoIOT.isp_offset.LVN_Security._dword = NoIOT_offset_pointer;	//LNV Security offset

		message.Format(_T("Security offset:0x%x\r\n"), isp_headpage_NoIOT.isp_offset.LVN_Security._dword);
		user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
		user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
#endif
}

WORD CUserBinMergeToolDlg::Make_CheckSum(CString TargetFile_Input) {
	//char* temp_buff;
	char temp_char;
	//DWORD temp_checksum = 0;

	int file_size = 0;
	file_size = user_Get_File_Size(TargetFile_Input);

	DWORD m_BytesFile;

	/*
	fstream m_temp;
	m_temp.open(TargetFile_Input, ios::binary | ios::in);

	//temp_buff = new char[file_size];
	//memset(temp_buff, 0, sizeof(temp_buff));

	for (int i = 0; i < 10; i++)
	{
		m_temp.get(temp_char);
		temp_checksum = temp_checksum + (unsigned)temp_char;
		//temp_checksum = temp_checksum + temp_char;
	}

	m_temp.close();
	*/

	HANDLE hISPFile = ::CreateFile(TargetFile_Input, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hISPFile == INVALID_HANDLE_VALUE) {
		//Display_Info::SendEvent(_T("Open file fail!"));
		CloseHandle(hISPFile);
		return 0;
	}
	m_BytesFile = GetFileSize(hISPFile, NULL);
	//	m_strLog.Format(_T("Filea open ok. size=%d BYTES"),	nBytesFile);
	//	Display_Info::SendEvent(m_strLog);

	//m_strLog.Format(_T("EJ898C Bin file size: %d"), m_BytesFile);
	//Display_Info::SendEvent(m_strLog);
	SetFilePointer(hISPFile, 0, NULL, 0);
	BYTE tempbuf[512];
	int bResult;
	DWORD nBytesRead = 0;

	//m_BytesFile = 10;

	WORD checksum = 0;
	while (m_BytesFile) {
		bResult = ReadFile(hISPFile, tempbuf, 512, &nBytesRead, NULL);
		if (bResult) {
			for (DWORD i = 0; i < nBytesRead; i++) {
				checksum += tempbuf[i];
			}
			//if (nBytesRead != 512) {
			//	Display_Info::SendEvent(_T("last rount!"));
			//}
		}
		//else {
		//	break;
		//}
		//check1++;
		if (m_BytesFile > 512) {
			m_BytesFile -= 512;
			//point += 512;
			//SetFilePointer(hISPFile, point, NULL, 0);
		}
		else {
			m_BytesFile = 0;
		}
		//m_strLog.Format(_T("%07d checksum: %08X"), check1, checksum);
		//Display_Info::SendEvent(m_strLog);

	}
	CloseHandle(hISPFile);

	CString message;
	//message.Format(_T("Checksum value:0x%04x\r\n"), temp_checksum);
	//message.Format(_T("Checksum value:0x%04x\r\n"), checksum);
	//user_DisplayInfo.AppendToLogAndScroll(message, RED);

	//temp_checksum = Private_key ^ temp_checksum;
	//checksum ^= Private_key_1;	//XOR private key.

	return checksum;
}

DWORD CUserBinMergeToolDlg::Make_Security_CheckSum(CString TargetFile_Input) {
	//char* temp_buff;
	char temp_char;
	//DWORD temp_checksum = 0;

	int file_size = 0;
	file_size = user_Get_File_Size(TargetFile_Input);

	DWORD m_BytesFile;

	/*
	fstream m_temp;
	m_temp.open(TargetFile_Input, ios::binary | ios::in);

	//temp_buff = new char[file_size];
	//memset(temp_buff, 0, sizeof(temp_buff));

	for (int i = 0; i < 10; i++)
	{
		m_temp.get(temp_char);
		temp_checksum = temp_checksum + (unsigned)temp_char;
		//temp_checksum = temp_checksum + temp_char;
	}

	m_temp.close();
	*/

	HANDLE hISPFile = ::CreateFile(TargetFile_Input, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hISPFile == INVALID_HANDLE_VALUE) {
		//Display_Info::SendEvent(_T("Open file fail!"));
		CloseHandle(hISPFile);
		return 0;
	}
	m_BytesFile = GetFileSize(hISPFile, NULL);
	//	m_strLog.Format(_T("Filea open ok. size=%d BYTES"),	nBytesFile);
	//	Display_Info::SendEvent(m_strLog);

	//m_strLog.Format(_T("EJ898C Bin file size: %d"), m_BytesFile);
	//Display_Info::SendEvent(m_strLog);
	SetFilePointer(hISPFile, 0, NULL, 0);
	BYTE tempbuf[512];
	int bResult;
	DWORD nBytesRead = 0;

	//m_BytesFile = 10;

	DWORD checksum = 0;
	while (m_BytesFile) {
		bResult = ReadFile(hISPFile, tempbuf, 512, &nBytesRead, NULL);
		if (bResult) {
			for (DWORD i = 0; i < nBytesRead; i++) {
				checksum += tempbuf[i];
			}
			//if (nBytesRead != 512) {
			//	Display_Info::SendEvent(_T("last rount!"));
			//}
		}
		//else {
		//	break;
		//}
		//check1++;
		if (m_BytesFile > 512) {
			m_BytesFile -= 512;
			//point += 512;
			//SetFilePointer(hISPFile, point, NULL, 0);
		}
		else {
			m_BytesFile = 0;
		}
		//m_strLog.Format(_T("%07d checksum: %08X"), check1, checksum);
		//Display_Info::SendEvent(m_strLog);

	}
	CloseHandle(hISPFile);

	CString message;
	//message.Format(_T("Checksum value:0x%04x\r\n"), temp_checksum);
	message.Format(_T("Checksum value:0x%04x\r\n"), checksum);
	user_DisplayInfo.AppendToLogAndScroll(message, RED);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

	DWORD temp_value = checksum;

	//temp_checksum = Private_key ^ temp_checksum;
	checksum ^= MCU_Key;	//XOR private key.
	DWORD temp = (APP_Key - constant_CC) / 2;
	new_gb_Check_sum._dword = (APP_Key - constant_CC) / 2 + temp_value;	//新的演算法

	return checksum;
}

DWORD CUserBinMergeToolDlg::Create_CheckSum_File(void) 
{
	char CheckSum_buff[CheckFile_Size];
	BYTE temp_rand_number;
	srand(time(NULL));

	

	for (int i = 0; i < CheckFile_Size; i++) {
		temp_rand_number = (rand() % 0xff) + 1;
		CheckSum_buff[i] = temp_rand_number;
	}

	m_ChecksumFile_Path.Format(_T("%s\\checksum.bin"), m_IspOriginalPath.GetString());

	if (PathFileExists(m_ChecksumFile_Path)) {
		DeleteFile(m_ChecksumFile_Path);
	}

	CheckSum_buff[0] = gb_Check_sum._byte[3];
	CheckSum_buff[1] = gb_Check_sum._byte[2];
	CheckSum_buff[510] = gb_Check_sum._byte[1];
	CheckSum_buff[511] = gb_Check_sum._byte[0];

	//Add new Check sum value, if head file Addr 0x100 is value 0x05
	CheckSum_buff[256] = new_gb_Check_sum._byte[3];
	CheckSum_buff[257] = new_gb_Check_sum._byte[2];
	CheckSum_buff[258] = new_gb_Check_sum._byte[1];
	CheckSum_buff[259] = new_gb_Check_sum._byte[0];


	user_CreateFile(m_ChecksumFile_Path, CheckSum_buff, CheckFile_Size);

	return 0;


}

void CUserBinMergeToolDlg::OnBnClickedButtonBinToExe()
{
	if (!PathFileExists(m_Combine_path_IOT)) {
		AfxMessageBox(_T("No seleted IOT bin file."), MB_ICONEXCLAMATION);
		return;
	}

	if (!PathFileExists(m_Combine_path_NoIOT)) {
		AfxMessageBox(_T("No seleted No IOT bin file."), MB_ICONEXCLAMATION);
		return;
	}

	if (!PathFileExists(m_PathFile_Exe)) {
		AfxMessageBox(_T("No seleted exe file."), MB_ICONEXCLAMATION);
		return;
	}

	if (!PathFileExists(m_CodeVer_Path)) {
		AfxMessageBox(_T("No CodeVer.txt file."), MB_ICONEXCLAMATION);
		return;
	}

	GetDlgItem(IDC_BUTTON_BIN_TO_EXE)->EnableWindow(false);

	CString m_pathname, m_BinSum_File;


	CFileDialog fdlg(false, CString(_T(".exe")), m_IspOriginalPath + _T("\\"), NULL, _T("Exe Files|*.exe||"));	//建立單選型檔案對話盒物件
	if (fdlg.DoModal() != IDOK) {
		GetDlgItem(IDC_BUTTON_BIN_TO_EXE)->EnableWindow(true);
		return;
	}
	m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

	if (PathFileExists(m_pathname))
	{
		DeleteFile(m_pathname);
	}
	
	CString message;
	message.Format(_T("%s"), m_pathname);
	int Check_Result = message.ReverseFind(_T('\\'));
	message = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	user_log_save.m_FileName_Execution_Name = message;

	message.Format(_T("%s"), m_Combine_path_IOT);
	Check_Result = message.ReverseFind(_T('\\'));
	message = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	user_log_save.m_FileName_MergedFile_IOT = message;


	message.Format(_T("%s"), m_Combine_path_NoIOT);
	Check_Result = message.ReverseFind(_T('\\'));
	message = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	user_log_save.m_FileName_MergedFile_NoIOT = message;

	user_CombineFile(m_PathFile_Exe, m_pathname);
	
	user_CombineFile(m_CodeVer_Path, m_pathname);

	user_CombineFile(m_Combine_path_IOT, m_pathname);	//IOT

	user_CombineFile(m_CodeVer_Path, m_pathname);

	user_CombineFile(m_Combine_path_NoIOT, m_pathname);	//No IOT

	user_CombineFile(m_CodeVer_Path, m_pathname);


	//另外建立Summary Bin，不包含EXE
	//Check_Result = m_pathname.Find(_T(".exe"), 0);
	m_BinSum_File = m_pathname.Mid(0, m_pathname.GetLength() - 4);	//去掉尾巴".exe"，四個字
	m_BinSum_File += _T(".bin");

	user_CombineFile(m_CodeVer_Path, m_BinSum_File);

	user_CombineFile(m_Combine_path_IOT, m_BinSum_File);	//IOT

	user_CombineFile(m_CodeVer_Path, m_BinSum_File);

	user_CombineFile(m_Combine_path_NoIOT, m_BinSum_File);	//No IOT

	user_CombineFile(m_CodeVer_Path, m_BinSum_File);


	user_log_save.doSaveCSVLog_MergedEXE(_T("Combined Execution History"));

	AfxMessageBox(_T("Build OK!"), MB_ICONEXCLAMATION);

	GetDlgItem(IDC_BUTTON_BIN_TO_EXE)->EnableWindow(true);
}


void CUserBinMergeToolDlg::OnBnClickedButtonPathCombinedBin()
{
	CString m_pathname;
	CString message;

	CFileDialog fdlg(false, CString(_T(".bin")), m_IspOriginalPath + _T("\\"), NULL, _T("Bin Files|*.bin||"));	//建立單選型檔案對話盒物件
	if (fdlg.DoModal() != IDOK) {
		return;
	}
	m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

	m_Combine_path_IOT = m_pathname;
	user_DisplayInfo.AppendToLogAndScroll(_T("\r\n") + m_Combine_path_IOT, ORAGE);

	message.Format(_T("%s"), m_pathname);
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_COMBINED_BIN_IOT)->SetWindowText(file_name.GetString());


}

void CUserBinMergeToolDlg::OnBnClickedButtonPathCombinedBinNoiot()
{
	CString m_pathname;
	CString message;

	CFileDialog fdlg(false, CString(_T(".bin")), m_IspOriginalPath + _T("\\"), NULL, _T("Bin Files|*.bin||"));	//建立單選型檔案對話盒物件
	if (fdlg.DoModal() != IDOK) {
		return;
	}
	m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

	m_Combine_path_NoIOT = m_pathname;
	user_DisplayInfo.AppendToLogAndScroll(_T("\r\n") + m_Combine_path_NoIOT, GREEN);

	message.Format(_T("%s"), m_pathname);
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_COMBINED_BIN_NOIOT)->SetWindowText(file_name.GetString());

}

DWORD CUserBinMergeToolDlg::Reduction_File(int chip_num, CString file_path_input) {

	CString message;
	message.Format(_T("%s"), file_path_input.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));
	CString file_original_path = message.Mid(0, Check_Result);



	DWORD reducted_lengh = 0;

	int file_size = 0;
	file_size = user_Get_File_Size(file_path_input);

	fstream m_temp;
	m_temp.open(file_path_input, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));

	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	BOOL break_flag = 0;
	int block_cnt = 0;

	for (int i = (file_size-1); i > 0; i = i - 512)	//self define one block = 512
	{

		for (int j = i - 512; j < i; j++)
		{
			if ((BYTE)temp_buff[j] != 0xff)
			{
				break_flag = true;
				break;
			}
		}

		if (break_flag != 0)
		{

			break;
		}
		block_cnt++;

	}

	reducted_lengh = file_size - (block_cnt * 512);

	CreateFolder(file_original_path + _T("\\Reduction_temp\\"));

	if (PathFileExists(file_original_path + _T("\\Reduction_temp\\") + file_name))
	{
		DeleteFile(file_original_path + _T("\\Reduction_temp\\") + file_name);
	}

	user_CreateFile(file_original_path + _T("\\Reduction_temp\\") + file_name, temp_buff, reducted_lengh);

	m_Reduction_PathFile[chip_num] = file_original_path + _T("\\Reduction_temp\\") + file_name;
	
	//m_Head_Reduction[chip_num].lenth.integer = reducted_lengh;		//暫時關掉，後續要開

	m_temp.close();
	delete[] temp_buff;

	return reducted_lengh;
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathCodeverTag()
{
	CString m_pathname;

	CFileDialog fdlg(false, CString(_T(".txt")), m_IspOriginalPath + _T("\\"), NULL, _T("Txt Files|*.txt||"));	//建立單選型檔案對話盒物件
	if (fdlg.DoModal() != IDOK) {
		return;
	}
	m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

	m_CodeVer_Path = m_pathname;

	CString message;
	message.Format(_T("%s"), m_pathname);
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_CODEVER_TAG)->SetWindowText(file_name.GetString());






}


void CUserBinMergeToolDlg::OnBnClickedButtonPathSecurity()
{
	Initial_PathSecurity(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_PathSecurity(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_Security = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_SECURITY)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_Security))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_Security.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_SECURITY)->SetWindowText(file_name);

	user_log_save.m_FileName_Security.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_Security);

	isp_headpage.isp_length.LVN_Security._dword = file_size;
	isp_headpage_NoIOT.isp_length.LVN_Security._dword = isp_headpage.isp_length.LVN_Security._dword;


	message.Format(_T("Security length:0x%x\r\n"), isp_headpage.isp_length.LVN_Security._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

	isp_headpage.isp_checksum.LVN_Security._word = Make_CheckSum(m_PathFile_Security);
	isp_headpage_NoIOT.isp_checksum.LVN_Security._word = isp_headpage.isp_checksum.LVN_Security._word;
	message.Format(_T("LNV Security checksum:0x%x\r\n"), isp_headpage.isp_checksum.LVN_Security._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

}

CString CUserBinMergeToolDlg::SetRelative_Path(CString path)
{
	CString message;

	message.Format(_T("%s"), path.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	message.Format(_T("%s\\%s"), m_IspOriginalPath, file_name);

	return message;
}

void CUserBinMergeToolDlg::OnEnChangeEditHeadVersion()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CDialogEx::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。

	// TODO:  在此加入控制項告知處理常式程式碼
}

void CUserBinMergeToolDlg::Eding_Swap(BYTE* addr, BYTE length) {
	BYTE temp;

	switch (length)
	{
	case 2:
		temp = *addr;
		*addr = *(addr + 1);
		*(addr + 1) = temp;
		break;
	case 4:
		temp = *addr;
		*addr = *(addr + 3);
		*(addr + 3) = temp;

		temp = *(addr + 1);
		*(addr+1) = *(addr + 2);
		*(addr + 2) = temp;
		break;
	case 5:
		temp = *addr;
		*addr = *(addr + 4);
		*(addr + 4) = temp;

		temp = *(addr + 1);
		*(addr + 1) = *(addr + 3);
		*(addr + 3) = temp;
		break;


	}


}





void CUserBinMergeToolDlg::OnBnClickedButtonPathUsb3HubNoiot()
{
	Initial_USB3_NoIOT_Value(Buttom_Selection);
}
void CUserBinMergeToolDlg::Initial_USB3_NoIOT_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_USB_NoIOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_USB3_NoIOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_USB_NoIOT))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_USB_NoIOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_USB3_NoIOT)->SetWindowText(file_name);

	user_log_save.m_FileName_USB3_NoIOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_USB_NoIOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	isp_headpage_NoIOT.isp_length.USB3._dword = file_size;

	message.Format(_T("USB_Hub1 length:0x%x\r\n"), isp_headpage_NoIOT.isp_length.USB3._dword);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_USB_NoIOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	for (int i = 0; i < file_size; i++)
	{
		int j = 0;
		if ((BYTE)temp_buff[i] == 0xEF) {
			j = i + 1;
			if ((BYTE)temp_buff[j] == 0x17) {
				j++;
				if ((BYTE)temp_buff[j] == 0xB6) {
					j++;
					if ((BYTE)temp_buff[j] == 0x30) {
						j++;

						isp_headpage_NoIOT.isp_version_in_mcu.USB3[0] = temp_buff[j] + 3;	//Alan,20201228, restruct version format
						j++;

						isp_headpage_NoIOT.isp_version_in_mcu.USB3[1] = temp_buff[j];	//Alan,20201228, restruct version format
						break;
					}
				}


			}
		}

	}

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%02x%02x"), isp_headpage_NoIOT.isp_version_in_mcu.USB3[1], isp_headpage_NoIOT.isp_version_in_mcu.USB3[0]);
	GetDlgItem(IDC_EDIT_VSERION_USB3_NoIOT)->SetWindowText(message);


	isp_headpage_NoIOT.isp_checksum.USB3._word = Make_CheckSum(m_PathFile_USB_NoIOT);
	message.Format(_T("USB3.0 checksum:0x%x\r\n"), isp_headpage_NoIOT.isp_checksum.USB3._word);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathGdMcu()
{
	Initial_GD_MCU_Value(Buttom_Selection);
}
void CUserBinMergeToolDlg::Initial_GD_MCU_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_GD_MCU = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_GD_MCU)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_GD_MCU))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_GD_MCU);
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_GD_MCU)->SetWindowText(file_name);

	user_log_save.m_FileName_GD_MCU.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_GD_MCU);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//isp_headpage.GD_MCU.Length._dword = file_size;
	Int2BytesArray(isp_headpage.GD_MCU.Length, sizeof(isp_headpage.GD_MCU.Length), file_size);
	//isp_headpage_NoIOT.GD_MCU.Length._dword = file_size;
	Int2BytesArray(isp_headpage_NoIOT.GD_MCU.Length, sizeof(isp_headpage_NoIOT.GD_MCU.Length), file_size);

	//message.Format(_T("GD MCU length:0x%x\r\n"), isp_headpage.GD_MCU.Length._dword);
	message.Format(_T("GD MCU length:0x%02x%02x%02x%02x\r\n"), isp_headpage.GD_MCU.Length[3], isp_headpage.GD_MCU.Length[2], isp_headpage.GD_MCU.Length[1], isp_headpage.GD_MCU.Length[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_GD_MCU, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	//isp_headpage.isp_version_in_mcu.MCU[1] = temp_buff[0x1000];	//Alan,20201228, restruct version format
	//isp_headpage.isp_version_in_mcu.MCU[0] = temp_buff[0x1001];

	//isp_headpage_NoIOT.isp_version_in_mcu.MCU[1] = temp_buff[0x1000];	//Alan,20210317
	//isp_headpage_NoIOT.isp_version_in_mcu.MCU[0] = temp_buff[0x1001];

	isp_headpage.GD_MCU.Version[1] = temp_buff[0x14f00];	//Alan,20201228, restruct version format
	isp_headpage.GD_MCU.Version[0] = temp_buff[0x14f01];

	isp_headpage_NoIOT.GD_MCU.Version[1] = temp_buff[0x14f00];	//Alan,20210317
	isp_headpage_NoIOT.GD_MCU.Version[0] = temp_buff[0x14f01];

	m_temp.close();
	delete[] temp_buff;
#endif


	message.Format(_T("%02x%02x"), isp_headpage.GD_MCU.Version[1], isp_headpage.GD_MCU.Version[0]);
	GetDlgItem(IDC_EDIT_VSERION_GD_MCU)->SetWindowText(message);

	//message.Format(_T("%dkB"), (file_size / 1024));
	//GetDlgItem(IDC_STATIC_MCU_LENGTH_ORIGINAL)->SetWindowText(message);
	//message.Format(_T("%dkB"), (Reduction_File(Chip_MCU, m_PathFile_MCU) / 1024));
	//GetDlgItem(IDC_STATIC_MCU_LENGTH_REDUCTION)->SetWindowText(message);

	//isp_headpage.GD_MCU.Checksum._word = Make_CheckSum(m_PathFile_GD_MCU);
	Int2BytesArray(isp_headpage.GD_MCU.Checksum, sizeof(isp_headpage.GD_MCU.Checksum), Make_CheckSum(m_PathFile_GD_MCU));
	//isp_headpage_NoIOT.GD_MCU.Checksum._word = isp_headpage.GD_MCU.Checksum._word;
	Int2BytesArray(isp_headpage_NoIOT.GD_MCU.Checksum, sizeof(isp_headpage_NoIOT.GD_MCU.Checksum), Make_CheckSum(m_PathFile_GD_MCU));
	message.Format(_T("GD MCU checksum:0x%02x%02x\r\n"), isp_headpage.GD_MCU.Checksum[1], isp_headpage.GD_MCU.Checksum[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathExtraUsb3Iot()
{
	
	Initial_Extra_USB3_IOT_Value(Buttom_Selection);

}

void CUserBinMergeToolDlg::Initial_Extra_USB3_IOT_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_Extra_USB_IOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_EXTRA_USB3_IOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_Extra_USB_IOT))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_Extra_USB_IOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_EXTRA_USB3_IOT)->SetWindowText(file_name);

	user_log_save.m_FileName_Extra_USB3_IOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_Extra_USB_IOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//isp_headpage.Extra_USB3.Length._dword = file_size;
	Int2BytesArray(isp_headpage.Extra_USB3.Length, sizeof(isp_headpage.Extra_USB3.Length), file_size);

	//message.Format(_T("Extra USB_Hub IOT length:0x%x\r\n"), isp_headpage.Extra_USB3.Length._dword);
	message.Format(_T("Extra USB_Hub IOT length:0x%02x%02x%02x%02x\r\n"), isp_headpage.Extra_USB3.Length[3], isp_headpage.Extra_USB3.Length[2], isp_headpage.Extra_USB3.Length[1], isp_headpage.Extra_USB3.Length[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_Extra_USB_IOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	for (int i = 0; i < file_size; i++)
	{
		int j = 0;
		if ((BYTE)temp_buff[i] == 0xEF) {
			j = i + 1;
			if ((BYTE)temp_buff[j] == 0x17) {
				j++;
				if ((BYTE)temp_buff[j] == 0xB6) {
					j++;
					if ((BYTE)temp_buff[j] == 0x30) {
						j++;

						//isp_headpage.isp_version_in_mcu.USB3[0] = temp_buff[j] + 3;	//Alan,20201228, restruct version format
						isp_headpage.Extra_USB3.Version[0] = temp_buff[j] + 3;	//Alan,20201228, restruct version format
						j++;

						//isp_headpage.isp_version_in_mcu.USB3[1] = temp_buff[j];	//Alan,20201228, restruct version format
						isp_headpage.Extra_USB3.Version[1] = temp_buff[j];
						break;
					}
				}


			}
		}

	}

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%02x%02x"), isp_headpage.Extra_USB3.Version[1], isp_headpage.Extra_USB3.Version[0]);
	GetDlgItem(IDC_EDIT_VSERION_EXTRA_USB3_IOT)->SetWindowText(message);

	//isp_headpage.Extra_USB3.Checksum._word = Make_CheckSum(m_PathFile_Extra_USB_IOT);
	Int2BytesArray(isp_headpage.Extra_USB3.Checksum, sizeof(isp_headpage.Extra_USB3.Checksum), Make_CheckSum(m_PathFile_Extra_USB_IOT));
	message.Format(_T("Extra USB3.0 IOT checksum:0x%02x%02x\r\n"), isp_headpage.Extra_USB3.Checksum[1], isp_headpage.Extra_USB3.Checksum[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}


void CUserBinMergeToolDlg::OnBnClickedButtonPathExtraUsb3Noiot()
{
	Initial_Extra_USB3_NoIOT_Value(Buttom_Selection);
}

void CUserBinMergeToolDlg::Initial_Extra_USB3_NoIOT_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_Extra_USB_NoIOT = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_EXTRA_USB3_NoIOT)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_Extra_USB_NoIOT))
		{
			return;
		}
	}

	CString message;
	message.Format(_T("%s"), m_PathFile_Extra_USB_NoIOT.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_EXTRA_USB3_NoIOT)->SetWindowText(file_name);

	user_log_save.m_FileName_Extra_USB3_NoIOT.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_Extra_USB_NoIOT);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//isp_headpage_NoIOT.Extra_USB3.Length._dword = file_size;
	Int2BytesArray(isp_headpage_NoIOT.Extra_USB3.Length, sizeof(isp_headpage_NoIOT.Extra_USB3.Length), file_size);

	//message.Format(_T("Extra USB_Hub No IOT length:0x%x\r\n"), isp_headpage_NoIOT.Extra_USB3.Length._dword);
	message.Format(_T("Extra USB_Hub IOT length:0x%02x%02x%02x%02x\r\n"), isp_headpage_NoIOT.Extra_USB3.Length[3], isp_headpage_NoIOT.Extra_USB3.Length[2], isp_headpage_NoIOT.Extra_USB3.Length[1], isp_headpage_NoIOT.Extra_USB3.Length[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_Extra_USB_NoIOT, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	for (int i = 0; i < file_size; i++)
	{
		int j = 0;
		if ((BYTE)temp_buff[i] == 0xEF) {
			j = i + 1;
			if ((BYTE)temp_buff[j] == 0x17) {
				j++;
				if ((BYTE)temp_buff[j] == 0xB6) {
					j++;
					if ((BYTE)temp_buff[j] == 0x30) {
						j++;


						isp_headpage_NoIOT.Extra_USB3.Version[0] = temp_buff[j] + 3;	//Alan,20201228, restruct version format
						j++;

						isp_headpage_NoIOT.Extra_USB3.Version[1] = temp_buff[j];
						break;
					}
				}


			}
		}

	}

	m_temp.close();
	delete[] temp_buff;
#endif

	message.Format(_T("%02x%02x"), isp_headpage_NoIOT.Extra_USB3.Version[1], isp_headpage_NoIOT.Extra_USB3.Version[0]);
	GetDlgItem(IDC_EDIT_VSERION_EXTRA_USB3_NoIOT)->SetWindowText(message);

	//isp_headpage_NoIOT.Extra_USB3.Checksum._word = Make_CheckSum(m_PathFile_Extra_USB_NoIOT);
	Int2BytesArray(isp_headpage_NoIOT.Extra_USB3.Checksum, sizeof(isp_headpage_NoIOT.Extra_USB3.Checksum), Make_CheckSum(m_PathFile_Extra_USB_NoIOT));
	message.Format(_T("Extra USB3.0 No IOT checksum:0x%02x%02x\r\n"), isp_headpage_NoIOT.Extra_USB3.Checksum[1], isp_headpage_NoIOT.Extra_USB3.Checksum[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

void CUserBinMergeToolDlg::OnBnClickedButtonPathExtraDp()
{
	Initial_Extra_DP_Value(Buttom_Selection);
}
void CUserBinMergeToolDlg::Initial_Extra_DP_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_Extra_DP = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_EXTRA_DP)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_Extra_DP))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_Extra_DP.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_EXTRA_DP)->SetWindowText(file_name);

	user_log_save.m_FileName_Extra_DP.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_Extra_DP);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//isp_headpage.Extra_DP.Length._dword = file_size;	//兩顆chip，放一個就好
	Int2BytesArray(isp_headpage.Extra_DP.Length, sizeof(isp_headpage.Extra_DP.Length), file_size);
	//isp_headpage_NoIOT.Extra_DP.Length._dword = file_size;	//兩顆chip，放一個就好
	Int2BytesArray(isp_headpage_NoIOT.Extra_DP.Length, sizeof(isp_headpage_NoIOT.Extra_DP.Length), file_size);

	//message.Format(_T("Extra DP length:0x%x\r\n"), isp_headpage.Extra_DP.Length._dword);
	message.Format(_T("Extra DP length:0x%02x%02x%02x%02x\r\n"), isp_headpage.Extra_DP.Length[3], isp_headpage.Extra_DP.Length[2], isp_headpage.Extra_DP.Length[1], isp_headpage.Extra_DP.Length[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_Extra_DP, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

	if (temp_buff[0x10D] != 0)		//Check 5xxx test version or not
	{

		//isp_headpage.isp_version_in_mcu.DP5x[0] = temp_buff[0x10D];	//Alan,20201228, restruct version format
		//isp_headpage.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0] = temp_buff[0x10D];	//Alan,20201228, restruct version format
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		isp_headpage.Extra_DP.Version_DP5[0] = temp_buff[0x10D];	//Alan,20220207, restruct version format
		isp_headpage.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage.Extra_DP.Version_DP5[2] = temp_buff[0x18400];

		isp_headpage_NoIOT.Extra_DP.Version_DP5[0] = temp_buff[0x10D];	//Alan,20220207, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage_NoIOT.Extra_DP.Version_DP5[2] = temp_buff[0x18400];

	}
	else
	{

		//isp_headpage.isp_version_in_mcu.DP5x[0] = temp_buff[0x18402];	//Alan,20201228, restruct version format
		//isp_headpage.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0] = temp_buff[0x18402];	//Alan,20201228, restruct version format
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		isp_headpage.Extra_DP.Version_DP5[0] = temp_buff[0x18402];	//Alan,20220207, restruct version format
		isp_headpage.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage.Extra_DP.Version_DP5[2] = temp_buff[0x18400];

		isp_headpage_NoIOT.Extra_DP.Version_DP5[0] = temp_buff[0x18402];	//Alan,20220207, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage_NoIOT.Extra_DP.Version_DP5[2] = temp_buff[0x18400];
	}

	if (temp_buff[0x10020D] != 0)		//Check 6xxx test version or not
	{
		isp_headpage.Extra_DP.Version_DP6[0] = temp_buff[0x10020D];	//Alan,20201228, restruct version format
		isp_headpage.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage.Extra_DP.Version_DP6[2] = temp_buff[0x154000];

		isp_headpage_NoIOT.Extra_DP.Version_DP6[0] = temp_buff[0x10020D];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage_NoIOT.Extra_DP.Version_DP6[2] = temp_buff[0x154000];
	}
	else
	{

		isp_headpage.Extra_DP.Version_DP6[0] = temp_buff[0x154002];	//Alan,20201228, restruct version format
		isp_headpage.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage.Extra_DP.Version_DP6[2] = temp_buff[0x154000];

		isp_headpage_NoIOT.Extra_DP.Version_DP6[0] = temp_buff[0x154002];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage_NoIOT.Extra_DP.Version_DP6[2] = temp_buff[0x154000];
	}

	m_temp.close();
	delete[] temp_buff;
#endif
	message.Format(_T("%02d%02d%03d"), isp_headpage.Extra_DP.Version_DP5[2], isp_headpage.Extra_DP.Version_DP5[1], isp_headpage.Extra_DP.Version_DP5[0]);

	GetDlgItem(IDC_EDIT_VSERION_EXTRA_DP_5XXX)->SetWindowText(message);


	message.Format(_T("%02d%02d%03d"), isp_headpage.Extra_DP.Version_DP6[2], isp_headpage.Extra_DP.Version_DP6[1], isp_headpage.Extra_DP.Version_DP6[0]);
	GetDlgItem(IDC_EDIT_VSERION_EXTRA_DP_6XXX)->SetWindowText(message);


	//isp_headpage.Extra_DP.Checksum._word = Make_CheckSum(m_PathFile_Extra_DP);
	Int2BytesArray(isp_headpage.Extra_DP.Checksum, sizeof(isp_headpage.Extra_DP.Checksum), Make_CheckSum(m_PathFile_Extra_DP));
	//isp_headpage_NoIOT.Extra_DP.Checksum._word = isp_headpage.Extra_DP.Checksum._word;
	Int2BytesArray(isp_headpage_NoIOT.Extra_DP.Checksum, sizeof(isp_headpage_NoIOT.Extra_DP.Checksum), Make_CheckSum(m_PathFile_Extra_DP));
	message.Format(_T("Extra DP checksum:0x%02x%02x\r\n"), isp_headpage.Extra_DP.Checksum[1], isp_headpage.Extra_DP.Checksum[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}

//Victor 20220520, TBD
void CUserBinMergeToolDlg::OnBnClickedButtonPathRTKMST()
{
	Initial_RTK_MST_Value(Buttom_Selection);
}
void CUserBinMergeToolDlg::Initial_RTK_MST_Value(BYTE action)
{
	if (action == Buttom_Selection)
	{
		CString m_pathname;
		//CString Ini_File_Path = _T("\\Test_Case\\");

		CFileDialog fdlg(true, NULL, m_IspOriginalPath + _T("\\"), NULL, _T(""));	//建立單選型檔案對話盒物件
		if (fdlg.DoModal() != IDOK) {
			return;
		}
		m_pathname = fdlg.GetPathName(); //取得被選取的檔案名稱與路徑

		m_PathFile_RTK_MST = m_pathname;

		GetDlgItem(IDC_EDIT_PATH_EXTRA_DP)->EnableWindow(false);
	}
	else if (action == Initial_Selection)
	{
		if (!PathFileExists(m_PathFile_RTK_MST))
		{
			return;
		}
	}



	CString message;
	message.Format(_T("%s"), m_PathFile_RTK_MST.GetString());
	int Check_Result = message.ReverseFind(_T('\\'));
	CString file_name = message.Mid(Check_Result + 1, message.GetLength() - (Check_Result + 1));

	GetDlgItem(IDC_EDIT_PATH_RTK_MST)->SetWindowText(file_name);

	user_log_save.m_FileName_RTK_MST.Format(_T("%s"), file_name);

	int file_size = 0;
	file_size = user_Get_File_Size(m_PathFile_RTK_MST);
	if (!file_size)		//if file size = 0, return
	{
		return;
	}

	//isp_headpage.Extra_DP.Length._dword = file_size;	//兩顆chip，放一個就好
	Int2BytesArray(isp_headpage.RTK_MST.Length, sizeof(isp_headpage.RTK_MST.Length), file_size);
	//isp_headpage_NoIOT.Extra_DP.Length._dword = file_size;	//兩顆chip，放一個就好
	Int2BytesArray(isp_headpage_NoIOT.RTK_MST.Length, sizeof(isp_headpage_NoIOT.RTK_MST.Length), file_size);

	//message.Format(_T("Extra DP length:0x%x\r\n"), isp_headpage.Extra_DP.Length._dword);
	message.Format(_T("RTK MST length:0x%02x%02x%02x%02x\r\n"), isp_headpage.RTK_MST.Length[3], isp_headpage.RTK_MST.Length[2], isp_headpage.RTK_MST.Length[1], isp_headpage.RTK_MST.Length[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);

#if 1

	fstream m_temp;
	m_temp.open(m_PathFile_RTK_MST, ios::binary | ios::in);

	char* temp_buff = new char[file_size];
	memset(temp_buff, 0, sizeof(temp_buff));



	for (int i = 0; i < file_size; i++)
	{
		m_temp.get(temp_buff[i]);
	}

#if 0 //Victor comment out
	if (temp_buff[0x10D] != 0)		//Check 5xxx test version or not
	{

		//isp_headpage.isp_version_in_mcu.DP5x[0] = temp_buff[0x10D];	//Alan,20201228, restruct version format
		//isp_headpage.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0] = temp_buff[0x10D];	//Alan,20201228, restruct version format
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		isp_headpage.Extra_DP.Version_DP5[0] = temp_buff[0x10D];	//Alan,20220207, restruct version format
		isp_headpage.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage.Extra_DP.Version_DP5[2] = temp_buff[0x18400];

		isp_headpage_NoIOT.Extra_DP.Version_DP5[0] = temp_buff[0x10D];	//Alan,20220207, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage_NoIOT.Extra_DP.Version_DP5[2] = temp_buff[0x18400];

	}
	else
	{

		//isp_headpage.isp_version_in_mcu.DP5x[0] = temp_buff[0x18402];	//Alan,20201228, restruct version format
		//isp_headpage.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[0] = temp_buff[0x18402];	//Alan,20201228, restruct version format
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[1] = temp_buff[0x18401];
		//isp_headpage_NoIOT.isp_version_in_mcu.DP5x[2] = temp_buff[0x18400];

		isp_headpage.Extra_DP.Version_DP5[0] = temp_buff[0x18402];	//Alan,20220207, restruct version format
		isp_headpage.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage.Extra_DP.Version_DP5[2] = temp_buff[0x18400];

		isp_headpage_NoIOT.Extra_DP.Version_DP5[0] = temp_buff[0x18402];	//Alan,20220207, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP5[1] = temp_buff[0x18401];
		isp_headpage_NoIOT.Extra_DP.Version_DP5[2] = temp_buff[0x18400];
	}

	if (temp_buff[0x10020D] != 0)		//Check 6xxx test version or not
	{
		isp_headpage.Extra_DP.Version_DP6[0] = temp_buff[0x10020D];	//Alan,20201228, restruct version format
		isp_headpage.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage.Extra_DP.Version_DP6[2] = temp_buff[0x154000];

		isp_headpage_NoIOT.Extra_DP.Version_DP6[0] = temp_buff[0x10020D];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage_NoIOT.Extra_DP.Version_DP6[2] = temp_buff[0x154000];
	}
	else
	{

		isp_headpage.Extra_DP.Version_DP6[0] = temp_buff[0x154002];	//Alan,20201228, restruct version format
		isp_headpage.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage.Extra_DP.Version_DP6[2] = temp_buff[0x154000];

		isp_headpage_NoIOT.Extra_DP.Version_DP6[0] = temp_buff[0x154002];	//Alan,20201228, restruct version format
		isp_headpage_NoIOT.Extra_DP.Version_DP6[1] = temp_buff[0x154001];
		isp_headpage_NoIOT.Extra_DP.Version_DP6[2] = temp_buff[0x154000];
	}
#endif

	m_temp.close();
	delete[] temp_buff;
#endif
	message.Format(_T("%02d%02d%03d"), isp_headpage.Extra_DP.Version_DP5[2], isp_headpage.Extra_DP.Version_DP5[1], isp_headpage.Extra_DP.Version_DP5[0]);

	GetDlgItem(IDC_EDIT_VSERION_EXTRA_DP_5XXX)->SetWindowText(message);


	message.Format(_T("%02d%02d%03d"), isp_headpage.Extra_DP.Version_DP6[2], isp_headpage.Extra_DP.Version_DP6[1], isp_headpage.Extra_DP.Version_DP6[0]);
	GetDlgItem(IDC_EDIT_VSERION_EXTRA_DP_6XXX)->SetWindowText(message);


	//isp_headpage.RTK_MST.Checksum._word = Make_CheckSum(m_PathFile_RTK_MST);
	Int2BytesArray(isp_headpage.RTK_MST.Checksum, sizeof(isp_headpage.RTK_MST.Checksum), Make_CheckSum(m_PathFile_RTK_MST));
	//isp_headpage_NoIOT.RTK_MST.Checksum._word = isp_headpage.RTK_MST.Checksum._word;
	Int2BytesArray(isp_headpage_NoIOT.RTK_MST.Checksum, sizeof(isp_headpage_NoIOT.RTK_MST.Checksum), Make_CheckSum(m_PathFile_RTK_MST));
	message.Format(_T("RTK_MST checksum:0x%02x%02x\r\n"), isp_headpage.RTK_MST.Checksum[1], isp_headpage.RTK_MST.Checksum[0]);
	user_DisplayInfo.AppendToLogAndScroll(message, BLUE);
	user_log.CreateFileTxt(m_IspOriginalPath, user_log.LogFileName, &message);
}