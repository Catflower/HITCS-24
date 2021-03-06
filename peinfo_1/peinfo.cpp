#include <windows.h> 
#include <stdio.h> 
#include <tlhelp32.h> 
#include <iostream>
#include "peinfo.h"

using namespace std; 

unsigned long RvaToData(LPVOID lpBaseAddr, unsigned long VirtualAddress)
{

	//定位到节表
	IMAGE_DOS_HEADER *lpDosHeader=(IMAGE_DOS_HEADER *)lpBaseAddr;
	
	IMAGE_NT_HEADERS *lpNtHeader=(IMAGE_NT_HEADERS*)((BYTE*)lpBaseAddr+
		lpDosHeader->e_lfanew);
	 
	DWORD dwRva=0;;
 
    IMAGE_SECTION_HEADER *SectionHeader=NULL;

	DWORD dwSecCount=m_ntHeader.FileHeader.NumberOfSections;//节区数量

	for (DWORD dwCount=0;dwCount<dwSecCount;dwCount++)
	{

		SectionHeader=(IMAGE_SECTION_HEADER*)((int)&lpNtHeader->OptionalHeader+
		                             lpNtHeader->FileHeader.SizeOfOptionalHeader);
		if (VirtualAddress > SectionHeader[dwCount].VirtualAddress && VirtualAddress <
		       SectionHeader[dwCount].VirtualAddress+SectionHeader[dwCount].SizeOfRawData)
		{
			dwRva=VirtualAddress-SectionHeader[dwCount].VirtualAddress;
			dwRva+=SectionHeader[dwCount].PointerToRawData;
			return dwRva;
		}
	}
	return dwRva; 	
}

unsigned long RvaToOffset(unsigned long uRvaAddr)
{
	unsigned long dwReturn=0;
	dwReturn=uRvaAddr - muVAddr;
	if (dwReturn>0)
		dwReturn = muPAddr + dwReturn;
	else
		dwReturn=-1;
	return dwReturn;
}


int isPeFile(LPCWSTR strPath)
{
	HANDLE h;
	HANDLE hMapFile;

	PIMAGE_DOS_HEADER myDosHeader=NULL;
	PIMAGE_NT_HEADERS myNTHeader=NULL;
	h = CreateFile(strPath,            
		GENERIC_ALL,                
		FILE_SHARE_READ | FILE_SHARE_WRITE ,           
		NULL,                       
		OPEN_EXISTING,             
		FILE_ATTRIBUTE_NORMAL,     
		NULL);                     
	
	if (h == INVALID_HANDLE_VALUE) 
	{   
		 return -1;
	}  


	hMapFile = CreateFileMapping(h,     
		NULL,                             
		PAGE_READWRITE,                   
		0,                                
		0,                               
		TEXT("MyFileMappingObject"));           
	
	if (hMapFile == NULL) 
	{ 
		 return -1;
	} 
	
	hMapAddress = MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,             
		0,                               
		0,                               
		0);                               
	
	if (hMapAddress == NULL) 
	{ 
		 return -1;
	} 
    
	CloseHandle(hMapFile);
	CloseHandle(h);

    myDosHeader=(IMAGE_DOS_HEADER *)hMapAddress;
	myNTHeader=(IMAGE_NT_HEADERS*)((DWORD)hMapAddress+myDosHeader->e_lfanew);
 
	if (myDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
	{
		 return 0;
	}

	if (myNTHeader->Signature!=IMAGE_NT_SIGNATURE)
	{
		 return 0;
	}
	muVAddr=0;
	muPAddr=0;

	memcpy(&m_dosHeader,hMapAddress,sizeof(IMAGE_DOS_HEADER));
	memcpy(&m_ntHeader,myNTHeader,sizeof(IMAGE_NT_HEADERS));

	return 1;
}



void getExportTable()
{
	unsigned long dwOffset;
	char* strName;
	int intStup=0;
	void* temp;
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)new BYTE[sizeof(IMAGE_EXPORT_DIRECTORY)];
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)new BYTE[sizeof(IMAGE_FILE_HEADER)];
	PIMAGE_OPTIONAL_HEADER pOptional = (PIMAGE_OPTIONAL_HEADER)new BYTE[sizeof(IMAGE_OPTIONAL_HEADER)];
	PIMAGE_SECTION_HEADER pSection = NULL;

	temp=(IMAGE_FILE_HEADER*)((DWORD)hMapAddress+m_dosHeader.e_lfanew+4);
	memcpy(pFileHeader,temp,sizeof(IMAGE_FILE_HEADER));

	temp=(IMAGE_OPTIONAL_HEADER*)((DWORD)pFileHeader+sizeof(IMAGE_FILE_HEADER));
	memcpy(pOptional,temp,sizeof(IMAGE_OPTIONAL_HEADER));

	pSection=(IMAGE_SECTION_HEADER*)new BYTE[sizeof(IMAGE_SECTION_HEADER) * pFileHeader->NumberOfSections];
	temp=(IMAGE_SECTION_HEADER*)((DWORD)pOptional+sizeof(IMAGE_OPTIONAL_HEADER));
	memcpy(pSection,temp,sizeof(IMAGE_SECTION_HEADER) * pFileHeader->NumberOfSections);

	printf("\tNumOfSecs:%d",pFileHeader->NumberOfSections);
    
	for (intStup=0;intStup<pFileHeader->NumberOfSections;intStup++)
	{
		if (pSection[intStup].VirtualAddress!=0 && pSection[intStup].PointerToRawData!=0)
		{
			muVAddr=pSection[intStup].VirtualAddress;
			muPAddr = pSection[intStup].PointerToRawData;
			break;
		}
	}
	if (!(muVAddr!=0 && muPAddr!=0))
	{
		return ;
	}
	if (pOptional->DataDirectory[0].VirtualAddress==0)
	{
		printf("\t没有导出表信息!!\n");
		return ;
	}
	dwOffset=RvaToOffset(pOptional->DataDirectory[0].VirtualAddress);

	temp=(IMAGE_EXPORT_DIRECTORY*)((DWORD)hMapAddress+dwOffset);
	memcpy(pExport,temp,sizeof(IMAGE_EXPORT_DIRECTORY));

	printf("\tNumOfFuncs:%d\n",pExport->NumberOfFunctions);

	dwOffset=RvaToOffset(pExport->Name);

	strName=(char*)new BYTE[128];
	memset(strName,0,sizeof(strName));
	temp=(char*)((DWORD)hMapAddress+dwOffset);
	memcpy(strName,temp,sizeof(pExport->Name));

	printf("模块：%s\n",strName);
	intStup=0;
	do 
	{
		if (intStup>=(int)pExport->NumberOfFunctions) break;
		dwOffset=RvaToOffset(pExport->AddressOfNames + intStup * sizeof(intStup));
		if (dwOffset==0) break;

		temp=(char*)((DWORD)hMapAddress+dwOffset);
	    memcpy(&dwOffset,temp,sizeof(dwOffset));

		dwOffset=RvaToOffset(dwOffset);
		if (dwOffset==0) break;

		temp=(char*)((DWORD)hMapAddress+dwOffset);
		memset(strName,0,sizeof(strName));
	    memcpy(strName,temp,sizeof(strName));

		printf("\t函数：%s\n",strName);
		intStup ++;
	} while (dwOffset!=0 && intStup < 15);

	delete []pExport;
	delete []pFileHeader;
	delete []pOptional;
	delete []pSection;
}

void getImportTable()
{
    //定位到第一个导入表
    int nRowCount=0;
	IMAGE_IMPORT_BY_NAME *ImportName;
     
	IMAGE_IMPORT_DESCRIPTOR *lpImport=(IMAGE_IMPORT_DESCRIPTOR *)((BYTE*)hMapAddress+
		                     RvaToData(hMapAddress,m_ntHeader.OptionalHeader.
		            DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));
 
	while(lpImport->FirstThunk)
	{
         //得到DLL名
        string csTemp;
		char*pDllName=(char*)((BYTE*)hMapAddress+RvaToData(hMapAddress,lpImport->Name));
        
    	//得到序号
		PIMAGE_THUNK_DATA pThunk=(PIMAGE_THUNK_DATA)((BYTE*)hMapAddress+RvaToData(hMapAddress,
			              lpImport->OriginalFirstThunk));
        
    
		while(pThunk->u1.Function)
		{
			   if (pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32)
			   {
					//序号方式
 				    printf("Dll名称：%s",pDllName);
					printf("函数名称：NULL");
					printf("函数序号：%d",pThunk->u1.Ordinal&0xFFFF);
 				    printf("函数地址：%p",pThunk->u1.Function); 

			   }
			   else
			   {
					ImportName=(IMAGE_IMPORT_BY_NAME *)((BYTE*)hMapAddress+RvaToData(hMapAddress,
							  (DWORD)pThunk->u1.AddressOfData));

				   //名称方式
				    printf("Dll名称：%s",pDllName);
					printf("函数名称：%s",(char*)ImportName->Name);
					printf("函数序号：NULL");
 				    printf("函数地址：%p",pThunk->u1.Function);
			   }
			   pThunk++;  
		}
		lpImport++;
	}
}

