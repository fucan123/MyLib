#include "Peb.h"
#include <stdio.h>

// 伪装
void FakeCsrss()
{

	PPEB peb = (PPEB)GetPebBaseAddress64();
	PLDR_DATA_TABLE_ENTRY ldr = (PLDR_DATA_TABLE_ENTRY)peb->Ldr->InLoadOrderModuleList.Flink;

	memset(ldr->BaseDllName.Buffer, 0, ldr->BaseDllName.MaximumLength);
	memcpy(ldr->BaseDllName.Buffer, L"csrss.exe", 10 * 2);
	ldr->BaseDllName.Length = 10 * 2;
	ldr->BaseDllName.MaximumLength = ldr->BaseDllName.Length + 2;

	memset(ldr->FullDllName.Buffer, 0, ldr->FullDllName.MaximumLength);
	memcpy(ldr->FullDllName.Buffer, L"C:\\Windows\\System32\\csrss.exe", 30 * 2);
	ldr->FullDllName.Length = 30 * 2;
	ldr->FullDllName.MaximumLength = ldr->FullDllName.Length + 2;
}

// 获得进程PEB
ULONG32 GetPebBaseAddress32(DWORD pid)
{
	if (pid == 0)
		pid = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess)
		return 0;

	ULONG32 peb = NULL;
	pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetNtdllProcAddress_Self("NtQueryInformationProcess");
	PROCESS_BASIC_INFORMATION32 pbi32 = { 0 };
	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi32, sizeof(pbi32), NULL)))
	{
		peb = pbi32.PebBaseAddress;
	}

	return peb;
}


// 获得进程PEB
ULONG64 GetPebBaseAddress64(DWORD pid)
{
	if (pid == 0)
		pid = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess)
		return 0;

	ULONG64 peb = NULL;
	pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetNtdllProcAddress_Self("NtQueryInformationProcess");
	::printf("NtQueryInformationProcess:%p\n", NtQueryInformationProcess);
	PROCESS_BASIC_INFORMATION64 pbi64 = { 0 };
	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi64, sizeof(pbi64), NULL)))
	{
		peb = pbi64.PebBaseAddress;
	}

	return peb;
}

// 进程是否调试
BOOL IsDebugger32(ULONG32 peb)
{
	BOOL debug = FALSE;
	if (peb) {
		debug = *(PBYTE)(peb + 2);
	}
	else {
#ifndef  x64

		__asm {
			mov eax, fs:[0x30]
			movzx eax, byte ptr[eax + 2]
			mov byte ptr debug, al
		}
#endif // ! x64
	}
	return debug;
}

// 进程是否调试
BOOL IsDebugger64(ULONG64 peb)
{
	BOOL debug = FALSE;
	if (!peb) {
		peb = GetPebBaseAddress64();
	}
	if (peb) {
		debug = *(PBYTE)(peb + 2);
	}
	return debug;
}

// 清除调试标志
VOID ZeroDebugger32(ULONG32 peb)
{
	if (peb) {
		*(PBYTE)(peb + 2) = 0;
	}
}

// 清除调试标志
VOID ZeroDebugger64(ULONG64 peb)
{
	if (peb) {
		*(PBYTE)(peb + 2) = 0;
	}
}

// 获取NyGlobalFlag
ULONG GetNtGlobalFlag32(ULONG32 peb)
{
	return peb ? *(ULONG*)(peb + 0x68) : -1;
}

// 获取NyGlobalFlag
ULONG GetNtGlobalFlag64(ULONG64 peb)
{
	return peb ? *(ULONG*)(peb + 0xbc) : -1;
}

// 获取ntdll里面函数
FARPROC GetNtdllProcAddress_Self(LPCSTR name)
{
	return GetProcAddress(GetModuleHandle(L"ntdll.dll"), name);
}
