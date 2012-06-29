#include "StdAfx.h"
#include "ShareMemory.h"


 CShareMemory::CShareMemory(void)
 {
 }
 CShareMemory::CShareMemory(const char * pszMapName, 
			int nFileSize, BOOL bServer) : m_hFileMap(NULL), m_pBuffer(NULL)
{
	LPCWSTR mypszMapName=(LPCWSTR)pszMapName;
	if(bServer)
	{
		// ����һ���ڴ�ӳ���ļ�����
		m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 
			NULL, PAGE_READWRITE, 0, nFileSize, mypszMapName);
	}
	else
	{
		// ��һ���ڴ�ӳ���ļ�����
		m_hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mypszMapName);
	}
	
	// ӳ�������ڴ棬ȡ�ù����ڴ���׵�ַ
	m_pBuffer = (LPBYTE)MapViewOfFile(
		m_hFileMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0
		);
}
 CShareMemory::~CShareMemory()
{
	// ȡ���ļ���ӳ�䣬�ر��ļ�ӳ�������
	UnmapViewOfFile(m_pBuffer);
	CloseHandle(m_hFileMap);
}


