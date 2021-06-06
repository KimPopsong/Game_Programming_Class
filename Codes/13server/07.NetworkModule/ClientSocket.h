#pragma once
#include <Winsock2.h>
#include <TChar.h>
#include "packet.h"


enum WindowMessage {
	WM_CLIENTSOCKET_BASE = WM_APP,
	WM_CLIENTSOCKET_CONNECT,
	WM_CLIENTSOCKET_CLOSE
};


class ClientSocket
{
public:
	typedef bool (*CallbackFunction)(Packet& packet);

private:
	CallbackFunction callbackFunc;

	SOCKET socket;
	HWND notificationWindow;
	UCHAR receiveBuffer[PACKETBUFFERSIZE];
	int receivedLength;
	UCHAR buf[PACKETBUFFERSIZE]; //onReceive 함수가 임시로 사용함.

	WSAEVENT recvEvent;

public:
	ClientSocket();
	virtual ~ClientSocket();

	static DWORD getHostByName(char* name, char* host);

	bool create(HWND notifyWnd, CallbackFunction callback);
	bool connectTo(char* ipAddr, int portNumber);
	void close();

	int sendPacket(Packet& packet);

	LRESULT selectEvent();

private:
	void onConnect(DWORD error);
	void onClose();
	void onReceive();

};
