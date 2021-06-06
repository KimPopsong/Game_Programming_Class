// NetworkModuleServer - 네트워크모듈 서버 프로그램임.
//

#include "ServerSocket.h"
#include "Session.h"


void main()
{
	Acceptor acceptor;

	if (acceptor.initialize("127.0.0.1", 8600) == false)
		return;
	printf("수락 담당 쓰레드가 실행을 시작함.\n");

	if (CompletionHandler::getInstance()->initialize() == false)
		return;
	printf("작업자 쓰레드들이 실행을 시작함.\n");

	printf("메인 쓰레드는 다른 일을 하면서 대기중에 있음.\n");

	ThreadManager::getInstance()->join();
}
