#pragma once
#include <Winsock2.h>
// 주의: Winsock2.h를 먼저 include한 다음에 windows.h를 include하자.
#include "Thread.h"


class ServerSocket
{
protected:
	SOCKET socketListen;

public:
	ServerSocket();
	virtual ~ServerSocket();

	bool initialize(char* ipAddr, int portNumber, int backlogCount = SOMAXCONN);
	void close();

	SOCKET getSocketListen() { return socketListen; }

	SOCKET acceptConnection(struct timeval timeout);

};


class Acceptor : public Thread
{
private:
	ServerSocket serverSocket;
	HANDLE hEventKill;

public:
	Acceptor();
	virtual ~Acceptor();

	virtual void run();
	bool initialize(char* ipAddr, int portNumber);

};
