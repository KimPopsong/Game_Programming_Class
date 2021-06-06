#pragma warning (disable: 4786)

#include <Windows.h>
#include <list>
#include "Thread.h"
#include "Synchronized.h"

CRITICAL_SECTION cs;

std::list<int> listNumber;

// other threads

class ThreadInsert : public Thread
{
public:
	ThreadInsert(){}
	virtual ~ThreadInsert(){}
	virtual void run();
};

class ThreadDelete : public Thread
{
public:
	ThreadDelete(){}
	virtual ~ThreadDelete(){}
	virtual void run();
};

class ThreadOutput : public Thread
{
public:
	ThreadOutput(){}
	virtual ~ThreadOutput(){}
	virtual void run();
};

// other threads implementations

void ThreadInsert::run()
{
	int i;

	while (1)
	{
		::Sleep(rand() % 100);

		{
			Synchronized sync(cs);

			for (i = 0 ; i < 10 ; i++)
				listNumber.push_back(i);
		}
	}
}

void ThreadDelete::run()
{
	while (1)
	{
		::Sleep(rand() % 100);

		{
			Synchronized sync(cs);

			while (listNumber.size() > 0)
				listNumber.erase(listNumber.begin());
		}
	}
}

void ThreadOutput::run()
{
	std::list < int >::iterator iter;

	while (1)
	{
		::Sleep(rand() % 100);

		{
			Synchronized sync(cs);

			iter = listNumber.begin();
			while(iter != listNumber.end())
			{
				printf("%d\n", *iter);
				iter++;
			}
		}
	}
}

void main()
{
	ThreadInsert insertThread;
	ThreadDelete deleteThread;
	ThreadOutput outputThread;

	::InitializeCriticalSection(&cs);

	insertThread.begin();
	deleteThread.begin();
	outputThread.begin();

	::Sleep(5000);
}
