#include "KbdMou.h"
#include "JumpSafe.h"
#include <stdio.h>

// ����
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
	UINT32 OpCode;    // ������
	union {
		struct {
			USHORT MakeCode;  // ������
			USHORT Flags;     // ���»���
		} Kbd;
		struct {
			/*
			MOUSE_MOVE_RELATIVE[��LastX��LastY�������ǰ��λ������]
			MOUSE_MOVE_ABSOLUTE[��LastX��LastYֵ����Ϊ����ֵ]
			*/
			USHORT Flags;     // ��־
			union {
				ULONG Buttons;
				struct {
					USHORT ButtonFlags; // ����ĸ������µ���
					USHORT ButtonData;
				};
			};
			ULONG  LastX;     // ָ��x�����ϵĴ�������Ի�����˶�
			ULONG  LastY;     // ָ��y�����ϵĴ�������Ի�����˶�
		} Mou;
	};
} CTL_BUFF, *PCTL_BUFF;

static JumpSafe __safe;
static HANDLE   __hDevice = NULL;
static USHORT   __nScreenX = 0;
static USHORT   __nScreenY = 0;

// ��װ����
bool Drv_InstallDriver()
{
	return __safe.Install(L"KbdMouseDriver_2019", L"KbdMouse Driver(2019)", L"C:\\\\Kb_Mouse_Driver.sys");
}

// ж������
bool Drv_UnstallDriver()
{
	return __safe.UnInstall();
}

// ���ӵ�����
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

// �ر�����
bool Drv_DisConnectDriver()
{
	if (!__hDevice)
		return false;

	return CloseHandle(__hDevice);
}

// ���¼� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
bool Drv_KeyDown(USHORT MakeCode, bool IsScanCode)
{
	return Drv_KeyMake(KEYBOARD_DOWN, MakeCode, IsScanCode);
}

// �ͷż� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
bool Drv_KeyUp(USHORT MakeCode, bool IsScanCode)
{
	return Drv_KeyMake(KEYBOARD_UP, MakeCode, IsScanCode);
}

// ���������� LastX=����x LastY����y
bool Drv_MouseLeftDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_LEFT_BUTTON_DOWN, LastX, LastY);
}

// �������ͷ� LastX=����x LastY����y
bool Drv_MouseLeftUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_LEFT_BUTTON_UP, LastX, LastY);
}

// ����Ҽ����� LastX=����x LastY����y
bool Drv_MouseRightDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_RIGHT_BUTTON_DOWN, LastX, LastY);
}

// ����Ҽ��ͷ� LastX=����x LastY����y
bool Drv_MouseRightUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(0, MOUSE_RIGHT_BUTTON_DOWN, LastX, LastY);
}

// ����ƶ�[���]
bool Drv_MouseMovRelative(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_RELATIVE, 0, LastX, LastY);
}

// ����ƶ�[����]
bool Drv_MouseMovAbsolute(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, 0, LastX, LastY);
}

// ���¼� Flags=���»��� MakeCode=������ IsScanCode=�Ƿ�����ɨ���룬���ǻ��Զ�ת��
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
