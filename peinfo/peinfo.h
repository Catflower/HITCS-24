#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <stdio.h> 
#include <tlhelp32.h>
#include <iostream>

struct Export_Info
{
	int size;
	THREADENTRY32* list;
};

Export_Info *Export_list;
unsigned long muVAddr,muPAddr;
LPVOID   hMapAddress;
IMAGE_DOS_HEADER   m_dosHeader;
IMAGE_NT_HEADERS   m_ntHeader;
IMAGE_SECTION_HEADER m_secHeader;

unsigned long RvaToOffset(unsigned long uRvaAddr);
int isPeFile(char* strPath);
void getExportTable(char* strPath);

