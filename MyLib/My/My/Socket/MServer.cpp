#include "MServer.h"
#include <stdio.h>

void MServer::Listen(USHORT port)
{
	WSADATA     wsaData;
	SOCKET      sListen, sClient;
	SOCKADDR_IN local, client;
	int         iaddrSize = sizeof(SOCKADDR_IN);
	DWORD       dwThreadId;

	WSAStartup(0x0202, &wsaData);

	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	local.sin_addr.S_un.S_addr = htonl(0);
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	bind(sListen, (struct sockaddr *)&local, sizeof(SOCKADDR_IN));

	listen(sListen, 3);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)the_main_test, NULL, 0, &dwThreadId);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WebSocket::run, this, 0, &dwThreadId);
	printf("listen!\n");

	while (true) {
		sClient = accept(sListen, (struct sockaddr *)&client, &iaddrSize);
		ULONG addr = client.sin_addr.S_un.S_addr;

		bool exists = false;
		/*for (int i = 0; i < count; i++) {
			if (this->cons[i].addr == addr) { // 已有连接了
				closesocket(sClient);
				exists = true;
				break;
			}
		}*/

		if (!exists) {
			this->cons[count].socket = sClient;
			this->cons[count].addr = addr;
			count++;

			this->onconect(sClient, client, count - 1);
		}

		printf("连接: %d\n", count);
	}

	closesocket(sListen);
	WSACleanup();
}

void MServer::Close(SOCKET scoket, int index)
{
	this->count--;
	this->onclose(scoket, index);

	SOCKET tmp = this->cons[index].socket;
	int max_index = count;
	if (index == max_index) {
		this->cons[index].socket = 0;
		this->cons[index].addr = 0;
	}
	else {
		for (int i = index; i < max_index; i++) {
			this->cons[i].socket = this->cons[i + 1].socket;
			this->cons[i].addr = this->cons[i + 1].addr;
		}
		this->cons[max_index].socket = 0;
		this->cons[max_index].addr = 0;
	}
	
	closesocket(tmp);
}

UINT MServer::ReadMsg(MServer* $this)
{
	fd_set         fdread;
	struct timeval tv = { 1, 200 };
	while (true) {
		FD_ZERO(&fdread);
		for (int i = 0; i < $this->count; i++) {
			//printf("xxx:%d\n", i);
			FD_SET($this->cons[i].socket, &fdread);
		}

		int ret = select(0, &fdread, NULL, NULL, &tv);
		if (ret == 0) {
			continue;
		}

		for (int i = 0; i < $this->count; i++) {
			if (FD_ISSET($this->cons[i].socket, &fdread)) {
				if ((ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)) {
					//printf("客户端关闭\n");
					$this->Close($this->cons[i].socket, i);
					continue;
				}
				//printf("$:%d\n", ret);
				char recv_data[1024];
				ret = recv($this->cons[i].socket, recv_data, 5, 0);
				if (ret <= 0) {
					//printf("客户端关闭2\n");
					$this->Close($this->cons[i].socket, i);
					continue;
				}

				int length = atoi(recv_data) - 5;
				if (length >= 0) {
					int ret = recv($this->cons[i].socket, recv_data, length, 0);
					if (ret > 0) {
						recv_data[ret] = 0;
						$this->onread(recv_data, i);
					}
				}
				//printf("p:%d, length:%d\n", ret, length);
			}
		}
	}
	return 0;
}
