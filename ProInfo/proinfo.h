#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

struct Thread_Info
{
	int size;
	THREADENTRY32* list;
};

struct Module_Info
{
	int size;
	MODULEENTRY32* list;
};

struct Process_ID
{
	PROCESSENTRY32 Process;
	Thread_Info* tlist;
	Module_Info* mlist;
};

struct Process_Info
{
    int size;
    Process_ID* list;
};

//获取系统当前的所有进程  
Process_Info* GetProcessList(); 
//获取当前进程下的所有的线程信息 
Thread_Info* ListProcessThreads(DWORD dwOwnerPID);
//获取指定进程所引用的模块信息 
Module_Info* ListProcessModules(DWORD dwPID);
//结束进程
int KillProcess(int pid);