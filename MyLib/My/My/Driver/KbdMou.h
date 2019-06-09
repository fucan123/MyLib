#pragma once
#include <windows.h>

// ��װ����
bool Drv_InstallDriver();
// ж������
bool Drv_UnstallDriver();
// ���ӵ�����
bool Drv_ConnectDriver();
// �ر�����
bool Drv_DisConnectDriver();

// ���¼� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
bool Drv_KeyDown(USHORT MakeCode, bool IsScanCode=false);
// �ͷż� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
bool Drv_KeyUp(USHORT MakeCode, bool IsScanCode = false);

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


// Flags=���»��� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
static bool Drv_KeyMake(USHORT Flags, USHORT MakeCode, bool IsScanCode = false);
// Flags ButtonFlags LastX LastY
static bool Drv_MouseMake(USHORT Flags, USHORT ButtonFlags, USHORT LastX, USHORT LastY);