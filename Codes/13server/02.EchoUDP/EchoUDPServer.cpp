// EchoUDPServer - ���� �ܼ��� UDP��� echo Ŭ���̾�Ʈ ���α׷�.
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
		printf("���� ���� ����: %d\n", ::WSAGetLastError());
		return;
	}

	struct sockaddr_in echoServerAddr;
	::memset(&echoServerAddr, 0, sizeof(echoServerAddr));
	echoServerAddr.sin_family = AF_INET;
	echoServerAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	echoServerAddr.sin_port = ::htons(8599);

	if (::bind(listenSocket, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != 0) {
		printf("���ε� ����: %d\n", ::WSAGetLastError());
		return;
	}
	printf("������ �غ�Ǿ����ϴ�.\n");

	char echoBuffer[BUFFER_LENGTH];

	while (1) {
		struct sockaddr_in echoClientAddr;
		int clientAddrLen = sizeof(echoClientAddr);

		// Ŭ���̾�Ʈ�κ��� �޽����� �����ϱ⸦ ��ٸ���.
		int receiveSize = ::recvfrom(listenSocket, echoBuffer, BUFFER_LENGTH, 0, (struct sockaddr*)&echoClientAddr, &clientAddrLen);
		if (receiveSize < 0)
			continue;

		printf("Ŭ���̾�Ʈ(%s)�κ��� %d ����Ʈ�� �޾���: %s\n", ::inet_ntoa(echoClientAddr.sin_addr), receiveSize, echoBuffer);

		// ���� �޽����� Ŭ���̾�Ʈ�� �ǵ��� ������.
		if (::sendto(listenSocket, echoBuffer, receiveSize, 0, (struct sockaddr*)&echoClientAddr, sizeof(echoClientAddr)) != receiveSize) {
			__asm int 3;
			break;
		}
	}

	::shutdown(listenSocket, SD_BOTH);

	::closesocket(listenSocket);

	::WSACleanup();
}
