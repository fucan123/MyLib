#include "Client.h"
#include <stdio.h>
#include <time.h>

void Client::Connect(const char* host, USHORT port)
{
	WSADATA     wsaData;
	SOCKET      socket_client;
	SOCKADDR_IN server_in;
	int         iaddrSize = sizeof(SOCKADDR_IN);
	DWORD       dwThreadId;

	WSAStartup(0x0202, &wsaData);

	socket_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	server_in.sin_addr.S_un.S_addr = inet_addr(host);
	server_in.sin_family = AF_INET;
	server_in.sin_port = htons(port);
	if (connect(socket_client, (struct sockaddr *)&server_in, sizeof(server_in)) == SOCKET_ERROR) {
		if (this->onconnect_error) {
			this->onconnect_error(host, port);
		}
		return;
	}

	this->onconect(host, port, socket_client);

	time_t last_time = time(NULL);
	char  wist[4];
	char* recv_data = new char[1024 * 4];
	while (!m_Close) {
		if (m_iTimeOut > 0) {
			time_t now_time = time(NULL);
			if ((now_time - last_time) >= m_iTimeOut) {
				if (this->ontimeout) {
					this->ontimeout(host, port, now_time - last_time);
				}
				break;
			}
		}
		int ret = recv(socket_client, recv_data, 4, 0);
		if (ret > 0) {
			int length = s_n2hi(&recv_data[0]) - 4;
			//printf("%c%c%c%c %08x\n", recv_data[0], recv_data[1], recv_data[2], recv_data[3], length);
			if (length >= 0) {
				int ret = recv(socket_client, recv_data, length, 0);
				int opcode = s_n2hi(&recv_data[0]);
				if (ret > 0) {
					recv_data[ret] = 0;
					this->onread(host, port, opcode, &recv_data[4], ret - 4);
				}
				else {
					goto error;
				}
			}
		}
	error:
		if (ret == 0) {
			//当ret == 0 说明服务器掉线。
			break;
		}
		else if (ret < 0) {
			//当ret < 0 说明出现了异常 例如阻塞状态解除，或者读取数据时出现指针错误等。
			//所以我们这里要主动断开和客户端的链接。
			if (this->onerror) {
				this->onerror(host, port);
			}
			closesocket(socket_client);
			break;
		}
	}
	delete[] recv_data;
	closesocket(socket_client);
	WSACleanup();

	this->onclose(host, port);
}

void Client::Close(bool var)
{
	m_Close = var;
}

int Client::Intel2Length(const char* data)
{
	int length = 0;
	length |= (data[0] << 24) & 0xff000000;
	length |= (data[1] << 16) & 0x00ff0000;
	length |= (data[2] << 8)  & 0x0000ff00;
	length |= (data[3] << 0)  & 0x000000ff;
	return length;
}
