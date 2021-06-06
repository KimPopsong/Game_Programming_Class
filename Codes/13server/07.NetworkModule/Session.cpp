#include "Session.h"
#include "Thread.h"
#include <tchar.h>
#include <stdio.h>


//
// Synchronized

Synchronized::Synchronized(CRITICAL_SECTION& cs)
{
	criticalSection = &cs;
	::EnterCriticalSection(criticalSection);
}

Synchronized::~Synchronized()
{
	::LeaveCriticalSection(criticalSection);
}


//
// Session

Session::Session(SOCKET _socket)
{
	socket = _socket;
	receivedPacketSize = 0;
	hEventRecv = INVALID_HANDLE_VALUE;
	waitForPacketReceive();
}

Session::~Session()
{
	::WSACloseEvent(hEventRecv);

	if (socket != INVALID_SOCKET) {
		::closesocket(socket);
	}
}

void Session::onCreate()
{
	if (socket == INVALID_SOCKET) {
		return;
	}
}

void Session::onDestroy()
{
	::closesocket(socket);
	socket = INVALID_SOCKET;
	receivedPacketSize = 0;
}

bool Session::connectTo(char* address, int portNumber)
{
	closeConnection();

	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (socket == INVALID_SOCKET) {
		return false;
	}

	unsigned long argp = 1;
	::ioctlsocket(socket, FIONBIO, &argp);

	struct sockaddr_in serv_addr;
	::ZeroMemory(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(address);
	serv_addr.sin_port = htons(portNumber);
	::connect(socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socket, &fds);

	struct timeval t = { 1, 0 };
	::select(0, 0, &fds, 0, &t);

	if (!FD_ISSET(socket, &fds)) {
		::closesocket(socket);
		return false;
	}

	hEventRecv = ::WSACreateEvent();

	return true;
}

void Session::closeConnection()
{
	::shutdown(socket, SD_BOTH);
	::closesocket(socket);
	socket = INVALID_SOCKET;

	receivedPacketSize = 0;
}

bool Session::isDisconnected()
{
	if (hEventRecv == INVALID_HANDLE_VALUE)
		return false;

	WSANETWORKEVENTS netEvent;
	::ZeroMemory(&netEvent, sizeof(netEvent));
	::WSAEventSelect(socket, hEventRecv, FD_READ | FD_CLOSE);
	::WSAEnumNetworkEvents(socket, hEventRecv, &netEvent);

	return ((netEvent.lNetworkEvents & FD_CLOSE) == FD_CLOSE);
}

bool Session::sendMessage(Packet& packet)
{
	if (socket == INVALID_SOCKET)
		return false;

	::ZeroMemory(&overlappedSend, sizeof(OVERLAPPED2));
	overlappedSend.flags = ASYNCFLAG_SEND;
	DWORD writtenBytes = 0;

	if (!::WriteFile((HANDLE)socket, packet.getPacketBuffer(), packet.getPacketLength(), &writtenBytes, (LPOVERLAPPED)&overlappedSend)) {
		DWORD lastError = ::GetLastError();
		if (lastError != ERROR_IO_PENDING && lastError != ERROR_SUCCESS)
			return false;
	}

	return true;
}

bool Session::waitForPacketReceive()
{
	if (socket == INVALID_SOCKET)
		return false;

	DWORD bufSize = PACKETBUFFERSIZE - receivedPacketSize;
	::ZeroMemory(&overlappedRecv, sizeof(OVERLAPPED2));
	overlappedRecv.flags = ASYNCFLAG_RECEIVE;
	DWORD readBytes = 0;

	if (!::ReadFile((HANDLE)socket, &receiveBuffer[receivedPacketSize], bufSize, &readBytes, (LPOVERLAPPED)&overlappedRecv)) {
		DWORD lastError = ::GetLastError();
		if (lastError != ERROR_IO_PENDING && lastError != ERROR_SUCCESS)
			return false;
	}

	return true;
}

bool Session::dispatch(DWORD bytesTransferred, OVERLAPPED* _overlapped)
{
	if (bytesTransferred == 0)
		return false;

	OVERLAPPED2* overlapped = (OVERLAPPED2*)_overlapped;
	if (overlapped->flags == ASYNCFLAG_RECEIVE) {
		if (! dispatchReceive(bytesTransferred)) {
			return false;
		} else {
			return true;
		}
	}

	return true;
}

bool Session::dispatchReceive(DWORD bytesTransferred)
{
	Packet receivedPacket;

	receivedPacketSize += bytesTransferred;

	while (receivedPacketSize > 0) {
		receivedPacket.copyToBuffer(receiveBuffer, receivedPacketSize);

		if ((receivedPacket.isValidPacket() == true) && (receivedPacketSize >= (int)receivedPacket.getPacketLength())) {
			parsePacket(receivedPacket); //수신한 패킷을 파싱함.

			receivedPacketSize -= receivedPacket.getPacketLength();
			if (receivedPacketSize > 0) {
				::CopyMemory(buf, (receiveBuffer + receivedPacket.getPacketLength()), receivedPacketSize);
				::CopyMemory(receiveBuffer, buf, receivedPacketSize);
			}
		}
		else { break; }
	}

	return waitForPacketReceive();
}


//
// WorkerThread

WorkerThread::WorkerThread()
{
	handleKillEvent = ::CreateEvent(0, 0, 0, 0);
}

WorkerThread::~WorkerThread()
{
	::SetEvent(handleKillEvent);

	::WaitForSingleObject(threadHandle, 1000);

	::CloseHandle(handleKillEvent);
}

void WorkerThread::run()
{
	DWORD bytesTransferred, keyValue;
	OVERLAPPED* overlapped;
	HANDLE hIOCP = CompletionHandler::getInstance()->getWorkerIOCP();

	while (1)
	{
		if (::WaitForSingleObject(handleKillEvent, 1) == WAIT_OBJECT_0)
			break;

		keyValue = 0;
		overlapped = 0;
		BOOL retVal = ::GetQueuedCompletionStatus(hIOCP, &bytesTransferred, &keyValue, &overlapped, 1);

		if (retVal == TRUE && keyValue != 0 && bytesTransferred != 0 && overlapped != 0) {

			Session* session = (Session*)keyValue;
			if (! SessionManager::getInstance()->isValidSession(session)) {
				// 이미 소멸된 세션에 대한 이벤트가 올 수 있으므로, 현재 유효한지를 검사해야 함.
				continue;
			}

			bool success = session->dispatch(bytesTransferred, overlapped);
			if (! success) {
				// 세션을 제거함.
				session->closeConnection();
				SessionManager::getInstance()->removeSession((Session*)keyValue);
			}
		}
		else {
			if (keyValue != 0 && ::GetLastError() != ERROR_OPERATION_ABORTED) {
				// 세션을 제거함.
				SessionManager::getInstance()->removeSession((Session*)keyValue);
			}
		}
	}
}


//
// CompletionHandler

CompletionHandler* CompletionHandler::selfInstance = 0;

CompletionHandler::CompletionHandler()
{
	hWorkerIOCP = 0;
}

CompletionHandler::~CompletionHandler()
{
	for (int i=0; i<NUM_WORKER_THREADS; i++) {
		delete workerThread[i];
	}
}

CompletionHandler* CompletionHandler::getInstance()
{
	if (selfInstance == 0) {
		selfInstance = new CompletionHandler();
	}
	return selfInstance;
}

void CompletionHandler::releaseInstance()
{
	if (selfInstance != 0) {
		delete selfInstance;
		selfInstance = 0;
	}
}

bool CompletionHandler::initialize()
{
	hWorkerIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, NUM_WORKER_THREADS);
	if (hWorkerIOCP == 0) return false;

	for (int i=0; i<NUM_WORKER_THREADS; i++) {
		workerThread[i] = new WorkerThread();
		workerThread[i]->begin();
	}
	return true;
}

HANDLE CompletionHandler::getWorkerIOCP()
{
	return hWorkerIOCP;
}

bool CompletionHandler::addHandleToIOCP(HANDLE handle, DWORD keyValue)
{
	::CreateIoCompletionPort(handle, hWorkerIOCP, keyValue, 0);
	return true;
}


//
// SessionManager

SessionManager* SessionManager::selfInstance = 0;

SessionManager::SessionManager()
{
	::InitializeCriticalSection(&cs);
}

SessionManager::~SessionManager()
{
	std::list<Session*>::iterator pos, posPrev;

	pos = sessions.begin();
	while (pos != sessions.end()) {
		posPrev = pos;
		Session* session = *pos;
		pos++;

		session->onDestroy();
		delete session;
		sessions.erase(posPrev);
	}

	::DeleteCriticalSection(&cs);
}

SessionManager* SessionManager::getInstance()
{
	if (selfInstance == 0)
		selfInstance = new SessionManager();

	return selfInstance;
}

void SessionManager::releaseInstance()
{
	if (selfInstance != 0) {
		delete selfInstance;
		selfInstance = 0;
	}
}

void SessionManager::createSession(SOCKET socket)
{
	User* newSession = new User(socket);

	CompletionHandler::getInstance()->addHandleToIOCP((HANDLE)socket, (DWORD)newSession);

	{ //쓰레드 동기화를 위해 블럭으로 묶음.
		Synchronized sync(cs);
		newSession->onCreate();
		sessions.push_back(newSession);
	}

	printf("새 세션 생성됨 (현재 세션수 = %d).\n", sessions.size());

}

void SessionManager::removeSession(Session* session)
{
	Synchronized sync(cs);
	std::list<Session*>::iterator pos;

	pos = sessions.begin();
	while (pos != sessions.end()) {
		Session* item = *pos;
		if (item == session) break;
		pos++;
	}

	session->onDestroy();
	delete session;
	sessions.erase(pos);

	printf("세션 제거됨 (현재 세션수 = %d).\n", sessions.size());
}


bool SessionManager::isValidSession(Session* session)
{
	// 인자로 주어지는 세션 포인터가 현재 리스트에 있는 유효한 포인터인지를 검사함.

	Synchronized sync(cs);
	std::list<Session*>::iterator pos;

	pos = sessions.begin();
	while (pos != sessions.end()) {
		if (*pos == session)
			return true;
		pos++;
	}

	return false;
}


//
// User - 응용에 따라 적절히 작성하면 됨.

User::User(SOCKET _socket)
	: Session(_socket)
{
}

User::~User()
{
}

void User::onCreate()
{
	Session::onCreate();

	// 서버와의 접속이 성공적으로 이루어졌음을 통보한다.
	Packet sendPacket(PT_CONNECTIONSUCCESS_ACK);
	sendMessage(sendPacket);
}

void User::onDestroy()
{
	Session::onDestroy();
}

void User::parsePacket(Packet& packet)
{
	switch (packet.getTypeID()) {
	case  PT_CLOSE_REQ :		onPTConnectionCloseReq(packet); break;
	case  PT_TESTPACKET1_REQ :	onPTTestPacket1Req(packet); break;
	case  PT_TESTPACKET2_REQ :	onPTTestPacket2Req(packet); break;
	case  PT_TESTPACKET3_REQ :	onPTTestPacket3Req(packet); break;
	default: return;
	}
}

void User::onPTConnectionCloseReq(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_CLOSE_REQ 수신됨: %s\n", str);

	Packet sendPacket;
	sendPacket.setTypeID(PT_CLOSE_ACK);
	sendPacket << str;
	sendMessage(sendPacket);

	// 클라이언트와의 연결을 끊음.
	closeConnection();
	printf("클라이언트와의 연결을 끊었음.\n");
}


void User::onPTTestPacket1Req(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET1_REQ 수신됨: %s\n", str);

	Packet sendPacket;
	sendPacket.setTypeID(PT_TESTPACKET1_ACK);
	sendPacket << str;
	sendMessage(sendPacket);
}

void User::onPTTestPacket2Req(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET2_REQ 수신됨: %s\n", str);

	Packet sendPacket;
	sendPacket.setTypeID(PT_TESTPACKET2_ACK);
	sendPacket << str;
	sendMessage(sendPacket);
}

void User::onPTTestPacket3Req(Packet& packet)
{
	char str[127];
	packet >> str;
	printf("PT_TESTPACKET3_REQ 수신됨: %s\n", str);

	Packet sendPacket;
	sendPacket.setTypeID(PT_TESTPACKET3_ACK);
	sendPacket << str;
	sendMessage(sendPacket);
}
