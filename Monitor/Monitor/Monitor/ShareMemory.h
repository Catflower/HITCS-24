#pragma once

#ifndef __SHAREMEMORY_H__
#define __SHAREMEMORY_H__

class CShareMemory
{
public:
// 构造函数和析构函数
	CShareMemory(void);
	CShareMemory(const char * pszMapName, int nFileSize = 0, BOOL bServer = FALSE);
	~CShareMemory();
// 属性
	LPVOID GetBuffer() const { return  m_pBuffer; }
// 实现
private:
	HANDLE	m_hFileMap;
	LPVOID	m_pBuffer;
};

#endif // __SHAREMEMORY_H__

