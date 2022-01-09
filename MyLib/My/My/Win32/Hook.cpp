#include "../../../Asm.h"
#include "Hook.h"
#include <stdio.h>

Hook* Hook::self = NULL;

Hook::Hook() {
	m_hkLen = 0;
	memset(&m_hkData, 0, sizeof(m_hkData));
	m_funcDisasm64 = nullptr;
	m_nBitMode = 0;
}

// ����ģʽ bit=32|64 ����64��Ҫ����path path=�����dll�ļ�·�� 
bool Hook::SetMode(int bit, const char* path)
{
	if (bit != 32 && bit != 64)
		return false;

	m_nBitMode = bit;
	if (bit == 32)
		return true;
	if (path == nullptr)
		return false;

	HMODULE hModule = LoadLibraryA(path);
	if (!hModule)
		return false;

	m_funcDisasm64 = (Disasm64Func)GetProcAddress(hModule, "Disasm64");
	return m_funcDisasm64 != nullptr;
}

// old_address=Ҫhook��λ�� new_address=�º�����ַ
bool Hook::Attach(LPVOID old_address, LPVOID new_address)
{
	BYTE index = m_hkLen;
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].old_address == old_address) {
			if (m_hkData[i].flag & 0x01) {
				return true; // �Ѿ�HOOK
			}
			index = i;
			break;
		}
	}

	LPVOID old_address2 = (LPVOID)((long long)old_address);
	SIZE_T size = MakeShellCode(old_address2, new_address, &m_hkData[index]);
	if (size == 0)
		return false;

	DWORD old;
	if (!VirtualProtect(old_address2, size, PAGE_EXECUTE_READWRITE, &old))
		return false;

	RtlMoveMemory(m_hkData[index].old_code, old_address2, size); // ����ԭ�д���
	RtlMoveMemory(old_address2, m_hkData[index].new_code, size); // hook����

	m_hkData[index].old_address = old_address; // ԭ��ַ
	m_hkData[index].new_address = new_address; // �µ�ַ
	m_hkData[index].size = size;
	m_hkData[index].flag |= 0x01; // ��־Ϊ��HOOK
	m_hkLen++;

	VirtualProtectEx(GetCurrentProcess(), old_address, size, old, &old); // ��ԭҳ������
	return true;
}

// ����hook���� old_address = Ҫhook��λ�� new_address = �º�����ַ ���ش��볤��
int Hook::MakeShellCode(LPVOID old_address, LPVOID new_address, HK_DATA* p_hook_data)
{
	if (m_nBitMode == 32) { // 32λ
		DWORD jmpAddr = (DWORD)new_address - (DWORD)old_address - 5;
		p_hook_data->new_code[0] = 0xe9; // jmp�������ַ
		RtlMoveMemory(&p_hook_data->new_code[1], &jmpAddr, 4);
		p_hook_data->size = 5;

		//::printf("%08X-%08X-5=%08X\n", old_address, new_address, jmpAddr);
		// mov eax, ��ֵ (0xB8)
		// jmp eax (0xFF 0xE0)
		// ret (C3)
		BYTE code[8];
		code[0] = 0xB8;
		code[5] = 0xFF;
		code[6] = 0xE0;
		code[7] = 0xC3;
		long jmpOldAddr = (DWORD)old_address + 5; // ����HOOK����
		RtlMoveMemory(&code[1], &jmpOldAddr, sizeof(jmpOldAddr));

		
		RtlMoveMemory(p_hook_data->jmp_old_code, old_address, 5); // ����ԭ����(��ִ��ԭ����)
		RtlMoveMemory(&p_hook_data->jmp_old_code[5], code, sizeof(code)); // ��ת��ԭ��ַ����
		p_hook_data->jmp_old_size = 5 + sizeof(code);

		return 5;
	}
	else if (m_nBitMode == 64) {
		int asm_size = GetDisasmCodeSize(old_address, 12); // 12=hook����
		if (asm_size == 0)
			return 0;

		// mov rax, ���� (0x48 0xB8)
		// push rax (0x50)
		// ret (0xC3)
		BYTE code[12];
		code[0] = 0x48;
		code[1] = 0xB8;
		code[10] = 0x50;
		code[11] = 0xC3;

		new_address = (LPVOID)((long long)new_address);

		RtlMoveMemory(&code[2], &new_address, 8);
		RtlMoveMemory(p_hook_data->new_code, code, sizeof(code)); // hook����

		long long jmp_addr = (long long)old_address + asm_size;  // �������Ƶ�ԭ���볤��
		RtlMoveMemory(&code[2], &jmp_addr, 8);
		RtlMoveMemory(p_hook_data->jmp_old_code, old_address, asm_size); // ����ԭ����(��ִ��ԭ����)
		RtlMoveMemory(&p_hook_data->jmp_old_code[asm_size], code, sizeof(code)); // ��ת��ԭ��ַ����

		p_hook_data->size = 12;
		p_hook_data->jmp_old_size = asm_size + sizeof(code);
		return 12;
	}

	return 0;
}

/*
	��ȡ�������볤��(ȡ���˵�ַ����������,����hook����ת��ԭ����֮ǰִ��,ȷ��ִ����ȷ)
	address=������ַ
	size=���ٳ���(������Ҫȡhook�ĳ���,�����ƻ�֮ǰ����)
	������Ҫ�����ĳ���
*/
int Hook::GetDisasmCodeSize(LPVOID address, int size)
{
	if (m_nBitMode == 32) // 32λ��ʵ����Ҫ�����, ������ͷ5���ֽڲ��ᱻ�ƻ�
		return 5;

	if (m_funcDisasm64 == nullptr)
		return 0;

	size_t oplen = 0;
	char*  szMnemonic;
	size_t iLen, mLen;
	char* addr = (char*)address;
	while (oplen < size) {
		szMnemonic = m_funcDisasm64(addr + oplen, 32, &iLen, &mLen, 0x00);
		//printf("���ָ�%s (%d)\n", szMnemonic, iLen);
		oplen += iLen;

		if (oplen >= size)
			break;
	}

	return oplen;
}

// ��ȡ��ת��ԭ����֮������ַ arg_num=ԭ����������
JMPOLDFUNC Hook::GetJmptOldFuncAddress(LPVOID old_address)
{
	BYTE index = m_hkLen;
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].old_address == old_address) {
			if (m_hkData[i].flag == 1) {
				char* p = (char*)m_hkData[i].jmp_old_code;;
				//return (JMPOLDFUNC)p; // �Ѿ�HOOK
			}
			index = i;
			break;
		}
	}
	if (m_hkData[index].flag & 0x02)
		return Asm_JmpHookOldFunc;

	DWORD old;
	if (!VirtualProtect(Asm_JmpHookOldFunc, m_hkData[index].jmp_old_size, PAGE_EXECUTE_READWRITE, &old))
		return nullptr;

	RtlMoveMemory(Asm_JmpHookOldFunc, m_hkData[index].jmp_old_code, m_hkData[index].jmp_old_size); // hook����
	VirtualProtectEx(GetCurrentProcess(), Asm_JmpHookOldFunc, m_hkData[index].jmp_old_size, old, &old); // ��ԭҳ������

	m_hkData[index].flag |= 0x02; // ��־Ϊ��д��
	return Asm_JmpHookOldFunc;
#if 0
	PVOID asm_func = Asm_JmpHookOldFunc;
	// push rcx (0x51)
	// push rdx (0x52)
	// push r8  (0x41 0x50)
	// push r9  (0x41 0x51)
	// mov  rax, ��ֵ (0x48 0xB8 64λ��)
	// mov  qword ptr [rsp+20h],rax (0x48 0x89 0x44 0x24 20[��ֵ])
	//Asm_JmpHookOldFunc();
	unsigned char code[64];
	int size = 0;
	switch (arg_num)
	{
	case 0: // push rcx
		code[0] = 0x51;
		size = 1;
		break;
	case 1: // push rdx
		code[0] = 0x52;
		size = 1;
		break;
	case 2: // push r8
		code[0] = 0x41;
		code[1] = 0x50;
		size = 2;
		break;
	case 3: // push r9
		code[0] = 0x41;
		code[1] = 0x51;
		size = 2;
		break;
	default:
		code[0] = 0x48;
		code[1] = 0xB8;
		break;
	}

	return (JMPOLDFUCN)asm_func;
#endif
}

// ȡ����ַΪaddress�ĸ���
void Hook::Detach(LPVOID address)
{
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].old_address == address) {
			DWORD old;
			SIZE_T size = m_hkData[i].size;
			if (VirtualProtectEx(GetCurrentProcess(), address, size, PAGE_EXECUTE_READWRITE, &old)) {
				RtlMoveMemory(address, m_hkData[i].old_code, size);
				//WriteProcessMemory(GetCurrentProcess(), address, m_hkData[i].oldCode, size, NULL);
				VirtualProtectEx(GetCurrentProcess(), address, size, old, &old);

				m_hkData[i].flag = 0; // ��־Ϊδ�ҹ�
				//printf("ȡ���ɹ�(%dλ)\n", m_nBitMode);
			}
		}
	}
}

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