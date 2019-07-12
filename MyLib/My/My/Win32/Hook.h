#pragma once
#include <Windows.h>

//  ���֧�ֶ��ٸ�����HOOK
#define MAX_HOOK_FUNC_NUM 16

typedef struct _hook_data {
	LPVOID address;
	BYTE   oldCode[12];
	SIZE_T size; // �ı���ֽڴ�С
	BYTE   flag; // 0-û��HOOK 1-��HOOK
} HK_DATA;

class Hook {
private :
	HWND m_hWnd;
	BYTE m_hkLen;
	HK_DATA m_hkData[MAX_HOOK_FUNC_NUM];
private:
	Hook();
public:
	static Hook* self;
public:
	// ����hwnd
	void SetHwnd(HWND hwnd);
	// ���ӵ�ַΪaddress�ĺ��� ��ת��func
	void Attach(LPVOID address, LPVOID func);
	// ȡ����ַΪaddress�ĸ���
	void Detach(LPVOID address);
	// ����ʵ������
	static Hook* GetInstance();
	// ɾ������ʵ������
	static void  DeleteInstance();
};