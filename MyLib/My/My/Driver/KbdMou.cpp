#include "KbdMou.h"
#include "JumpSafe.h"
#include <stdio.h>

// 链接
#define SYMBOLICLINK_NAME TEXT("\\\\.\\MyKbdMouseDriver")

#define CTL_XXX CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTL_INSTALL_KBD 0x01
#define CTL_INSTALL_MOU 0x02
#define CTL_SENDKBD     0x10
#define CTL_SENDMOU     0x20

#define KEYBOARD_DOWN 0x00
#define KEYBOARD_UP   0x01

#define MOUSE_LEFT_BUTTON_DOWN   0x0001  // Left Button changed to down.
#define MOUSE_LEFT_BUTTON_UP     0x0002  // Left Button changed to up.
#define MOUSE_RIGHT_BUTTON_DOWN  0x0004  // Right Button changed to down.
#define MOUSE_RIGHT_BUTTON_UP    0x0008  // Right Button changed to up.
#define MOUSE_MIDDLE_BUTTON_DOWN 0x0010  // Middle Button changed to down.
#define MOUSE_MIDDLE_BUTTON_UP   0x0020  // Middle Button changed to up.

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
} CTL_BUFF, *PCTL_BUFF;

static JumpSafe __safe;
static HANDLE   __hDevice = NULL;
static USHORT   __nScreenX = 0;
static USHORT   __nScreenY = 0;

// 安装驱动
bool Drv_InstallDriver()
{
	return __safe.Install(L"KbdMouseDriver_2019", L"KbdMouse Driver(2019)", L"C:\\\\Kb_Mouse_Driver.sys");
}

// 卸载驱动
bool Drv_UnstallDriver()
{
	return __safe.UnInstall();
}

// 连接到驱动
bool Drv_ConnectDriver()
{
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
	printf("x:%d y:%d\n", __nScreenX, __nScreenY);

	return __hDevice != INVALID_HANDLE_VALUE;
}

// 关闭连接
bool Drv_DisConnectDriver()
{
	if (!__hDevice)
		return false;

	return CloseHandle(__hDevice);
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

// 鼠标左键按下 LastX=坐标x LastY坐标y
bool Drv_MouseLeftDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_LEFT_BUTTON_DOWN, LastX, LastY);
}

// 鼠标左键释放 LastX=坐标x LastY坐标y
bool Drv_MouseLeftUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_LEFT_BUTTON_UP, LastX, LastY);
}

// 鼠标右键按下 LastX=坐标x LastY坐标y
bool Drv_MouseRightDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_RIGHT_BUTTON_DOWN, LastX, LastY);
}

// 鼠标右键释放 LastX=坐标x LastY坐标y
bool Drv_MouseRightUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_RIGHT_BUTTON_DOWN, LastX, LastY);
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
static bool Drv_MouseMake(USHORT Flags, USHORT ButtonFlags, USHORT LastX, USHORT LastY)
{
	if (__nScreenX && LastX) {
		ULONG v = LastX * 0xffff;
		LastX = v / __nScreenX;
	}
	if (__nScreenY && LastY) {
		ULONG v = LastY * 0xffff;
		LastY = v / __nScreenY;
	}

	CTL_BUFF c;
	c.OpCode = CTL_SENDMOU;
	c.Mou.Flags = Flags;
	c.Mou.Buttons = 0;
	c.Mou.ButtonFlags = ButtonFlags;
	c.Mou.LastX = LastX;
	c.Mou.LastY = LastY;

	//printf("ButtonFlags:%d %d,%d\n", ButtonFlags, LastX, LastY);

	int result = 0;
	BOOL r = DeviceIoControl(__hDevice, CTL_XXX, (LPVOID)&c, sizeof(CTL_BUFF), &result, sizeof(DWORD), NULL, NULL);

	return r && result;
}
