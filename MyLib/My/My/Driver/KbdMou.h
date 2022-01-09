#pragma once
#include <windows.h>

// ��װ����
bool Drv_InstallDriver(const wchar_t* file);
// ж������
bool Drv_UnstallDriver();
// ���ӵ�����
bool Drv_ConnectDriver();
// �ر�����
bool Drv_DisConnectDriver();

// ��������
bool Drv_Inputs(const char* keys, int length=-1);
// ��������
bool Drv_Input(char key, DWORD sleep=0);
// ���¼� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
bool Drv_KeyDown(USHORT MakeCode, bool IsScanCode=false);
// �ͷż� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
bool Drv_KeyUp(USHORT MakeCode, bool IsScanCode = false);
// ת�ɴ�д
char Drv_Key2Cap(char key);
// �Ƿ��д
bool Drv_KeyIsCap(char key);

//  ������ LastX=����x LastY����y
bool Drv_LeftClick(USHORT LastX = 0, USHORT LastY = 0);
//  �Ҽ���� LastX=����x LastY����y
bool Drv_RightClick(USHORT LastX = 0, USHORT LastY = 0);
// ���������� LastX=����x LastY����y
bool Drv_MouseLeftDown(USHORT LastX=0, USHORT LastY=0);
// �������ͷ� LastX=����x LastY����y
bool Drv_MouseLeftUp(USHORT LastX = 0, USHORT LastY = 0);
// ����Ҽ����� LastX=����x LastY����y
bool Drv_MouseRightDown(USHORT LastX = 0, USHORT LastY = 0);
// ����Ҽ��ͷ� LastX=����x LastY����y
bool Drv_MouseRightUp(USHORT LastX = 0, USHORT LastY = 0);
// ����ƶ�[���]
bool Drv_MouseMovRelative(USHORT LastX, USHORT LastY);
// ����ƶ�[����]
bool Drv_MouseMovAbsolute(USHORT LastX, USHORT LastY);
// ������[����]
bool Drv_MouseWheel(USHORT ButtonData);

// Flags=���»��� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
static bool Drv_KeyMake(USHORT Flags, USHORT MakeCode, bool IsScanCode = false);
// Flags ButtonFlags LastX LastY
static bool Drv_MouseMake(USHORT Flags, USHORT ButtonFlags, USHORT LastX, USHORT LastY, USHORT ButtonData=0);

bool Drv_SetCdKey(const char* cdkey, int len);
bool Drv_SetMac(const char* mac, int len);
static bool Drv_SetCh(int opcode, const char* str, int len);
