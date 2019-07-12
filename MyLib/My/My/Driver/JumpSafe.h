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
	/* ���������豸ͨ�� */
	BOOL  CreateDevice();
	/* 
	  αװ��ϵͳ����
	  @SystemName ϵͳ�������� Ĭ�� csrss.exe
	*/
	BOOL  FakeProcess(WCHAR* SystemName=NULL, bool ce=false, bool od=false);
	/*  �ر��豸 */
	VOID  CloseDevice();
	/*
	  ���ô�����Ϣ
	  @ErrorCode ������
	  @ErrorStr  �����ַ�
	*/
	VOID  SetError(INT32 ErrorCode, CONST WCHAR* ErrorStr=NULL);
	/* ��ô����� */
	INT32 GetErrorCode();
	/* ��ô����벢��ӡ������Ϣ */
	INT32 GetErrorCodeAndPrint();
	/* ��ȡ������Ϣ */
	WCHAR* GetErrorInfo();
	/* ��ӡ������Ϣ */
	VOID  PrintError();
	~JumpSafe();
private:
	WCHAR   m_ErrorStr[128];
	INT32  m_ErrorCode;
	HANDLE m_hDevice;
};
