#include "KbdMou.h"
#include "Sys.h"
#include <shellscalingapi.h>
#include <math.h>
#include <stdio.h>

// 链接
#define SYMBOLICLINK_NAME TEXT("\\\\.\\google_mail")

#define CTL_XXX CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTL_INSTALL_KBD 0x01
#define CTL_INSTALL_MOU 0x02
#define CTL_SENDKBD     0x10
#define CTL_SENDMOU     0x20
#define CTL_SETCDKEY    0x80
#define CTL_SETMAC      0x90

#define KEYBOARD_DOWN 0x00
#define KEYBOARD_UP   0x01

#define MOUSE_LEFT_BUTTON_DOWN   0x0001  // Left Button changed to down.
#define MOUSE_LEFT_BUTTON_UP     0x0002  // Left Button changed to up.
#define MOUSE_RIGHT_BUTTON_DOWN  0x0004  // Right Button changed to down.
#define MOUSE_RIGHT_BUTTON_UP    0x0008  // Right Button changed to up.
#define MOUSE_MIDDLE_BUTTON_DOWN 0x0010  // Middle Button changed to down.
#define MOUSE_MIDDLE_BUTTON_UP   0x0020  // Middle Button changed to up.
#define MOUSE_WHEEL              0x0400

typedef struct _CTL_BUFF {
	UINT32 OpCode;    // 操作码
	union {
		struct {
			USHORT MakeCode;  // 键盘码
			USHORT Flags;     // 按下或弹起
		} Kbd;
		struct {
			/*
			MOUSE_MOVE_RELATIVE[该LastX和LastY相对于以前的位置设置]
			MOUSE_MOVE_ABSOLUTE[该LastX和LastY值设置为绝对值]
			*/
			USHORT Flags;     // 标志
			union {
				ULONG Buttons;
				struct {
					USHORT ButtonFlags; // 鼠标哪个键按下弹起
					USHORT ButtonData;
				};
			};
			ULONG  LastX;     // 指定x方向上的带符号相对或绝对运动
			ULONG  LastY;     // 指定y方向上的带符号相对或绝对运动
		} Mou;
	};
	char ch[255];
	int  ch_len;
} CTL_BUFF, *PCTL_BUFF;

static Sys      __sysKbdMou;
static HANDLE   __hDevice = NULL;
static USHORT   __nScreenX = 0;
static USHORT   __nScreenY = 0;

// 安装驱动
bool Drv_InstallDriver(const wchar_t* file)
{
	return __sysKbdMou.Install(L"google_chrome", L"Google Chrome", file);
}

// 卸载驱动
bool Drv_UnstallDriver()
{
	return __sysKbdMou.UnInstall();
}

// 连接到驱动
bool Drv_ConnectDriver()
{
	SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	__hDevice = CreateFile(SYMBOLICLINK_NAME,
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	__nScreenX = GetSystemMetrics(SM_CXSCREEN);
	__nScreenY = GetSystemMetrics(SM_CYSCREEN);
	//printf("x:%d y:%d\n", __nScreenX, __nScreenY);

	return __hDevice != INVALID_HANDLE_VALUE;
}

// 关闭连接
bool Drv_DisConnectDriver()
{
	if (!__hDevice)
		return false;

	return CloseHandle(__hDevice);
}

// 键盘输入
bool Drv_Inputs(const char* keys, int length)
{
	if (length == -1)
		length = strlen(keys);
	for (int i = 0; i < length; i++) {
		Drv_Input(keys[i]);
		Sleep(250);
	}
	return true;
}

// 键盘输入
bool Drv_Input(char key, DWORD sleep)
{
	bool is_cap = false;
	if (Drv_KeyIsCap(key)) {
		is_cap = true;
		Drv_KeyDown(VK_SHIFT, false);
		Sleep(100);
	}
	else if (key >= 'a' && key <= 'z') {
		key -= 32;
	}

	//printf("输入:%c(%d)\n", key, key);
	key = Drv_Key2Cap(key);
	Drv_KeyDown(key, false);
	if (sleep) {
		Sleep(sleep);
	}
	if (is_cap) {
		Drv_KeyUp(VK_SHIFT, false);
		Sleep(100);
	}
	return Drv_KeyUp(key);
}

// 按下键 MakeCode=按键码 IsScanCode=是否已是扫描码，不是会自动转换
bool Drv_KeyDown(USHORT MakeCode, bool IsScanCode)
{
	return Drv_KeyMake(KEYBOARD_DOWN, MakeCode, IsScanCode);
}

// 释放键 MakeCode=按键码 IsScanCode=是否已是扫描码，不是会自动转换
bool Drv_KeyUp(USHORT MakeCode, bool IsScanCode)
{
	return Drv_KeyMake(KEYBOARD_UP, MakeCode, IsScanCode);
}

// 转成大写
char Drv_Key2Cap(char key)
{
	if (key >= 'a' && key <= 'z')
		return key - 32;
	return key;
}

// 是否大写
bool Drv_KeyIsCap(char key)
{
	return key >= 'A' && key <= 'Z';
}

//  左键点击 LastX=坐标x LastY坐标y
bool Drv_LeftClick(USHORT LastX, USHORT LastY)
{
	if (Drv_MouseMovAbsolute(LastX, LastY)) {
		//printf("Drv_LeftClick:%d.%d\n", LastX, LastY);
		Sleep(50);
		Drv_MouseLeftDown(LastX, LastY);
		Sleep(25);
		Drv_MouseLeftUp(LastX, LastY);
		return true;
	}
	return false;
}

//  右键点击 LastX=坐标x LastY坐标y
bool Drv_RightClick(USHORT LastX, USHORT LastY)
{
	if (Drv_MouseMovAbsolute(LastX, LastY)) {
		Sleep(50);
		Drv_MouseRightDown();
		Drv_MouseRightUp();
		return true;
	}
	return false;
}

// 鼠标左键按下 LastX=坐标x LastY坐标y
bool Drv_MouseLeftDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_LEFT_BUTTON_DOWN, LastX, LastY);
}

// 鼠标左键释放 LastX=坐标x LastY坐标y
bool Drv_MouseLeftUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_LEFT_BUTTON_UP, LastX, LastY);
}

// 鼠标右键按下 LastX=坐标x LastY坐标y
bool Drv_MouseRightDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_RIGHT_BUTTON_DOWN, LastX, LastY);
}

// 鼠标右键释放 LastX=坐标x LastY坐标y
bool Drv_MouseRightUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_RIGHT_BUTTON_UP, LastX, LastY);
}

// 鼠标移动[相对]
bool Drv_MouseMovRelative(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_RELATIVE, 0, LastX, LastY);
}

// 鼠标移动[绝对]
bool Drv_MouseMovAbsolute(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, 0, LastX, LastY);
}

// 鼠标滚轮[滚起]
bool Drv_MouseWheel(USHORT ButtonData)
{
	return Drv_MouseMake(0, MOUSE_WHEEL, 0, 0, ButtonData);
}

// 按下键 Flags=按下或弹起 MakeCode=按键码 IsScanCode=是否已是扫描码，不是会自动转换
static bool Drv_KeyMake(USHORT Flags, USHORT MakeCode, bool IsScanCode)
{
	if (!__hDevice)
		return false;

	CTL_BUFF c;
	c.OpCode = CTL_SENDKBD;
	c.Kbd.MakeCode = IsScanCode ? MakeCode : MapVirtualKey(MakeCode, 0);
	c.Kbd.Flags = Flags;

	int result = 0;
	BOOL r = DeviceIoControl(__hDevice, CTL_XXX, (LPVOID)&c, sizeof(CTL_BUFF), &result, sizeof(DWORD), NULL, NULL);

	return r && result;
}

// Flags ButtonFlags LastX LastY
static bool Drv_MouseMake(USHORT Flags, USHORT ButtonFlags, USHORT LastX, USHORT LastY, USHORT ButtonData)
{
	if (Flags == MOUSE_MOVE_ABSOLUTE) {
		if (__nScreenX && LastX) {
			//printf("mx %d %d\n", LastX, __nScreenX);
			ULONG v = LastX * 0x10000;
			double v2 = ceil((double)v / (double)__nScreenX);
			LastX = (int)v2;
		}
		if (__nScreenY && LastY) {
			//printf("my %d %d\n", LastY, __nScreenY);
			ULONG v = LastY * 0x10000;
			double v2 = ceil((double)v / (double)__nScreenY);
			LastY = (int)v2;
		}
	}
	
	CTL_BUFF c;
	c.OpCode = CTL_SENDMOU;
	c.Mou.Flags = Flags;
	c.Mou.Buttons = 0;
	c.Mou.ButtonFlags = ButtonFlags;
	c.Mou.ButtonData = ButtonData;
	c.Mou.LastX = LastX;
	c.Mou.LastY = LastY;

	//printf("ButtonFlags:%d %d,%d\n", ButtonFlags, LastX, LastY);

	int result = 0;
	BOOL r = DeviceIoControl(__hDevice, CTL_XXX, (LPVOID)&c, sizeof(CTL_BUFF), &result, sizeof(DWORD), NULL, NULL);

	return r && result;
}

bool Drv_SetCdKey(const char* cdkey, int len)
{
	return Drv_SetCh(CTL_SETCDKEY, cdkey, len);
}

bool Drv_SetMac(const char* mac, int len)
{
	return Drv_SetCh(CTL_SETMAC, mac, len);
}

bool Drv_SetCh(int opcode, const char* str, int len)
{
	CTL_BUFF c;
	c.OpCode = opcode;
	memcpy(c.ch, str, len);
	c.ch_len = len;

	int result = 0;
	BOOL r = DeviceIoControl(__hDevice, CTL_XXX, (LPVOID)&c, sizeof(CTL_BUFF), &result, sizeof(DWORD), NULL, NULL);

	return r && result;
}
