#pragma once

#ifndef __SHAREMEMORY_H__
#define __SHAREMEMORY_H__

class CShareMemory
{
public:
// ���캯������������
	CShareMemory(void);
	CShareMemory(const char * pszMapName, int nFileSize = 0, BOOL bServer = FALSE);
	~CShareMemory();
// ����
	LPVOID GetBuffer() const { return  m_pBuffer; }
// ʵ��
private:
	HANDLE	m_hFileMap;
	LPVOID	m_pBuffer;
};

#endif // __SHAREMEMORY_H__

