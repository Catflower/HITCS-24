
// MonitorDlg.h : 头文件
//

#pragma once
#include <vector>
using namespace std;

//**************************************

enum ACTION { ADDED=1, REMOVED=2, MODIFIED=3, RENAMED=4 };
class CFileInfo
{
public:
	CFileInfo(ACTION type, CString source, CString dest);

	//返回一个摘要
	CString ToString() const;
	CString GetTypeString() const;
	CString GetDiskName() const;
	CString GetPath() const;
	CString GetTimeString() const;
	CString GetPathRenamed() const;

public:
	ACTION type;
	CString source;
	CString dest;
	time_t occurTime;
};
//**********************************************
class FileSystemWatcher
{
public:
    bool Run( CString path, LPVOID pParam)
    {
        WatchedDir = path;
		this->pParam = pParam;

        DWORD ThreadId;
        hThread = CreateThread( NULL, 0, Routine, this, 0, &ThreadId );
        return NULL != hThread;
    }

    void Close()
    {
        if( NULL != hThread )
        {
            TerminateThread( hThread, 0 );
            hThread = NULL;
        }
        if( INVALID_HANDLE_VALUE != hDir )
        {
            CloseHandle( hDir );
            hDir = INVALID_HANDLE_VALUE;
        }
    }

	FileSystemWatcher() : pParam(NULL), hThread(NULL), hDir(INVALID_HANDLE_VALUE)
    {
    }

    ~FileSystemWatcher();

private:
	CString WatchedDir;
	LPVOID pParam;
    HANDLE hThread;
    HANDLE hDir;

	bool IsAllowed(CString str);
	void DealFunc( ACTION act, CString filename1, CString filename2);

private:
    FileSystemWatcher( const FileSystemWatcher& );
    FileSystemWatcher operator=( const FileSystemWatcher );
private:
    static DWORD WINAPI Routine( LPVOID lParam );
};
//***********************************

// CMonitorDlg 对话框
class CMonitorDlg : public CDialogEx
{
// 构造
public:
	CMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数
//********************************
	char szID[56];
	CString szCID;
	CString szPID;

	CString	m_time;
	CString	m_minute;
	CString	m_second;
	CString	m_nNumberofProcess;

	void Insert(CListCtrl m_l,CString str);
	void UpdateProcess();
	CString GetProcessModule(DWORD dwID);
	CString GetProcessCreateTime(DWORD dwID,int i);
//*********************************
// 对话框数据
	enum { IDD = IDD_MONITOR_DIALOG };
//********************************
	CButton m_update;
	CButton m_new;
	CButton m_end;
	CButton	m_stop;
	CButton	m_clear;
	CButton	m_start;

	CListCtrl	m_list;
	CListCtrl	m_listmod;

	CListCtrl	m_log;
	CListBox	m_context;
//********************************
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
//******************************
	afx_msg void OnBtnUpdate();
	afx_msg void OnBtnNew();
	afx_msg void OnBtnEnd();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnStop();
	afx_msg void OnClickListTotal(NMHDR* pNMHDR, LRESULT* pResult);
//*******************************
	DECLARE_MESSAGE_MAP()
//**************************
private:
	CString m_logOutStr;
	vector<FileSystemWatcher*> m_vpFSW;
	CMutex m_logOutMutex;
	CMutex m_fiMutex;
//**************************
public:
	void AddOneItem(const CFileInfo &fi);
	afx_msg void OnLvnItemchangedListLog(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedBtnEnd();
	afx_msg void OnBnClickedButton1();
	//CString ProcessName;
};
