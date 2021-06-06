// EchoUDPServer - 가장 단순한 UDP기반 echo 클라이언트 프로그램.
//

#include <stdio.h>
#include <Winsock2.h>

#define BUFFER_LENGTH	255

void main()
{
	WSADATA wsaData;
	::WSAStartup(0x202, &wsaData);

	SOCKET listenSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (listenSocket == INVALID_SOCKET) {
		printf("소켓 생성 에러: %d\n", ::WSAGetLastError());
		return;
	}

	struct sockaddr_in echoServerAddr;
	::memset(&echoServerAddr, 0, sizeof(echoServerAddr));
	echoServerAddr.sin_family = AF_INET;
	echoServerAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	echoServerAddr.sin_port = ::htons(8599);

	if (::bind(listenSocket, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != 0) {
		printf("바인드 에러: %d\n", ::WSAGetLastError());
		return;
	}
	printf("서버가 준비되었습니다.\n");

	char echoBuffer[BUFFER_LENGTH];

	while (1) {
		struct sockaddr_in echoClientAddr;
		int clientAddrLen = sizeof(echoClientAddr);

		// 클라이언트로부터 메시지가 도착하기를 기다린다.
		int receiveSize = ::recvfrom(listenSocket, echoBuffer, BUFFER_LENGTH, 0, (struct sockaddr*)&echoClientAddr, &clientAddrLen);
		if (receiveSize < 0)
			continue;

		printf("클라이언트(%s)로부터 %d 바이트를 받았음: %s\n", ::inet_ntoa(echoClientAddr.sin_addr), receiveSize, echoBuffer);

		// 받은 메시지를 클라이언트로 되돌려 보낸다.
		if (::sendto(listenSocket, echoBuffer, receiveSize, 0, (struct sockaddr*)&echoClientAddr, sizeof(echoClientAddr)) != receiveSize) {
			__asm int 3;
			break;
		}
	}

	::shutdown(listenSocket, SD_BOTH);

	::closesocket(listenSocket);

	::WSACleanup();
}
