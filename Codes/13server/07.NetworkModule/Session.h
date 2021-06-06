#pragma once
#include "Packet.h"
#include "Thread.h"
#include <list>


// Synchronized

class Synchronized
{
private:
	CRITICAL_SECTION* criticalSection;

public:
	Synchronized(CRITICAL_SECTION& cs);
	virtual ~Synchronized();
};


// Session

#define ASYNCFLAG_SEND				0x01
#define ASYNCFLAG_RECEIVE			0x02


class Session
{
	friend class SessionManager;

private:
	typedef struct OVERLAPPED2 : OVERLAPPED {
		DWORD flags;
	} OVERLAPPED2;

protected:
	SOCKET socket;

private:
	bool dispatchReceive(DWORD bytesTransferred);

	OVERLAPPED2 overlappedSend;
	OVERLAPPED2 overlappedRecv;
	UCHAR receiveBuffer[PACKETBUFFERSIZE];
	int receivedPacketSize;
	UCHAR buf[PACKETBUFFERSIZE]; //dispatchReceive 함수가 임시로 사용함.

	HANDLE hEventRecv;

protected:
	// 생성자와 소멸자를 protected로 선언함.
	Session(SOCKET _socket);
	virtual ~Session();

public:
	virtual void onCreate();
	virtual void onDestroy();

	virtual bool dispatch(DWORD bytesTransferred, OVERLAPPED* _overlapped);	
	virtual bool sendMessage(Packet& packet);

	bool connectTo(char* address, int portNumber);
	void closeConnection();
	bool isDisconnected();

protected:
	virtual void parsePacket(Packet& packet){}
	bool waitForPacketReceive();
};


// WorkerThread

#define NUM_WORKER_THREADS		3


class WorkerThread : public Thread
{

private:
	HANDLE handleKillEvent;

public:
	WorkerThread();
	virtual ~WorkerThread();

	virtual void run();
};


// CompletionHandler

class CompletionHandler
{
private:
	static CompletionHandler* selfInstance;
	
	HANDLE hWorkerIOCP;
	WorkerThread* workerThread[NUM_WORKER_THREADS];

public:
	static CompletionHandler* getInstance();
	static void releaseInstance();

	bool initialize();

	HANDLE getWorkerIOCP();
	bool addHandleToIOCP(HANDLE handle, DWORD keyValue);

private:
	CompletionHandler();
	virtual ~CompletionHandler();
};


// SessionManager

class SessionManager
{
private:
	static SessionManager* selfInstance;

	std::list<Session*> sessions;
	CRITICAL_SECTION cs;

public:
	static SessionManager* getInstance();
	static void releaseInstance();

	void createSession(SOCKET socket);
	void removeSession(Session* session);
	bool isValidSession(Session* session);

private:
	SessionManager();
	virtual ~SessionManager();
};


// User

class User : public Session
{
	// SessionManager에서 protected로 선언된 User 클래스의 생성자와 소멸자를 사용할 수 있도록 함.
	friend class SessionManager;

public:
	virtual void onCreate();
	virtual void onDestroy();

protected:
	// 생성자와 소멸자를 protected로 선언함.
	User(SOCKET _socket);
	virtual ~User();

	virtual void parsePacket(Packet& packet);

private:
	void onPTConnectionCloseReq(Packet& packet);
	void onPTTestPacket1Req(Packet& packet);
	void onPTTestPacket2Req(Packet& packet);
	void onPTTestPacket3Req(Packet& packet);
};
