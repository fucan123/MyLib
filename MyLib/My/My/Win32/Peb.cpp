#include "Peb.h"
#include <stdio.h>

// ��ý���PEB
ULONG32 GetPebBaseAddress32(DWORD pid)
{
	if (pid == 0)
		pid = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess)
		return 0;

	ULONG32 peb = NULL;
	pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetNtdllProcAddress("NtQueryInformationProcess");
	PROCESS_BASIC_INFORMATION32 pbi32 = { 0 };
	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi32, sizeof(pbi32), NULL)))
	{
		peb = pbi32.PebBaseAddress;
	}

	return peb;
}


// ��ý���PEB
ULONG64 GetPebBaseAddress64(DWORD pid)
{
	if (pid == 0)
		pid = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess)
		return 0;

	ULONG64 peb = NULL;
	pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetNtdllProcAddress("NtWow64QueryInformationProcess64");
	PROCESS_BASIC_INFORMATION64 pbi64 = { 0 };
	if (NT_SUCCESS(NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi64, sizeof(pbi64), NULL)))
	{
		peb = pbi64.PebBaseAddress;
	}

	return peb;
}

// �����Ƿ����
BOOL IsDebugger32(ULONG32 peb)
{
	BOOL debug = FALSE;
	if (peb) {
		debug = *(PBYTE)(peb + 2);
	}
	else {
		__asm {
			mov eax, fs:[0x30]
			movzx eax, byte ptr[eax + 2]
			mov byte ptr debug, al
		}
	}
	return debug;
}

// �����Ƿ����
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

// ������Ա�־
VOID ZeroDebugger32(ULONG32 peb)
{
	if (peb) {
		*(PBYTE)(peb + 2) = 0;
	}
}

// ������Ա�־
VOID ZeroDebugger64(ULONG64 peb)
{
	if (peb) {
		*(PBYTE)(peb + 2) = 0;
	}
}

// ��ȡNyGlobalFlag
ULONG GetNtGlobalFlag32(ULONG32 peb)
{
	return peb ? *(ULONG*)(peb + 0x68) : -1;
}

// ��ȡNyGlobalFlag
ULONG GetNtGlobalFlag64(ULONG64 peb)
{
	return peb ? *(ULONG*)(peb + 0xbc) : -1;
}

// ��ȡntdll���溯��
FARPROC GetNtdllProcAddress(LPCSTR name)
{
	HMODULE NtdllModule = GetModuleHandle(L"ntdll.dll");
	return GetProcAddress(NtdllModule, name);
}
