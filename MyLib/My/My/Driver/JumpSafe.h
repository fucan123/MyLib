#pragma once
#include "Sys.h"
#include <Windows.h>
#include <stdio.h>


typedef struct _CTL_FAKE {
	UINT64 Code;
	UINT64 FromPid;
	UINT64 ToPid;
} CTL_FAKE, *PCTL_FAKE;

class JumpSafe : public Sys
{
public:
	JumpSafe():
		m_ErrorCode(0),
		m_hDevice(NULL)
	{ 
		ZeroMemory(m_ErrorStr, sizeof(m_ErrorStr));
	}
	/* 创建驱动设备通信 */
	BOOL  CreateDevice();
	/* 
	  伪装成系统进程
	  @SystemName 系统进程名称 默认 csrss.exe
	*/
	BOOL  FakeProcess(WCHAR* SystemName=NULL, bool ce=false, bool od=false);
	/*  关闭设备 */
	VOID  CloseDevice();
	/*
	  设置错误信息
	  @ErrorCode 错误码
	  @ErrorStr  错误字符
	*/
	VOID  SetError(INT32 ErrorCode, CONST WCHAR* ErrorStr=NULL);
	/* 获得错误码 */
	INT32 GetErrorCode();
	/* 获得错误码并打印错误信息 */
	INT32 GetErrorCodeAndPrint();
	/* 获取错误信息 */
	WCHAR* GetErrorInfo();
	/* 打印错误信息 */
	VOID  PrintError();
	~JumpSafe();
private:
	WCHAR   m_ErrorStr[128];
	INT32  m_ErrorCode;
	HANDLE m_hDevice;
};
