#include <stdio.h>
#include <windows.h>

#include "stdlib.h"
#include "tlhelp32.h"
#include "stdafx.h"
#include "resource.h"	


#if _MSC_VER > 1000
#pragma once
#endif 

#ifndef __AFXWIN_H__

#endif

// dllinfoÀàÉùÃ÷:
class dllinfo : public CWinApp
{
public:
	dllinfo();
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

