#include <windows.h> 
#include <stdio.h> 
#include <tlhelp32.h> 
#include <iostream>
#include "proinfo.h"

using namespace std; 
  
//��ȡ�������б�  
Process_Info* GetProcessList() 
{ 
    HANDLE             hProcessSnap; 
    PROCESSENTRY32     pe32; 
	int pCount;
  
    //��ϵͳ�е�ǰ���еĽ������¿���  
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if(hProcessSnap == INVALID_HANDLE_VALUE) 
    { 
        return NULL; 
    } 
  
    //��ʹ�� PROCESSENTRY32 �ṹ֮����Ҫ�����úøýṹ�Ĵ�С  
    pe32.dwSize = sizeof(PROCESSENTRY32); 
  
    //��ȡ��һ������  
    if(!Process32First(hProcessSnap, &pe32)) 
    { 
        CloseHandle(hProcessSnap); 
        return NULL; 
    } 

	pCount = 0;
    do
	{
		pCount++;
	}while (Process32Next(hProcessSnap, &pe32));
    
    //��ʼ����������
    Process_ID* pList = (Process_ID*) malloc(sizeof(Process_ID) * pCount);
    Process_Info* pInfo = (Process_Info*)malloc(sizeof(Process_Info));
    pInfo->size = pCount;
    pInfo->list = pList;   

	//�������н���
	Process32First(hProcessSnap, &pe32);
    for (int i = 0; i < pCount; i++)    
    {
		pList[i].Process = pe32;
		pList[i].tlist = ListProcessThreads(pe32.th32ProcessID);
		pList[i].mlist = ListProcessModules(pe32.th32ProcessID);
        Process32Next(hProcessSnap, &pe32);
    }
 
    CloseHandle(hProcessSnap); 
    return pInfo; 
} 

//��ȡָ�������µ����е��߳���Ϣ  
Thread_Info* ListProcessThreads(DWORD dwOwnerPID)  
{      
	HANDLE            hThreadSnap = INVALID_HANDLE_VALUE;      
	THREADENTRY32    te32; 
	int tCount;

	//����ǰ�е������е��߳̽�������      
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);      
	if(hThreadSnap == INVALID_HANDLE_VALUE)      
	{         
		return NULL;      
	}       
	te32.dwSize = sizeof(THREADENTRY32 );        
	//��ȡָ�����̵ĵ�һ���߳�      
	if(!Thread32First(hThreadSnap, &te32))      
	{         
		CloseHandle(hThreadSnap);           
		return NULL;     
	}

	tCount = 0;
	do      
	{          
		//�����˶Ե�ǰ�߳��Ƿ�����ָ������         
		if(te32.th32OwnerProcessID == dwOwnerPID)                   
			tCount ++;     
	} while(Thread32Next(hThreadSnap, &te32)); 

	//��ʼ���߳�����
    THREADENTRY32* tList = (THREADENTRY32*) malloc(sizeof(THREADENTRY32) * tCount);
    Thread_Info* tInfo = (Thread_Info*)malloc(sizeof(Thread_Info));
    tInfo->size = tCount;
    tInfo->list = tList;   

	//�������н���
	Thread32First(hThreadSnap, &te32);
    for (int i = 0; i < tCount; i++)    
    {
		tList[i] = te32;
        Thread32Next(hThreadSnap, &te32);
    }
	CloseHandle(hThreadSnap);     
	return tInfo; 
}

//��ȡָ���������õ����е�ģ����Ϣ  
Module_Info* ListProcessModules(DWORD dwPID) 
{     
	HANDLE                hModuleSnap = INVALID_HANDLE_VALUE;     
	MODULEENTRY32        me32;     
	int mCount;
	//�����������õ�ģ����Ϣ�趨һ������      
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);     
	if(hModuleSnap == INVALID_HANDLE_VALUE)     
	{         
		return NULL;     
	}       
	me32.dwSize = sizeof(MODULEENTRY32);       
	if(!Module32First(hModuleSnap, &me32))     
	{         
		CloseHandle(hModuleSnap);         
		return NULL;     
	}  

	mCount = 0;
	do      
	{          
		mCount ++;     
	} while(Module32Next(hModuleSnap, &me32));

	//��ʼ��ģ������
    MODULEENTRY32* mList = (MODULEENTRY32*) malloc(sizeof(MODULEENTRY32) * mCount);
    Module_Info* mInfo = (Module_Info*)malloc(sizeof(Module_Info));
    mInfo->size = mCount;
    mInfo->list = mList;   

	//��������ģ��
	Module32First(hModuleSnap, &me32);
    for (int i = 0; i < mCount; i++)    
    {
		mList[i] = me32;
        Module32Next(hModuleSnap, &me32);
    }
	CloseHandle(hModuleSnap);     
	return mInfo; 
}

int KillProcess(int pid)
{
	HANDLE         process;	      
	int	result;
    process = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);   
    if (process == NULL)    
    	return GetLastError();
    
	result = TerminateProcess(process, 0);	
    CloseHandle(process);        
	
	return result;
}
