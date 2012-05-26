#if !defined(AFX_HOOKINFO_H__D44F115C_76F1_4CC7_BD61_4C393417DA10__INCLUDED_)
#define AFX_HOOKINFO_H__D44F115C_76F1_4CC7_BD61_4C393417DA10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _HOOKSTRUCT
{
FARPROC pfFunAddr; //���ڱ���API������ַ
BYTE    OldCode[5]; //����ԭAPIǰ5���ֽ�
BYTE    NewCode[5]; //JMP XXXX����XXXXJMP�ĵ�ַ
}HOOKSTRUCT;

class CHookInfo
{
public:
//HOOK ������
CHookInfo(LPCWSTR strDllName, LPCSTR strFunName, DWORD dwMyFunAddr);
virtual ~CHookInfo(); //��������
HOOKSTRUCT *pHook; //HOOK�ṹ
void HookStatus(BOOL blnHook); //�ر�/��HOOK״̬
};

CHookInfo::CHookInfo(LPCWSTR strDllName, LPCSTR strFunName, DWORD dwMyFunAddr)
{
pHook = new HOOKSTRUCT;
HMODULE hModule = LoadLibrary(strDllName);
//��¼������ַ
pHook->pfFunAddr = GetProcAddress(hModule,strFunName);
FreeLibrary(hModule);
if(pHook->pfFunAddr == NULL)
return ;
//����ԭ������ǰ5���ֽڣ�һ���WIN32 API��__stdcall������API�����϶�������������HOOK
memcpy(pHook->OldCode, pHook->pfFunAddr, 5);
pHook->NewCode[0] = 0xe9; //����JMP
DWORD dwJmpAddr = dwMyFunAddr - (DWORD)pHook->pfFunAddr - 5; //����JMP��ַ
memcpy(&pHook->NewCode[1], &dwJmpAddr, 4);
HookStatus(TRUE);//��ʼ����HOOK
}

CHookInfo::~CHookInfo()
{
//�ر�HOOK�ָ�ԭ����
HookStatus(FALSE);
}

void CHookInfo::HookStatus(BOOL blnHook)
{
if(blnHook)
WriteProcessMemory((HANDLE)-1, pHook->pfFunAddr, pHook->NewCode, 5, 0);//�滻������ַ
else
WriteProcessMemory((HANDLE)-1, pHook->pfFunAddr, pHook->OldCode, 5, 0);//��ԭ������ַ
}
#endif // !defined(AFX_HOOKINFO_H__1967D554_7A9F_40C5_9D86_5899019EB3CD__INCLUDED_)
