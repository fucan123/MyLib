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
	// 获得网卡码
	char* GetNetWorkCode();
	// 获取CPUID
	void  GetCPUID(int cpuid[12]);
	// 获取硬盘序列
	void  GetDisk(char key[4]);
	// 获取硬盘序列
	BOOL GetHDID(PCHAR pIDBufer);
	// 执行CMD命令并获取到结果
	bool ExecCmd(const char* cmd, char* result, int size, bool trim_space=true);
	// 字节转成十六进制字符
	char* GetHexByByte(BYTE*, UINT);
};