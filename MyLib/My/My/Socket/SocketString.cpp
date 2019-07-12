#include "SocketString.h"
#include <string.h>
#include <stdio.h>

#define MAX_LENGTH 4096

SocketString::SocketString()
{
	m_SendString.value = new char[MAX_LENGTH];
	ClearSendString();
}

SocketString::~SocketString()
{
	delete[] m_SendString.value;
	m_SendString.value = nullptr;
}

SocketString& SocketString::SetKeyValue(const char* key, int value)
{
	char string[16];
	sprintf_s(string, "%d", value);
	return SetKeyValue(key, string);
}

SocketString& SocketString::SetKeyValue(const char * key, float value)
{
	char string[16];
	sprintf_s(string, "%.2f", value);
	return SetKeyValue(key, string);
}

SocketString& SocketString::SetKeyValue(const char* key, const char* value)
{
	int key_len = strlen(key);
	int value_len = strlen(value);

	memcpy(&m_SendString.value[m_SendString.length], key, key_len);
	m_SendString.length += key_len;
	memcpy(&m_SendString.value[m_SendString.length], ":", 1);
	m_SendString.length += 1;
	memcpy(&m_SendString.value[m_SendString.length], value, value_len);
	m_SendString.length += value_len;
	memcpy(&m_SendString.value[m_SendString.length], " | ", 3);
	m_SendString.length += 3;

	return *this;
}

SocketString& SocketString::SetString(const char * string, int len)
{
	return SetContent((void*)string, len);
}

SocketString& SocketString::SetInt(int value)
{
	SetInt(m_SendString.length, value);

	return *this;
}

// index除去前8个字节
SocketString& SocketString::SetInt(int index, int value)
{
	memcpy(&m_SendString.value[index], &value, 4);
	m_SendString.length += 4;

	return *this;
}

// 设置内容
SocketString& SocketString::SetContent(void * ptr, int len)
{
	memcpy(&m_SendString.value[m_SendString.length], ptr, len);
	m_SendString.length += len;

	return *this;
}


char* SocketString::MakeSendString(int opcode)
{
	m_SendString.length += 8;
	for (int i = m_SendString.length; i >= 0; i--) {
		m_SendString.value[i] = m_SendString.value[i - 8];
	}
	
	h2ni_s(&m_SendString.value[0], m_SendString.length);
	h2ni_s(&m_SendString.value[4], opcode);
	
	return m_SendString.value;
}

void SocketString::SetRecvString(const char* string)
{
	strcpy(m_RecvString, string);
}

int SocketString::GetRecvValueInt(const char * key, int default_value)
{
	const char* ptr = strstr(m_RecvString, key);
	if (!ptr) {
		return default_value;
	}

	int value = 0;
	ptr += strlen(key);
	for (int i = 0; true; i++) {
		if (ptr[i] < '0' || ptr[i] > '9') {
			break;
		}
		value = value * 10 + (ptr[i] - '0');
	}

	return value;
}

char* SocketString::GetRecvValue(const char* key)
{
	const char* ptr = strstr(m_RecvString, key);
	if (!ptr) {
		return nullptr;
	}
	ptr += strlen(key);
	//printf("%s\n", ptr);
	int i = 0;
	for (; true; i++) {
		if (ptr[i] == 0 || i > 100)
			break;

		if (ptr[i] == ' ' && ptr[i + 1] == '|' && ptr[i + 2] == ' ') {
			break;
		}
		//printf("i:%d\n", i);
		m_Value[i] = ptr[i];
	}
	m_Value[i] = 0;

	return m_Value;
}

// 网络数字转成正常数字
int SocketString::s_n2hi(const char * data)
{
	int length = 0;
	length |= (data[0] << 24) & 0xff000000;
	length |= (data[1] << 16) & 0x00ff0000;
	length |= (data[2] << 8) & 0x0000ff00;
	length |= (data[3] << 0) & 0x000000ff;
	return length;
}

// 数字复制到char字符
void SocketString::h2ni_s(char* dst, int value)
{
	for (int i = 0; i < 4; i++) {
		dst[i] = (value >> ((3 - i) * 8)) & 0xff;
	}
}


