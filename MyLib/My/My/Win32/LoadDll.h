#pragma once
#include <Windows.h>

class LoadDll
{
private:
	HMODULE m_hModule = NULL;
public:
	PVOID Load(const char* file, bool entry=false, bool print=false);
	PVOID LoadByBuffer(void* buffer, bool entry = false, bool print = false);
	PVOID GetProcAddress(const char* name, bool print=false);
	HMODULE GetHandle();
};