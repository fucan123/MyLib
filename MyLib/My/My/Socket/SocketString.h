#pragma once

#define _CRT_SECURE_NO_WARNINGS

class SocketString
{
private:
	struct {
		char* value;
		int   length;
	} m_SendString;
	char m_RecvString[1024];
	char m_Value[128];
public:
	SocketString();
	~SocketString();
	SocketString& SetKeyValue(const char* key, int value);
	SocketString& SetKeyValue(const char* key, float value);
	SocketString& SetKeyValue(const char* key, const char* value);
	SocketString& SetString(const char* string, int len);
	SocketString& SetInt(int value);
	SocketString& SetInt(int index, int value);
	SocketString& SetContent(void* ptr, int len); // 设置内容
	char* MakeSendString(int opcode, int type=1);
	char* GetSendString() { return m_SendString.value; }
	int   GetSendLength() { return m_SendString.length; }
	void  SetRecvString(const char* string);
	int   GetRecvValueInt(const char* key, int default_value=0);
	const char* GetRecvValue(const char* key);
	void ClearSendString() {
		m_SendString.value[0] = 0;
		m_SendString.length = 0;
	}

	// 网络数字转成正常数字
	static int s_n2hi(const char* data);
	// 正常数字转成网络数字
	static void h2ni_s(char* dst, int value);
};