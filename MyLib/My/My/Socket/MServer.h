#pragma once
#include <WinSock2.h>
#include "SocketString.h"

#define SOCKET_MAX 16

typedef struct struct_cnts
{
	SOCKET socket;
	ULONG  addr;
} MCON;

typedef void(*onconnect_func)(SOCKET, SOCKADDR_IN&, int);
typedef void(*onread_func)(SOCKET, int index, int opcode, const char* data, int len);
typedef void(*onclose_func)(SOCKET, int);

class MServer : public SocketString
{
public:
	onconnect_func onconect;
	onread_func onread;
	onclose_func onclose;

	// 所有连接数量
	int count;
	// 所有连接
	MCON cons[SOCKET_MAX];
public:
	MServer()
		:onconect(nullptr)
	{
		ZeroMemory(cons, sizeof(cons));
		count = 0;
		//SocketString();
	}
	void Listen(USHORT port, int af=AF_INET);
	void Close(SOCKET, int);
	static UINT ReadMsg(MServer*);
};