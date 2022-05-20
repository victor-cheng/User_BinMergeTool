
#pragma once
#include "afxcmn.h"

#define RED        RGB(127,  0,  0)
#define GREEN      RGB(  0,127,  0)
#define BLUE       RGB(  0,  0,127)
#define LIGHTRED   RGB(255,  0,  0)
#define LIGHTGREEN RGB(  0,255,  0)
#define LIGHTBLUE  RGB(  0,  0,255)
#define BLACK      RGB(  0,  0,  0)
#define WHITE      RGB(255,255,255)
#define GRAY       RGB(192,192,192)
#define LIGHTPINK       RGB(255, 182, 193)
#define MEDPURPLE       RGB(186,85,211)
#define ORAGE       RGB(255,165,0)
#define DEEPSKYBLUE       RGB(0,191,255)
#define LIGHTSKYBLUE       RGB(135,206,250)
#define LIGHTYELLOW       RGB(255, 255, 0)

// Defines for the font style
#define FS_NORMAL			0x00
#define FS_BOLD				0x01
#define FS_ITALIC			0x02
#define FS_UNDERLINED		0x04
#define FS_STRIKETHROUGH	0x08
#define FS_ANTIALIAS		0x10

// Defines for horizontal alignment
#define FS_CENTER			0x20
#define FS_LEFT				0x40
#define FS_RIGHT			0x80

class CColorEdit : public CEdit {
	DECLARE_DYNAMIC(CColorEdit)

public:
	CColorEdit();
	virtual ~CColorEdit();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyEdit)
public:
//	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	//}}AFX_VIRTUAL


	void SetBackground(DWORD dwBgColor);
	void SetTextColor(DWORD dwColor);
//	void SetTextColorUser(DWORD dwColor);
	void SetFontStyle(CString szFont, int nSize, DWORD dwStyle);

	bool m_bBold;
	bool m_bItalic;
	bool m_bUnderlined;
	bool m_bStrikethrough;
	bool m_bAntialias;
	//	bool m_bBgColor;

	bool m_bCenter;
	bool m_bLeft;
	bool m_bRight;

	CEdit *m_myAddr;
	CString m_szFont;
	DWORD m_dwColor;
	DWORD m_dwBgColor;
	int m_nSize;
	CBrush m_brBkgnd; // Holds Brush Color for the Edit Box

	CString m_Text;
	COLORREF m_BackColor;
protected:
	//{{AFX_MSG(CColorEdit)
	//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor); // This Function Gets Called Every Time Your Window Gets Redrawn.
													   //}}AFX_MSG
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnChange();

//	afx_msg void OnSetFocus(CWnd* pOldWnd);
//	afx_msg void OnKillFocus(CWnd* pNewWnd);
//	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};




