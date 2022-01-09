#pragma once
#include <Windows.h>

// 获取PE文件大小
DWORD GetPEFileRealSize(LPBYTE lpBuffer);
// 获得DLL里面函数地址
PVOID GetDllFunctionAddress32(PCWSTR lpFunctionName, PVOID BaseAddress, PVOID xxx=0);

PVOID GetDllFunctionAddress32_2(PCWSTR lpFunctionName, PVOID BaseAddress);

// 获得DLL里面函数地址
DWORD GetDllFunctionAddress32(DWORD pid, PCSTR lpFunctionName, HANDLE hMod);