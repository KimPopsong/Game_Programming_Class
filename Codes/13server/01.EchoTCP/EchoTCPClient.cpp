// EchoTCPClient - 가장 단순한 TCP기반 echo 클라이언트 프로그램.
//

#include <stdio.h>
#include <WinSock2.h>

void main()
{
	WSADATA wsaData;
	SOCKET socketConnect;
	struct sockaddr_in serverAddr;

	::WSAStartup(0x202, &wsaData);

	socketConnect = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketConnect == INVALID_SOCKET) {
		printf("소켓 생성 에러: %d\n", ::WSAGetLastError());
		return;
	}

	// 접속할 서버의 정보를 설정한다.
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	serverAddr.sin_port = ::htons(8600);

	if (::connect(socketConnect, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("서버와 연결할 수 없음 - 서버를 먼저 실행하세요: %d\n", ::WSAGetLastError());
		return;
	}

	// 서버와 통신을 한다.
	{
		char sendBuffer[127] = "[클라이언트가 보낸 메시지]";
		int sentBytes = ::send(socketConnect, sendBuffer, ::strlen(sendBuffer) + 1, 0);
		printf("%d 바이트를 보냈음: %s\n", sentBytes, sendBuffer);

		char recvBuffer[127];
		int recvBytes = ::recv(socketConnect, recvBuffer, 127, 0);
		printf("%d 바이트를 받았음: %s\n", recvBytes, recvBuffer);
	}

	::shutdown(socketConnect, SD_BOTH);
	::closesocket(socketConnect);
	::WSACleanup();
}
