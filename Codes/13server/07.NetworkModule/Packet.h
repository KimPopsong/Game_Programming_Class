// ������ Ŭ���̾�Ʈ ����.

#pragma once
#include <winsock2.h>
// ����: Winsock2.h�� ���� include�� ������ windows.h�� include����.
#include <windows.h>


enum PacketTypelList {
	PT_CONNECTIONSUCCESS_ACK	=	100,
	PT_CLOSE_REQ,			PT_CLOSE_ACK,

	PT_TESTPACKET1_REQ,		PT_TESTPACKET1_ACK,
	PT_TESTPACKET2_REQ,		PT_TESTPACKET2_ACK,
	PT_TESTPACKET3_REQ,		PT_TESTPACKET3_ACK

};


#define PACKETBUFFERSIZE		4096
#define PACKETHEADERSIZE		4


class Packet
{
private:
	UCHAR packetBuffer[PACKETBUFFERSIZE];
	// ��ġ �����͵�.
	USHORT* posTypeID; //����� packet Type ID.
	USHORT* posBodyLength; //����� packet body�� length.
	UCHAR* posBody; //��ü ���� ��ġ.
	UCHAR* posEnd; //��ü �� ��ġ.
	UCHAR* posRead;
	UCHAR* posWrite;

	int receivedLength;

public:
	Packet();
	Packet(WORD typeID);
	Packet(const Packet& source);
	virtual ~Packet();
	void clear();

	bool isValidHeader() { return (getPacketLength() >= PACKETHEADERSIZE); }
	bool isValidPacket() { return (isValidHeader() && (getPacketLength() >= receivedLength)); }

	void setTypeID(USHORT typeID) { *posTypeID = typeID; }
	USHORT getTypeID() { return *posTypeID; }
	USHORT getBodyLength() { return *posBodyLength; }
	int getPacketLength() { return (*posBodyLength + PACKETHEADERSIZE); }
	int getReceivedLength() { return receivedLength; }
	UCHAR* getPacketBuffer() { return packetBuffer; }

	void copyToBuffer(UCHAR* buffer, int size);

	void readData(void* buffer, int size);
	void writeData(void* buffer, int size);

	Packet&	operator = (Packet& packet);

	// ��Ŷ ����� ���Լ���.

	Packet& operator << (Packet& arg);
	Packet& operator >> (Packet& arg);

	Packet& operator << (LPTSTR arg) {	writeData(arg, lstrlen(arg)*sizeof(TCHAR)+sizeof(TCHAR)); return *this; }
	Packet& operator >> (LPTSTR arg) {	readData(arg, lstrlen((LPTSTR)posRead)*sizeof(TCHAR)+sizeof(TCHAR)); return *this; }
	Packet& operator << (bool arg) {	writeData(&arg, sizeof(bool)); return *this; }
	Packet& operator >> (bool& arg) {	readData(&arg, sizeof(bool)); return *this; }
	Packet& operator << (int arg) {	writeData(&arg, sizeof(int)); return *this; }
	Packet& operator >> (int& arg) {	readData(&arg, sizeof(int)); return *this; }
	Packet& operator << (long arg) {	writeData(&arg, sizeof(long)); return *this; }
	Packet& operator >> (long& arg) {	readData(&arg, sizeof(long)); return *this; }
	Packet& operator << (DWORD arg) {	writeData(&arg, sizeof(DWORD)); return *this; }
	Packet& operator >> (DWORD& arg) { readData(&arg, sizeof(DWORD)); return *this; }
	Packet& operator << (__int64 arg) {	writeData(&arg, sizeof(__int64)); return *this; }
	Packet& operator >> (__int64& arg) {	readData(&arg, sizeof(__int64)); return* this; }

};
