#include "LoadDll.h"
#include <stdio.h>

typedef struct BASE_RELOCATION_BLOCK {
	DWORD PageAddress;
	DWORD BlockSize;
} BASE_RELOCATION_BLOCK, * PBASE_RELOCATION_BLOCK;
typedef struct BASE_RELOCATION_ENTRY {
	USHORT Offset : 12;
	USHORT Type : 4;
} BASE_RELOCATION_ENTRY, * PBASE_RELOCATION_ENTRY;
using DLLEntry = BOOL(WINAPI*)(HINSTANCE dll, DWORD reason, LPVOID reserved);

PVOID LoadDll::Load(const char* file, bool entry, bool print)
{
	//本地加载dll内容至内存中
	HANDLE dll = CreateFileA(file, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	DWORD dllSize = GetFileSize(dll, NULL);
	if (dllSize == 0 || dllSize == INVALID_FILE_SIZE) {
		if (print) {
			::printf("!GetFileSize %d %d\n", dllSize, GetLastError());
		}
		return 0;
	}
	
	LPVOID dllBytes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dllSize);
	DWORD outSize = 0;
	if (!ReadFile(dll, dllBytes, dllSize, &outSize, NULL)) {
		if (print) {
			::printf("!ReadFile %d %d\n", dllSize, GetLastError());
		}
		return 0;
	}


	return LoadByBuffer(dllBytes, entry, print);
}

PVOID LoadDll::LoadByBuffer(void* buffer, bool entry, bool print)
{
	LPVOID dllBytes = buffer;
	//获取已加载至内存中的dll的头部数据
	PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)dllBytes;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)dllBytes + dosHeaders->e_lfanew);
	SIZE_T dllImageSize = ntHeaders->OptionalHeader.SizeOfImage;
	//分配dll加载时所需的内存空间
	LPVOID dllBase = VirtualAlloc((LPVOID)ntHeaders->OptionalHeader.ImageBase, dllImageSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//得到实际分配的内存基址与预期的基址差值,便于后续进行重定向
	DWORD_PTR deltaImageBase = (DWORD_PTR)dllBase - (DWORD_PTR)ntHeaders->OptionalHeader.ImageBase;
	//将dll头部数据复制到分配的内存空间
	memcpy(dllBase, dllBytes, ntHeaders->OptionalHeader.SizeOfHeaders);
	//加载节区数据至新的内存空间
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
	for (size_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
	{
		LPVOID sectionDestination = (LPVOID)((DWORD_PTR)dllBase + (DWORD_PTR)section->VirtualAddress);
		LPVOID sectionBytes = (LPVOID)((DWORD_PTR)dllBytes + (DWORD_PTR)section->PointerToRawData);
		memcpy(sectionDestination, sectionBytes, section->SizeOfRawData);
		section++;
	}
	// 开始dll加载实现重定位
	IMAGE_DATA_DIRECTORY relocations = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	DWORD_PTR relocationTable = relocations.VirtualAddress + (DWORD_PTR)dllBase;
	DWORD relocationsProcessed = 0;
	while (relocationsProcessed < relocations.Size)
	{
		PBASE_RELOCATION_BLOCK relocationBlock = (PBASE_RELOCATION_BLOCK)(relocationTable + relocationsProcessed);
		relocationsProcessed += sizeof(BASE_RELOCATION_BLOCK);
		DWORD relocationsCount = (relocationBlock->BlockSize - sizeof(BASE_RELOCATION_BLOCK)) / sizeof(BASE_RELOCATION_ENTRY);
		PBASE_RELOCATION_ENTRY relocationEntries = (PBASE_RELOCATION_ENTRY)(relocationTable + relocationsProcessed);
		for (DWORD i = 0; i < relocationsCount; i++)
		{
			relocationsProcessed += sizeof(BASE_RELOCATION_ENTRY);
			if (relocationEntries[i].Type == 0)
			{
				continue;
			}
			DWORD_PTR relocationRVA = relocationBlock->PageAddress + relocationEntries[i].Offset;
			DWORD_PTR addressToPatch = 0;
			ReadProcessMemory(GetCurrentProcess(), (LPCVOID)((DWORD_PTR)dllBase + relocationRVA), &addressToPatch, sizeof(DWORD_PTR), NULL);
			addressToPatch += deltaImageBase;
			memcpy((PVOID)((DWORD_PTR)dllBase + relocationRVA), &addressToPatch, sizeof(DWORD_PTR));
		}
	}
	//解析导入表
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
	IMAGE_DATA_DIRECTORY importsDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importsDirectory.VirtualAddress + (DWORD_PTR)dllBase);
	LPCSTR libraryName = "";
	HMODULE library = NULL;
	while (importDescriptor->Name != NULL)
	{
		libraryName = (LPCSTR)importDescriptor->Name + (DWORD_PTR)dllBase;
		library = LoadLibraryA(libraryName);
		if (print) {
			::printf("libraryName %s\n", libraryName);
		}
		if (library)
		{
			PIMAGE_THUNK_DATA thunk = NULL;
			thunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)dllBase + importDescriptor->FirstThunk);
			while (thunk->u1.AddressOfData != NULL)
			{
				if (IMAGE_SNAP_BY_ORDINAL(thunk->u1.Ordinal))
				{
					LPCSTR functionOrdinal = (LPCSTR)IMAGE_ORDINAL(thunk->u1.Ordinal);
					thunk->u1.Function = (DWORD_PTR)::GetProcAddress(library, functionOrdinal);
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME functionName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)dllBase + thunk->u1.AddressOfData);
					DWORD_PTR functionAddress = (DWORD_PTR)::GetProcAddress(library, functionName->Name);
					thunk->u1.Function = functionAddress;
				}
				++thunk;
			}
		}
		importDescriptor++;
	}

	if (entry) {
		//执行加载的dll
		//::printf("entry\n");
		DLLEntry DllEntry = (DLLEntry)((DWORD_PTR)dllBase + ntHeaders->OptionalHeader.AddressOfEntryPoint);
		bool r = (*DllEntry)((HINSTANCE)dllBase, DLL_PROCESS_ATTACH, 0);
	}

	m_hModule = (HMODULE)dllBase;
	return m_hModule;
}

PVOID LoadDll::GetProcAddress(const char* name, bool print)
{
	if (!m_hModule)
		return NULL;

	//得到DOS头
	IMAGE_DOS_HEADER* dosheader = (PIMAGE_DOS_HEADER)m_hModule;
	//效验是否DOS头
	if (dosheader->e_magic != IMAGE_DOS_SIGNATURE)return NULL;

#ifndef x64
	//NT头
	PIMAGE_NT_HEADERS32 NtHdr = (PIMAGE_NT_HEADERS32)((CHAR*)m_hModule + dosheader->e_lfanew);
	//效验是否PE头
	if (NtHdr->Signature != IMAGE_NT_SIGNATURE)return NULL;
	//得到PE选项头
	IMAGE_OPTIONAL_HEADER32* opthdr = (PIMAGE_OPTIONAL_HEADER64)((PBYTE)m_hModule + dosheader->e_lfanew + 24);
#else
	//NT头
	PIMAGE_NT_HEADERS64 NtHdr = (PIMAGE_NT_HEADERS64)((CHAR*)m_hModule + dosheader->e_lfanew);
	//效验是否PE头
	if (NtHdr->Signature != IMAGE_NT_SIGNATURE)return NULL;
	//得到PE选项头
	IMAGE_OPTIONAL_HEADER64* opthdr = (PIMAGE_OPTIONAL_HEADER64)((PBYTE)m_hModule + dosheader->e_lfanew + 24);
#endif

	//得到导出表
	IMAGE_EXPORT_DIRECTORY* pExportTable = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)m_hModule + opthdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	//得到函数地址列表
	PDWORD arrayOfFunctionAddresses = (PDWORD)((PBYTE)m_hModule + pExportTable->AddressOfFunctions);
	//得到函数名称列表
	PDWORD arrayOfFunctionNames = (PDWORD)((PBYTE)m_hModule + pExportTable->AddressOfNames);
	//得到函数序号
	PWORD arrayOfFunctionOrdinals = (PWORD)((PBYTE)m_hModule + pExportTable->AddressOfNameOrdinals);
	//导出表基地址
	DWORD Base = pExportTable->Base;

	if (print) {
		::printf("pExportTable->NumberOfNames: %u\n", pExportTable->NumberOfNames);
	}

	//循环导出表
	for (DWORD x = 0; x < pExportTable->NumberOfNames; x++)			//导出函数有名称 编号之分，导出函数总数=名称导出+编号导出，这里是循环导出名称的函数
	{
		//得到函数名 
		PSTR functionName = (PSTR)((PBYTE)m_hModule + arrayOfFunctionNames[x]);
		if (print) {
			::printf("函数 %s\n", functionName);
		}
		if (strcmp(functionName, name) == 0) { // 存在此名称
			DWORD functionOrdinal = arrayOfFunctionOrdinals[x];
			return PVOID((ULONG_PTR)m_hModule + arrayOfFunctionAddresses[functionOrdinal]);
		}
	}

	return NULL;
}

HMODULE LoadDll::GetHandle()
{
	return m_hModule;
}
