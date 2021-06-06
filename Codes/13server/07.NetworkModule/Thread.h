#pragma once
#include <winsock2.h>
// 주의: Winsock2.h를 먼저 include한 다음에 windows.h를 include하자.
#include <windows.h>
#include <list>


class Thread
{
protected:
	HANDLE threadHandle;

private:
	bool started;
	DWORD threadID;

public:
	Thread();
	virtual ~Thread();

	virtual void run() {}

	DWORD getThreadID() { return threadID; }

	void begin();

private:
	static DWORD WINAPI threadProcedure(LPVOID parameter);

};


class ThreadManager
{
private:
	static ThreadManager* selfInstance;

	std::list<HANDLE> threadHandles;

private:
	ThreadManager();
	virtual ~ThreadManager();

public:
	static ThreadManager* getInstance();
	static void releaseInstance();
	
	void join();
	HANDLE spawn(LPTHREAD_START_ROUTINE startAddress, LPVOID parameter, DWORD* identifier );

};
