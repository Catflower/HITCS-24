#include "stdafx.h"
#include "dllinfo.h"
#include "dllinfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL dllinfoDlg::isPeFile()
{
	
  IMAGE_DOS_HEADER *lpDosHeader=(IMAGE_DOS_HEADER *)m_lpMapAddress;
    
  if (lpDosHeader->e_magic!='ZM')
  {
	  return FALSE;
  }
  IMAGE_NT_HEADERS *lpNtHeader=(IMAGE_NT_HEADERS*)((DWORD)m_lpMapAddress+lpDosHeader->e_lfanew);

  if (lpNtHeader->Signature!=0X4550)
  {
	  return FALSE;
  }
  return TRUE;
}

void dllinfoDlg::ErrorHandler()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	
	::MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
 	LocalFree( lpMsgBuf );
	
}