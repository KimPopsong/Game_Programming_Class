// NetworkModuleClient - 네트워크모듈 클라이언트 프로그램임.
//

#include "ClientSocket.h"
#include <stdio.h>


ClientSocket clientSocket;


void main()
{
	DWORD WINAPI threadProcedure(LPVOID parameter);
	bool parsePacket(Packet& packet);

	// 첫번째 인자는 윈도우 핸들, 두번째 인자는 수신된 패킷을 처리하는 함수이다.
	clientSocket.create(0, parsePacket);

	if (! clientSocket.connectTo("127.0.0.1", 8600)) {
		printf("서버와 연결 시도가 실패하였음.\n");
		clientSocket.close();
		return;
	} else {
		printf("서버와 연결 되었음.\n");
	}

	HANDLE handleThread = ::CreateThread(0, 0, threadProcedure, &clientSocket, 0, 0);
	printf("일꾼 쓰레드 시작함.\n");

	printf("메인 쓰레드는 지금 다른 일을 하고 있음...\n");

	::WaitForSingleObject(handleThread, INFINITE);
}


DWORD WINAPI threadProcedure(LPVOID parameter)
{
	// 계속해서 클라이언트 소켓의 이벤트 감시 함수를 호출한다.

	ClientSocket* clientSocket = (ClientSocket*)parameter;
	while (1) {
		// 네트워크 이벤트를 검사한다.
		if (! clientSocket->selectEvent()) {
			break;
		}

		::Sleep(100);
	}
	printf("일꾼 쓰레드 종료.\n");
	return TRUE;

}


int counter = 0;

bool parsePacket(Packet& packet)
{
	// 에러가 발생하면 false를 리턴함. false가 리턴되면 소켓을 닫고 연결을 끊음.

	bool onPTConnectionSuccessAck(Packet& packet);
	bool onPTConnectionCloseAck(Packet& packet);
	bool onPTTestPacket1Ack(Packet& packet);
	bool onPTTestPacket2Ack(Packet& packet);
	bool onPTTestPacket3Ack(Packet& packet);

	// 상태를 관찰하기 위해서 느리게 함.
	::Sleep(1000);

	switch (packet.getTypeID()) {
	case PT_CONNECTIONSUCCESS_ACK:	return onPTConnectionSuccessAck(packet);
	case PT_CLOSE_ACK:		return onPTConnectionCloseAck(packet);
	case PT_TESTPACKET1_ACK:		return onPTTestPacket1Ack(packet);
	case PT_TESTPACKET2_ACK:
		if (counter++ < 5)		return onPTTestPacket1Ack(packet);
		else			return onPTTestPacket2Ack(packet);
	case PT_TESTPACKET3_ACK:		return onPTTestPacket3Ack(packet);
	default:				return true;
	}
}


bool onPTConnectionSuccessAck(Packet& packet)
{
	Packet sendPacket(PT_TESTPACKET1_REQ);

	sendPacket << "[패킷 데이터 1]";
	clientSocket.sendPacket(sendPacket);
	return true;
}

bool onPTConnectionCloseAck(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_CLOSE_ACK 수신됨: %s\n", str);

	clientSocket.close();
	printf("서버와의 연결을 끊었음.\n");
	return false;
}


bool onPTTestPacket1Ack(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET1_ACK 수신됨: %s\n", str);

	Packet sendPacket(PT_TESTPACKET2_REQ);
	sendPacket << "[패킷 데이터 2]";
	clientSocket.sendPacket(sendPacket);
	return true;
}

bool onPTTestPacket2Ack(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET2_ACK 수신됨: %s\n", str);

	Packet sendPacket(PT_TESTPACKET3_REQ);
	sendPacket << "[패킷 데이터 3]";
	clientSocket.sendPacket(sendPacket);
	return true;
}

bool onPTTestPacket3Ack(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET3_ACK 수신됨: %s\n", str);

	Packet sendPacket(PT_CLOSE_REQ);
	sendPacket << "[연결 끊음 요청 패킷 데이터]";
	clientSocket.sendPacket(sendPacket);
	return true;
}
