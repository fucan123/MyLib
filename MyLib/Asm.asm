.CODE

Asm_JmpHookOldFunc PROC
    push rcx
	push rdx
	push r8
	push r9
	mov  rax, 5000000008h
	mov  qword ptr [rsp+20h], rax
	mov  qword ptr [rsp+28h], rax
	mov  qword ptr [rsp+30h], rax
	mov  qword ptr [rsp+168h], rax
	mov  qword ptr [rsp+16899h], rax

	ret
Asm_JmpHookOldFunc ENDP

END