// PathFindingConsole - A* �˰����� ���ۿ����� ���� �����ϵ��� ��������� �����ش�.
//

#include "PathFinder.h"
#include <tchar.h>
#define TRACE	printf


int _tmain(int argc, _TCHAR* argv[])
{
	// �� ������.
	const int mapWidth = 5;
	const int mapHeight = 5;
	BYTE map[mapWidth*mapHeight] = { //(x,y)
		1, 0, 1, 1, 1, //(0,0),(1,0),(2,0),(3,0),(4,0)
		1, 0, 0, 0, 1, //(0,1),(1,1),(2,1),(3,1),(4,1)
		1, 0, 0, 0, 1, //(0,2),(1,2),(2,2),(3,2),(4,2)
		1, 0, 0, 0, 1, //(0,3),(1,3),(2,3),(3,3),(4,3)
		1, 0, 0, 0, 1, //(0,4),(1,4),(2,4),(3,4),(4,4)
	};

	// ��ã�� Ŭ���� �ʱ�ȭ.
	PathFinder* pathFinder = new PathFinder();
	pathFinder->init(map, mapWidth, mapHeight);

	// ��ã�� �Լ� ȣ��.
	bool found = pathFinder->search(2, 2, 1, 0); //�����(x,y)=(2,2), ������(x,y)=(1,0).

	if (found) {
		TRACE("��θ� ã�ҽ��ϴ�.\n\t ���: ");
		std::deque<NodeLocation*> path = pathFinder->getPath();
		for (DWORD i=0; i<path.size(); i++) {
			TRACE("(%d, %d) %s ", path[i]->x, path[i]->y, (i==(path.size()-1))?"\n":"->");
		}
	} else {
		TRACE("��θ� ã�� �� �����ϴ�.\n");
	}

	delete pathFinder;
	return 0;
}
