#pragma once
#include <Windows.h>

typedef char* (*Disasm64Func)(PVOID Ptr, size_t BufferLength, size_t *InstructionLength, size_t *MnemonicLength, size_t pc);
#ifdef x64
typedef int(__stdcall* JMPOLDFUNC)(...);
#else
typedef int(__cdecl* JMPOLDFUNC)(...);
#endif

//  ���֧�ֶ��ٸ�����HOOK
#define MAX_HOOK_FUNC_NUM 16

typedef struct hook_old_func_arg
{
	int Param[15]; // ԭ��������(�������ҿ�ʼ��, ��send(SOCKET,char*,int,int), 0=SOCKET, 1=char*, ...)
	int Length;    // ԭ������������
} HookOldFuncArg;  // ԭ��������(32λ���˽ṹ��, ��ʹ��JmpOldFunc32��ת)


typedef struct _hook_data {
	BYTE   old_code[64];     // �ɴ���(ԭ��ַ����)
	BYTE   new_code[64];     // �´���(hook����)
	BYTE   jmp_old_code[64]; // ��ת��ԭ��������[����hook����,��ִ��old_code����]
	LPVOID old_address;      // hook��λ��
	LPVOID new_address;      // �µĵ�ַλ��
	SIZE_T size;             // �ı���ֽڴ�С
	SIZE_T jmp_old_size;     // jmp_old_code��С
	INT    arg_num;          // ������������
	BYTE   flag;             // 0-ʲô��û�� and 0x01=��hook and 0x02=��д��jmp����
} HK_DATA;

class Hook {
private :
	BYTE m_hkLen;
	HK_DATA m_hkData[MAX_HOOK_FUNC_NUM];
	Disasm64Func m_funcDisasm64;

	// 32��64λ
	int m_nBitMode;
private:
	Hook();
public:
	static Hook* self;
private:
	// ����hook���� old_address=Ҫhook��λ�� new_address=�º�����ַ ���ش��볤��
	int MakeShellCode(LPVOID old_address, LPVOID new_address, HK_DATA* p_hook_data);
	/*
		��ȡ�������볤��(ȡ���˵�ַ����������,����hook����ת��ԭ����֮ǰִ��,ȷ��ִ����ȷ) 
		address=������ַ
		size=���ٳ���(������Ҫȡhook�ĳ���,�����ƻ�֮ǰ����)
		������Ҫ�����ĳ���
	*/
	int GetDisasmCodeSize(LPVOID address, int size);
public:
	// ����ģʽ bit=32|64 ����64��Ҫ����path path=�����dll�ļ�·�� 
	bool SetMode(int bit, const char* path=nullptr);
	// old_address=Ҫhook��λ�� new_address=�º�����ַ
	bool Attach(LPVOID old_address, LPVOID new_address);
	// ��ȡ��ת��ԭ����֮������ַ arg_num=ԭ����������
	JMPOLDFUNC GetJmptOldFuncAddress(LPVOID old_address);
	// ȡ����ַΪaddress�ĸ���
	void Detach(LPVOID address);
	// ����ʵ������
	static Hook* GetInstance();
	// ɾ������ʵ������
	static void  DeleteInstance();
};