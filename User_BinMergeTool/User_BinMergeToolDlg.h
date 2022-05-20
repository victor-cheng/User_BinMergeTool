
// User_BinMergeToolDlg.h: 標頭檔
//

#pragma once
#include <fstream>
#include <iostream>
#include <time.h>

#include "user_DisplayInfo.h"
#include "CColorEdit.h"
#include "user_FileManager.h"

//typedef union {
//	unsigned _int16 int_16;
//	struct {
//		//----------------------------
//		unsigned char PC_00 : 1;
//		unsigned char PC_01 : 1;
//		unsigned char PC_02 : 1;
//		unsigned char PC_03 : 1;
//		unsigned char PC_04 : 1;
//		unsigned char PC_05 : 1;
//		unsigned char PC_06 : 1;
//		unsigned char PC_07 : 1;
//		unsigned char PC_08 : 1;
//		unsigned char PC_09 : 1;
//		unsigned char PC_10 : 1;
//		unsigned char PC_11 : 1;
//		unsigned char PC_12 : 1;
//		unsigned char PC_13 : 1;
//		unsigned char PC_14 : 1;
//		unsigned char PC_15 : 1;
//	}bits;
//} PC_distributor_t;

enum Chip_Number
{
	Chip_DMC,
	Chip_PD,
	Chip_TBT4,
	Chip_DP,
	Chip_USB_Hub1,
	Chip_U2_Hub,
	Chip_Audio,
	Chip_LAN_I225,
	Chip_MCU,
	File_Security,
};

enum Action_Selection
{
	Buttom_Selection,
	Initial_Selection,
};

typedef union {
	unsigned int integer = 0;
	unsigned char _byte[4];


}Union_length_t;

typedef union {
	unsigned int integer = 0;
	unsigned char _byte[6];

}Union_version_t;

typedef union {
	WORD _word = 0;
	byte _byte[2];


}Union_WORD_t;

struct _Head_struct
{
	//Union_length_t offset;
	//Union_length_t lenth;
	Union_version_t version_0;
	Union_version_t version_1;
	//Union_WORD_t checksum;
};

typedef union {
	DWORD _dword = 0;
	unsigned char _byte[4];

}Union_DWORD_t;

typedef struct _IspVersionInMcu_t {		//總共49Bytes
	BYTE DMC[5];
	BYTE PD[5];
	BYTE DP5x[5];
	BYTE DP6x[5];
	BYTE TBT4[5];
	BYTE USB3[5];
	BYTE USB2[5];
	BYTE AUDIO[5];
	BYTE I255[5];
	BYTE MCU[2];
	BYTE bcdVersion[2];
} IspVersionInMcu_t;

typedef struct _IspOffset_t {		//總共44Bytes
	Union_DWORD_t DMC;
	Union_DWORD_t PD;
	Union_DWORD_t DP5x;
	Union_DWORD_t DP6x;
	Union_DWORD_t TBT4;
	Union_DWORD_t USB3;
	Union_DWORD_t USB2;
	Union_DWORD_t AUDIO;
	Union_DWORD_t I255;
	Union_DWORD_t MCU;
	Union_DWORD_t LVN_Security;
} IspOffset_t;

typedef struct _IspLength_t {		//總共48Bytes
	Union_DWORD_t DMC;
	Union_DWORD_t PD;
	Union_DWORD_t DP5x;
	Union_DWORD_t DP6x;
	Union_DWORD_t TBT4;
	Union_DWORD_t USB3;
	Union_DWORD_t USB2;
	Union_DWORD_t AUDIO;
	Union_DWORD_t I255;
	Union_DWORD_t MCU;
	Union_DWORD_t LVN_Security;
	Union_DWORD_t Summary_Length;
} IspLength_t;

typedef struct _IspChecksum_t {		//總共22Bytes
	Union_WORD_t DMC;
	Union_WORD_t PD;
	Union_WORD_t DP5x;
	Union_WORD_t DP6x;
	Union_WORD_t TBT4;
	Union_WORD_t USB3;
	Union_WORD_t USB2;
	Union_WORD_t AUDIO;
	Union_WORD_t I255;
	Union_WORD_t MCU;
	Union_WORD_t LVN_Security;
} IspChecksum_t;

typedef struct _IspExtendChip_t {		//總共15Bytes
	BYTE Version[5];			//5 Bytes
	BYTE Offset[4];		//4 Bytes
	BYTE Length[4];
	BYTE Checksum[2];		//2 Bytes
} IspExtendChip_t;

typedef struct _IspExtendChip_2Ver_t {		//總共20Bytes
	BYTE Version_DP5[5];			//5 Bytes
	BYTE Version_DP6[5];			//5 Bytes
	BYTE Offset[4];		//4 Bytes
	BYTE Length[4];		//4 Bytes
	BYTE Checksum[2];		//2 Bytes
} IspExtendChip_2Ver_t;

typedef struct _IspHeadPage_t {		//總共512Bytes
	IspVersionInMcu_t isp_version_in_mcu;	//總共49Bytes
	BYTE Reserve_0[15];
	IspOffset_t isp_offset;					//總共44Bytes
	BYTE Reserve_1[20];
	IspLength_t isp_length;					//總共48Bytes
	BYTE Reserve_2[16];
	IspChecksum_t isp_checksum;				//總共22Bytes
	BYTE Reserve_3[26];
	BYTE CompareKeyFlag[1];
	BYTE Reserve_4[13];
	BYTE SetCheckReady[2];    // 2     address 254, 0xFE
	IspExtendChip_t GD_MCU;			//15 Bytes
	IspExtendChip_t Extra_USB3;		//15 Bytes
	IspExtendChip_2Ver_t Extra_DP;		//20 Bytes
	BYTE Reserve_5[206];	//剩下的緩存空間	
} IspHeadPage_t;


// CUserBinMergeToolDlg 對話方塊
class CUserBinMergeToolDlg : public CDialogEx
{
// 建構
public:
	CUserBinMergeToolDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_BINMERGETOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPathDmcIOT();

	afx_msg void OnBnClickedButtonPathDmcNoIOT();
	afx_msg void OnBnClickedButtonPathTbt4();
	afx_msg void OnBnClickedButtonPathDp();
	afx_msg void OnBnClickedButtonPathUsbHub1();
	afx_msg void OnBnClickedButtonPathUsb2Hub();
	afx_msg void OnBnClickedButtonPathUsbAudio();
	afx_msg void OnBnClickedButtonPathLanI225();
	afx_msg void OnBnClickedButtonPathMcu();
	afx_msg void OnBnClickedButtonPathExe();
	afx_msg void OnBnClickedButtonCombine();
	int user_Get_File_Size(CString file_path);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void Initial_DMC_Value(BYTE action);
	void Initial_DMC_NoIOT_Value(BYTE action);
	void Initial_TBT4_Value(BYTE action);
	void Initial_DP_Value(BYTE action);
	void Initial_USB3_IOT_Value(BYTE action);
	void Initial_USB2_HUB_Value(BYTE action);
	void Initial_Audio_Value(BYTE action);
	void Initial_LanI225_Value(BYTE action);
	void Initial_MCU_Value(BYTE action);
	void Initial_PathSecurity(BYTE action);

	CString SetRelative_Path(CString path);

	void Initial_Write(void);
	void Initial_Read(void);

	void Initial_Value_Set(void);

	void user_CreateFile(CString target_path, char* content, int buff_lenth);

	void user_CombineFile(CString file_path_input, CString file_path_output);

	std::string user_CT2A(CString C_str);

	void user_Adding_Tag(CString target_path);

	void Create_HeaderFile(void);

	void Set_Offset_Value(void);

	WORD Make_CheckSum(CString TargetFile_Input);

	DWORD Make_Security_CheckSum(CString TargetFile_Input);

	DWORD Create_CheckSum_File(void);

	//m_PathFile_DMC
	CString  m_PathFile_PD, m_PathFile_DP, m_PathFile_USB2_HUB, m_PathFile_USB_AUDIO, m_PathFile_LAN_I225, m_PathFile_MCU, m_PathFile_Exe , m_PathFile_Security;
	CString m_PathFile_USB_IOT, m_PathFile_USB_NoIOT;
	CString m_PathFile_TBT4_IOT, m_PathFile_TBT4_NoIOT;
	CString m_Reduction_PathFile[9];
	CString m_PathFile_DMC_IOT, m_PathFile_DMC_NoIOT;

	CString m_PathFile_Extra_USB_IOT, m_PathFile_Extra_USB_NoIOT;	//Alan,20220207
	CString m_PathFile_Extra_DP;	//Alan,20220207
	CString m_PathFile_GD_MCU;	//Alan,20220207

	//CString m_Version_DMC, m_Version_PD, m_Version_TBT4, m_Version_DP_5xxx, m_Version_DP_6xxx, m_Version_USB2_HUB, m_Version_USB_AUDIO, m_Version_LAN_I225, m_Version_MCU, m_Version_Exe;

	CString m_Company_Name,  m_Edge_Tag;
	//m_Head_Version
	CString m_Head_Version_IOT, m_Head_Version_NoIOT;

	CString  m_ChecksumFile_Path, m_CodeVer_Path;
	CString  m_HeadFile_Path_IOT, m_HeadFile_Path_NoIOT;

	//CString m_Combine_path;
	CString m_Combine_path_IOT, m_Combine_path_NoIOT;

	//_Head_struct   m_Head_TBT4, m_Head_DP,  m_Head_USB2_Hub, m_Head_Audio, m_Head_LanI225, m_Head_MCU, m_Head_Security;
	//_Head_struct m_Head_LengthSum;
	_Head_struct m_Head_Reduction[9];

	Union_DWORD_t gb_Check_sum , new_gb_Check_sum;	//增加新的演算法Checksum
	afx_msg void OnBnClickedButtonBinToExe();
	afx_msg void OnBnClickedButtonPathCombinedBin();
	DWORD Reduction_File(int chip_num, CString file_path_input);
	afx_msg void OnBnClickedButtonPathCodeverTag();
	afx_msg void OnBnClickedButtonPathSecurity();

	afx_msg void OnEnChangeEditHeadVersion();

	void Eding_Swap(BYTE* addr, BYTE length);

	//IspVersionInMcu_t isp_version_in_mcu;
	IspHeadPage_t isp_headpage;
	IspHeadPage_t isp_headpage_NoIOT;

	IspHeadPage_t* temp_point, *temp_point_NoIOT;


	afx_msg void OnBnClickedButtonPathCombinedBinNoiot();
	afx_msg void OnBnClickedButtonPathUsb3HubNoiot();
	void Initial_USB3_NoIOT_Value(BYTE action);
	afx_msg void OnBnClickedButtonPathTbt4Noiot();
	void Initial_TBT4_Value_NoIOT(BYTE action);
	afx_msg void OnBnClickedButtonPathGdMcu();

	afx_msg void OnBnClickedButtonPathExtraUsb3Iot();
	afx_msg void OnBnClickedButtonPathExtraUsb3Noiot();
	afx_msg void OnBnClickedButtonPathExtraDp();

	void Initial_Extra_DP_Value(BYTE action);

	void Initial_Extra_USB3_IOT_Value(BYTE action);
	void Initial_Extra_USB3_NoIOT_Value(BYTE action);

	void Initial_GD_MCU_Value(BYTE action);
};
