//#include "stdafx.h"
#include "pch.h"

#include "user_DisplayInfo.h"

int User_DisplayInfo::AppendToLogAndScroll(CString str, COLORREF color)     //Don't declare to main class member
{
	// http://www.orcode.com/article/Edit_20120879.html
	long nVisible = 0;
	long nInsertionPoint = 0;
	CHARFORMAT cf;

	m_CtrlLogMsg.GetSelectionCharFormat(cf);
	// Initialize character format structure
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR

	cf.crTextColor = color;
	cf.yHeight = 8 * 8;
	// Set insertion point to end of text
	nInsertionPoint = m_CtrlLogMsg.GetWindowTextLength();
	m_CtrlLogMsg.SetSel(nInsertionPoint, -1);


	//m_CtrlLogMsg.SetBackgroundColor(FALSE, MEDPURPLE);

	// Set the character format
	m_CtrlLogMsg.SetSelectionCharFormat(cf);

	// Replace selection. Because we have nothing
	// selected, this will simply insert
	// the string at the current caret position.
	m_CtrlLogMsg.ReplaceSel(str);

	// Get number of currently visible lines or maximum number of visible lines
	// (We must call GetNumVisibleLines() before the first call to LineScroll()!)
	nVisible = GetNumVisibleLines(&m_CtrlLogMsg);

	// Now this is the fix of CRichEditCtrl's abnormal behaviour when used
	// in an application not based on dialogs. Checking the focus prevents
	// us from scrolling when the CRichEditCtrl does so automatically,
	// even though ES_AUTOxSCROLL style is NOT set.
	if (&m_CtrlLogMsg != m_CtrlLogMsg.GetFocus())
	{
		m_CtrlLogMsg.LineScroll(INT_MAX);
		m_CtrlLogMsg.LineScroll(1 - nVisible);
	}

	return 0;
}

int User_DisplayInfo::GetNumVisibleLines(CRichEditCtrl* pCtrl)
{
	CRect rect;
	long nFirstChar, nLastChar;
	long nFirstLine, nLastLine;

	// Get client rect of rich edit control
	pCtrl->GetClientRect(rect);

	// Get character index close to upper left corner
	nFirstChar = pCtrl->CharFromPos(CPoint(0, 0));

	// Get character index close to lower right corner
	nLastChar = pCtrl->CharFromPos(CPoint(rect.right, rect.bottom));
	if (nLastChar < 0)
	{
		nLastChar = pCtrl->GetTextLength();
	}

	// Convert to lines
	nFirstLine = pCtrl->LineFromChar(nFirstChar);
	nLastLine = pCtrl->LineFromChar(nLastChar);

	return (nLastLine - nFirstLine);
}

void User_DisplayInfo::CleanLog(void) {
	m_CtrlLogMsg.SetReadOnly(FALSE);		//Default at 
	m_CtrlLogMsg.SetSel(0, -1);
	m_CtrlLogMsg.Clear();
	m_CtrlLogMsg.SetReadOnly(TRUE);
}