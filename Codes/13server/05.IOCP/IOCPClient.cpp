// IOCPClient.cpp : Defines the entry point for the console application.
// Ref: 열혈강의 TCP/IP 소켓 프로그래밍(윤성우)

#include <stdio.h>
#include <winsock2.h>

// 에러 처리 함수.
void ErrorHandling(LPCSTR pszMessage)
{
	fputs(pszMessage, stderr);
	fputc('\n', stderr);
	exit(1);
}

// 상수 정의
#define BUFSIZE			1024

// main 함수 구현

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET  hSocket;
	SOCKADDR_IN recvAddr;

	WSABUF dataBuf;
	CHAR  message[] = "Network is Computer!";
	DWORD dwSendBytes = 0;

	WSAEVENT event;
	WSAOVERLAPPED overlapped;

	//if (argc != 3)
	//{
	//	printf("Usage : %s <ip> <port>\n", argv[0]);
	//	exit(1);
	//}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}

	hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0,
		WSA_FLAG_OVERLAPPED);
	if (hSocket == INVALID_SOCKET)
	{
		ErrorHandling("WSASocket() error!");
	}

	memset(&recvAddr, 0, sizeof(recvAddr));
	recvAddr.sin_family      = AF_INET;
	recvAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); //inet_addr(argv[1]);
	recvAddr.sin_port        = ::htons(8600); //htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*) &recvAddr, sizeof(recvAddr))
		== SOCKET_ERROR)
	{
		ErrorHandling("connect() error!");
	}

	event = WSACreateEvent();
	memset(&overlapped, 0, sizeof(overlapped));

	overlapped.hEvent = event;
	dataBuf.len = strlen(message);
	dataBuf.buf = message;

	if (WSASend(hSocket, &dataBuf, 1, &dwSendBytes, 0, &overlapped,
		NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			ErrorHandling("WSASend() error!");
		}
	}

	// 데이타 수신 진행 중에 다른 작업을 한다.
	puts("데이타 수신 중에 다른 일을 하고 있습니다.");
	puts("데이타 수신은 Background 로 진행 중에 있습니다.");

	WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE);
	WSAGetOverlappedResult(hSocket, &overlapped, &dwSendBytes, FALSE,
		NULL);
	printf("전송된 바이트 수 : %d\n", dwSendBytes);

	closesocket(hSocket);
	WSACleanup();

	return 0;
}
