#include "Thread.h"


//
// Thread

Thread::Thread()
{
	started = false;
	threadID = 0;
}

Thread::~Thread()
{
}

void Thread::begin()
{
	// �����带 �����ϵ��� �ܺο��� ȣ���ϴ� �Լ�.

	if (started) return;

	// �����带 ����� �Լ��� ȣ����. �����尡 ��������� threadProcedure �Լ��� �ڵ�ȣ���.
	threadHandle = ThreadManager::getInstance()->spawn(threadProcedure, this, &threadID);

	started = true;
}

DWORD WINAPI Thread::threadProcedure(LPVOID parameter)
{
	// ���� parameter�� �ڽ� (this)��. �� �Լ��� static�̹Ƿ� ���ڸ� ���� �ǳ�����.
	Thread* thread = (Thread*)parameter;

	thread->run(); //���� ���� ���α׷��� �۾� �Լ�.

	return 0;
}


//
// ThreadManager

ThreadManager* ThreadManager::selfInstance = 0;


ThreadManager::ThreadManager()
{
}

ThreadManager::~ThreadManager()
{
	while (threadHandles.size() > 0) {
		threadHandles.erase(threadHandles.begin());
	}
}

ThreadManager* ThreadManager::getInstance()
{
	// singleton ����.
	if (selfInstance == 0)
		selfInstance = new ThreadManager();

	return selfInstance;
}

void ThreadManager::releaseInstance()
{
	// singleton ����.
	if (selfInstance != 0) {
		delete selfInstance;
		selfInstance = 0;
	}
}

void ThreadManager::join()
{
	// ��� ��������� ����� ������ �����. �����Լ��� ����Ǳ� ���� ȣ���� �־�� ��.
	// �����Լ� ���� ���������� ȣ����: ThreadManager::getInstance()->join();

	std::list<HANDLE>::iterator pos, posPrev;

	pos = threadHandles.begin();
	while (pos != threadHandles.end()) {
		posPrev = pos++;
		::WaitForSingleObject(*posPrev, INFINITE);
		threadHandles.erase(posPrev);
	}
}

HANDLE ThreadManager::spawn(LPTHREAD_START_ROUTINE startAddress, LPVOID parameter, DWORD* threadID)
{
	// Thread Ŭ�������� ȣ����. ���� parameter�� ȣ���� Thread ��ü �ڽ���.

	// ������ �����带 ������.
	HANDLE threadHandle = ::CreateThread(0, 0, startAddress, parameter, 0, threadID);
	threadHandles.push_back(threadHandle);
	return threadHandle;
}
