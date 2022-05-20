#pragma once

class User_DisplayInfo
{	//用於子執行緒與主執行緒的通信，顯示輸出資訊
public:


	CRichEditCtrl m_CtrlLogMsg;      //put this to Global, not in CGwCypressSwUpgradeDlg public.  Why?
	int AppendToLogAndScroll(CString str, COLORREF color);
	int GetNumVisibleLines(CRichEditCtrl* pCtrl);
	void CleanLog(void);

};
