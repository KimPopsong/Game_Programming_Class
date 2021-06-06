// IOCPServer.cpp : Defines the entry point for the console application.
// Ref: 열혈강의 TCP/IP 소켓 프로그래밍(윤성우)

#include <stdio.h>
#include <winsock2.h>
#include <process.h>

// 에러 처리 함수.
void ErrorHandling(LPCSTR pszMessage)
{
	fputs(pszMessage, stderr);
	fputc('\n', stderr);
	exit(1);
}

// 상수 정의
#define BUFSIZE			100

// 구조체 선언

// 소켓 정보를 구조체화
typedef struct
{
	SOCKET		hClntSock;
	SOCKADDR_IN	clntAddr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// 소켓의 버퍼 정보를 구조체화
typedef struct
{
	OVERLAPPED	overlapped;
	CHAR		buffer[ BUFSIZE ];
	WSABUF		wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

// 함수 원형 선언
UINT WINAPI CompletionThread(LPVOID pCompletionPortIO);

// main 함수 구현

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	HANDLE  hCompletionPort;

	SYSTEM_INFO systemInfo;
	SOCKADDR_IN servAddr;

	LPPER_IO_DATA     perIoData;
	LPPER_HANDLE_DATA perHandleData;

	SOCKET hServSock;
	DWORD  dwRecvBytes;
	DWORD  i, dwFlags;

	//if (argc != 2)
	//{
	//	printf("Usage : %s <port>\n", argv[0]);
	//	exit(1);
	//}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{	// Load Winsock 2.2 DLL
		ErrorHandling("WSAStartup() error!");
	}

	// 1. Completion Port 생성
	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL,
		0, 0);
	GetSystemInfo(&systemInfo);
	printf("num processes = %d\n", systemInfo.dwNumberOfProcessors);

	// 2. 쓰레드를 CPU 개수만큼 생성
	for (i=0; i<systemInfo.dwNumberOfProcessors; i++)
	{
		_beginthreadex(NULL, 0, CompletionThread,
			(LPVOID) hCompletionPort, 0, NULL);
	}

	hServSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0,
		WSA_FLAG_OVERLAPPED);
	if (hServSock == INVALID_SOCKET)
	{
		ErrorHandling("WSASocket() error!");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family      = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port        = ::htons(8600); //htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR *) &servAddr, sizeof(servAddr))
		== SOCKET_ERROR)
	{
		ErrorHandling("bind() error!");
	}

	if (listen(hServSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen() error!");
	}

	while (TRUE)
	{
		SOCKET      hClntSock;
		SOCKADDR_IN clntAddr;
		int         nAddrLen = sizeof(clntAddr);

		hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr,
			&nAddrLen);
		if (hClntSock == INVALID_SOCKET)
		{
			ErrorHandling("accept() error!");
		}

		// 연결된 클라이언트의 소켓 핸들 정보와 주소 정보를 설정
		perHandleData = new PER_HANDLE_DATA;
		perHandleData->hClntSock = hClntSock;
		memcpy(&(perHandleData->clntAddr), &clntAddr, nAddrLen);

		// 3. Overlapped 소켓과 Completion Port 의 연결
		CreateIoCompletionPort((HANDLE) hClntSock, hCompletionPort,
			(DWORD) perHandleData, 0);

		// 클라이언트를 위한 버퍼를 설정, OVERLAPPED 변수 초기화
		perIoData = new PER_IO_DATA;
		memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
		perIoData->wsaBuf.len = BUFSIZE;
		perIoData->wsaBuf.buf = perIoData->buffer;
		dwFlags = 0;

		// 4. 중첩된 데이타 입력
		WSARecv(perHandleData->hClntSock,		// 데이타 입력 소켓
			&(perIoData->wsaBuf),		// 데이타 입력 버퍼 포인터
			1,				// 데이타 입력 버퍼의 수
			&dwRecvBytes,
			&dwFlags,
			&(perIoData->overlapped),		// OVERLAPPED 변수 포인터
			NULL
		);
	}

	return 0;
}

// CompletionThread 함수 구현

UINT WINAPI CompletionThread(LPVOID pCompletionPortIO)
{
	HANDLE hCompletionPort = (HANDLE) pCompletionPortIO;
	DWORD  dwBytesTransferred;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_DATA     perIoData;
	DWORD  dwFlags;

	while (TRUE)
	{
		// 5. 입.출력이 완료된 소켓의 정보 얻음
		GetQueuedCompletionStatus(hCompletionPort,		// Completion Port
			&dwBytesTransferred,			// 전송된 바이트 수
			(LPDWORD) &perHandleData,
			(LPOVERLAPPED*) &perIoData,
			INFINITE
		);

		if (dwBytesTransferred == 0) // EOF 전송 시
		{
			closesocket(perHandleData->hClntSock);

			delete perHandleData;
			delete perIoData;

			continue;
		}

		// 6. 메세지. 클라이언트로 에코
		printf("%d bytes를 다시 보냅니다..\n", dwBytesTransferred);
		perIoData->wsaBuf.len = dwBytesTransferred;
		WSASend(perHandleData->hClntSock, &(perIoData->wsaBuf), 1, NULL,
			0, NULL, NULL);

		// RECEIVE AGAIN
		memset(&(perIoData->overlapped), 0, sizeof(OVERLAPPED));
		perIoData->wsaBuf.len = BUFSIZE;
		perIoData->wsaBuf.buf = perIoData->buffer;

		dwFlags = 0;
		WSARecv(perHandleData->hClntSock,
			&(perIoData->wsaBuf),
			1,
			NULL,
			&dwFlags,
			&(perIoData->overlapped),
			NULL
		);
	}

	return 0;
}
