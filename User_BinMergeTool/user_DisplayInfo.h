#pragma once

class User_DisplayInfo
{	//�Ω�l������P�D��������q�H�A��ܿ�X��T
public:


	CRichEditCtrl m_CtrlLogMsg;      //put this to Global, not in CGwCypressSwUpgradeDlg public.  Why?
	int AppendToLogAndScroll(CString str, COLORREF color);
	int GetNumVisibleLines(CRichEditCtrl* pCtrl);
	void CleanLog(void);

};
