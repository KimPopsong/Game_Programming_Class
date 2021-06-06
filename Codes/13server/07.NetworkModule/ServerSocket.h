#pragma once
#include <Winsock2.h>
// ����: Winsock2.h�� ���� include�� ������ windows.h�� include����.
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
