#include "MachineID.h"

void MachineID::GetMachineID(char id[32])
{
	int cpuid[12];
	memset(cpuid, 0, sizeof(cpuid));
	GetCPUID(cpuid);
	char* netCode = GetNetWorkCode();
	size_t netLength = strlen(netCode);

	char sernum[4];
	GetDisk(sernum);

	//printf("cpuid:%08X\n", cpuid);
	// {B3D14E88-5B57-4C1D-B0C5-9ED27940F745}:9883892CA38C
	// 92680432
	//printf("netCode:%s\n", netCode);
	// BF6D6C819EC975B043AEC502167C3D15

	size_t length = sizeof(cpuid) + netLength + 4;
	UCHAR* encrypt = new UCHAR[length];
	memcpy(encrypt, cpuid, sizeof(cpuid));
	memcpy(encrypt + sizeof(cpuid), netCode, netLength);
	memcpy(encrypt + sizeof(cpuid) + netLength, sernum, 4);

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
		delete netCode;
	}
}

char* MachineID::GetNetWorkCode()
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
	if (ERROR_SUCCESS == nRel) {
		//输出网卡信息
		 //可能有多网卡,因此通过循环去判断
		char* macHex = GetHexByByte(pIpAdapterInfo->Address, pIpAdapterInfo->AddressLength);
		size_t macSize = strlen(macHex);
		size_t nameSize = strlen(pIpAdapterInfo->AdapterName);
		size_t codeSize = nameSize + macSize;

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
		}
		if (macHex) {
			delete macHex;
		}
	}
	//释放内存空间
	if (pIpAdapterInfo) {
		delete pIpAdapterInfo;
	}
	return netCode;
}

void MachineID::GetCPUID(int cpuid[12])
{
	__cpuidex(cpuid, 0, 0);
	// 00000016 756E6547 6C65746E 49656E69
	//printf("%08X %08X %08X %08X\n", cpuid[0], cpuid[1], cpuid[2], cpuid[3]);
	//__cpuidex(&cpuid[4], 1, 0);
	// 000806E9 00100800 7FFAFBBF BFEBFBFF
	//printf("%08X %08X %08X %08X\n", cpuid[4], cpuid[5], cpuid[6], cpuid[7]);
	//__cpuidex(&cpuid[8], 2, 0);
	// 76036301 00F0B5FF 00000000 00C30000
	//printf("%08X %08X %08X %08X\n", cpuid[8], cpuid[9], cpuid[10], cpuid[11]);
}

// 获取硬盘序列
void MachineID::GetDisk(char key[4])
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

char* MachineID::GetHexByByte(BYTE* origin, UINT length)
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
