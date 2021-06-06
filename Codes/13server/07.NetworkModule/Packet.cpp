// 서버와 클라이언트 공통.

#include "Packet.h"


//
// Packet

Packet::Packet()
{
	posBody = NULL;
	posRead = NULL;
	posWrite = NULL;
	receivedLength = 0;
	clear();
}

Packet::Packet(USHORT typeID)
{
	posBody = NULL;
	posRead = NULL;
	posWrite = NULL;
	receivedLength = 0;
	clear();

	setTypeID(typeID);
}

Packet::Packet(const Packet& source)
{
	posBody = NULL;
	posRead = NULL;
	posWrite = NULL;
	receivedLength = 0;
	clear();

	::CopyMemory(packetBuffer, source.packetBuffer, PACKETBUFFERSIZE);
	receivedLength = source.receivedLength;

	posRead += ((DWORD)source.posRead - (DWORD)source.posBody);
	posWrite += ((DWORD)source.posWrite - (DWORD)source.posBody);
}

Packet::~Packet()
{
}


void Packet::clear()
{
	::ZeroMemory(packetBuffer, PACKETBUFFERSIZE);

	// posTypeID 변수는 2바이트, posBodyLength 변수는 2바이트.
	posTypeID = (USHORT*)packetBuffer + 0;
	posBodyLength = (USHORT*)((UCHAR*)packetBuffer + 2);

	posBody = &packetBuffer[4];
	posRead = posWrite = posBody;
	posEnd = &packetBuffer[PACKETBUFFERSIZE - 1];

	setTypeID(0);

	receivedLength = 0;
}

void Packet::copyToBuffer(UCHAR* buffer, int size)
{
	clear();
	::CopyMemory(packetBuffer, buffer, size);
	receivedLength += size;
}


void Packet::readData(void* buffer, int size)
{
	if ((posRead + size) > (posBody + getBodyLength()) || (posRead + size) > posEnd)
		return;

	::CopyMemory(buffer, posRead, size);
	posRead += size;
}

void Packet::writeData(void* buffer, int size)
{
	if (posWrite + size > posEnd)
		return;

	::CopyMemory(posWrite, buffer, size);
	posWrite += size;
	receivedLength += size;

	*posBodyLength += size;
}


// 패킷 입출력 편리함수들.

Packet& Packet::operator = (Packet& packet)
{
	::CopyMemory(posBody, packet.posBody, packet.getBodyLength());
	*posTypeID = packet.getTypeID();
	*posBodyLength = packet.getBodyLength();
	return *this;
}

Packet& Packet::operator << (Packet& arg)
{
	USHORT typeID = arg.getTypeID();
	USHORT bodyLength = arg.getBodyLength();
	writeData(&typeID, sizeof(USHORT));
	writeData(&bodyLength, sizeof(USHORT));
	writeData(arg.posBody, bodyLength);
	return *this;
}

Packet& Packet::operator >> (Packet& arg)
{
	USHORT typeID;
	USHORT bodyLength;
	UCHAR buffer[PACKETBUFFERSIZE];
	readData(&typeID, sizeof(USHORT));
	readData(&bodyLength, sizeof(USHORT));
	readData(buffer, bodyLength);
	arg.setTypeID(typeID);
	arg.writeData(buffer, bodyLength);
	return *this;
}
