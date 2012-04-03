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

//��ȡϵͳ��ǰ�����н���  
Process_Info* GetProcessList(); 
//��ȡ��ǰ�����µ����е��߳���Ϣ 
Thread_Info* ListProcessThreads(DWORD dwOwnerPID);
//��ȡָ�����������õ�ģ����Ϣ 
Module_Info* ListProcessModules(DWORD dwPID);
//��������
int KillProcess(int pid);