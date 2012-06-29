// FunctionsInfor.cpp : 实现文件
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

// FunctionsInfor 对话框

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


// FunctionsInfor 消息处理程序


void FunctionsInfor::OnBnClickedCancel()
{
	//取消API的监控，或者关闭对框框
	CDialog::OnCancel();
}


void FunctionsInfor::OnBnClickedButton1()
{
	// 清空API的监控结果
	GetDlgItem(IDC_EDIT1)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT2)->SetWindowText(L"");
}


void FunctionsInfor::OnBnClickedOk()
{
	// ＡＰＩ的监控
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
	//添加列信息
	FunctionList.InsertColumn(0,L"进程名",LVCFMT_LEFT,150);
	FunctionList.InsertColumn(1,L"模块名",LVCFMT_LEFT,150);
	FunctionList.InsertColumn(2,L"函数名",LVCFMT_LEFT,150);
	FunctionList.InsertColumn(3,L"函数地址",LVCFMT_LEFT,150);
	//设置整行的选择属性
	FunctionList.SetExtendedStyle(FunctionList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	//获取进程的句柄号
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
		//一个IMAGE_THUNK_DATA 就是一个双字，它指定了一个导入函数
		IMAGE_THUNK_DATA *pThunk=(IMAGE_THUNK_DATA*)((BYTE*)hMod+pImportDesc->OriginalFirstThunk);
		int n = 0;
		while(pThunk->u1.Function){

			if((lstrcmpiA(pszDllName,"mfc100ud.dll")==0)||(lstrcmpiA(pszDllName,"oleaut32.dll")==0))
			 {
			   break;
		     }

			//取得函数名称。hint/name 表向前2个字节是函数序号，后面才是函数名字字符串
		    char* pszFunName =(char*)((BYTE*)hMod+(DWORD)pThunk->u1.AddressOfData+2);
			//获取函数地址。IAT表就是一个DWORD类型的数组，每个成员记录一个函数的地址
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
	// 异常: OCX 属性页应返回 FALSE
}


void FunctionsInfor::OnBnClickedBrowser()
{
	//浏览进程的目录
	CFileDialog file(TRUE);

	// 显示选择文件对话框
	if(file.DoModal() == IDOK)
	{
		GetDlgItem(IDC_EDIT3)->SetWindowText(file.GetPathName());
	}
}
