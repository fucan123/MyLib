#pragma once
#include <Windows.h>

// ��ȡPE�ļ���С
DWORD GetPEFileRealSize(LPBYTE lpBuffer);
// ���DLL���溯����ַ
PVOID GetDllFunctionAddress32(PCWSTR lpFunctionName, PVOID BaseAddress, PVOID xxx=0);

PVOID GetDllFunctionAddress32_2(PCWSTR lpFunctionName, PVOID BaseAddress);

// ���DLL���溯����ַ
DWORD GetDllFunctionAddress32(DWORD pid, PCSTR lpFunctionName, HANDLE hMod);