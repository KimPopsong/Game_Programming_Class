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
	// ws2_32.dll을 사용하기 위해 초기화한다.
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
	// 소켓을 생성함.
	socket = ::socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET) 
		onConnect(::WSAGetLastError());

	struct sockaddr_in serv_addr;
	::ZeroMemory(&serv_addr, sizeof(sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ::inet_addr(ipAddr);
	serv_addr.sin_port = ::htons(portNumber);

	// 서버로 연결을 시도함.
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
	// 수신된 패킷을 처리함.

	Packet receivedPacket;
	DWORD bytesTransferred;
	DWORD bufSize = PACKETBUFFERSIZE - receivedLength;
	OVERLAPPED overlapped;

	// receiveBuffer는 직전의 onReceive 함수호출 이후에 처리되지 않은 패킷들을 보관하고 있음.
	// 그 뒤에 이어서 저장하도록 하자.

	::ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	::ReadFile((HANDLE)socket, &receiveBuffer[receivedLength], bufSize, &bytesTransferred, (OVERLAPPED*)&overlapped);

	if (bytesTransferred > 0)
		receivedLength += bytesTransferred;

	while (receivedLength > 0) {
		receivedPacket.copyToBuffer(receiveBuffer, receivedLength);

		if (receivedPacket.isValidPacket() == true && receivedLength >= (int)receivedPacket.getPacketLength()) {
			// 수신함 패킷을 파싱함.
			if (callbackFunc != 0) {
				bool success = (*callbackFunc)(receivedPacket);
				if (! success) { // 패킷 파싱 함수에서 소켓을 종료했거나 에러가 발생했음.
					// 연결을 끊음.
					close();
					return;
				}
			}

			// 콜백 함수에서 처리한 패킷을 receivedBuffer에서 삭제한다. 처리된 한 패킷 크기만 잘라낸다.
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
	// 송수신 종료 이벤트가 일어났는지를 감시하고 해당 패킷 처리 함수를 호출함.

	WSANETWORKEVENTS netEvent;
	::ZeroMemory(&netEvent, sizeof(WSANETWORKEVENTS));
	if (::WSAEventSelect(socket, recvEvent, FD_READ|FD_CLOSE) == SOCKET_ERROR) {
		return FALSE; //에러로 인한 통신 종료.
	}
	if (::WSAEnumNetworkEvents(socket, recvEvent, &netEvent) == SOCKET_ERROR) {
		return FALSE; //에러로 인한 통신 종료.
	}

	if ((netEvent.lNetworkEvents & FD_READ) == FD_READ) {
		onReceive(); //수신된 패킷을 처리함.
		return TRUE;
	} else if ((netEvent.lNetworkEvents & FD_CLOSE) == FD_CLOSE) {
		onClose();
		return FALSE; //정상적인 통신 종료.
	} else {
		return TRUE;
	}
}
