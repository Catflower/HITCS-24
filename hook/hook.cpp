#include <windows.h>
#pragma argsused

//�Զ���APIHOOK�ṹ
typedef struct
{
	FARPROC funcaddr;
	BYTE olddata[5];
	BYTE newdata[5];
}HOOKSTRUCT;

HHOOK g_hHook;
HINSTANCE g_hinstDll;
HMODULE hModule ;
HANDLE g_hForm; //������Ϣ���ھ��
DWORD dwIdOld, dwIdNew;

HOOKSTRUCT HookStruct;
void HookOn();
void HookOff();
BOOL Init();
extern "C" __declspec(dllexport) __stdcall BOOL InstallHook();
extern "C" __declspec(dllexport) __stdcall BOOL UninstallHook();
BOOL HookAPI(char *dllname, char *procname, DWORD myfuncaddr, HOOKSTRUCT *hookfunc);
int WINAPI  MyMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

// ��������ṹ�ػ�API
BOOL HookAPI(char *dllname, char *procname, DWORD myfuncaddr, HOOKSTRUCT *hookfunc)
{
	hModule = LoadLibrary(dllname);
	if(hModule)
	{
		hookfunc->funcaddr = GetProcAddress(hModule, procname);
		if(hookfunc->funcaddr == NULL)
			return false;

		memcpy(hookfunc->olddata, hookfunc->funcaddr, 6);
		hookfunc->newdata[0] = 0xe9;
		DWORD jmpaddr = myfuncaddr - (DWORD)hookfunc->funcaddr - 5;
		memcpy(&hookfunc->newdata[1], &jmpaddr, 5);
		return true;
	}
	else
	{
		return false;
    }
}

BOOL Init()
{
	HookAPI("user32.dll", "MessageBoxA", (DWORD)MyMessageBoxA, &HookStruct);
	dwIdNew = GetCurrentProcessId(); // �õ��������̵�ID
	dwIdOld = dwIdNew;
	HookOn(); // ��ʼ����
	return true;
}

//�յĹ��Ӻ���
LRESULT WINAPI Hook(int nCode, WPARAM wParam, LPARAM lParam)
{
	return(CallNextHookEx(g_hHook, nCode, wParam, lParam));
}


//��װ����
extern "C" __declspec(dllexport) __stdcall BOOL InstallHook()
{
	g_hHook = SetWindowsHookEx(WH_MOUSE, (HOOKPROC)Hook, g_hinstDll, 0);//��װΪһ��ȫ����Ϣ����
	if (!g_hHook)
	{
		MessageBoxA(NULL, "��װ����ʧ��!", "����", MB_OK);
		return(false);
	}
	return(true);
}

//ж�ع���
extern "C" __declspec(dllexport) __stdcall BOOL UninstallHook()
{
	HookOff();
	if(g_hHook == NULL)
		return true;
	return(UnhookWindowsHookEx(g_hHook));
}

void HookOnOne(HOOKSTRUCT *hookfunc)
{
	HANDLE hProc;
	dwIdOld = dwIdNew;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwIdOld);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, PAGE_READWRITE,&dwIdOld);
	WriteProcessMemory(hProc, hookfunc->funcaddr, hookfunc->newdata, 5, 0);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, dwIdOld, &dwIdOld);
}

void HookOn()
{
	HookOnOne(&HookStruct);
}

void HookOffOne(HOOKSTRUCT *hookfunc)
{
	HANDLE hProc;
	dwIdOld = dwIdNew;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwIdOld);
	VirtualProtectEx(hProc, hookfunc->funcaddr,5, PAGE_READWRITE, &dwIdOld);
	WriteProcessMemory(hProc, hookfunc->funcaddr, hookfunc->olddata, 5, 0);
	VirtualProtectEx(hProc, hookfunc->funcaddr, 5, dwIdOld, &dwIdOld);
}

void HookOff()
{
	HookOffOne(&HookStruct);
}

//�����滻API�������Զ��庯��
int WINAPI MyMessageBoxA(HWND hWnd, LPCTSTR lpText,LPCTSTR lpCaption, UINT uType)
{
	int nReturn;
	HookOff();	//���ȹر����أ�Ȼ����ܵ��ñ����ص�Api����
    nReturn = MessageBoxA(hWnd, "���Թ����е�����", lpCaption, MB_OK | MB_ICONINFORMATION);
    HookOn();
	return(nReturn);
}

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	 switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			g_hinstDll = hinst;//��ȡ��ǰ���̵ľ��,������װ����
			g_hForm = FindWindow(NULL, "ZwelL");
			if(!Init())
			{
				MessageBoxA(NULL,"��ʼ������ʧ��","����",MB_OK);
				return(false);
			}
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			UninstallHook();
			break;
	}
	return TRUE;
}
