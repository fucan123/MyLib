#include "JumpSafe.h"
#include "../Common/func.h"

#define CTL_DEVICE CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define CTLCODE_FAKE 0x10

BOOL JumpSafe::CreateDevice()
{
	m_hDevice = CreateFile(TEXT("\\\\.\\MyDDK"),
		GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (m_hDevice == INVALID_HANDLE_VALUE) {
		SetError(1, L"设备打开失败");
		return FALSE;
	}
	return TRUE;
}

BOOL JumpSafe::FakeProcess(WCHAR * SystemName, bool ce)
{
	DWORD Pid = 0;
	if (SystemName)
		Pid = SGetProcessId(SystemName);
	else 
		Pid = SGetProcessId(L"csrss.exe");
		
	if (!Pid) {
		SetError(2, L"获取进程ID失败");
		return FALSE;
	}

	CTL_FAKE Ctl;
	Ctl.Code = CTLCODE_FAKE;
	Ctl.FromPid = Pid;
	Ctl.ToPid = GetCurrentProcessId();
	DWORD dwRet;
	if (!DeviceIoControl(m_hDevice, CTL_DEVICE, (LPVOID)&Ctl, sizeof(Ctl), NULL, 0, &dwRet, NULL)) {
		SetError(3, L"伪装进程失败");
		return FALSE;
	}
	if (ce) {
		DWORD cePid = SGetProcessId(L"cheatengine-x86_64.exe");
		if (cePid) {
			Ctl.ToPid = cePid;
			DeviceIoControl(m_hDevice, CTL_DEVICE, (LPVOID)&Ctl, sizeof(Ctl), NULL, 0, &dwRet, NULL);
		}
	}
	return TRUE;
}

VOID JumpSafe::CloseDevice()
{
	if (m_hDevice)
		CloseHandle(m_hDevice);
	m_hDevice = NULL;
}

VOID JumpSafe::SetError(INT32 ErrorCode, CONST WCHAR* ErrorStr)
{
	m_ErrorCode = ErrorCode;
	if (ErrorStr) {
		wcscpy_s(m_ErrorStr, _countof(m_ErrorStr), ErrorStr);
	}
}

INT32 JumpSafe::GetErrorCode()
{
	return m_ErrorCode;
}

INT32 JumpSafe::GetErrorCodeAndPrint()
{
	PrintError();
	return m_ErrorCode;
}

WCHAR* JumpSafe::GetErrorInfo()
{
	return m_ErrorStr;
}

VOID JumpSafe::PrintError()
{
	
}

JumpSafe::~JumpSafe()
{
}
