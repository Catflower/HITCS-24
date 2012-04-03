#include <windows.h> 
#include <stdio.h> 
#include <tlhelp32.h> 
#include <iostream>
#include "proinfo.h"

using namespace std; 
  
//获取到进程列表  
Process_Info* GetProcessList() 
{ 
    HANDLE             hProcessSnap; 
    PROCESSENTRY32     pe32; 
	int pCount;
  
    //对系统中当前所有的进程拍下快照  
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if(hProcessSnap == INVALID_HANDLE_VALUE) 
    { 
        return NULL; 
    } 
  
    //在使用 PROCESSENTRY32 结构之间需要先设置好该结构的大小  
    pe32.dwSize = sizeof(PROCESSENTRY32); 
  
    //获取第一个进程  
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
    
    //初始化进程链表
    Process_ID* pList = (Process_ID*) malloc(sizeof(Process_ID) * pCount);
    Process_Info* pInfo = (Process_Info*)malloc(sizeof(Process_Info));
    pInfo->size = pCount;
    pInfo->list = pList;   

	//遍历所有进程
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

//获取指定进程下的所有的线程信息  
Thread_Info* ListProcessThreads(DWORD dwOwnerPID)  
{      
	HANDLE            hThreadSnap = INVALID_HANDLE_VALUE;      
	THREADENTRY32    te32; 
	int tCount;

	//给当前行的下所有的线程进行拍照      
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);      
	if(hThreadSnap == INVALID_HANDLE_VALUE)      
	{         
		return NULL;      
	}       
	te32.dwSize = sizeof(THREADENTRY32 );        
	//获取指定进程的第一个线程      
	if(!Thread32First(hThreadSnap, &te32))      
	{         
		CloseHandle(hThreadSnap);           
		return NULL;     
	}

	tCount = 0;
	do      
	{          
		//用来核对当前线程是否属于指定进程         
		if(te32.th32OwnerProcessID == dwOwnerPID)                   
			tCount ++;     
	} while(Thread32Next(hThreadSnap, &te32)); 

	//初始化线程链表
    THREADENTRY32* tList = (THREADENTRY32*) malloc(sizeof(THREADENTRY32) * tCount);
    Thread_Info* tInfo = (Thread_Info*)malloc(sizeof(Thread_Info));
    tInfo->size = tCount;
    tInfo->list = tList;   

	//遍历所有进程
	Thread32First(hThreadSnap, &te32);
    for (int i = 0; i < tCount; i++)    
    {
		tList[i] = te32;
        Thread32Next(hThreadSnap, &te32);
    }
	CloseHandle(hThreadSnap);     
	return tInfo; 
}

//获取指定进程引用的所有的模块信息  
Module_Info* ListProcessModules(DWORD dwPID) 
{     
	HANDLE                hModuleSnap = INVALID_HANDLE_VALUE;     
	MODULEENTRY32        me32;     
	int mCount;
	//给进程所引用的模块信息设定一个快照      
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

	//初始化模块链表
    MODULEENTRY32* mList = (MODULEENTRY32*) malloc(sizeof(MODULEENTRY32) * mCount);
    Module_Info* mInfo = (Module_Info*)malloc(sizeof(Module_Info));
    mInfo->size = mCount;
    mInfo->list = mList;   

	//遍历所有模块
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
