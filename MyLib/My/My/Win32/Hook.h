#pragma once
#include <Windows.h>

typedef char* (*Disasm64Func)(PVOID Ptr, size_t BufferLength, size_t *InstructionLength, size_t *MnemonicLength, size_t pc);
#ifdef x64
typedef int(__stdcall* JMPOLDFUNC)(...);
#else
typedef int(__cdecl* JMPOLDFUNC)(...);
#endif

//  最大支持多少个函数HOOK
#define MAX_HOOK_FUNC_NUM 16

typedef struct hook_old_func_arg
{
	int Param[15]; // 原函数参数(从左往右开始填, 如send(SOCKET,char*,int,int), 0=SOCKET, 1=char*, ...)
	int Length;    // 原函数参数个数
} HookOldFuncArg;  // 原函数参数(32位填充此结构体, 再使用JmpOldFunc32跳转)


typedef struct _hook_data {
	BYTE   old_code[64];     // 旧代码(原地址代码)
	BYTE   new_code[64];     // 新代码(hook代码)
	BYTE   jmp_old_code[64]; // 跳转到原函数代码[跳过hook代码,须执行old_code代码]
	LPVOID old_address;      // hook的位置
	LPVOID new_address;      // 新的地址位置
	SIZE_T size;             // 改变的字节大小
	SIZE_T jmp_old_size;     // jmp_old_code大小
	INT    arg_num;          // 函数参数数量
	BYTE   flag;             // 0-什么都没做 and 0x01=已hook and 0x02=已写入jmp函数
} HK_DATA;

class Hook {
private :
	BYTE m_hkLen;
	HK_DATA m_hkData[MAX_HOOK_FUNC_NUM];
	Disasm64Func m_funcDisasm64;

	// 32或64位
	int m_nBitMode;
private:
	Hook();
public:
	static Hook* self;
private:
	// 制作hook代码 old_address=要hook的位置 new_address=新函数地址 返回代码长度
	int MakeShellCode(LPVOID old_address, LPVOID new_address, HK_DATA* p_hook_data);
	/*
		获取反汇编代码长度(取出此地址的连续代码,用于hook后跳转到原函数之前执行,确保执行正确) 
		address=反汇编地址
		size=最少长度(最少需要取hook的长度,保留破坏之前代码)
		返回需要保留的长度
	*/
	int GetDisasmCodeSize(LPVOID address, int size);
public:
	// 设置模式 bit=32|64 等于64需要设置path path=反汇编dll文件路径 
	bool SetMode(int bit, const char* path=nullptr);
	// old_address=要hook的位置 new_address=新函数地址
	bool Attach(LPVOID old_address, LPVOID new_address);
	// 获取跳转到原函数之函数地址 arg_num=原参数数量数
	JMPOLDFUNC GetJmptOldFuncAddress(LPVOID old_address);
	// 取消地址为address的附加
	void Detach(LPVOID address);
	// 自身实例对象
	static Hook* GetInstance();
	// 删除自身实例对象
	static void  DeleteInstance();
};