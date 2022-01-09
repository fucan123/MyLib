#include "MachineID2.h"
#include "mystring.h"

#pragma comment(lib,"ws2_32.lib")

#if 0
#define dbg(...) ::printf(__VA_ARGS__);
#else
#define dbg(...)
#endif

void MachineID2::GetMachineID(char id[32])
{
#if 1
	int cpuid[12];
	memset(cpuid, 0, sizeof(cpuid));
	GetCPUID(cpuid);
	char* netCode = GetNetWorkCode();
	if (!netCode) {
		netCode = new char[10];
		memcpy(netCode, "abcdefg89", 10);
	}
	dbg("netCode:%s (%d)\n", netCode, (int)strlen(netCode));
	size_t netLength = strlen(netCode);

	// baseboard
	char disknum[512];
	memset(disknum, 0, sizeof(disknum));
#if 0
	ExecCmd("wmic diskdrive get serialnumber", disknum, sizeof(disknum));
	replace(disknum, 0, 0);
	dbg("磁盘:%s (%d)...\n", disknum, (int)strlen(disknum));
	for (int i = 0; i < sizeof(cpuid) / sizeof(int); i++) {
		dbg("cpuid[%d]=%08X.\n", i, cpuid[i]);
	}
#endif
	
	char zhuban[512];
	memset(zhuban, 0, sizeof(zhuban));
#if 0
	ExecCmd("wmic baseboard get serialnumber", zhuban, sizeof(zhuban));
	replace(zhuban, 0, 0);
	dbg("主板:%s (%d)...\n", zhuban, (int)strlen(zhuban));
#endif

	size_t length = sizeof(cpuid) + netLength + strlen(disknum) + strlen(zhuban);
	UCHAR* encrypt = new UCHAR[length];
	memset(encrypt, 0, length);
	memcpy(encrypt, cpuid, sizeof(cpuid));
	memcpy(encrypt + sizeof(cpuid), netCode, netLength);
	memcpy(encrypt + sizeof(cpuid) + netLength, disknum, strlen(disknum));
	memcpy(encrypt + sizeof(cpuid) + netLength + strlen(disknum), zhuban, strlen(zhuban));
	dbg("%s.\n", encrypt + sizeof(cpuid));

	UCHAR decrypt[16];

	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, encrypt, length);
	MD5Final(&md5, decrypt);

	int index = 0;
	for (int i = 0; i < 16; i++) {
		char tmp[3];
		sprintf_s(tmp, "%02X", decrypt[i]);
		id[index++] = tmp[0];
		id[index++] = tmp[1];
	}

	if (netCode) {
		delete[] netCode;
	}
#endif
}

char* MachineID2::GetNetWorkCode()
{
	char* netCode = nullptr;
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}

	PIP_ADAPTER_INFO pTmpIpAdapterInfo = pIpAdapterInfo;
	if (ERROR_SUCCESS == nRel) {
		while (pIpAdapterInfo) {
			//输出网卡信息
			 //可能有多网卡,因此通过循环去判断
			if (strstr(pIpAdapterInfo->Description, "VMware") || strstr(pIpAdapterInfo->Description, "Bluetooth")
				|| strstr(pIpAdapterInfo->Description, "vmware") || strstr(pIpAdapterInfo->Description, "bluetooth")) {
				pIpAdapterInfo = pIpAdapterInfo->Next;
				continue;
			}
			if (inet_addr(pIpAdapterInfo->IpAddressList.IpAddress.String) == 0) {
				pIpAdapterInfo = pIpAdapterInfo->Next;
				continue;
			}

			char* macHex = GetHexByByte(pIpAdapterInfo->Address, pIpAdapterInfo->AddressLength);
			size_t macSize = strlen(macHex);
			size_t nameSize = strlen(pIpAdapterInfo->AdapterName);
			size_t codeSize = nameSize + macSize;
			MIB_IF_TYPE_OTHER;
			netCode = new char[codeSize + 3];
			if (netCode) {
				char* copyNetCode = netCode;
				memcpy(copyNetCode, pIpAdapterInfo->AdapterName, nameSize);
				copyNetCode += nameSize;
				*copyNetCode = ':';
				if (macHex) {
					copyNetCode++;
					memcpy(copyNetCode, macHex, macSize);
					copyNetCode += macSize;
				}
				*copyNetCode = 0;

#if 0
				::printf("%s:%s %s %d\n", 
					pIpAdapterInfo->Description, netCode, 
					pIpAdapterInfo->IpAddressList.IpAddress.String, pIpAdapterInfo->Type);
				::printf("ip:%d.\n", inet_addr(pIpAdapterInfo->IpAddressList.IpAddress.String));
#endif

			}
			if (macHex) {
				delete macHex;
			}

			pIpAdapterInfo = pIpAdapterInfo->Next;
			break;
		}
	}
	//释放内存空间
	if (pTmpIpAdapterInfo) {
		delete pTmpIpAdapterInfo;
	}
	return netCode;
}

void MachineID2::GetCPUID(int cpuid[12])
{
	__cpuidex(cpuid, 0, 0);
	__cpuidex(&cpuid[4], 1, 0);
	cpuid[5] = 0;
	cpuid[6] = 0;
	__cpuidex(&cpuid[8], 0x80000001, 0);
}

// 获取硬盘序列
void MachineID2::GetDisk(char key[4])
{
	//获取机器码
	char     m_Volume[256];//卷标名  
	char     m_FileSysName[256];
	DWORD   m_SerialNum;//序列号  
	DWORD   m_FileNameLength;
	DWORD   m_FileSysFlag;
	::GetVolumeInformationA("c:\\",
		m_Volume,
		256,
		&m_SerialNum,
		&m_FileNameLength,
		&m_FileSysFlag,
		m_FileSysName,
		256);

	//printf("m_Volume:%s m_FileSysName:%s m_SerialNum:%08X\n", m_Volume, m_FileSysName, m_SerialNum);
	memcpy(key, &m_SerialNum, 4);
}

BOOL MachineID2::GetHDID(PCHAR pIDBufer)
{
	HANDLE hDevice = NULL;
	hDevice = ::CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (!hDevice) {
		hDevice = ::CreateFileA("\\\\.\\Scsi0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	}

	if (!hDevice)
	{
		printf("!hDevice\n");
		return FALSE;
	}

	DWORD dwBytesReturned = 0;
	GETVERSIONINPARAMS gVersionParsams;
	ZeroMemory(&gVersionParsams, sizeof(GETVERSIONINPARAMS));

	if (!DeviceIoControl(hDevice, SMART_GET_VERSION, NULL, NULL, &gVersionParsams, sizeof(GETVERSIONINPARAMS), &dwBytesReturned, NULL)
		|| dwBytesReturned == 0 || gVersionParsams.bIDEDeviceMap <= 0)
	{
		printf("!DeviceIoControl:%d\n", GetLastError());
		::CloseHandle(hDevice);
		return FALSE;
	}

	SENDCMDINPARAMS scip;

	ZeroMemory(&scip, sizeof(SENDCMDINPARAMS));
	scip.cBufferSize = IDENTIFY_BUFFER_SIZE;
	scip.irDriveRegs.bSectorCountReg = 1;
	scip.irDriveRegs.bSectorNumberReg = 1;
	scip.irDriveRegs.bDriveHeadReg = 0xA0;
	scip.irDriveRegs.bCommandReg = 0xEC;

	BYTE btBuffer[1024] = { 0 };

	if (!DeviceIoControl(hDevice, SMART_RCV_DRIVE_DATA, &scip, sizeof(SENDCMDINPARAMS),
		btBuffer, 1024, &dwBytesReturned, NULL))
	{
		::CloseHandle(hDevice);
		return FALSE;
	}

	int nPos = 0x24;            //序列号的开始位置,具体请参考SENDCMDOUTPARAMS与IDSECTOR结构
	while (btBuffer[nPos] && btBuffer[nPos] < 128)
	{
		*pIDBufer = btBuffer[nPos++];
		pIDBufer++;
	}
	*pIDBufer = 00;

	::CloseHandle(hDevice);
	return TRUE;
}

// 执行CMD命令并获取到结果
bool MachineID2::ExecCmd(const char* cmd, char* result, int size, bool trim_space)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		printf("CreatePipe Failed\n");
		return false;
	}

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	char cmdline[200];
	sprintf(cmdline, "cmd /C %s", cmd);
	//printf("%s %s\n", m_chDnconsole, cmd);

	if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
		printf("CreateProcess failed!%d\n", GetLastError());
		return false;
	}
	WaitForSingleObject(pi.hThread, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hWrite);

	DWORD bytesRead;
	bool r = ReadFile(hRead, result, size, &bytesRead, NULL);
	CloseHandle(hRead);

	if (trim_space)
		replace(result, ' ', 0);

	return r;
}

char* MachineID2::GetHexByByte(BYTE* origin, UINT length)
{
	char* str = new char[length * 2 + 1];
	if (str) {
		UINT index = 0;
		for (UINT i = 0; i < length; i++) {
			char tmp[3];
			sprintf_s(tmp, "%02X", origin[i]);
			str[index++] = tmp[0];
			str[index++] = tmp[1];
		}
		str[index] = 0;
	}
	
	return str;
}
