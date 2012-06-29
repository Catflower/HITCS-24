#pragma once
#include "afxcmn.h"
#include <cstring>
#include "RemThreadInjector.h"
//#include "../09APISpyLib/APISpyLib.h"
//#include "APISpyLib.h"
//#pragma comment(lib,"09APISpyLib.lib")

// FunctionsInfor �Ի���

class FunctionsInfor : public CDialog
{
	DECLARE_DYNAMIC(FunctionsInfor)

public:
	FunctionsInfor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~FunctionsInfor();

// �Ի�������
	enum { IDD = IDD_FUNCTIONSINFOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl FunctionList;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBrowser();
	CString ProcessName;
	DWORD ProcessId;

	CRemThreadInjector* m_pInjector;
	//CMyShareMem* m_pShareMem;
	DWORD m_dwProcessId;

	int m_nCount;

};
