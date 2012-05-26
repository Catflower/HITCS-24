#if !defined(AFX_HOOKINFO_H__D44F115C_76F1_4CC7_BD61_4C393417DA10__INCLUDED_)
#define AFX_HOOKINFO_H__D44F115C_76F1_4CC7_BD61_4C393417DA10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _HOOKSTRUCT
{
FARPROC pfFunAddr; //用于保存API函数地址
BYTE    OldCode[5]; //保存原API前5个字节
BYTE    NewCode[5]; //JMP XXXX其中XXXXJMP的地址
}HOOKSTRUCT;

class CHookInfo
{
public:
//HOOK 处理函数
CHookInfo(LPCWSTR strDllName, LPCSTR strFunName, DWORD dwMyFunAddr);
virtual ~CHookInfo(); //析构函数
HOOKSTRUCT *pHook; //HOOK结构
void HookStatus(BOOL blnHook); //关闭/打开HOOK状态
};

CHookInfo::CHookInfo(LPCWSTR strDllName, LPCSTR strFunName, DWORD dwMyFunAddr)
{
pHook = new HOOKSTRUCT;
HMODULE hModule = LoadLibrary(strDllName);
//纪录函数地址
pHook->pfFunAddr = GetProcAddress(hModule,strFunName);
FreeLibrary(hModule);
if(pHook->pfFunAddr == NULL)
return ;
//备份原函数的前5个字节，一般的WIN32 API以__stdcall声明的API理论上都可以这样进行HOOK
memcpy(pHook->OldCode, pHook->pfFunAddr, 5);
pHook->NewCode[0] = 0xe9; //构造JMP
DWORD dwJmpAddr = dwMyFunAddr - (DWORD)pHook->pfFunAddr - 5; //计算JMP地址
memcpy(&pHook->NewCode[1], &dwJmpAddr, 4);
HookStatus(TRUE);//开始进行HOOK
}

CHookInfo::~CHookInfo()
{
//关闭HOOK恢复原函数
HookStatus(FALSE);
}

void CHookInfo::HookStatus(BOOL blnHook)
{
if(blnHook)
WriteProcessMemory((HANDLE)-1, pHook->pfFunAddr, pHook->NewCode, 5, 0);//替换函数地址
else
WriteProcessMemory((HANDLE)-1, pHook->pfFunAddr, pHook->OldCode, 5, 0);//还原函数地址
}
#endif // !defined(AFX_HOOKINFO_H__1967D554_7A9F_40C5_9D86_5899019EB3CD__INCLUDED_)
