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
		// 创建一个内存映射文件对象
		m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, 
			NULL, PAGE_READWRITE, 0, nFileSize, mypszMapName);
	}
	else
	{
		// 打开一个内存映射文件对象
		m_hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, mypszMapName);
	}
	
	// 映射它到内存，取得共享内存的首地址
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
	// 取消文件的映射，关闭文件映射对象句柄
	UnmapViewOfFile(m_pBuffer);
	CloseHandle(m_hFileMap);
}


