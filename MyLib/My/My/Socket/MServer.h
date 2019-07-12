#pragma once
#include <WinSock2.h>

#define SOCKET_MAX 1024

typedef struct struct_cnts
{
	SOCKET socket;
	ULONG  addr;
} MCON;

typedef void(*onconnect_func)(SOCKET, SOCKADDR_IN&, int);
typedef void(*onread_func)(const char*, int);
typedef void(*onclose_func)(SOCKET, int);

class MServer
{
public:
	onconnect_func onconect;
	onread_func onread;
	onclose_func onclose;

	int count;
	MCON cons[1024];
public:
	MServer()
		:onconect(nullptr)
	{
		ZeroMemory(cons, sizeof(cons));
		count = 0;
	}
	void Listen(USHORT port);
	void Close(SOCKET, int);
	static UINT ReadMsg(MServer*);
};