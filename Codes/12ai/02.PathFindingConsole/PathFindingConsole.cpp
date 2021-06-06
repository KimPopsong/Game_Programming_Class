// PathFindingConsole - A* 알고리즘의 동작원리를 쉽게 이해하도록 실행과정을 보여준다.
//

#include "PathFinder.h"
#include <tchar.h>
#define TRACE	printf


int _tmain(int argc, _TCHAR* argv[])
{
	// 맵 데이터.
	const int mapWidth = 5;
	const int mapHeight = 5;
	BYTE map[mapWidth*mapHeight] = { //(x,y)
		1, 0, 1, 1, 1, //(0,0),(1,0),(2,0),(3,0),(4,0)
		1, 0, 0, 0, 1, //(0,1),(1,1),(2,1),(3,1),(4,1)
		1, 0, 0, 0, 1, //(0,2),(1,2),(2,2),(3,2),(4,2)
		1, 0, 0, 0, 1, //(0,3),(1,3),(2,3),(3,3),(4,3)
		1, 0, 0, 0, 1, //(0,4),(1,4),(2,4),(3,4),(4,4)
	};

	// 길찾기 클래스 초기화.
	PathFinder* pathFinder = new PathFinder();
	pathFinder->init(map, mapWidth, mapHeight);

	// 길찾기 함수 호출.
	bool found = pathFinder->search(2, 2, 1, 0); //출발점(x,y)=(2,2), 도착점(x,y)=(1,0).

	if (found) {
		TRACE("경로를 찾았습니다.\n\t 경로: ");
		std::deque<NodeLocation*> path = pathFinder->getPath();
		for (DWORD i=0; i<path.size(); i++) {
			TRACE("(%d, %d) %s ", path[i]->x, path[i]->y, (i==(path.size()-1))?"\n":"->");
		}
	} else {
		TRACE("경로를 찾을 수 없습니다.\n");
	}

	delete pathFinder;
	return 0;
}
