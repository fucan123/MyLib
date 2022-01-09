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

	// 512�ֽڶ���
	if (dwSize % 512)
		dwSize = (dwSize / 512) * 512 + 512;

	for (int i = 0; i < pImgNtHeaders->FileHeader.NumberOfSections; i++)
		dwSize += pImgSecHeader[i].SizeOfRawData;

	return dwSize;
}

// ���DLL���溯����ַ
PVOID GetDllFunctionAddress32(PCWSTR lpFunctionName, PVOID BaseAddress, PVOID xxx)
{
	printf("GetDllFunctionAddress32 Start\n");
	HANDLE hSection = NULL, hFile = NULL;
	SIZE_T size = 0;
	NTSTATUS status;
	//�������
	PIMAGE_NT_HEADERS32 NtHdr;

	//��ȡPEͷ��Ϣ
	IMAGE_DOS_HEADER* dosheader;
	IMAGE_OPTIONAL_HEADER32* opthdr;
	IMAGE_EXPORT_DIRECTORY* pExportTable;
	PDWORD arrayOfFunctionAddresses, arrayOfFunctionNames;
	PWORD arrayOfFunctionOrdinals;
	DWORD functionOrdinal;
	PVOID functionAddress = NULL;
	PSTR functionName;

	//ģ����
	HANDLE hMod = BaseAddress;
	//�õ�DOSͷ
	dosheader = (PIMAGE_DOS_HEADER)hMod;
	//Ч���Ƿ�DOSͷ
	if (dosheader->e_magic != IMAGE_DOS_SIGNATURE)return NULL;

	//NTͷ
	NtHdr = (PIMAGE_NT_HEADERS32)((CHAR*)BaseAddress + dosheader->e_lfanew);
	//Ч���Ƿ�PEͷ
	if (NtHdr->Signature != IMAGE_NT_SIGNATURE)return NULL;

	DWORD Rva_import_table = NtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	printf("XXXXXXXX\n");
	//�õ�PEѡ��ͷ
	opthdr = (PIMAGE_OPTIONAL_HEADER32)((PBYTE)hMod + dosheader->e_lfanew + 24);
	//�õ�������
	pExportTable = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)hMod + opthdr->DataDirectory[0].VirtualAddress);
	//�õ�������ַ�б�
	arrayOfFunctionAddresses = (PDWORD)((PBYTE)hMod + pExportTable->AddressOfFunctions);
	//�õ����������б�
	arrayOfFunctionNames = (PDWORD)((PBYTE)hMod + pExportTable->AddressOfNames);
	//�õ��������
	arrayOfFunctionOrdinals = (PWORD)((PBYTE)hMod + pExportTable->AddressOfNameOrdinals);
	//���������ַ
	DWORD Base = pExportTable->Base;
	//KdPrint(("my:��ʼ���:%d(%s).\n", Base, pExportTable->Name));
	// �ú����������ҵĻ���Base ��ֵ����û���κ�����
	Base = 1;
	//ת��������
	//RtlInitUnicodeString(&unFunctionNameSearch, lpFunctionName);
	//ѭ��������
	for (DWORD x = 0; x < pExportTable->NumberOfNames; x++)			//�������������� ���֮�֣�������������=���Ƶ���+��ŵ�����������ѭ���������Ƶĺ���
	{
		//�õ������� 
		functionName = (PSTR)((PBYTE)hMod + arrayOfFunctionNames[x]);
		functionOrdinal = arrayOfFunctionOrdinals[x];
		functionAddress = ((PBYTE)xxx + arrayOfFunctionAddresses[functionOrdinal]);

		if (strstr(functionName, "CHero")) {
			printf("%08X:%s\n", functionAddress, functionName);
		}
#if 0
		//ת��ΪANSI_STRING
		RtlInitAnsiString(&anFunName, functionName);
		//ת��ΪUNICODE_STRING
		RtlAnsiStringToUnicodeString(&unFunctionName, &anFunName, TRUE);
		//��ӡ������Ϣ
		//KdPrint(("my:%d/%d,FunName:%wZ\n", x + 1, pExportTable->NumberOfNames, &unFunctionName));
		//�ȽϺ�������
		if (RtlCompareUnicodeString(&unFunctionName, &unFunctionNameSearch, TRUE) == 0)
		{
			//�õ��ú�����ַ
			functionOrdinal = arrayOfFunctionOrdinals[x] + Base - 1;
			functionAddress = ((PBYTE)hMod + arrayOfFunctionAddresses[functionOrdinal]);
			break;
		}
#endif
	}
	//�����ͷ���Դ���صĵ�ַ����Ч �����ȴ������
	//ZwUnmapViewOfSection (NtCurrentProcess(), BaseAddress);
	//pAddress = BaseAddress;

	return functionAddress;
}

// ���DLL���溯����ַ2
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

	//������rva��0x2a000;
	DWORD Rva_import_table = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (Rva_import_table == 0)
	{
		printf("no import table!");
		return 0;
	}

	//�����Ȼ���ڴ��ַ�����Ǽ�ȥ�ļ���ͷ�ĵ�ַ�������ļ���ַ��
	//�����ַ����ֱ�Ӵ������ȡ����Ҫ�Ķ�����
	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)ImageRvaToVa(
		pNtHeaders,
		lpBaseAddress,
		Rva_import_table,
		NULL
	);

	//��ȥ�ڴ�ӳ����׵�ַ�������ļ���ַ�ˡ������ܼ򵥰ɣ�
	printf("FileAddress Of ImportTable: %p\n", ((DWORD)pImportTable - (DWORD)lpBaseAddress));

	//���������˵�������ǰ��IMAGE_IMPORT_DESCRIPTOR ���飨��0Ԫ��Ϊ��ֹ��
	//�����ʾ�����β��nullԪ�أ�
	IMAGE_IMPORT_DESCRIPTOR null_iid;
	IMAGE_THUNK_DATA null_thunk;
	memset(&null_iid, 0, sizeof(null_iid));
	memset(&null_thunk, 0, sizeof(null_thunk));

	int i, j;
	//ÿ��Ԫ�ش�����һ�������DLL��
	for (i = 0; memcmp(pImportTable + i, &null_iid, sizeof(null_iid)) != 0; i++)
	{
		//LPCSTR: ���� const char*
		LPCSTR szDllName = (LPCSTR)ImageRvaToVa(
			pNtHeaders, lpBaseAddress,
			pImportTable[i].Name, //DLL���Ƶ�RVA
			NULL);

		//�õ���DLL������
		printf("-----------------------------------------\n");
		printf("[%d]: %s\n", i, szDllName);
		printf("-----------------------------------------\n");

		//����ȥ�����Ӹ�DLL����������Щ����
		//����������DLL�� IMAGE_TRUNK_DATA ���飨IAT�������ַ��ǰ��
		PIMAGE_THUNK_DATA32 pThunk = (PIMAGE_THUNK_DATA32)ImageRvaToVa(
			pNtHeaders, lpBaseAddress,
			pImportTable[i].OriginalFirstThunk, //��ע�⡿����ʹ�õ���OriginalFirstThunk
			NULL);

		for (j = 0; memcmp(pThunk + j, &null_thunk, sizeof(null_thunk)) != 0; j++)
		{
			//����ͨ��RVA�����λ�жϺ����ĵ��뷽ʽ��
			//������λΪ1������ŵ��룬�������Ƶ���
			if (pThunk[j].u1.AddressOfData & IMAGE_ORDINAL_FLAG32)
			{
				printf("\t [%d] \t %ld \t ����ŵ���\n", j, pThunk[j].u1.AddressOfData & 0xffff);
			}
			else
			{
				//�����Ƶ��룬�����ٴζ��򵽺�����ź�����
				//ע�����ַ����ֱ���ã���Ϊ��Ȼ��RVA��
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

// ���DLL���溯����ַ
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
	//�������
	IMAGE_NT_HEADERS32 NtHdr;

	//��ȡPEͷ��Ϣ
	IMAGE_DOS_HEADER dosheader;
	IMAGE_OPTIONAL_HEADER32 opthdr;
	IMAGE_EXPORT_DIRECTORY pExportTable;
	DWORD arrayOfFunctionAddresses, arrayOfFunctionNames;
	DWORD arrayOfFunctionOrdinals;
	DWORD functionOrdinal;
	DWORD functionAddress = 0;
	PSTR functionName;

	//�õ�DOSͷ
	if (!ReadProcessMemory(hProcess, hMod, &dosheader, sizeof(IMAGE_DOS_HEADER), NULL))
		return NULL;

	//Ч���Ƿ�DOSͷ;
	if (dosheader.e_magic != IMAGE_DOS_SIGNATURE)return NULL;

	//NTͷ
	if (!ReadProcessMemory(hProcess, ((CHAR*)hMod + dosheader.e_lfanew), &NtHdr, sizeof(IMAGE_NT_HEADERS32), NULL))
		return NULL;
	//Ч���Ƿ�PEͷ
	if (NtHdr.Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	DWORD Rva_import_table = NtHdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	//�õ�PEѡ��ͷ
	if (!ReadProcessMemory(hProcess, ((PBYTE)hMod + dosheader.e_lfanew + 24), &opthdr, sizeof(IMAGE_OPTIONAL_HEADER32), NULL))
		return NULL;
	//�õ�������
	if (!ReadProcessMemory(hProcess, ((PBYTE)hMod + opthdr.DataDirectory[0].VirtualAddress), &pExportTable, sizeof(IMAGE_EXPORT_DIRECTORY), NULL))
		return NULL;
	//�õ�������ַ�б�
	arrayOfFunctionAddresses = ((DWORD)hMod + pExportTable.AddressOfFunctions);
	//�õ����������б�
	arrayOfFunctionNames = ((DWORD)hMod + pExportTable.AddressOfNames);
	//�õ��������
	arrayOfFunctionOrdinals = ((DWORD)hMod + pExportTable.AddressOfNameOrdinals);
	//���������ַ
	DWORD Base = pExportTable.Base;
	//printf("my:��ʼ���:%d(%08X) %d.\n", Base, pExportTable.Name, pExportTable.NumberOfNames);
	// �ú����������ҵĻ���Base ��ֵ����û���κ�����
	Base = 1;
	//ת��������
	//RtlInitUnicodeString(&unFunctionNameSearch, lpFunctionName);
	//ѭ��������
	for (DWORD x = 0; x < pExportTable.NumberOfNames; x++)			//�������������� ���֮�֣�������������=���Ƶ���+��ŵ�����������ѭ���������Ƶĺ���
	{
		//printf("arrayOfFunctionNames[%d]:%08X\n", x * 4, (PBYTE)arrayOfFunctionNames + (x * 4));
		//�õ�������
		DWORD nameAddr = 0;
		// arrayOfFunctionNames[x]
		if (!ReadProcessMemory(hProcess, (PBYTE)arrayOfFunctionNames + (x * 4), &nameAddr, sizeof(nameAddr), NULL))
			continue;

		//printf("arrayOfFunctionNames[%d]:%08X\n", nameAddr);
		// PSTR functionName = (PSTR)((PBYTE)hMod + arrayOfFunctionNames[x]);
		CHAR functionName[128];
		if (!ReadProcessMemory(hProcess, (PBYTE)hMod+nameAddr, &functionName, sizeof(functionName), NULL))
			continue;
		
		
		if (strstr(functionName, lpFunctionName)) { // ���ڴ�����
			//printf("%s\n", functionName);
			// functionOrdinal = arrayOfFunctionOrdinals[x]; PWORD arrayOfFunctionOrdinals
			DWORD functionOrdinal = 0;
			if (!ReadProcessMemory(hProcess, (PBYTE)arrayOfFunctionOrdinals +(x*2), &functionOrdinal, sizeof(WORD), NULL))
				continue;

			//printf("functionOrdinal:%08X\n", functionOrdinal);
			//�õ�������
			DWORD addr = 0;
			// arrayOfFunctionAddresses[functionOrdinal]
			if (!ReadProcessMemory(hProcess, (PBYTE)arrayOfFunctionAddresses + (functionOrdinal*4), &addr, sizeof(addr), NULL))
				continue;
			// functionAddress = (DWORD)hMod + arrayOfFunctionAddresses[functionOrdinal];
			functionAddress = (DWORD)hMod + addr;
		}
	}
	//�����ͷ���Դ���صĵ�ַ����Ч �����ȴ������
	//ZwUnmapViewOfSection (NtCurrentProcess(), BaseAddress);
	//pAddress = BaseAddress;

	CloseHandle(hProcess);
	return functionAddress;
}
