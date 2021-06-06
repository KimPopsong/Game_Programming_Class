// EchoUDPClient - ���� �ܼ��� UDP��� echo Ŭ���̾�Ʈ ���α׷�.
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
		printf("���� ���� ����: %d\n", ::WSAGetLastError());
		return;
	}

	struct sockaddr_in echoServerAddr;
	::memset(&echoServerAddr, 0, sizeof(echoServerAddr));
	echoServerAddr.sin_family = AF_INET;
	echoServerAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	echoServerAddr.sin_port = ::htons(8599);

	char echoString[] = "[Ŭ���̾�Ʈ�� ���� �޽���]";
	char echoBuffer[BUFFER_LENGTH];
	int echoStringLen = (int)::strlen(echoString) + 1;

	// ���� ������ �޽����� �����Ѵ�.
	if (::sendto(socketValue, echoString, echoStringLen, 0, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != echoStringLen) {
		__asm int 3;
		return;
	}

	// ���� �����κ��� �޽����� �����ϱ⸦ ��ٸ���.
	struct sockaddr_in fromAddr;
	int fromSize = sizeof(fromAddr);
	int recvStringLen;
	if ((recvStringLen = ::recvfrom(socketValue, echoBuffer, BUFFER_LENGTH, 0, (struct sockaddr*)&fromAddr, &fromSize)) != echoStringLen) {
		__debugbreak();
		return;
	}

	// ���ŵ� ��Ŷ�� ���� ������ ���� ������ �ƴ� ��� ���� ó���� �Ѵ�.
	if (echoServerAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
		printf("���� ��Ŷ�� �𸣴� �����κ��� ����!!");
		return;
	}

	// ���� �����κ��� ���� �޽����� ����Ѵ�.
	echoBuffer[recvStringLen] = '\0';
	printf("%d ����Ʈ�� �޾���: %s\n", recvStringLen, echoBuffer);

	::shutdown(socketValue, SD_BOTH);

	::closesocket(socketValue);

	::WSACleanup();
}
