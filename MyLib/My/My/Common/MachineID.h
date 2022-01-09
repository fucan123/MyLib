#pragma once

#include "md5.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <intrin.h>
#include <iostream>

using namespace std;

#pragma comment ( lib, "Iphlpapi.lib" )

class MachineID
{
public:
	void GetMachineID(char id[32]);
	// ���������
	char* GetNetWorkCode();
	// ��ȡCPUID
	void  GetCPUID(int cpuid[12]);
	// ��ȡӲ������
	void  GetDisk(char key[4]);
	// �ֽ�ת��ʮ�������ַ�
	char* GetHexByByte(BYTE*, UINT);
};