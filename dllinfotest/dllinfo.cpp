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


// dllinfo �ṹ
dllinfo::dllinfo()
{
}

// Ψһ�� dllinfo ����
dllinfo theApp;


// dllinfo ��ʼ��

BOOL dllinfo::InitInstance()
{
	AfxEnableControlContainer();

	// ��׼��ʼ��

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
