#pragma once

#ifdef x64
extern "C"
{
	int __stdcall Asm_JmpHookOldFunc(...);
}
#else
__declspec(naked) int __cdecl Asm_JmpHookOldFunc(...)
{
	// ǰ��5�ֽڵȴ����Ϊԭ����ָ��
	__asm {
		mov esi, esi;  // ռλ
		push ebp;      // ռλ
		mov ebp, esp;  // ռλ
		mov eax, 0x00;
		jmp eax;
		ret;
	}
}
#endif
