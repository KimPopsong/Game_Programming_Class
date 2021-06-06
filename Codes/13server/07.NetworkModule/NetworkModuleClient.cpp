// NetworkModuleClient - ��Ʈ��ũ��� Ŭ���̾�Ʈ ���α׷���.
//

#include "ClientSocket.h"
#include <stdio.h>


ClientSocket clientSocket;


void main()
{
	DWORD WINAPI threadProcedure(LPVOID parameter);
	bool parsePacket(Packet& packet);

	// ù��° ���ڴ� ������ �ڵ�, �ι�° ���ڴ� ���ŵ� ��Ŷ�� ó���ϴ� �Լ��̴�.
	clientSocket.create(0, parsePacket);

	if (! clientSocket.connectTo("127.0.0.1", 8600)) {
		printf("������ ���� �õ��� �����Ͽ���.\n");
		clientSocket.close();
		return;
	} else {
		printf("������ ���� �Ǿ���.\n");
	}

	HANDLE handleThread = ::CreateThread(0, 0, threadProcedure, &clientSocket, 0, 0);
	printf("�ϲ� ������ ������.\n");

	printf("���� ������� ���� �ٸ� ���� �ϰ� ����...\n");

	::WaitForSingleObject(handleThread, INFINITE);
}


DWORD WINAPI threadProcedure(LPVOID parameter)
{
	// ����ؼ� Ŭ���̾�Ʈ ������ �̺�Ʈ ���� �Լ��� ȣ���Ѵ�.

	ClientSocket* clientSocket = (ClientSocket*)parameter;
	while (1) {
		// ��Ʈ��ũ �̺�Ʈ�� �˻��Ѵ�.
		if (! clientSocket->selectEvent()) {
			break;
		}

		::Sleep(100);
	}
	printf("�ϲ� ������ ����.\n");
	return TRUE;

}


int counter = 0;

bool parsePacket(Packet& packet)
{
	// ������ �߻��ϸ� false�� ������. false�� ���ϵǸ� ������ �ݰ� ������ ����.

	bool onPTConnectionSuccessAck(Packet& packet);
	bool onPTConnectionCloseAck(Packet& packet);
	bool onPTTestPacket1Ack(Packet& packet);
	bool onPTTestPacket2Ack(Packet& packet);
	bool onPTTestPacket3Ack(Packet& packet);

	// ���¸� �����ϱ� ���ؼ� ������ ��.
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

	sendPacket << "[��Ŷ ������ 1]";
	clientSocket.sendPacket(sendPacket);
	return true;
}

bool onPTConnectionCloseAck(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_CLOSE_ACK ���ŵ�: %s\n", str);

	clientSocket.close();
	printf("�������� ������ ������.\n");
	return false;
}


bool onPTTestPacket1Ack(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET1_ACK ���ŵ�: %s\n", str);

	Packet sendPacket(PT_TESTPACKET2_REQ);
	sendPacket << "[��Ŷ ������ 2]";
	clientSocket.sendPacket(sendPacket);
	return true;
}

bool onPTTestPacket2Ack(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET2_ACK ���ŵ�: %s\n", str);

	Packet sendPacket(PT_TESTPACKET3_REQ);
	sendPacket << "[��Ŷ ������ 3]";
	clientSocket.sendPacket(sendPacket);
	return true;
}

bool onPTTestPacket3Ack(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET3_ACK ���ŵ�: %s\n", str);

	Packet sendPacket(PT_CLOSE_REQ);
	sendPacket << "[���� ���� ��û ��Ŷ ������]";
	clientSocket.sendPacket(sendPacket);
	return true;
}
