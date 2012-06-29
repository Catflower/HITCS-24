// FunctionsInfor.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Monitor.h"
#include "FunctionsInfor.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstring>
#include "MonitorDlg.h"

// FunctionsInfor �Ի���

IMPLEMENT_DYNAMIC(FunctionsInfor, CDialog)

FunctionsInfor::FunctionsInfor(CWnd* pParent /*=NULL*/)
	: CDialog(FunctionsInfor::IDD, pParent)
{

}

FunctionsInfor::~FunctionsInfor()
{
}

void FunctionsInfor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, FunctionList);
}


BEGIN_MESSAGE_MAP(FunctionsInfor, CDialog)
	ON_BN_CLICKED(IDCANCEL, &FunctionsInfor::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &FunctionsInfor::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &FunctionsInfor::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BROWSER, &FunctionsInfor::OnBnClickedBrowser)
END_MESSAGE_MAP()


// FunctionsInfor ��Ϣ�������


void FunctionsInfor::OnBnClickedCancel()
{
	//ȡ��API�ļ�أ����߹رնԿ��
	CDialog::OnCancel();
}


void FunctionsInfor::OnBnClickedButton1()
{
	// ���API�ļ�ؽ��
	GetDlgItem(IDC_EDIT1)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT2)->SetWindowText(L"");
}


void FunctionsInfor::OnBnClickedOk()
{
	// ���Уɵļ��
}


BOOL FunctionsInfor::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CString pszProcessName;
	//int n = ProcessName.GetLength();
	//pszProcessName=ProcessName.Left(n-4);

	CRect rect ;
	FunctionList.GetClientRect(&rect);
	int nColInterval = rect.Width()/6;
	//�������Ϣ
	FunctionList.InsertColumn(0,L"������",LVCFMT_LEFT,150);
	FunctionList.InsertColumn(1,L"ģ����",LVCFMT_LEFT,150);
	FunctionList.InsertColumn(2,L"������",LVCFMT_LEFT,150);
	FunctionList.InsertColumn(3,L"������ַ",LVCFMT_LEFT,150);
	//�������е�ѡ������
	FunctionList.SetExtendedStyle(FunctionList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	//��ȡ���̵ľ����
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessId);

	HMODULE hMod,hMod2;
	//hMod = GetModuleHandle(ProcessName);
	hMod= GetModuleHandle(NULL);
	//CString hand1,hand2;
	//hand1.Format(L"%ld",hMod);
	//MessageBox(hand1,0,0);
	//MessageBox(L"**********************",0,0);
	//hand2.Format(L"%ld",hMod2);
	//MessageBox(hand2,0,0);
	//MessageBox(L"################################",0,0);

	IMAGE_DOS_HEADER *pDosHeader =(IMAGE_DOS_HEADER*)hMod;
	IMAGE_OPTIONAL_HEADER *pOptHeader = (IMAGE_OPTIONAL_HEADER *)((BYTE*) hMod+pDosHeader->e_lfanew+24);
	IMAGE_IMPORT_DESCRIPTOR * pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE*)hMod+pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	int nItemNum = 0;
	nItemNum = FunctionList.GetItemCount();
	CString funcId;
	//int m = 0;
	while(pImportDesc->FirstThunk){
		char* pszDllName = (char*)((BYTE*)hMod+pImportDesc->Name);
		CString str(pszDllName);
		//MessageBox(str,0,0);
		//һ��IMAGE_THUNK_DATA ����һ��˫�֣���ָ����һ�����뺯��
		IMAGE_THUNK_DATA *pThunk=(IMAGE_THUNK_DATA*)((BYTE*)hMod+pImportDesc->OriginalFirstThunk);
		int n = 0;
		while(pThunk->u1.Function){

			if((lstrcmpiA(pszDllName,"mfc100ud.dll")==0)||(lstrcmpiA(pszDllName,"oleaut32.dll")==0))
			 {
			   break;
		     }

			//ȡ�ú������ơ�hint/name ����ǰ2���ֽ��Ǻ�����ţ�������Ǻ��������ַ���
		    char* pszFunName =(char*)((BYTE*)hMod+(DWORD)pThunk->u1.AddressOfData+2);
			//��ȡ������ַ��IAT�����һ��DWORD���͵����飬ÿ����Ա��¼һ�������ĵ�ַ
			PDWORD lpAddr =(DWORD*)((BYTE*)hMod+pImportDesc->FirstThunk)+n;

			CString dallName(pszDllName);
			CString funcName(pszFunName);
            funcId.Format(L"%ld",lpAddr);

		   FunctionList.InsertItem(nItemNum,ProcessName);
		   FunctionList.SetItemText(nItemNum,1,dallName);
		   FunctionList.SetItemText(nItemNum,2,funcName);
		   FunctionList.SetItemText(nItemNum,3,funcId);
		   n++;
		   pThunk++;
		 }
		//m++;
		pImportDesc++;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void FunctionsInfor::OnBnClickedBrowser()
{
	//������̵�Ŀ¼
	CFileDialog file(TRUE);

	// ��ʾѡ���ļ��Ի���
	if(file.DoModal() == IDOK)
	{
		GetDlgItem(IDC_EDIT3)->SetWindowText(file.GetPathName());
	}
}
