// EchoUDPClient - 가장 단순한 UDP기반 echo 클라이언트 프로그램.
//

#include <stdio.h>
#include <Winsock2.h>

#define BUFFER_LENGTH	255

void main()
{
	WSADATA wsaData;
	::WSAStartup(0x202, &wsaData);

	SOCKET socketValue = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketValue == INVALID_SOCKET) {
		printf("소켓 생성 에러: %d\n", ::WSAGetLastError());
		return;
	}

	struct sockaddr_in echoServerAddr;
	::memset(&echoServerAddr, 0, sizeof(echoServerAddr));
	echoServerAddr.sin_family = AF_INET;
	echoServerAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	echoServerAddr.sin_port = ::htons(8599);

	char echoString[] = "[클라이언트가 보낸 메시지]";
	char echoBuffer[BUFFER_LENGTH];
	int echoStringLen = (int)::strlen(echoString) + 1;

	// 에코 서버에 메시지를 전송한다.
	if (::sendto(socketValue, echoString, echoStringLen, 0, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != echoStringLen) {
		__asm int 3;
		return;
	}

	// 에코 서버로부터 메시지가 도착하기를 기다린다.
	struct sockaddr_in fromAddr;
	int fromSize = sizeof(fromAddr);
	int recvStringLen;
	if ((recvStringLen = ::recvfrom(socketValue, echoBuffer, BUFFER_LENGTH, 0, (struct sockaddr*)&fromAddr, &fromSize)) != echoStringLen) {
		__debugbreak();
		return;
	}

	// 수신된 패킷을 보낸 서버가 에코 서버가 아닐 경우 에러 처리를 한다.
	if (echoServerAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
		printf("받은 패킷이 모르는 곳으로부터 왔음!!");
		return;
	}

	// 에코 서버로부터 받은 메시지를 출력한다.
	echoBuffer[recvStringLen] = '\0';
	printf("%d 바이트를 받았음: %s\n", recvStringLen, echoBuffer);

	::shutdown(socketValue, SD_BOTH);

	::closesocket(socketValue);

	::WSACleanup();
}
