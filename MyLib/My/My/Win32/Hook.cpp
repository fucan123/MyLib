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

// 设置模式 bit=32|64 等于64需要设置path path=反汇编dll文件路径 
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

// old_address=要hook的位置 new_address=新函数地址
bool Hook::Attach(LPVOID old_address, LPVOID new_address)
{
	BYTE index = m_hkLen;
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].old_address == old_address) {
			if (m_hkData[i].flag & 0x01) {
				return true; // 已经HOOK
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

	RtlMoveMemory(m_hkData[index].old_code, old_address2, size); // 保存原有代码
	RtlMoveMemory(old_address2, m_hkData[index].new_code, size); // hook代码

	m_hkData[index].old_address = old_address; // 原地址
	m_hkData[index].new_address = new_address; // 新地址
	m_hkData[index].size = size;
	m_hkData[index].flag |= 0x01; // 标志为已HOOK
	m_hkLen++;

	VirtualProtectEx(GetCurrentProcess(), old_address, size, old, &old); // 还原页码属性
	return true;
}

// 制作hook代码 old_address = 要hook的位置 new_address = 新函数地址 返回代码长度
int Hook::MakeShellCode(LPVOID old_address, LPVOID new_address, HK_DATA* p_hook_data)
{
	if (m_nBitMode == 32) { // 32位
		DWORD jmpAddr = (DWORD)new_address - (DWORD)old_address - 5;
		p_hook_data->new_code[0] = 0xe9; // jmp后面跟地址
		RtlMoveMemory(&p_hook_data->new_code[1], &jmpAddr, 4);
		p_hook_data->size = 5;

		//::printf("%08X-%08X-5=%08X\n", old_address, new_address, jmpAddr);
		// mov eax, 数值 (0xB8)
		// jmp eax (0xFF 0xE0)
		// ret (C3)
		BYTE code[8];
		code[0] = 0xB8;
		code[5] = 0xFF;
		code[6] = 0xE0;
		code[7] = 0xC3;
		long jmpOldAddr = (DWORD)old_address + 5; // 跳过HOOK代码
		RtlMoveMemory(&code[1], &jmpOldAddr, sizeof(jmpOldAddr));

		
		RtlMoveMemory(p_hook_data->jmp_old_code, old_address, 5); // 复制原代码(先执行原代码)
		RtlMoveMemory(&p_hook_data->jmp_old_code[5], code, sizeof(code)); // 跳转到原地址代码
		p_hook_data->jmp_old_size = 5 + sizeof(code);

		return 5;
	}
	else if (m_nBitMode == 64) {
		int asm_size = GetDisasmCodeSize(old_address, 12); // 12=hook长度
		if (asm_size == 0)
			return 0;

		// mov rax, 常数 (0x48 0xB8)
		// push rax (0x50)
		// ret (0xC3)
		BYTE code[12];
		code[0] = 0x48;
		code[1] = 0xB8;
		code[10] = 0x50;
		code[11] = 0xC3;

		new_address = (LPVOID)((long long)new_address);

		RtlMoveMemory(&code[2], &new_address, 8);
		RtlMoveMemory(p_hook_data->new_code, code, sizeof(code)); // hook代码

		long long jmp_addr = (long long)old_address + asm_size;  // 跳过复制的原代码长度
		RtlMoveMemory(&code[2], &jmp_addr, 8);
		RtlMoveMemory(p_hook_data->jmp_old_code, old_address, asm_size); // 复制原代码(先执行原代码)
		RtlMoveMemory(&p_hook_data->jmp_old_code[asm_size], code, sizeof(code)); // 跳转到原地址代码

		p_hook_data->size = 12;
		p_hook_data->jmp_old_size = asm_size + sizeof(code);
		return 12;
	}

	return 0;
}

/*
	获取反汇编代码长度(取出此地址的连续代码,用于hook后跳转到原函数之前执行,确保执行正确)
	address=反汇编地址
	size=最少长度(最少需要取hook的长度,保留破坏之前代码)
	返回需要保留的长度
*/
int Hook::GetDisasmCodeSize(LPVOID address, int size)
{
	if (m_nBitMode == 32) // 32位其实不需要反汇编, 函数开头5个字节不会被破坏
		return 5;

	if (m_funcDisasm64 == nullptr)
		return 0;

	size_t oplen = 0;
	char*  szMnemonic;
	size_t iLen, mLen;
	char* addr = (char*)address;
	while (oplen < size) {
		szMnemonic = m_funcDisasm64(addr + oplen, 32, &iLen, &mLen, 0x00);
		//printf("汇编指令：%s (%d)\n", szMnemonic, iLen);
		oplen += iLen;

		if (oplen >= size)
			break;
	}

	return oplen;
}

// 获取跳转到原函数之函数地址 arg_num=原参数数量数
JMPOLDFUNC Hook::GetJmptOldFuncAddress(LPVOID old_address)
{
	BYTE index = m_hkLen;
	for (BYTE i = 0; i < m_hkLen; i++) {
		if (m_hkData[i].old_address == old_address) {
			if (m_hkData[i].flag == 1) {
				char* p = (char*)m_hkData[i].jmp_old_code;;
				//return (JMPOLDFUNC)p; // 已经HOOK
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

	RtlMoveMemory(Asm_JmpHookOldFunc, m_hkData[index].jmp_old_code, m_hkData[index].jmp_old_size); // hook代码
	VirtualProtectEx(GetCurrentProcess(), Asm_JmpHookOldFunc, m_hkData[index].jmp_old_size, old, &old); // 还原页码属性

	m_hkData[index].flag |= 0x02; // 标志为已写入
	return Asm_JmpHookOldFunc;
#if 0
	PVOID asm_func = Asm_JmpHookOldFunc;
	// push rcx (0x51)
	// push rdx (0x52)
	// push r8  (0x41 0x50)
	// push r9  (0x41 0x51)
	// mov  rax, 数值 (0x48 0xB8 64位数)
	// mov  qword ptr [rsp+20h],rax (0x48 0x89 0x44 0x24 20[变值])
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

// 取消地址为address的附加
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

				m_hkData[i].flag = 0; // 标志为未挂钩
				//printf("取消成功(%d位)\n", m_nBitMode);
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