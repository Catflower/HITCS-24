#include <stdio.h>
#include <windows.h>

#include "stdlib.h"
#include "tlhelp32.h"
#include "stdafx.h"
#include "resource.h"	


#if _MSC_VER > 1000
#pragma once
#endif 

// dllinfoDlg ÉùÃ÷
class dllinfoDlg : public CDialog
{
public:
	dllinfoDlg(CWnd* pParent = NULL);	

	//{{AFX_DATA(CTestFunDlg)
	enum { IDD = IDD_TESTFUN_DIALOG };
	CListCtrl	m_ListCtrl;
	CString	m_csPathName;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CTestFunDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	HICON m_hIcon;
	//{{AFX_MSG(CTestFunDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
 	afx_msg void OnButtonOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

 private:
	CString m_csFileName;
	IMAGE_DOS_HEADER   m_dosHreader;
	IMAGE_NT_HEADERS   m_ntHeader;
	IMAGE_SECTION_HEADER m_secHeader;
	LPVOID   m_lpMapAddress;
    void      ErrorHandler();
	BOOL      isPeFile();
	BOOL      openFile();
    void    InitListCtrl();
    void    AutoListSize();

	DWORD   RvaToData(LPVOID lpBaseAddr,DWORD VirtualAddress);
	BOOL    GetDosHeader();
	BOOL    GetNtHeader();
	BOOL    GetDllInfo();
};

