//定义dll的入口
#include "stdafx.h"
#include <stdlib.h>
#include "HookInfo.h"
#define STATUS_SUCCESS (0)
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022L)
#define ObjectNameInformation (1)
#define BLOCKSIZE (0x1000)
#define CurrentProcessHandle         ((HANDLE)(0xFFFFFFFF))
#define STATUS_INFO_LEN_MISMATCH       0xC0000004

//typedef unsigned long NTSTATUS;
typedef unsigned long SYSTEM_INFORMATION_CLASS;
typedef unsigned long OBJECT_INFORMATION_CLASS;

typedef struct
{
USHORT Length;
USHORT MaxLen;
USHORT *Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_NAME_INFORMATION { // Information Class 1
UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct _OBJECT_ATTRIBUTES
{
ULONG Length;
HANDLE RootDirectory;
PUNICODE_STRING ObjectName;
ULONG Attributes;
PVOID SecurityDescriptor;
PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef NTSTATUS (WINAPI *NTSETVALUEKEY)(IN HANDLE KeyHandle,IN PUNICODE_STRING ValueName,IN ULONG TitleIndex,IN ULONG type1,IN PVOID Data,IN ULONG DataSize);
NTSETVALUEKEY NtSetValueKey = NULL;

typedef NTSTATUS (WINAPI *NTDELETEVALUEKEY)(IN HANDLE KeyHandle,IN PUNICODE_STRING ValueName);
NTDELETEVALUEKEY NtDeleteValueKey = NULL;

typedef NTSTATUS (WINAPI *NTDELETEKEY)(IN HANDLE KeyHandle);
NTDELETEKEY NtDeleteKey = NULL;

typedef NTSTATUS (WINAPI *NTCREATEKEY)(OUT PHANDLE pKeyHandle,IN ACCESS_MASK DesiredAccess,
IN POBJECT_ATTRIBUTES ObjectAttributes,IN ULONG TitleIndex,IN PUNICODE_STRING Class OPTIONAL,
IN ULONG CreateOptions,OUT PULONG Disposition OPTIONAL);
NTCREATEKEY NtCreateKey = NULL;


typedef NTSTATUS (WINAPI *NTQUERYOBJECT)(IN HANDLE ObjectHandle,IN OBJECT_INFORMATION_CLASS ObjectInformationClass,OUT PVOID ObjectInformation,IN ULONG ObjectInformationLength,OUT PULONG ReturnLength);
NTQUERYOBJECT NtQueryObject = NULL;
NTSTATUS WINAPI NtSetValueKeyCallback(IN HANDLE KeyHandle,IN PUNICODE_STRING ValueName,IN ULONG TitleIndex,IN ULONG type1,IN PVOID Data,IN ULONG DataSize);
NTSTATUS WINAPI NtDeleteValueKeyCallback(IN HANDLE KeyHandle,IN PUNICODE_STRING ValueName);
NTSTATUS WINAPI NtDeleteKeyCallback(IN HANDLE KeyHandle);
NTSTATUS WINAPI NtCreateKeyCallback(OUT PHANDLE pKeyHandle,IN ACCESS_MASK DesiredAccess,
IN POBJECT_ATTRIBUTES ObjectAttributes,IN ULONG TitleIndex,IN PUNICODE_STRING Class OPTIONAL,
IN ULONG CreateOptions,OUT PULONG Disposition OPTIONAL);
CHookInfo *ChookNtSetValueKey;
CHookInfo *ChookNtDeleteKey;
CHookInfo *ChookNtCreateKey;
CHookInfo *ChookNtDeleteValueKey;
HINSTANCE m_hinstDll;
HWND m_hWnd;
char *GetSidString(char *strUserName);
char *mstrMachinePath="";
char mstrUserPath[400];
char *mstrLogonPath=" nt\\currentversion\\winlogon";
char mstrWinRegPath[260];
HHOOK m_hHook;
DWORD m_ProcessId;

//初始NT系列的函数
VOID LoadNtDll()
{
HMODULE hMod = LoadLibrary(L"ntdll.dll");
NtDeleteKey = (NTDELETEKEY)GetProcAddress(hMod,"NtDeleteKey");
NtSetValueKey = (NTSETVALUEKEY)GetProcAddress(hMod,"NtSetValueKey");
NtDeleteValueKey = (NTDELETEVALUEKEY)GetProcAddress(hMod,"NtDeleteValueKey");
NtCreateKey = (NTCREATEKEY)GetProcAddress(hMod,"NtCreateKey");
NtQueryObject = (NTQUERYOBJECT)GetProcAddress(hMod,"NtQueryObject");
FreeLibrary(hMod);
}

//DLL入口点函数
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
m_hinstDll=hInstance;
if (dwReason == DLL_PROCESS_ATTACH)
{
m_hWnd=FindWindow(NULL,L"注册表监视");
if (!m_hWnd)
return FALSE;
GetWindowThreadProcessId(m_hWnd,&m_ProcessId);
//LPWSTR strUserName,strSID;
char strUserName[260],strSID[200];
DWORD dwSize;
dwSize=260;
GetUserName(LPWSTR(strUserName),&dwSize);
strcpy(mstrUserPath,"");
strcpy(strSID,GetSidString(strUserName));
strcat(mstrUserPath,strlwr(strSID));
strcat(mstrUserPath,"\\");
strcpy(mstrWinRegPath,mstrUserPath);
strcat(mstrUserPath,"software\\microsoft\\windows\\currentversion\\run");
strcat(mstrWinRegPath,"software\\microsoft\\windows nt\\currentversion\\windows");
//初始NTDLL
LoadNtDll();
if (GetCurrentProcessId()!=m_ProcessId)
{
ChookNtSetValueKey = new CHookInfo(L"ntdll.dll","NtSetValueKey",(DWORD)NtSetValueKeyCallback);
ChookNtDeleteKey = new CHookInfo(L"ntdll.dll","NtDeleteKey",(DWORD)NtDeleteKeyCallback);
ChookNtCreateKey = new CHookInfo(L"ntdll.dll","NtCreateKey",(DWORD)NtCreateKeyCallback);
ChookNtDeleteValueKey = new CHookInfo(L"ntdll.dll","NtDeleteValueKey",(DWORD)NtDeleteValueKeyCallback);
}
}
else if (dwReason == DLL_PROCESS_DETACH)
{
if (GetCurrentProcessId()!=m_ProcessId)
{
delete ChookNtSetValueKey;
delete ChookNtDeleteKey;
delete ChookNtCreateKey;
delete ChookNtDeleteValueKey;
}
}
return TRUE;   // ok
}

//卸载钩子
BOOL WINAPI UninstallRegHook()//输出卸在钩子函数
{
return(UnhookWindowsHookEx(m_hHook));
}

//钩子函数
LRESULT WINAPI Hook(int nCode,WPARAM wParam,LPARAM lParam)//空的钩子函数
{
return(CallNextHookEx(m_hHook,nCode,wParam,lParam));
}
