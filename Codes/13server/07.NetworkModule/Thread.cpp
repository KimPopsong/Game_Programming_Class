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
	// 쓰레드를 시작하도록 외부에서 호출하는 함수.

	if (started) return;

	// 쓰레드를 만드는 함수를 호출함. 쓰레드가 만들어지면 threadProcedure 함수가 자동호출됨.
	threadHandle = ThreadManager::getInstance()->spawn(threadProcedure, this, &threadID);

	started = true;
}

DWORD WINAPI Thread::threadProcedure(LPVOID parameter)
{
	// 인자 parameter는 자신 (this)임. 이 함수가 static이므로 인자를 통해 건내받음.
	Thread* thread = (Thread*)parameter;

	thread->run(); //실제 응용 프로그램의 작업 함수.

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
	// singleton 구현.
	if (selfInstance == 0)
		selfInstance = new ThreadManager();

	return selfInstance;
}

void ThreadManager::releaseInstance()
{
	// singleton 구현.
	if (selfInstance != 0) {
		delete selfInstance;
		selfInstance = 0;
	}
}

void ThreadManager::join()
{
	// 모든 쓰레드들이 종료될 때까지 대기함. 메인함수가 종료되기 전에 호출해 주어야 함.
	// 메인함수 종료 직전에서의 호출방법: ThreadManager::getInstance()->join();

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
	// Thread 클래스에서 호출함. 인자 parameter가 호출한 Thread 객체 자신임.

	// 실제로 쓰래드를 생성함.
	HANDLE threadHandle = ::CreateThread(0, 0, startAddress, parameter, 0, threadID);
	threadHandles.push_back(threadHandle);
	return threadHandle;
}
