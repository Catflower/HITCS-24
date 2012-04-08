// dllinfo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "dllinfo.h"
#include "dllinfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// dllinfo
BEGIN_MESSAGE_MAP(dllinfo, CWinApp)
ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// dllinfo 结构
dllinfo::dllinfo()
{
}

// 唯一的 dllinfo 对象
dllinfo theApp;


// dllinfo 初始化

BOOL dllinfo::InitInstance()
{
	AfxEnableControlContainer();

	// 标准初始化

#ifdef _AFXDLL
	// Call this when using MFC in a shared DLL
	Enable3dControls();			
#else
	// Call this when linking to MFC statically
	Enable3dControlsStatic();	
#endif

	dllinfoDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}
	return FALSE;
}
