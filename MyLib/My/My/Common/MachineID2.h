#pragma once

#include "md5.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <intrin.h>
#include <iostream>

using namespace std;

#pragma comment ( lib, "Iphlpapi.lib" )

class MachineID2
{
public:
	void GetMachineID(char id[32]);
	// ���������
	char* GetNetWorkCode();
	// ��ȡCPUID
	void  GetCPUID(int cpuid[12]);
	// ��ȡӲ������
	void  GetDisk(char key[4]);
	// ��ȡӲ������
	BOOL GetHDID(PCHAR pIDBufer);
	// ִ��CMD�����ȡ�����
	bool ExecCmd(const char* cmd, char* result, int size, bool trim_space=true);
	// �ֽ�ת��ʮ�������ַ�
	char* GetHexByByte(BYTE*, UINT);
};