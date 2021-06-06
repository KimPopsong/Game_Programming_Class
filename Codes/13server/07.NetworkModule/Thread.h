#pragma once
#include <winsock2.h>
// ����: Winsock2.h�� ���� include�� ������ windows.h�� include����.
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
