
// QRConverterFinalDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CQRConverterFinalDlg dialog
class CQRConverterFinalDlg : public CDialogEx
{
// Construction
public:
	CQRConverterFinalDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_QRCONVERTERFINAL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HACCEL m_hAccel;
	void AddTaskbarIcon();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit EditBox_StringToConvert;
	CButton BGenerateCode;
	afx_msg void OnEnChangeEdit1();
	CEdit EditDecodeBOX;
	CEdit FromScreenText;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnTestOne();
	afx_msg void OnTestTwo();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnNotifyTaskbarIcon(WPARAM wParam, LPARAM lParam);
	CEdit Encrypted1;
	CEdit Encrypted2;
	CEdit SizeToScreenRead;
	bool minimized; 
	//To maximise to the normal size it's needed to record the initial position of the elements
	CRect size_screen, size_button1, size_button4, size_edit1, size_edit3, size_edit5, size_button3;




	CButton MinModeButton;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnStnClickedStatic2();
	
	CButton DCFromScreen;
};
