#pragma once
#include<vector>
#include <windows.h>
#include <tlhelp32.h>
#include "stdafx.h"
#include <afx.h>

struct HookAPIEntry{
 // 这个是模块的名字,比如usr32.dll
 CString  strMoudleName ;
 // 这个是需要下钩子的API函数
 CString  strFunctionName ;
 // API函数的原始地址
 PROC  pOriginalProc ;
 // API函数的新地址
 PROC  pNewProc ;
 HookAPIEntry(){
  pOriginalProc = pNewProc = NULL ;
 };
};

typedef HookAPIEntry * PHookAPIEntry ;
typedef HookAPIEntry * LPHookAPIEntry ;

typedef std::vector<PHookAPIEntry> HookAPIEntryList ;
typedef HookAPIEntryList * PHookAPIEntryList ;
typedef HookAPIEntryList * LPHookAPIEntryList ;
typedef std::vector<PHookAPIEntry>::iterator HookAPIEntryPos ;

class CHookAPI
{
public:
 CHookAPI(void);
 ~CHookAPI(void);

public:
 // 对钩子函数列表的操作
 HRESULT  AddHookAPIEntry(const HookAPIEntry & entry ) ;
 HRESULT  AddHookAPIEntry(CString strMoudleName,CString strFunctionName,PROC pNewProc) ;
 HRESULT  FindHookAPIEntry(CString strMoudleName,CString strFunctionName,LPHookAPIEntry * pResult);
 HRESULT  DeleteHookAPIEntry(CString strMoudleName,CString strFunctionName) ;
 HRESULT  ClearAllAPIEntry( void ) ;

 HRESULT  ReHookAllMoudleAPI( void ) ;
 HRESULT  UnHookAllMoudleAPI( void ) ;

 HRESULT  GetHookAPIEntryByNewAddress(PROC pNewAddress,LPHookAPIEntry * pResult) ;
protected:
 HRESULT  HookMoudleAPI(const HookAPIEntry & hookentry);
 HRESULT  UnHookMoudleAPI(const HookAPIEntry & hookentry);
 HRESULT  HookMoudleAPI(HMODULE hmoudle,const HookAPIEntry & hookentry);
 HRESULT  UnHookMoudleAPI(HMODULE hmoudle,const HookAPIEntry & hookentry);
 HRESULT  ReplaceMoudleAPI(HMODULE hmoudleCaller, const CString &strMoudleName,
  PROC pCurAddress,PROC pNewAddress);

 HRESULT  GetProcessAddress(const CString &strMoudleName,const CString &strFunctionName,
  PROC * pAddress ) ;
 HRESULT  UpdateOriginalProc( HookAPIEntry & hookentry);

protected:
 // 需要下钩子的函数集合
 HookAPIEntryList   m_HookAPIEntryList ;
 // 系统的信息
 SYSTEM_INFO     m_si;
};
