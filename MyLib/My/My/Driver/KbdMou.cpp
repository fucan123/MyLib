#include "KbdMou.h"
#include "Sys.h"
#include <shellscalingapi.h>
#include <math.h>
#include <stdio.h>

// ����
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
	char ch[255];
	int  ch_len;
} CTL_BUFF, *PCTL_BUFF;

static Sys      __sysKbdMou;
static HANDLE   __hDevice = NULL;
static USHORT   __nScreenX = 0;
static USHORT   __nScreenY = 0;

// ��װ����
bool Drv_InstallDriver(const wchar_t* file)
{
	return __sysKbdMou.Install(L"google_chrome", L"Google Chrome", file);
}

// ж������
bool Drv_UnstallDriver()
{
	return __sysKbdMou.UnInstall();
}

// ���ӵ�����
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

// �ر�����
bool Drv_DisConnectDriver()
{
	if (!__hDevice)
		return false;

	return CloseHandle(__hDevice);
}

// ��������
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

// ��������
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

	//printf("����:%c(%d)\n", key, key);
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

// ת�ɴ�д
char Drv_Key2Cap(char key)
{
	if (key >= 'a' && key <= 'z')
		return key - 32;
	return key;
}

// �Ƿ��д
bool Drv_KeyIsCap(char key)
{
	return key >= 'A' && key <= 'Z';
}

//  ������ LastX=����x LastY����y
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

//  �Ҽ���� LastX=����x LastY����y
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

// ���������� LastX=����x LastY����y
bool Drv_MouseLeftDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_LEFT_BUTTON_DOWN, LastX, LastY);
}

// �������ͷ� LastX=����x LastY����y
bool Drv_MouseLeftUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_LEFT_BUTTON_UP, LastX, LastY);
}

// ����Ҽ����� LastX=����x LastY����y
bool Drv_MouseRightDown(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_RIGHT_BUTTON_DOWN, LastX, LastY);
}

// ����Ҽ��ͷ� LastX=����x LastY����y
bool Drv_MouseRightUp(USHORT LastX, USHORT LastY)
{
	return Drv_MouseMake(MOUSE_MOVE_ABSOLUTE, MOUSE_RIGHT_BUTTON_UP, LastX, LastY);
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

// ������[����]
bool Drv_MouseWheel(USHORT ButtonData)
{
	return Drv_MouseMake(0, MOUSE_WHEEL, 0, 0, ButtonData);
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
