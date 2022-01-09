#include "PE.h"
#include <Dbghelp.h>
#include <stdio.h>

DWORD GetPEFileRealSize(LPBYTE lpBuffer)
{
	DWORD dwSize = 0;

	PIMAGE_DOS_HEADER pImgDosHeader = (PIMAGE_DOS_HEADER)lpBuffer;
	PIMAGE_NT_HEADERS32 pImgNtHeaders = (PIMAGE_NT_HEADERS32)(lpBuffer + pImgDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pImgSecHeader = (PIMAGE_SECTION_HEADER)(pImgNtHeaders + 1);

	dwSize = pImgDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS32) + pImgNtHeaders->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);

	// 512字节对齐
	if (dwSize % 512)
		dwSize = (dwSize / 512) * 512 + 512;

	for (int i = 0; i < pImgNtHeaders->FileHeader.NumberOfSections; i++)
		dwSize += pImgSecHeader[i].SizeOfRawData;

	return dwSize;
}

// 获得DLL里面函数地址
PVOID GetDllFunctionAddress32(PCWSTR lpFunctionName, PVOID BaseAddress, PVOID xxx)
{
	printf("GetDllFunctionAddress32 Start\n");
	HANDLE hSection = NULL, hFile = NULL;
	SIZE_T size = 0;
	NTSTATUS status;
	//定义变量
	PIMAGE_NT_HEADERS32 NtHdr;

	//读取PE头信息
	IMAGE_DOS_HEADER* dosheader;
	IMAGE_OPTIONAL_HEADER32* opthdr;
	IMAGE_EXPORT_DIRECTORY* pExportTable;
	PDWORD arrayOfFunctionAddresses, arrayOfFunctionNames;
	PWORD arrayOfFunctionOrdinals;
	DWORD functionOrdinal;
	PVOID functionAddress = NULL;
	PSTR functionName;

	//模块句柄
	HANDLE hMod = BaseAddress;
	//得到DOS头
	dosheader = (PIMAGE_DOS_HEADER)hMod;
	//效验是否DOS头
	if (dosheader->e_magic != IMAGE_DOS_SIGNATURE)return NULL;

	//NT头
	NtHdr = (PIMAGE_NT_HEADERS32)((CHAR*)BaseAddress + dosheader->e_lfanew);
	//效验是否PE头
	if (NtHdr->Signature != IMAGE_NT_SIGNATURE)return NULL;

	DWORD Rva_import_table = NtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	printf("XXXXXXXX\n");
	//得到PE选项头
	opthdr = (PIMAGE_OPTIONAL_HEADER32)((PBYTE)hMod + dosheader->e_lfanew + 24);
	//得到导出表
	pExportTable = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)hMod + opthdr->DataDirectory[0].VirtualAddress);
	//得到函数地址列表
	arrayOfFunctionAddresses = (PDWORD)((PBYTE)hMod + pExportTable->AddressOfFunctions);
	//得到函数名称列表
	arrayOfFunctionNames = (PDWORD)((PBYTE)hMod + pExportTable->AddressOfNames);
	//得到函数序号
	arrayOfFunctionOrdinals = (PWORD)((PBYTE)hMod + pExportTable->AddressOfNameOrdinals);
	//导出表基地址
	DWORD Base = pExportTable->Base;
	//KdPrint(("my:起始序号:%d(%s).\n", Base, pExportTable->Name));
	// 用函数名来查找的话，Base 的值现在没有任何意义
	Base = 1;
	//转换函数名
	//RtlInitUnicodeString(&unFunctionNameSearch, lpFunctionName);
	//循环导出表
	for (DWORD x = 0; x < pExportTable->NumberOfNames; x++)			//导出函数有名称 编号之分，导出函数总数=名称导出+编号导出，这里是循环导出名称的函数
	{
		//得到函数名 
		functionName = (PSTR)((PBYTE)hMod + arrayOfFunctionNames[x]);
		functionOrdinal = arrayOfFunctionOrdinals[x];
		functionAddress = ((PBYTE)xxx + arrayOfFunctionAddresses[functionOrdinal]);

		if (strstr(functionName, "CHero")) {
			printf("%08X:%s\n", functionAddress, functionName);
		}
#if 0
		//转化为ANSI_STRING
		RtlInitAnsiString(&anFunName, functionName);
		//转化为UNICODE_STRING
		RtlAnsiStringToUnicodeString(&unFunctionName, &anFunName, TRUE);
		//打印调试信息
		//KdPrint(("my:%d/%d,FunName:%wZ\n", x + 1, pExportTable->NumberOfNames, &unFunctionName));
		//比较函数名称
		if (RtlCompareUnicodeString(&unFunctionName, &unFunctionNameSearch, TRUE) == 0)
		{
			//得到该函数地址
			functionOrdinal = arrayOfFunctionOrdinals[x] + Base - 1;
			functionAddress = ((PBYTE)hMod + arrayOfFunctionAddresses[functionOrdinal]);
			break;
		}
#endif
	}
	//这里释放资源返回的地址将无效 所以先存放起来
	//ZwUnmapViewOfSection (NtCurrentProcess(), BaseAddress);
	//pAddress = BaseAddress;

	return functionAddress;
}

// 获得DLL里面函数地址2
PVOID GetDllFunctionAddress32_2(PCWSTR lpFunctionName, PVOID BaseAddress)
{
	return NULL;
#if 0
	LPBYTE lpBaseAddress = (LPBYTE)BaseAddress;

	if (lpBaseAddress == NULL)
	{
		printf("Could not map view of file (%d).\n", GetLastError());
		return 0;
	}

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBaseAddress;
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(lpBaseAddress + pDosHeader->e_lfanew);

	//导入表的rva：0x2a000;
	DWORD Rva_import_table = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (Rva_import_table == 0)
	{
		printf("no import table!");
		return 0;
	}

	//这个虽然是内存地址，但是减去文件开头的地址，就是文件地址了
	//这个地址可以直接从里面读取你想要的东西了
	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)ImageRvaToVa(
		pNtHeaders,
		lpBaseAddress,
		Rva_import_table,
		NULL
	);

	//减去内存映射的首地址，就是文件地址了。。（很简单吧）
	printf("FileAddress Of ImportTable: %p\n", ((DWORD)pImportTable - (DWORD)lpBaseAddress));

	//现在来到了导入表的面前：IMAGE_IMPORT_DESCRIPTOR 数组（以0元素为终止）
	//定义表示数组结尾的null元素！
	IMAGE_IMPORT_DESCRIPTOR null_iid;
	IMAGE_THUNK_DATA null_thunk;
	memset(&null_iid, 0, sizeof(null_iid));
	memset(&null_thunk, 0, sizeof(null_thunk));

	int i, j;
	//每个元素代表了一个引入的DLL。
	for (i = 0; memcmp(pImportTable + i, &null_iid, sizeof(null_iid)) != 0; i++)
	{
		//LPCSTR: 就是 const char*
		LPCSTR szDllName = (LPCSTR)ImageRvaToVa(
			pNtHeaders, lpBaseAddress,
			pImportTable[i].Name, //DLL名称的RVA
			NULL);

		//拿到了DLL的名字
		printf("-----------------------------------------\n");
		printf("[%d]: %s\n", i, szDllName);
		printf("-----------------------------------------\n");

		//现在去看看从该DLL中引入了哪些函数
		//我们来到该DLL的 IMAGE_TRUNK_DATA 数组（IAT：导入地址表）前面
		PIMAGE_THUNK_DATA32 pThunk = (PIMAGE_THUNK_DATA32)ImageRvaToVa(
			pNtHeaders, lpBaseAddress,
			pImportTable[i].OriginalFirstThunk, //【注意】这里使用的是OriginalFirstThunk
			NULL);

		for (j = 0; memcmp(pThunk + j, &null_thunk, sizeof(null_thunk)) != 0; j++)
		{
			//这里通过RVA的最高位判断函数的导入方式，
			//如果最高位为1，按序号导入，否则按名称导入
			if (pThunk[j].u1.AddressOfData & IMAGE_ORDINAL_FLAG32)
			{
				printf("\t [%d] \t %ld \t 按序号导入\n", j, pThunk[j].u1.AddressOfData & 0xffff);
			}
			else
			{
				//按名称导入，我们再次定向到函数序号和名称
				//注意其地址不能直接用，因为仍然是RVA！
				PIMAGE_IMPORT_BY_NAME pFuncName = (PIMAGE_IMPORT_BY_NAME)ImageRvaToVa(
					pNtHeaders, lpBaseAddress,
					pThunk[j].u1.AddressOfData,
					NULL);

				printf("\t [%d] \t %ld \t %s\n", j, pFuncName->Hint, pFuncName->Name);
			}
		}
	}

	return 0;
#endif
}

// 获得DLL里面函数地址
DWORD GetDllFunctionAddress32(DWORD pid, PCSTR lpFunctionName, HANDLE hMod)
{
	if (!hMod)
		return NULL;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess)
		return NULL;

	HANDLE hSection = NULL, hFile = NULL;
	SIZE_T size = 0;
	NTSTATUS status;
	//定义变量
	IMAGE_NT_HEADERS32 NtHdr;

	//读取PE头信息
	IMAGE_DOS_HEADER dosheader;
	IMAGE_OPTIONAL_HEADER32 opthdr;
	IMAGE_EXPORT_DIRECTORY pExportTable;
	DWORD arrayOfFunctionAddresses, arrayOfFunctionNames;
	DWORD arrayOfFunctionOrdinals;
	DWORD functionOrdinal;
	DWORD functionAddress = 0;
	PSTR functionName;

	//得到DOS头
	if (!ReadProcessMemory(hProcess, hMod, &dosheader, sizeof(IMAGE_DOS_HEADER), NULL))
		return NULL;

	//效验是否DOS头;
	if (dosheader.e_magic != IMAGE_DOS_SIGNATURE)return NULL;

	//NT头
	if (!ReadProcessMemory(hProcess, ((CHAR*)hMod + dosheader.e_lfanew), &NtHdr, sizeof(IMAGE_NT_HEADERS32), NULL))
		return NULL;
	//效验是否PE头
	if (NtHdr.Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	DWORD Rva_import_table = NtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	//得到PE选项头
	if (!ReadProcessMemory(hProcess, ((PBYTE)hMod + dosheader.e_lfanew + 24), &opthdr, sizeof(IMAGE_OPTIONAL_HEADER32), NULL))
		return NULL;
	//得到导出表
	if (!ReadProcessMemory(hProcess, ((PBYTE)hMod + opthdr.DataDirectory[0].VirtualAddress), &pExportTable, sizeof(IMAGE_EXPORT_DIRECTORY), NULL))
		return NULL;
	//得到函数地址列表
	arrayOfFunctionAddresses = ((DWORD)hMod + pExportTable.AddressOfFunctions);
	//得到函数名称列表
	arrayOfFunctionNames = ((DWORD)hMod + pExportTable.AddressOfNames);
	//得到函数序号
	arrayOfFunctionOrdinals = ((DWORD)hMod + pExportTable.AddressOfNameOrdinals);
	//导出表基地址
	DWORD Base = pExportTable.Base;
	//printf("my:起始序号:%d(%08X) %d.\n", Base, pExportTable.Name, pExportTable.NumberOfNames);
	// 用函数名来查找的话，Base 的值现在没有任何意义
	Base = 1;
	//转换函数名
	//RtlInitUnicodeString(&unFunctionNameSearch, lpFunctionName);
	//循环导出表
	for (DWORD x = 0; x < pExportTable.NumberOfNames; x++)			//导出函数有名称 编号之分，导出函数总数=名称导出+编号导出，这里是循环导出名称的函数
	{
		//printf("arrayOfFunctionNames[%d]:%08X\n", x * 4, (PBYTE)arrayOfFunctionNames + (x * 4));
		//得到函数名
		DWORD nameAddr = 0;
		// arrayOfFunctionNames[x]
		if (!ReadProcessMemory(hProcess, (PBYTE)arrayOfFunctionNames + (x * 4), &nameAddr, sizeof(nameAddr), NULL))
			continue;

		//printf("arrayOfFunctionNames[%d]:%08X\n", nameAddr);
		// PSTR functionName = (PSTR)((PBYTE)hMod + arrayOfFunctionNames[x]);
		CHAR functionName[128];
		if (!ReadProcessMemory(hProcess, (PBYTE)hMod+nameAddr, &functionName, sizeof(functionName), NULL))
			continue;
		
		
		if (strstr(functionName, lpFunctionName)) { // 存在此名称
			//printf("%s\n", functionName);
			// functionOrdinal = arrayOfFunctionOrdinals[x]; PWORD arrayOfFunctionOrdinals
			DWORD functionOrdinal = 0;
			if (!ReadProcessMemory(hProcess, (PBYTE)arrayOfFunctionOrdinals +(x*2), &functionOrdinal, sizeof(WORD), NULL))
				continue;

			//printf("functionOrdinal:%08X\n", functionOrdinal);
			//得到函数名
			DWORD addr = 0;
			// arrayOfFunctionAddresses[functionOrdinal]
			if (!ReadProcessMemory(hProcess, (PBYTE)arrayOfFunctionAddresses + (functionOrdinal*4), &addr, sizeof(addr), NULL))
				continue;
			// functionAddress = (DWORD)hMod + arrayOfFunctionAddresses[functionOrdinal];
			functionAddress = (DWORD)hMod + addr;
		}
	}
	//这里释放资源返回的地址将无效 所以先存放起来
	//ZwUnmapViewOfSection (NtCurrentProcess(), BaseAddress);
	//pAddress = BaseAddress;

	CloseHandle(hProcess);
	return functionAddress;
}
