#include "StdAfx.h"
#include "HookAPI.h"
#include "Dbghelp.h"

#pragma comment(lib , "imagehlp.lib")
#pragma comment(lib , "Dbghelp.lib")

CHookAPI::CHookAPI(void)
{
 GetSystemInfo(&m_si) ;
}


CHookAPI::~CHookAPI(void)
{
}

////

HRESULT CHookAPI::AddHookAPIEntry(const HookAPIEntry & entry ) {
 // �Ѿ�������,�Ͳ��������
 if( S_OK == FindHookAPIEntry(entry.strMoudleName,entry.strFunctionName,NULL ) )
  return S_FALSE ;
 LPHookAPIEntry pentry = new HookAPIEntry() ;
 * pentry = entry ;
 m_HookAPIEntryList.push_back(pentry) ;

 UpdateOriginalProc(*pentry) ;
 HookMoudleAPI(entry);
 return S_OK ;
}
HRESULT CHookAPI::AddHookAPIEntry(CString strMoudleName,CString strFunctionName,PROC pNewProc) {
 if( S_OK == FindHookAPIEntry(strMoudleName,strFunctionName,NULL ) )
  return S_FALSE ;
 LPHookAPIEntry pentry = new HookAPIEntry() ;
 pentry->strMoudleName = strMoudleName ;
 pentry->strFunctionName = strFunctionName ;
 pentry->pNewProc = pNewProc ;
 m_HookAPIEntryList.push_back(pentry) ;

 UpdateOriginalProc(*pentry) ;
 HookMoudleAPI(*pentry);
 return S_OK ;
}
HRESULT CHookAPI::FindHookAPIEntry(CString strMoudleName,CString strFunctionName,LPHookAPIEntry * pResult){
 HookAPIEntryPos pos ;
 for( pos = m_HookAPIEntryList.begin(); pos != m_HookAPIEntryList.end() ; pos ++ ){
  PHookAPIEntry pHookAPIEntry = * pos ;
  if( NULL != pHookAPIEntry &&
   pHookAPIEntry->strMoudleName.CompareNoCase(strMoudleName) == 0 &&
   pHookAPIEntry->strFunctionName.Compare(strFunctionName) == 0 ){
    // �Ѿ��ҵ���
    if( pResult == NULL )
     * pResult = pHookAPIEntry ;
    return S_OK ;
  }
 };
 return S_FALSE ;
}
HRESULT CHookAPI::DeleteHookAPIEntry(CString strMoudleName,CString strFunctionName) {
 HookAPIEntryPos pos ;
 for( pos = m_HookAPIEntryList.begin(); pos != m_HookAPIEntryList.end() ; pos ++ ){
  PHookAPIEntry pHookAPIEntry = * pos ;
  if( NULL != pHookAPIEntry &&
   pHookAPIEntry->strMoudleName.CompareNoCase(strMoudleName) == 0 &&
   pHookAPIEntry->strFunctionName.Compare(strFunctionName) == 0 ){
    // �Ѿ��ҵ���
    UnHookMoudleAPI(*pHookAPIEntry) ;

    m_HookAPIEntryList.erase(pos) ;
    delete pHookAPIEntry ;
    pHookAPIEntry = NULL ;
    return S_OK ;
  }
 };
 return S_OK ;
}
HRESULT CHookAPI::ClearAllAPIEntry( void ) {
 UnHookAllMoudleAPI() ;

 HookAPIEntryPos pos ;
 PHookAPIEntry pHookAPIEntry = NULL ;
 while( m_HookAPIEntryList.size() > 0 ) {
  pos = m_HookAPIEntryList.begin() ;
  pHookAPIEntry = * pos ;

  m_HookAPIEntryList.erase(pos) ;
  if( NULL == pHookAPIEntry ){
   delete pHookAPIEntry ;
   pHookAPIEntry = NULL ;
  }
 }
 return S_OK ;
}


HRESULT CHookAPI::GetHookAPIEntryByNewAddress(PROC pNewAddress,LPHookAPIEntry * pResult ) {
 HookAPIEntryPos pos ;
 for( pos = m_HookAPIEntryList.begin(); pos != m_HookAPIEntryList.end() ; pos ++ ){
  PHookAPIEntry pHookAPIEntry = * pos ;
  if( NULL != pHookAPIEntry &&
   pHookAPIEntry->pNewProc ==  pNewAddress){
    // �Ѿ��ҵ���
    if( NULL != pResult ){
     * pResult = * pos ;
     return S_OK ;
    }
  }
 };
 return S_FALSE ;
}

HRESULT CHookAPI::UpdateOriginalProc( HookAPIEntry & hookentry){
 if(S_OK != GetProcessAddress(hookentry.strMoudleName,
  hookentry.strFunctionName,&hookentry.pOriginalProc)){
   ASSERT(FALSE) ;
   return S_FALSE ;
 }

 if (hookentry.pOriginalProc > m_si.lpMaximumApplicationAddress)  {
  PBYTE pb = (PBYTE) hookentry.pOriginalProc;
  if (pb[0] == 0x86){
   PVOID pv = * (PVOID*) &pb[1];
   hookentry.pOriginalProc = (PROC) pv;
  }
 }
 return S_OK ;
}

HRESULT CHookAPI::GetProcessAddress(const CString &strMoudleName,const CString&strFunctionName,FARPROC * pAddress ){
 * pAddress = ::GetProcAddress(GetModuleHandle(strMoudleName), strFunctionName) ;
 return S_OK ;
}

HRESULT CHookAPI::ReHookAllMoudleAPI( void ) {
 CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());
 MODULEENTRY32 me = { sizeof(me) };
 for (BOOL fOk = th.ModuleFirst(&me); fOk; fOk = th.ModuleNext(&me))
 {
  //TRACE(me.szModule);
  // ������е��й��ļ���д���
  HookAPIEntryPos pos ;
  for( pos = m_HookAPIEntryList.begin(); pos != m_HookAPIEntryList.end() ; pos ++ ){
   PHookAPIEntry pHookAPIEntry = * pos ;
   if(pHookAPIEntry == NULL ){
    ASSERT(FALSE) ;
    continue ;
   }
   // ���»�ȡһ��ԭ���ĵ�ַ
   if( pHookAPIEntry->pOriginalProc == NULL ){
    if(S_OK != UpdateOriginalProc(*pHookAPIEntry)){
     ASSERT(FALSE) ;
     continue ;
    }
   }
   // ������ģ������������滻
   HookMoudleAPI(me.hModule,*pHookAPIEntry) ;
  }

 };
 return S_OK ;
}

HRESULT CHookAPI::UnHookAllMoudleAPI( void ) {
 CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());
 MODULEENTRY32 me = { sizeof(me) };
 for (BOOL fOk = th.ModuleFirst(&me); fOk; fOk = th.ModuleNext(&me))
 {
  //TRACE(me.szModule);
  // ������е��й��ļ���д���
  HookAPIEntryPos pos ;
  for( pos = m_HookAPIEntryList.begin(); pos != m_HookAPIEntryList.end() ; pos ++ ){
   PHookAPIEntry pHookAPIEntry = * pos ;
   if(pHookAPIEntry == NULL ){
    ASSERT(FALSE) ;
    continue ;
   }
   // ���»�ȡһ��ԭ���ĵ�ַ
   if( pHookAPIEntry->pOriginalProc == NULL ){
    if(S_OK != UpdateOriginalProc(*pHookAPIEntry)){
     ASSERT(FALSE) ;
     continue ;
    }
   }
   // ������ģ������������滻
   UnHookMoudleAPI(me.hModule,*pHookAPIEntry) ;
  }

 };
 return S_OK ;
}

HRESULT CHookAPI::HookMoudleAPI(const HookAPIEntry & hookentry){

 CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());
 MODULEENTRY32 me = { sizeof(me) };
 for (BOOL fOk = th.ModuleFirst(&me); fOk; fOk = th.ModuleNext(&me))
 {
  //TRACE(me.szModule);
  // ������е��й��ļ���д���
  HookMoudleAPI(me.hModule,hookentry) ;
 };
 return S_OK ;
}

HRESULT CHookAPI::HookMoudleAPI(HMODULE hmoudle,const HookAPIEntry & hookentry){
 return ReplaceMoudleAPI(hmoudle,hookentry.strMoudleName,hookentry.pOriginalProc,hookentry.pNewProc);
}

HRESULT CHookAPI::UnHookMoudleAPI(const HookAPIEntry & hookentry){
 CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());
 MODULEENTRY32 me = { sizeof(me) };
 for (BOOL fOk = th.ModuleFirst(&me); fOk; fOk = th.ModuleNext(&me))
 {
  //TRACE(me.szModule);
  // ������е��й��ļ���д���
  UnHookMoudleAPI(me.hModule,hookentry) ;
 };
 return S_OK ;
}

HRESULT CHookAPI::UnHookMoudleAPI(HMODULE hmoudle,const HookAPIEntry & hookentry){
 return ReplaceMoudleAPI(hmoudle,hookentry.strMoudleName,hookentry.pNewProc,hookentry.pOriginalProc);
}

HRESULT CHookAPI::ReplaceMoudleAPI(HMODULE hmoduleCaller, const CString &strMoudleName,
  PROC pCurAddress,PROC pNewAddress){
 // �ҵ���IAT��
 ULONG lsize = 0 ;
 PIMAGE_SECTION_HEADER psecHeader ;
 PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
  ImageDirectoryEntryToDataEx(hmoduleCaller,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&lsize,&psecHeader);
 if( pImportDesc == NULL ){
  // ASSERT(FALSE) ;
  return S_FALSE ;
 }

 // ����������Ӧ��ģ��
 while( pImportDesc->Name != 0){
  // RVA
  CString strdllname = (PSTR)((PBYTE)hmoduleCaller + pImportDesc->Name);
  if( strdllname.CompareNoCase(strMoudleName) == 0 )
   break ;
  pImportDesc ++ ;
 }

 if( pImportDesc->Name == 0){
  //ASSERT(FALSE) ;
  return S_FALSE ;
 }
 // �������ĺ�����ַ,FirstThunkΪRVA
 PIMAGE_THUNK_DATA pThunk=(PIMAGE_THUNK_DATA)((PBYTE)hmoduleCaller + pImportDesc->FirstThunk);
 PROC* ppfn;
 while( pThunk->u1.Function != 0 ){
  ppfn = (PROC*)&pThunk->u1.Function;
  if( *ppfn == pCurAddress ){
   // �ҵ������������еĺ�����ַ�������޸�һ��
   MEMORY_BASIC_INFORMATION mbi;
   VirtualQuery(ppfn,&mbi,sizeof(MEMORY_BASIC_INFORMATION));
   VirtualProtect(mbi.BaseAddress,mbi.RegionSize,PAGE_READWRITE,&mbi.Protect);
   *ppfn=*pNewAddress;
   DWORD dwOldProtect;
   VirtualProtect(mbi.BaseAddress,mbi.RegionSize,mbi.Protect,&dwOldProtect);
   return S_OK ;
  }
  pThunk ++ ;
 }

 return S_OK ;

}
