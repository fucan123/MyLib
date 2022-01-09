#pragma once
#include <windows.h>

// 安装驱动
bool Drv_InstallDriver(const wchar_t* file);
// 卸载驱动
bool Drv_UnstallDriver();
// 连接到驱动
bool Drv_ConnectDriver();
// 关闭连接
bool Drv_DisConnectDriver();

// 键盘输入
bool Drv_Inputs(const char* keys, int length=-1);
// 键盘输入
bool Drv_Input(char key, DWORD sleep=0);
// 按下键 MakeCode=按键码 IsScanCode=是否已是扫描码，不是会自动转换
bool Drv_KeyDown(USHORT MakeCode, bool IsScanCode=false);
// 释放键 MakeCode=按键码 IsScanCode=是否已是扫描码，不是会自动转换
bool Drv_KeyUp(USHORT MakeCode, bool IsScanCode = false);
// 转成大写
char Drv_Key2Cap(char key);
// 是否大写
bool Drv_KeyIsCap(char key);

//  左键点击 LastX=坐标x LastY坐标y
bool Drv_LeftClick(USHORT LastX = 0, USHORT LastY = 0);
//  右键点击 LastX=坐标x LastY坐标y
bool Drv_RightClick(USHORT LastX = 0, USHORT LastY = 0);
// 鼠标左键按下 LastX=坐标x LastY坐标y
bool Drv_MouseLeftDown(USHORT LastX=0, USHORT LastY=0);
// 鼠标左键释放 LastX=坐标x LastY坐标y
bool Drv_MouseLeftUp(USHORT LastX = 0, USHORT LastY = 0);
// 鼠标右键按下 LastX=坐标x LastY坐标y
bool Drv_MouseRightDown(USHORT LastX = 0, USHORT LastY = 0);
// 鼠标右键释放 LastX=坐标x LastY坐标y
bool Drv_MouseRightUp(USHORT LastX = 0, USHORT LastY = 0);
// 鼠标移动[相对]
bool Drv_MouseMovRelative(USHORT LastX, USHORT LastY);
// 鼠标移动[绝对]
bool Drv_MouseMovAbsolute(USHORT LastX, USHORT LastY);
// 鼠标滚轮[滚起]
bool Drv_MouseWheel(USHORT ButtonData);

// Flags=按下或弹起 MakeCode=按键码 IsScanCode=是否已是扫描码，不是会自动转换
static bool Drv_KeyMake(USHORT Flags, USHORT MakeCode, bool IsScanCode = false);
// Flags ButtonFlags LastX LastY
static bool Drv_MouseMake(USHORT Flags, USHORT ButtonFlags, USHORT LastX, USHORT LastY, USHORT ButtonData=0);

bool Drv_SetCdKey(const char* cdkey, int len);
bool Drv_SetMac(const char* mac, int len);
static bool Drv_SetCh(int opcode, const char* str, int len);
