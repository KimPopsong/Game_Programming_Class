// EchoTCPClient - ���� �ܼ��� TCP��� echo Ŭ���̾�Ʈ ���α׷�.
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
		printf("���� ���� ����: %d\n", ::WSAGetLastError());
		return;
	}

	// ������ ������ ������ �����Ѵ�.
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	serverAddr.sin_port = ::htons(8600);

	if (::connect(socketConnect, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		printf("������ ������ �� ���� - ������ ���� �����ϼ���: %d\n", ::WSAGetLastError());
		return;
	}

	// ������ ����� �Ѵ�.
	{
		char sendBuffer[127] = "[Ŭ���̾�Ʈ�� ���� �޽���]";
		int sentBytes = ::send(socketConnect, sendBuffer, ::strlen(sendBuffer) + 1, 0);
		printf("%d ����Ʈ�� ������: %s\n", sentBytes, sendBuffer);

		char recvBuffer[127];
		int recvBytes = ::recv(socketConnect, recvBuffer, 127, 0);
		printf("%d ����Ʈ�� �޾���: %s\n", recvBytes, recvBuffer);
	}

	::shutdown(socketConnect, SD_BOTH);
	::closesocket(socketConnect);
	::WSACleanup();
}
