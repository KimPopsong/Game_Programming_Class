// NetworkModuleServer - ��Ʈ��ũ��� ���� ���α׷���.
//

#include "ServerSocket.h"
#include "Session.h"


void main()
{
	Acceptor acceptor;

	if (acceptor.initialize("127.0.0.1", 8600) == false)
		return;
	printf("���� ��� �����尡 ������ ������.\n");

	if (CompletionHandler::getInstance()->initialize() == false)
		return;
	printf("�۾��� ��������� ������ ������.\n");

	printf("���� ������� �ٸ� ���� �ϸ鼭 ����߿� ����.\n");

	ThreadManager::getInstance()->join();
}
