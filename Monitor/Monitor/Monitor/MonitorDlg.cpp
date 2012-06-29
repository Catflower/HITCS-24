
// MonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Monitor.h"
#include "MonitorDlg.h"
#include "afxdialogex.h"
#include "windows.h"
#include "tlhelp32.h"
#include <cassert>
#include "comdef.h" 
#include "FunctionsInfor.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Psapi.h"
#pragma comment(lib,"psapi.lib")

//************************FileSystemWatcher**************************
extern CMonitorApp theApp;

bool FileSystemWatcher::IsAllowed(CString str)
{
	return true;
}

void FileSystemWatcher::DealFunc( ACTION act, CString filename1, CString filename2)
{
	unsigned char ch = 1;
	if (act == ADDED) ch = 1;
	else if (act == REMOVED) ch = 4;
	else if (act == MODIFIED) ch = 2;
	else if (act == RENAMED) ch = 8;

	if (!IsAllowed(filename1)) return;

	filename1 = WatchedDir + filename1;
	if (act == RENAMED)
		filename2 = WatchedDir + filename2;
	else
		filename2 = "";

	CFileInfo fi(act, filename1, filename2);
	theApp.AddOneItem(fi);
}

DWORD WINAPI FileSystemWatcher::Routine( LPVOID lParam )
{
    FileSystemWatcher* pFsw = (FileSystemWatcher*)lParam;
	CString tem1,tem2;
	CString str = (_T("[") + pFsw->WatchedDir + "] 开始监视!");
    pFsw->hDir = CreateFile(
        pFsw->WatchedDir,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if( INVALID_HANDLE_VALUE == pFsw->hDir ) return false;

    char buf[ 2*(sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH) ];
    FILE_NOTIFY_INFORMATION* pNotify=(FILE_NOTIFY_INFORMATION *)buf;
    DWORD BytesReturned;
    while(true)
    {
        if( ReadDirectoryChangesW( pFsw->hDir,
            pNotify,
            sizeof(buf),
            true,
            FILE_NOTIFY_CHANGE_FILE_NAME|
            FILE_NOTIFY_CHANGE_DIR_NAME|
            FILE_NOTIFY_CHANGE_ATTRIBUTES|
            FILE_NOTIFY_CHANGE_SIZE|
            FILE_NOTIFY_CHANGE_LAST_WRITE|
            FILE_NOTIFY_CHANGE_LAST_ACCESS|
            FILE_NOTIFY_CHANGE_CREATION|
            FILE_NOTIFY_CHANGE_SECURITY,
            &BytesReturned,
            NULL,
            NULL ) )
        {
            char tmp[MAX_PATH], str1[MAX_PATH], str2[MAX_PATH];
            memset( tmp, 0, sizeof(tmp) );
            WideCharToMultiByte( CP_ACP,0,pNotify->FileName,pNotify->FileNameLength/2,tmp,99,NULL,NULL );
            strcpy_s( str1,260, tmp );

            if( 0 != pNotify->NextEntryOffset )
            {
                PFILE_NOTIFY_INFORMATION p = (PFILE_NOTIFY_INFORMATION)((char*)pNotify+pNotify->NextEntryOffset);
                memset( tmp, 0, sizeof(tmp) );
                WideCharToMultiByte( CP_ACP,0,p->FileName,p->FileNameLength/2,tmp,99,NULL,NULL );
                strcpy_s( str2,260, tmp );
            }
			tem1= str1;
			tem2= str2;
            pFsw->DealFunc((ACTION)pNotify->Action, tem1, tem2);
        } else
        {
            break;
        }
    }
    return 0;
}

FileSystemWatcher::~FileSystemWatcher()
{
	Close();
}
//************************************************************
//****************************CFileInfo类*********************
CFileInfo::CFileInfo(ACTION type, CString source, CString dest)
{
	this->type = type;
	this->source = source;
	this->dest = dest;

	time(&occurTime);
}

CString CFileInfo::ToString() const
{
	CString str;
	switch (type)
	{
		case ADDED:	str = "[创建] ";	break;
		case REMOVED:	str = "[删除] ";	break;
		case MODIFIED:	str = "[修改] ";	break;
		case RENAMED:	str = "[改名] ";	break;
	}
	str += source;
	if (type == RENAMED)
	{
		str += " -> ";
		str += dest;
	}
	return str;
}

CString CFileInfo::GetTypeString() const
{
	CString temp;
	switch (type)
	{
		case ADDED:		temp="[创建]";return temp;
		case REMOVED:   temp="[删除] ";return temp;
		case MODIFIED:	temp="[修改] ";return temp;
		case RENAMED:	temp="[改名] ";return temp;
		default:		temp="[@@@@]";return temp;
	}
}

CString CFileInfo::GetDiskName() const
{
	return source.Mid(0, 3);
}

CString CFileInfo::GetPath() const
{
	return source;
}

CString CFileInfo::GetTimeString() const
{
	struct tm *newtime;
	newtime = localtime(&occurTime);
	CString str;

	str.Format(_T("%02d:%02d:%02d"),
				newtime->tm_hour,
				newtime->tm_min,
				newtime->tm_sec
				);
	return str;
}

CString CFileInfo::GetPathRenamed() const
{
	return dest;
}
//****************************************************
//****************************************************

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMonitorDlg 对话框


CMonitorDlg::CMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1_TOTAL, m_list);
	DDX_Control(pDX, IDC_LIST2, m_listmod);
	DDX_Text(pDX, IDC_STATIC1, m_nNumberofProcess);
	DDX_Text(pDX, IDC_EDIT1, m_time);
	DDX_Text(pDX, IDC_EDIT2, m_minute);
	DDX_Text(pDX, IDC_EDIT3, m_second);
	DDX_Control(pDX, IDC_BTN_UPDATE, m_update);
	DDX_Control(pDX, IDC_BTN_NEW, m_new);
	DDX_Control(pDX, IDC_BTN_END, m_end);

	DDX_Control(pDX, IDC_LIST_LOG, m_log);
	DDX_Control(pDX, IDC_LIST_CONTEXT, m_context);
	DDX_Control(pDX, IDC_BTN_STOP, m_stop);
	DDX_Control(pDX, IDC_CLEAR, m_clear);
	DDX_Control(pDX, IDC_BTN_START, m_start);
}

BEGIN_MESSAGE_MAP(CMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_NEW, OnBtnNew)
	ON_BN_CLICKED(IDC_BTN_END, OnBtnEnd)
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)

	ON_NOTIFY(NM_CLICK, IDC_LIST1_TOTAL, OnClickListTotal)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LOG, &CMonitorDlg::OnLvnItemchangedListLog)

	//ON_BN_CLICKED(IDC_BTN_END, &CMonitorDlg::OnBnClickedBtnEnd)
	ON_BN_CLICKED(IDC_BUTTON1, &CMonitorDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMonitorDlg 消息处理程序

BOOL CMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
		vector<CString> vDriveNames;

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_list.InsertColumn(1,_T("进程名称"),LVCFMT_CENTER,100,1);
	m_list.InsertColumn(2,_T("PID"),LVCFMT_CENTER,60,1);
	m_list.InsertColumn(3,_T("优先级"),LVCFMT_CENTER,60,1);
	m_list.InsertColumn(4,_T("线程数"),LVCFMT_CENTER,60,1);
	m_list.InsertColumn(5,_T("路径"),LVCFMT_CENTER,250,1);
	
	m_listmod.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listmod.InsertColumn(1,_T("模块名称"),LVCFMT_CENTER,250,1);
	m_listmod.InsertColumn(2,_T("模块ID"),LVCFMT_CENTER,100,1);

	UpdateData(TRUE);
	//获取所有可以监控的盘，不管是硬盘还是软盘
	DWORD drives;
	drives = ::GetLogicalDrives();
	for(int i=0; i<32; i++)
	{
		if(drives & 0x01)
		{
			CString str;
			str.Format(_T("%c:\\"), 'A' + i);
			if (GetDriveType(str) == DRIVE_FIXED
				|| GetDriveType(str) == DRIVE_REMOVABLE)
				vDriveNames.push_back(str);
		}
		drives >>= 1;
	}
	//添加监控的盘符
	unsigned int i;
	for (i=0; i<vDriveNames.size(); i++)
	{
		m_context.AddString(vDriveNames[i]);
	}
	//选择监控的盘符
	for (i=0; i<vDriveNames.size(); i++)
	{
		m_context.SetSel(i);
	}

	//添加列表控件的列
	m_log.InsertColumn(0, _T("操作"), LVCFMT_LEFT, 50);
	m_log.InsertColumn(1, _T("盘符"), LVCFMT_LEFT, 50);
	m_log.InsertColumn(2, _T("路径"), LVCFMT_LEFT, 380);
	m_log.InsertColumn(3, _T("时间"), LVCFMT_LEFT, 70);
	m_log.InsertColumn(4, _T("所属进程"), LVCFMT_LEFT, 80);
	m_log.InsertColumn(5, _T("注释"), LVCFMT_LEFT, 100);
	m_log.SetExtendedStyle(m_log.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
		
	int arWidth[] = { 250, -1 };
	UpdateData(FALSE);
	UpdateProcess();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//***************************************************************

//获取进程路径
CString GetProcessPath( DWORD idProcess )
{
	// 获取进程路径
    CString sPath;
	// 打开进程句柄
    HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                    FALSE, 
									idProcess );
    if( NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;
		// 获取路径
        if( EnumProcessModules( hProcess, &hMod, sizeof( hMod ), &cbNeeded ) )
        {
			DWORD dw = GetModuleFileNameEx( hProcess, hMod, sPath.GetBuffer( MAX_PATH ), MAX_PATH );
			sPath.ReleaseBuffer();
        }
	    CloseHandle( hProcess );
    }
	return( sPath );
}

//获取进程优先级
CString GetProcessPriority(HANDLE hProcess)
{
	CString sz1 =_T("NORMAL");
	CString sz2 =_T("IDLE");
	CString sz3 = _T("REALTIME");
	CString sz4 = _T("HIGH");
	CString sz5 =_T("NULL");
	CString sz6 = _T("ABOVENORMAL");
	CString sz7 = _T("BELOWNORMAL");

	//进程优先级返回
	if(GetPriorityClass(hProcess) == NORMAL_PRIORITY_CLASS)
		return sz1;	
	if(GetPriorityClass(hProcess) == IDLE_PRIORITY_CLASS)
		return sz2;
	if(GetPriorityClass(hProcess) == REALTIME_PRIORITY_CLASS)
		return sz3;
	if(GetPriorityClass(hProcess) == HIGH_PRIORITY_CLASS)
		return sz4;
	else
		return sz5;


}

//终止进程主函数
void TerminateProcessID(DWORD dwID)
{
	HANDLE hProcess = NULL;
	//打开进程句柄
	hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,dwID);
	if(hProcess != NULL)
	{
		//终止进程
		TerminateProcess(hProcess,0);
		::CloseHandle(hProcess);
	}
}

void CMonitorDlg::UpdateProcess()
{
	UpdateData(TRUE);
	m_list.DeleteAllItems();
	DWORD idxList = 1;
	int nCount = 0;
	HANDLE hProcess;
	char szCount[56];
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("CreateToolhelp32Snapshot调用失败"),_T("Warning"));
		return;
	}
	BOOL bMore = ::Process32First(hProcessSnap,&pe32);
	while(bMore)
	{	
		m_list.InsertItem(1,pe32.szExeFile);
		wsprintf(LPTSTR(szID),_T("%u"),pe32.th32ProcessID);
		wsprintf(LPTSTR(szCount),_T("%u"),pe32.cntThreads);
		m_list.SetItemText(idxList,1,LPTSTR(szID));
		hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe32.th32ProcessID);
		m_list.SetItemText(idxList,2,GetProcessPriority(hProcess));
		m_list.SetItemText(idxList,4,GetProcessPath(pe32.th32ProcessID));
		m_list.SetItemText(idxList,3,LPTSTR(szCount));
		m_list.SetItemData(idxList,pe32.th32ProcessID);
		nCount++;
		bMore = ::Process32Next(hProcessSnap,&pe32);
	}
	::CloseHandle(hProcessSnap);
	//char szNum[5];]
	LPTSTR szNum=new TCHAR[5];
	wsprintf(szNum,_T("%d"),nCount);
	m_nNumberofProcess = szNum;

	UpdateData(FALSE);
}

CString CMonitorDlg::GetProcessModule(DWORD dwID)
{
	CString ret;
	HMODULE hMod[1024];
	//char Buffer[256];
	LPWSTR Buffer=new TCHAR[256];
	HANDLE hProcess;
	DWORD cbNeed;
	unsigned int i;
	//char szModName[MAX_PATH];
	LPWSTR szModName=new TCHAR[MAX_PATH];
	m_listmod.DeleteAllItems();
	hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwID);
	if( hProcess &&  EnumProcessModules(hProcess, hMod, sizeof(hMod), &cbNeed))
    {
        for ( i = 0; i <=(cbNeed / sizeof(HMODULE)); i++ )
        {        			
			if ( GetModuleFileNameEx( hProcess, hMod[i], szModName,MAX_PATH))
            {
				m_listmod.InsertItem(1,szModName);
				wsprintf(Buffer,_T("%08x"),hMod[i]);	
				m_listmod.SetItemText(1,1,Buffer);
            }
        }
		CloseHandle( hProcess );
		ret = _T("");
		return ret;
    }
	else  
	{
		CloseHandle( hProcess );
		ret = _T("NULL");
		return ret;
    }
}

void CMonitorDlg::OnBtnUpdate()
{
	UpdateProcess();
	return;
}

void CMonitorDlg::OnBtnNew() 
{
	CString strFileName;
	CFileDialog fDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("Executable files(*.exe)|*.exe|All files (*.*)|*.*||"));
	if(fDlg.DoModal() == IDOK)
	{
		strFileName = fDlg.GetPathName();
	}
	else
	{
		return;
	}
	STARTUPINFO si = {sizeof(si)} ;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = TRUE;
	PROCESS_INFORMATION pi;
	///char *buf;
	LPWSTR buf;
	buf = strFileName.GetBuffer(100);
	
	//新建一个进程
	BOOL bRet = ::CreateProcess(NULL,
		buf,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi);
	OnBtnUpdate();	
}

void CMonitorDlg::OnBtnEnd() 
{
	CString szPID;

	int nItem = m_list.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	szPID = m_list.GetItemText(nItem,1);
	
	if(szPID == "")
	{
		MessageBox(_T("请选择一个进程"),_T("提示"));
		return;
	}
	
	int nSelect=::MessageBox(NULL,_T("是否终止该进程"),_T("提示"),MB_OKCANCEL|MB_ICONWARNING);

	if(nSelect ==IDOK)
	{ 
		TerminateProcessID(_wtol(szPID));
	} 
	if(nSelect ==IDCANCEL)
	{
		return;
	}
	OnBtnUpdate();
}

void CMonitorDlg::OnBtnStart() 
{
	//获取选择的盘符
	int i=0;
	int selCount = m_context.GetSelCount();
	//创造监视线程
	for(i=0; i<selCount; i++)
	{
		m_vpFSW.push_back(new FileSystemWatcher());
	}

	int sels[32];
	m_context.GetSelItems(32, sels);
	//开始监视
	for(i=0; i<selCount; i++)
	{
		CString str;
		m_context.GetText(sels[i], str);
		m_vpFSW[i]->Run((LPCTSTR)str, NULL);
	}


	m_stop.EnableWindow(TRUE);
	m_clear.EnableWindow(FALSE);
	m_start.EnableWindow(FALSE);
}

void CMonitorDlg::OnBtnClear() 
{
	m_log.DeleteAllItems();
}

void CMonitorDlg::OnBtnStop() 
{
	//删除线程信息，停止线程
	unsigned int i;
	for (i=0; i<m_vpFSW.size(); i++)
		delete m_vpFSW[i];
	m_vpFSW.clear();

	m_stop.EnableWindow(FALSE);
	m_start.EnableWindow(TRUE);
	m_clear.EnableWindow(TRUE);
}

void CMonitorDlg::OnClickListTotal(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);

	int nItem = m_list.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	szCID= m_list.GetItemText(nItem,1);
	
	DWORD dwPID = _wtol(szCID);
	m_time = GetProcessCreateTime(dwPID,1);
	m_minute = GetProcessCreateTime(dwPID,2);
	m_second = GetProcessCreateTime(dwPID,3);
	UpdateData(FALSE);

	GetProcessModule(dwPID);

	*pResult = 0;
}

void CMonitorDlg::AddOneItem(const CFileInfo &fi)
{
	m_fiMutex.Lock();
	m_log.InsertItem(0, _T(""));
	//0, "操作"
	//1, "盘符"
	//2, "路径"
	//3, "时间"
	//4, "所属进程"
	//5, "注释"
	m_log.SetItemText(0, 0, fi.GetTypeString());
	m_log.SetItemText(0, 1, fi.GetDiskName());
	m_log.SetItemText(0, 2, fi.GetPath());
	m_log.SetItemText(0, 3, fi.GetTimeString());
	m_log.SetItemText(0, 4, _T("      "));
	m_log.SetItemText(0, 5, fi.GetPathRenamed());

	m_fiMutex.Unlock();
}

void CMonitorDlg::OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


CString CMonitorDlg::GetProcessCreateTime(DWORD dwID,int i)	//获取进程建立时间
{			
	CString szHour1;
	CString nul;
	//char szHour[5];
	//char szMinute[5];
	//char szSecond[5];
	LPTSTR szHour=new TCHAR[5];
	LPTSTR szMinute=new TCHAR[5];
	LPTSTR szSecond=new TCHAR[5];
	FILETIME CPUTime;
	FILETIME CreateTime;
	FILETIME ExitTime;
	FILETIME KernelTime;
	FILETIME UserTime;
	SYSTEMTIME SystemTime;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwID);
	if(hProcess == 0)	//进程未建立
	{
		return nul;
	}
	else
	{
		if(!GetProcessTimes(hProcess,&CreateTime,&ExitTime,&KernelTime,&UserTime))
		{
			return nul;		//获取时间不成功
		}
		else
		{
			CPUTime.dwHighDateTime =  CreateTime.dwHighDateTime;
			CPUTime.dwLowDateTime =  CreateTime.dwLowDateTime;
			FileTimeToSystemTime(&CPUTime,&SystemTime);
		
			wsprintf(szHour,_T("%.2d"),(SystemTime.wHour+8)%24);
			wsprintf(szMinute,_T("%.2d"),SystemTime.wMinute);
			wsprintf(szSecond,_T("%.2d"),SystemTime.wSecond);
		}
	}
	if(i ==1)
		return szHour;
	if(i ==2)
		return szMinute;
	if(i ==3)
		return szSecond;
	else 
		return nul;
}

void CMonitorDlg::OnBnClickedButton1()
{
	//获取点击项的位置
	POSITION   pos   =   m_list.GetFirstSelectedItemPosition(); 
    if   (NULL   ==   pos) 
     { 
     return; 
     } 
    int   nItem   =  m_list.GetNextSelectedItem(pos);
 
    // 调用打印所选中进程调用函数列表的对话框
    FunctionsInfor aDalg;
	aDalg.ProcessName=m_list.GetItemText(nItem,0);
	aDalg.ProcessId=m_list.GetItemData(nItem);
	aDalg.DoModal();
}
