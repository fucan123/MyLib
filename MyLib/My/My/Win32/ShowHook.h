#pragma once
#include <Windows.h>
#include <vector>

#define MAX_FUNC_BYTE_SIZE 1024

typedef struct _original_func_
{
	char  Name[32];                 // 名称
	PBYTE Address;                  // 地址
	BYTE  Codes[MAX_FUNC_BYTE_SIZE]; // 代码
	int   Len;                      // 代码长度
} OriginalFunc;

class ShowHook
{
private:
	std::vector<OriginalFunc*> m_Funcs;
public:
	// 读取原始函数
	void ReadOrigFunc(const char* name, void* address, bool print=false);
	// 显示被HOOK的函数
	int  Show(bool printf=true);
};