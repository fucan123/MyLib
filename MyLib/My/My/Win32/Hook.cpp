#include "Hook.h"
#include <stdio.h>

Hook* Hook::self = NULL;

Hook::Hook() {
	m_hWnd = NULL;
}

void Hook::SetHwnd(HWND hwnd)
{
	m_hWnd = hwnd;
	m_hkLen = 0;
	memset(&m_hkData, 0, sizeof(m_hkData));
}

#ifndef x64
// ���ӵ�ַΪaddress�ĺ��� ��ת��func
void Hook::Attach(LPVOID address, LPVOID func)
{
	BYTE index = m_hkLen;
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].address == address) {
			if (m_hkData[i].flag == 1) {
				return; // �Ѿ�HOOK
			}
			index = i;
		}
	}

	DWORD oldProtect;
	if (VirtualProtect(address, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		LPVOID oldCode = m_hkData[index].oldCode;
		__asm {
			mov esi, address;
			mov edi, oldCode;
			cld;
			movsb;
			movsd;
		} // ����ɵ�����
		DWORD jmpAddr = (DWORD)func - (DWORD)address - 5;
		__asm {
			push eax;
			push ebx;
			mov eax, address;
			mov byte ptr[eax], 0xE9;
			inc eax;
			mov ebx, jmpAddr;
			mov dword ptr[eax], ebx;
			pop ebx;
			pop eax;
		} // HOOK���� ��ת��func
		VirtualProtect((LPVOID)address, 5, oldProtect, &oldProtect);

		m_hkData[index].address = address; // HOOK��ַ
		m_hkData[index].flag = 1; // ��־Ϊ��HOOK
		m_hkLen++;

		//printf("�ҹ��ɹ�%c", 10);
	}
}
#else
// ���ӵ�ַΪaddress�ĺ��� ��ת��func
void Hook::Attach(LPVOID address, LPVOID func)
{
	BYTE index = m_hkLen;
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].address == address) {
			if (m_hkData[i].flag == 1) {
				return; // �Ѿ�HOOK
			}
			index = i;
		}
	}

	unsigned char code[12];
	code[0] = 0x48;
	code[1] = 0xB8;
	code[10] = 0x50;
	code[11] = 0xC3;
	long long a = (long long)func;
	RtlMoveMemory(&code[2], &a, 8);

	SIZE_T size = 12;
#if 1
	DWORD jmpAddr = (DWORD)func - (DWORD)address - 5;
	code[0] = 0xe9;
	RtlMoveMemory(&code[1], &jmpAddr, 4);
	code[5] = 0x90;

	size = 5;
#endif

	DWORD old;
	if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old)) {
		SIZE_T writeLen = 0;
		RtlMoveMemory(m_hkData[index].oldCode, address, size);
		//WriteProcessMemory(GetCurrentProcess(), address, code, 12, &writeLen);
		RtlMoveMemory(address, code, size);
		VirtualProtectEx(GetCurrentProcess(), address, size, old, &old);

		m_hkData[index].address = address; // HOOK��ַ
		m_hkData[index].size = size;
		m_hkData[index].flag = 1; // ��־Ϊ��HOOK
		m_hkLen++;
		printf("\n�ҹ��ɹ�(64λ) д��:%lld\n", writeLen);
	}
}
#endif

#ifndef x64
// ȡ����ַΪaddress�ĸ���
void Hook::Detach(LPVOID address)
{
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].address == address) {
			DWORD oldProtect;
			if (VirtualProtect(address, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
				LPVOID oldCode = m_hkData[i].oldCode;
				__asm {
					mov esi, oldCode;
					mov edi, address;
					cld;
					movsb;
					movsd;
				} // �ָ�ԭ�ȴ���
				VirtualProtect(address, 5, oldProtect, &oldProtect);

				m_hkData[i].flag = 0; // ��־Ϊδ�ҹ�
				//printf("ȡ���ɹ�%c", 10);
			}
		}
	}
}
#else
// ȡ����ַΪaddress�ĸ���
void Hook::Detach(LPVOID address)
{
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].address == address) {
			DWORD old;
			SIZE_T size = m_hkData[i].size;
			if (VirtualProtectEx(GetCurrentProcess(), address, size, PAGE_EXECUTE_READWRITE, &old)) {
				RtlMoveMemory(address, m_hkData[i].oldCode, size);
				//WriteProcessMemory(GetCurrentProcess(), address, m_hkData[i].oldCode, size, NULL);
				VirtualProtectEx(GetCurrentProcess(), address, size, old, &old);

				m_hkData[i].flag = 0; // ��־Ϊδ�ҹ�
				printf("ȡ���ɹ�(64λ)%c", 10);
			}
		}
	}
}
#endif

Hook* Hook::GetInstance()
{
	if (!Hook::self)
		Hook::self = new Hook;
	return Hook::self;
}

void Hook::DeleteInstance() 
{
	if (Hook::self) {
		delete Hook::self;
		Hook::self = NULL;
	}
}