
// Monitor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "MonitorDlg.h" 
#include <afxmt.h>

// CMonitorApp:
// �йش����ʵ�֣������ Monitor.cpp
//

class CMonitorApp : public CWinApp
{
public:
	CMonitorApp();
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	void AddOneItem(const CFileInfo &fi);
};

