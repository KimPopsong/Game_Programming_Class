#include "ClientSocket.h"
#include <stdio.h>


ClientSocket::ClientSocket()
{
	notificationWindow = NULL;
	receivedLength = 0;
	socket = INVALID_SOCKET;
	callbackFunc = 0;
	recvEvent = INVALID_HANDLE_VALUE;
}

ClientSocket::~ClientSocket()
{
	close();
}

DWORD ClientSocket::getHostByName(char* name, char* host)
{
	HOSTENT* hostent = ::gethostbyname(name);
	if (::WSAGetLastError() == WSANOTINITIALISED)
		return WSANOTINITIALISED;

	char* ipAddrStr = ::inet_ntoa(*(struct in_addr*)*hostent->h_addr_list);
	::strcpy_s(host, strlen(ipAddrStr), ipAddrStr);

	return 0;
}

bool ClientSocket::create(HWND notifyWnd, CallbackFunction callback)
{
	// ws2_32.dll�� ����ϱ� ���� �ʱ�ȭ�Ѵ�.
	WSADATA wsaData;
	if (::WSAStartup(0x202, &wsaData) != 0)
		return false;
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		::WSACleanup();
		return false;
	}

	notificationWindow = notifyWnd;
	recvEvent = ::WSACreateEvent();
	callbackFunc = callback;

	return true;
}

bool ClientSocket::connectTo(char* ipAddr, int portNumber)
{
	// ������ ������.
	socket = ::socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET) 
		onConnect(::WSAGetLastError());

	struct sockaddr_in serv_addr;
	::ZeroMemory(&serv_addr, sizeof(sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ::inet_addr(ipAddr);
	serv_addr.sin_port = ::htons(portNumber);

	// ������ ������ �õ���.
	if (::connect(socket, (struct sockaddr*)&serv_addr, sizeof(sockaddr_in)) != 0) {
		onConnect(::WSAGetLastError());
		return false;
	}
	else {
		onConnect(0);
		return true;
	}
}

void ClientSocket::close()
{
	if (recvEvent != INVALID_HANDLE_VALUE) {
		::WSACloseEvent(recvEvent);
		recvEvent = INVALID_HANDLE_VALUE;
	}

	::shutdown(socket, SD_BOTH);
	::closesocket(socket);
}

int ClientSocket::sendPacket(Packet& packet)
{
	OVERLAPPED overlapped;
	DWORD writtenBytes;

	::ZeroMemory(&overlapped, sizeof(OVERLAPPED));

	if (::WriteFile((HANDLE)socket, packet.getPacketBuffer(), packet.getPacketLength(), &writtenBytes, (OVERLAPPED*)&overlapped)) {
		return writtenBytes;
	}
	else {
		return 0;
	}
}

void ClientSocket::onConnect(DWORD error) 
{
	if (notificationWindow == 0)
		return;
	::PostMessage(notificationWindow, WM_CLIENTSOCKET_CONNECT, error, 0);
}

void ClientSocket::onClose() 
{
	if (notificationWindow == 0)
		return;
	::PostMessage(notificationWindow, WM_CLIENTSOCKET_CLOSE, 0, 0);
}

void ClientSocket::onReceive() 
{
	// ���ŵ� ��Ŷ�� ó����.

	Packet receivedPacket;
	DWORD bytesTransferred;
	DWORD bufSize = PACKETBUFFERSIZE - receivedLength;
	OVERLAPPED overlapped;

	// receiveBuffer�� ������ onReceive �Լ�ȣ�� ���Ŀ� ó������ ���� ��Ŷ���� �����ϰ� ����.
	// �� �ڿ� �̾ �����ϵ��� ����.

	::ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	::ReadFile((HANDLE)socket, &receiveBuffer[receivedLength], bufSize, &bytesTransferred, (OVERLAPPED*)&overlapped);

	if (bytesTransferred > 0)
		receivedLength += bytesTransferred;

	while (receivedLength > 0) {
		receivedPacket.copyToBuffer(receiveBuffer, receivedLength);

		if (receivedPacket.isValidPacket() == true && receivedLength >= (int)receivedPacket.getPacketLength()) {
			// ������ ��Ŷ�� �Ľ���.
			if (callbackFunc != 0) {
				bool success = (*callbackFunc)(receivedPacket);
				if (! success) { // ��Ŷ �Ľ� �Լ����� ������ �����߰ų� ������ �߻�����.
					// ������ ����.
					close();
					return;
				}
			}

			// �ݹ� �Լ����� ó���� ��Ŷ�� receivedBuffer���� �����Ѵ�. ó���� �� ��Ŷ ũ�⸸ �߶󳽴�.
			receivedLength -= receivedPacket.getPacketLength();
			::CopyMemory(buf, (receiveBuffer + receivedPacket.getPacketLength()), receivedLength);
			::CopyMemory(receiveBuffer, buf, receivedLength);
		}
		else {
			break;
		}
	}

}

LRESULT ClientSocket::selectEvent()
{
	// �ۼ��� ���� �̺�Ʈ�� �Ͼ������ �����ϰ� �ش� ��Ŷ ó�� �Լ��� ȣ����.

	WSANETWORKEVENTS netEvent;
	::ZeroMemory(&netEvent, sizeof(WSANETWORKEVENTS));
	if (::WSAEventSelect(socket, recvEvent, FD_READ|FD_CLOSE) == SOCKET_ERROR) {
		return FALSE; //������ ���� ��� ����.
	}
	if (::WSAEnumNetworkEvents(socket, recvEvent, &netEvent) == SOCKET_ERROR) {
		return FALSE; //������ ���� ��� ����.
	}

	if ((netEvent.lNetworkEvents & FD_READ) == FD_READ) {
		onReceive(); //���ŵ� ��Ŷ�� ó����.
		return TRUE;
	} else if ((netEvent.lNetworkEvents & FD_CLOSE) == FD_CLOSE) {
		onClose();
		return FALSE; //�������� ��� ����.
	} else {
		return TRUE;
	}
}
