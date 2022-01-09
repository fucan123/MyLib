#pragma once

#ifdef x64
extern "C"
{
	int __stdcall Asm_JmpHookOldFunc(...);
}
#else
__declspec(naked) int __cdecl Asm_JmpHookOldFunc(...)
{
	// 前面5字节等待填充为原函数指令
	__asm {
		mov esi, esi;  // 占位
		push ebp;      // 占位
		mov ebp, esp;  // 占位
		mov eax, 0x00;
		jmp eax;
		ret;
	}
}
#endif
