#pragma once
#include <Windows.h>
#include <vector>

#define MAX_FUNC_BYTE_SIZE 1024

typedef struct _original_func_
{
	char  Name[32];                 // ����
	PBYTE Address;                  // ��ַ
	BYTE  Codes[MAX_FUNC_BYTE_SIZE]; // ����
	int   Len;                      // ���볤��
} OriginalFunc;

class ShowHook
{
private:
	std::vector<OriginalFunc*> m_Funcs;
public:
	// ��ȡԭʼ����
	void ReadOrigFunc(const char* name, void* address, bool print=false);
	// ��ʾ��HOOK�ĺ���
	int  Show(bool printf=true);
};