#include "HttpSocket.h"
#include "../Common/mystring.h"
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

// data_size = m_chData���ͻ򷵻ص����ݰ���ͷ����С, ����ʵ��ȥ����
HttpSocket::HttpSocket(int data_size)
{
	m_chData = new char[data_size];
	m_chBody = m_chData;
	m_chData[0] = 0;

	m_nSendDataCount = 0;
	m_nPostCount = 0;
	m_nResponseCount = 0;
	m_nBodyCount = 0;
	m_nDataMaxSize = data_size;
}
// ����
HttpSocket::~HttpSocket()
{
	//::printf("~HttpSocket.\n");
	Release();
}

// ����
void HttpSocket::Release()
{
	if (m_chData) {
		delete[] m_chData;
		m_chData = nullptr;
	}	
}

// GET
HTTP_STATUS HttpSocket::Get(const char* host, const char* path, int port)
{
	return Request(host, path, port, HTTP_GET);
}

// POST
HTTP_STATUS HttpSocket::Post(const char* host, const char* path, int port)
{
	return Request(host, path, port, HTTP_POST);
}

// PUT
HTTP_STATUS HttpSocket::Put(const char* host, const char* path, int port)
{
	return Request(host, path, port, HTTP_PUT);
}

// ����
HTTP_STATUS HttpSocket::Request(const char* host, const char* path, int port, int type)
{
	m_nResponseCount = 0;
	m_chData[m_nDataMaxSize - 1] = 0;

	HTTP_STATUS status = 0;

	WSADATA     wsaData;
	SOCKET      socket_client;
	SOCKADDR_IN server_in;
	int         iaddrSize = sizeof(SOCKADDR_IN);
	DWORD       dwThreadId;

	if (WSAStartup(0x0202, &wsaData) != 0) {
		//::printf("WSAStartup(0x0202, &wsaData) != 0.\n");
		FormatResponse();
		return 0;
	}

	socket_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	server_in.sin_family = AF_INET;
	server_in.sin_port = htons(port);

	if (!strstr(host, "com")) {
		server_in.sin_addr.S_un.S_addr = inet_addr(host);
	}
	else {
		struct hostent* host_ent = gethostbyname(host);
		//char* ip;
		//ip = inet_ntoa(*(struct in_addr*)*host_ent->h_addr_list);
		//printf("%s\n", ip);
		memcpy(&server_in.sin_addr.S_un.S_addr, host_ent->h_addr_list[0], host_ent->h_length);
	}
	if (connect(socket_client, (struct sockaddr*)&server_in, sizeof(server_in)) == SOCKET_ERROR) {
		//printf("connectʧ��.\n");
		closesocket(socket_client);
		WSACleanup();

		FormatResponse();
		return 0;
	}

	if (m_nPostCount == 0) { // û��POST����, ͷ����δ��β
		m_chData[m_nSendDataCount++] = '\r';
		m_chData[m_nSendDataCount++] = '\n';
		m_chData[m_nSendDataCount] = 0;
	}

	char header[512];
	sprintf_s(header, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\n", 
		type==HTTP_GET?"GET":(type==HTTP_POST?"POST":"PUT"), path, host);
	if (type != HTTP_GET) {
		char tmp[256];
		sprintf(&header[strlen(header)], 
			"Content-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n", 
			m_nPostCount);
	}
	if (!AddHeader(header)) {
		//::printf("!AddHeader.\n");
		FormatResponse();
		return 0;
	}

	//printf("������������:\n%s. %d %d\n", m_chData, m_nSendDataCount, m_nPostCount);
	if (send(socket_client, m_chData, m_nSendDataCount, 0) <= 0) {
		FormatResponse();
		return 0;
	}

	char ch, r = 0;
	while (recv(socket_client, &ch, 1, 0)) {
		if (ch == '\r' && !r) {
			r = ch;
			//if (response.find("222 OK") == std::string::npos)
			//	return 0;
		}
		if (m_nResponseCount < m_nDataMaxSize - 1) {
			m_chData[m_nResponseCount++] = ch;
		}
	}
	m_chData[m_nResponseCount] = 0;

	//printf("������������:\n%s %d.\n", m_chData, m_nResponseCount);

	FormatResponse();
	return  m_nResponseCount > 0 ? GetResponseStatus() : 0;
}

// ��ȡ��������
const char* HttpSocket::GetResponse()
{
	return m_chData;
}

// ��ȡ���س���
int HttpSocket::GetResponseCount()
{
	return m_nResponseCount;
}

// ��ȡ��������
const char* HttpSocket::GetBody()
{
	return m_chBody;
}

// �������峤��
int HttpSocket::GetBodyCount()
{
	return m_nBodyCount;
}

// ��ȡ����ͷ����Ϣ ����ֵ=��ȡ���ȴ�С
int HttpSocket::GetResponseHeader(const char* key, char* save, int save_len)
{
	save[0] = 0;
	if (m_nResponseCount == 0)
		return 0;

	char* string = strstr(m_chData, key);
	if (!string)
		return 0;

	string += strlen(key) + 2; // ����:�Ϳո�
	
	if (save_len > 0) // ���һλ��0
		save_len -= 1;

	int count = 0;
	while (string[count] && string[count] != '\r' && string[count] != '\n') {
		if (save_len != 0 && count == save_len)
			break;

		save[count] = string[count];
		count++;
	}
	save[count] = 0;

	return count;
}

// �����Ӧͷ��״̬
HTTP_STATUS HttpSocket::GetResponseStatus()
{
	HTTP_STATUS status = 0;
	bool space = false;
	for (int i = 0; i < 16; i++) {
		space = space ? space : m_chData[i] == ' ';
		if (!space)
			continue;

		if (m_chData[i] >= '0' && m_chData[i] <= '9') {
			status = status * 10 + m_chData[i] - '0';
		}
		else {
			if (status != 0)
				break;
		}
	}

	return status;
}

// ������������
void HttpSocket::FormatResponse()
{
	m_nBodyCount = 0;

	if (m_nResponseCount == 0) {
		m_chBody = &m_chData[m_nDataMaxSize - 1];
		return;
	}

	char* string = strstr(m_chData, "\r\n\r\n");
	if (!string) {
		m_chBody = &m_chData[m_nDataMaxSize - 1];
		return;
	}

	char* content_length = strstr(m_chData, "Content-Length:");
	if (!content_length) {
		content_length = strstr(m_chData, "content-length:");
	}
	if (content_length) {
		content_length += 16;
		if (*content_length == ' ') {
			content_length++;
		}

		string += 2;
		m_nBodyCount = atoi(content_length);
		//::printf("content_length:%d\n", m_nBodyCount);
	}
	else {
		m_nBodyCount = hex2int(string + 4);
		string = strstr(string + 4, "\r\n"); // ���Ⱥ��滻��
		if (!m_nBodyCount || !string) {
			m_chBody = &m_chData[m_nDataMaxSize - 1];
			return;
		}
	}
	
	if (string + m_nBodyCount + 2 < m_chData + m_nDataMaxSize) {
		string[m_nBodyCount + 2] = 0;
	}
	
	m_chBody = string + 2; // ���Ȼ��к����������
}

// ���ͷ������
bool HttpSocket::AddHeader(const char* header)
{
	size_t header_len = strlen(header);
	if (header_len + m_nSendDataCount >= m_nDataMaxSize) // �ռ䲻�����
		return false;

	for (int i = m_nSendDataCount - 1; i >= 0; i--) { // �Ӻ���ǰ�ƶ�
		m_chData[header_len + i] = m_chData[i];
	}
	memcpy(&m_chData[0], header, header_len); // ���Ƶ�ǰ��ȥ
	m_nSendDataCount += header_len;
	m_chData[m_nSendDataCount] = 0;

	return true;
}

// ���ͷ������
void HttpSocket::AddHeader(const char* key, const char* value)
{
	int key_len = strlen(key);
	int value_len = strlen(value);
	if (key_len + value_len + 4 >= m_nDataMaxSize) // ��: \r\n
		return;

	memcpy(&m_chData[m_nSendDataCount], key, key_len);
	m_nSendDataCount += key_len;
	m_chData[m_nSendDataCount++] = ':';
	m_chData[m_nSendDataCount++] = ' ';
	memcpy(&m_chData[m_nSendDataCount], value, value_len);
	m_nSendDataCount += value_len;
	m_chData[m_nSendDataCount++] = '\r';
	m_chData[m_nSendDataCount++] = '\n';
	m_chData[m_nSendDataCount] = 0;
}

// ���ͷ������
void HttpSocket::AddHeader(const char* key, int value)
{
	char string[32];
	sprintf_s(string, "%d", value);
	AddHeader(key, string);
}

// ���Cooike
void HttpSocket::AddCookie(const char* key, const char* value)
{
	int key_len = strlen(key);
	int value_len = strlen(value);
	if (key_len + value_len + 11 >= m_nDataMaxSize) // Cookie: ��=��\r\n һ��9
		return;

	memcpy(&m_chData[m_nSendDataCount], "Cookie: ", 8);
	m_nSendDataCount += 8; // Cookie: 
	memcpy(&m_chData[m_nSendDataCount], key, key_len);
	m_nSendDataCount += key_len;
	m_chData[m_nSendDataCount++] = '=';
	memcpy(&m_chData[m_nSendDataCount], value, value_len);
	m_nSendDataCount += value_len;
	m_chData[m_nSendDataCount++] = '\r';
	m_chData[m_nSendDataCount++] = '\n';
	m_chData[m_nSendDataCount] = 0;
}

// ���Cookie
void HttpSocket::AddCookie(const char* key, int value)
{
	char string[32];
	sprintf_s(string, "%d", value);
	AddCookie(key, string);
}

// ���POST����
void HttpSocket::AddPost(const char* key, const char* value)
{
	int key_len = strlen(key);
	int value_len = strlen(value);
	if (key_len + value_len + 9 >= m_nDataMaxSize) // Cookie: ����= һ��9
		return;

	if (m_nPostCount == 0) { // ��һ�μ�����
		// ����ͷ����β
		m_chData[m_nSendDataCount++] = '\r';
		m_chData[m_nSendDataCount++] = '\n';
	}

	memcpy(&m_chData[m_nSendDataCount], key, key_len);
	m_nSendDataCount += key_len;
	m_chData[m_nSendDataCount++] = '=';
	memcpy(&m_chData[m_nSendDataCount], value, value_len);
	m_nSendDataCount += value_len;
	m_chData[m_nSendDataCount++] = '&';
	m_chData[m_nSendDataCount] = 0;

	m_nPostCount += key_len + value_len + 2;
}

// ���POST����
void HttpSocket::AddPost(const char* key, int value)
{
	char string[32];
	sprintf_s(string, "%d", value);
	AddPost(key, string);
}
