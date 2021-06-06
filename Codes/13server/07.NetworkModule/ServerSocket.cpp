#include "ServerSocket.h"
#include "Session.h"
#include <stdio.h>


//
// ServerSocket

ServerSocket::ServerSocket()
{
	socketListen = INVALID_SOCKET;
}

ServerSocket::~ServerSocket()
{
	if (socketListen != INVALID_SOCKET) {
		closesocket(socketListen);
	}
}

bool ServerSocket::initialize(char* ipAddr, int portNumber, int backlogCount)
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return false;

	if (wsaData.wVersion != MAKEWORD(2, 2)) {
		::WSACleanup();
		return false;
	}

	socketListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketListen == INVALID_SOCKET) {
		::WSACleanup();
		return false;
	}

	int ival=1;
	::setsockopt(socketListen, SOL_SOCKET, SO_REUSEADDR, (char*)&ival, sizeof(int));

	unsigned long argp = 1;
	::ioctlsocket(socketListen, FIONBIO, &argp);

	struct sockaddr_in sockAddr;
	::ZeroMemory(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = ::inet_addr(ipAddr);
	sockAddr.sin_port = ::htons(portNumber);
	if (::bind(socketListen, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
		::WSACleanup();
		return false;
	}

	if (::listen(socketListen, backlogCount) == SOCKET_ERROR) {
		::WSACleanup();
		return false;
	}

	return true;
}

void ServerSocket::close()
{
	if (socketListen != INVALID_SOCKET) {
		::closesocket(socketListen);
	}

	::WSACleanup();
}


SOCKET ServerSocket::acceptConnection(struct timeval timeout)
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socketListen, &fds);

	if (::select(0, &fds, 0, 0, &timeout) == SOCKET_ERROR)
		return INVALID_SOCKET;

	if (FD_ISSET(socketListen, &fds)) {
		sockaddr_in clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		SOCKET socket = ::accept(socketListen, (sockaddr*)&clientAddr, &clientAddrLen);

		if (socket != INVALID_SOCKET) {
			// 클라이언트로부터의 연결이 수락되었음.
			in_addr clientIn;
			memcpy(&clientIn, &clientAddr.sin_addr.s_addr, 4);
			printf("클라이언트의 연결 요청을 수락함 (IP 주소: %s, 포트 번호 %d).\n", inet_ntoa(clientIn), ntohs(clientAddr.sin_port));
		}

		return socket;
	}

	return INVALID_SOCKET;
}


//
// Acceptor

Acceptor::Acceptor()
{
	hEventKill = ::CreateEvent(0, 0, 0, 0);
}

Acceptor::~Acceptor()
{
	::SetEvent(hEventKill);

	::WaitForSingleObject(threadHandle, 1000);

	::CloseHandle(hEventKill);
}


void Acceptor::run()
{
	SOCKET socketAccept;
	struct timeval timeout;

	timeout.tv_sec  = 0;
	timeout.tv_usec = 5; //대기시간 5ms로 지정.

	while (1) {
		if (::WaitForSingleObject(hEventKill, 1) == WAIT_OBJECT_0) {
			break;
		}

		socketAccept = serverSocket.acceptConnection(timeout);

		if (socketAccept != INVALID_SOCKET) {
			SessionManager::getInstance()->createSession(socketAccept);
		}

	}
}


bool Acceptor::initialize(char* ipAddr, int portNumber)
{
	if (serverSocket.initialize(ipAddr, portNumber) == false) {
		return false;
	}

	begin();

	return true;
}
