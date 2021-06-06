#include "PathFinder.h"

#define SAFE_FREE(p) { if(p) { free (p); (p)=NULL; } }
#define TRACE	printf


PathFinder::PathFinder()
{
	mapWidth = 0;
	mapHeight = 0;
	numMapTiles = 0;

	map = NULL;
	nodePool = NULL;

	valid = false;
}

PathFinder::~PathFinder()
{
	release();
}

bool PathFinder::init(BYTE* _map, int _mapWidth, int _mapHeight)
{
	// 주어진 맵 정보로 초기화한다.

	// 주의: map은 행(row)우선 순서임. 즉 (x,y)원소들의 순서는 (0,0),(1,0),(2,0),...임.
	// 인자 _map은 초기화한 후에 호출함수에서 free되어도 됨.

	bool shoudAllocate = false;
	if (! valid) { //최초 실행이거나, 이전에 release()가 호출되었음.
		shoudAllocate = true;
	} else if (mapWidth != _mapWidth || mapHeight != _mapHeight) {
		release();
		shoudAllocate = true;
	} else {
		//메모리를 할당할 필요 없음.
	}

	if (shoudAllocate) {
		//메모리를 새로 할당함.
		mapWidth = _mapWidth;
		mapHeight = _mapHeight;
		numMapTiles = mapWidth*mapHeight;
		nodePool = (Node*)malloc(sizeof(Node)*numMapTiles);
		if (nodePool == NULL) {
			TRACE("ERROR: 노드 풀 메모리를 할당할 수 없습니다!\n");
			return false;
		}
		map = (BYTE*)malloc(sizeof(BYTE)*numMapTiles);
		if (map == NULL) {
			TRACE("ERROR: 맵 메모리를 할당할 수 없습니다!\n");
			return false;
		}
	}

	// 맵 데이터를 복사함.
	memcpy(map, _map, sizeof(BYTE)*numMapTiles);

	path.clear();
	openList.clear();

	valid = true;

	return true;
}

bool PathFinder::release()
{
	if (!valid)
		return true;

	SAFE_FREE(nodePool);
	SAFE_FREE(map);

	mapWidth = 0;
	mapHeight = 0;
	numMapTiles = 0;

	path.clear();
	openList.clear();

	valid = false;

	return true;
}


//
// public 함수들.

bool PathFinder::search(int start_x, int start_y, int goal_x, int goal_y)
{
	int min_x, min_y, max_x, max_y; //맵에서 탐색할 영역.

	min_x = 0;
	min_y = 0;
	max_x = mapWidth-1;
	max_y = mapHeight-1;
	return searchRegion(start_x, start_y, goal_x, goal_y, min_x, min_y, max_x, max_y);
}

bool PathFinder::search(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y)
{
	//제한된 영역 내에서만 길찾기를 시도함.
	return searchRegion(start_x, start_y, goal_x, goal_y, min_x, min_y, max_x, max_y);
}


//
// 비용 계산

float PathFinder::estimateCost(NodeLocation startLocation, NodeLocation goalLocation) {
	//start에서 goal까지의 추정 비용.
	return (float)(abs(startLocation.x-goalLocation.x) + abs(startLocation.y-goalLocation.y));
}

float PathFinder::traverseCost(NodeLocation startLocation, NodeLocation destLocation) {
	//start에서 dest까지의 실제 비용.
	return (float)(abs(startLocation.x-destLocation.x) + abs(startLocation.y-destLocation.y));
}


//
// 리스트 관리 함수들.

Node* PathFinder::popPriorityQueue(PriorityQueue& pqueue)
{
	//Total time = O(log n)

	//가장 앞에 있는 노드가 total cost가 가장 작은 노드이므로 이 노드를 얻음.
	Node* node = pqueue.front();

	//pop_heap는 가장앞의 노드를 가장 끝으로 옮기고, 끝을 제외한 나머지 원소들을 정렬함.
	std::pop_heap(pqueue.begin(), pqueue.end(), NodeTotalGreater());
	//pop_back()은 마지막 원소를 제거함.
	pqueue.pop_back();

	return(node);
}

void PathFinder::pushPriorityQueue(PriorityQueue& pqueue, Node* node)
{
	//Total time = O(log n)

	//노드를 벡터의 끝에 추가함.
	pqueue.push_back(node);

	// 마지막 원소를 정렬이 되도록 적절한 위치로 옮김.
	// std::push_heap에 버거가 있으므로 사용하지 말자.
	std::make_heap(pqueue.begin(), pqueue.end(), NodeTotalGreater());
}


//
// 노드 풀 'nodePool'을 관리하는 함수들.

void PathFinder::clearNodePool() {
	for (int i=0; i<numMapTiles; i++) {
		nodePool[i].id = i;
		nodePool[i].allocated = false;
	}
}

int PathFinder::allocateNode() {
	for (int i=0; i<numMapTiles; i++) {
		if (! nodePool[i].allocated) {
			nodePool[i].allocated = true;
			return i;
		}
	}
	return -1;
}

void PathFinder::releaseNode(int index) {
	if (index < numMapTiles) {
		nodePool[index].allocated = false;
	}
}

int PathFinder::findNodeByLocation(NodeLocation loc) {
	for (int i=0; i<numMapTiles; i++) {
		if (nodePool[i].allocated) {
			if (nodePool[i].location.x == loc.x && nodePool[i].location.y == loc.y) {
				return i;
			}
		}
	}
	return -1;
}


//
// 길찾기 엔진.

bool PathFinder::searchRegion(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y)
{
	int nodeIndex;

	NodeLocation startLocation, goalLocation;
	startLocation.x = start_x;
	startLocation.y = start_y;
	goalLocation.x = goal_x;
	goalLocation.y = goal_y;

	clearNodePool(); //노드 pool을 clear함.
	path.clear(); //찾는 path를 clear함.

	openList.clear(); //openList를 empty로 만든다.

	//시작 노드를 초기화한다.
	nodeIndex = allocateNode();
	if (nodeIndex < 0) {
		TRACE("ERROR: 새 노드를 할당할 수 없습니다!\n");
		return false;
	}
	Node* startNode = &nodePool[nodeIndex];
	startNode->location = startLocation;
	startNode->cost = 0; //이 node까지의 비용은 0.
	float costToGoal = estimateCost(startLocation, goalLocation);
	startNode->total = costToGoal;
	startNode->parent = -1; //시작 노드임으로 부모 노드가 없음.
	//startNode를 openList에 추가함.
	startNode->onOpen = true;
	startNode->onClosed = false;
	pushPriorityQueue(openList, startNode);

	bool found = false;

	while (! openList.empty() ) { //openList가 비어있지 않는 한 계속함.

		Node* bestNode = popPriorityQueue(openList); //최저의 TotalCost를 가지는 node를 openList에서 pop함.
		TRACE("(%d,%d)를 방문함; f= %.1f; g= %.1f;\n", bestNode->location.x, bestNode->location.y, bestNode->total, bestNode->cost);

		if (bestNode->location.x == goalLocation.x && bestNode->location.y == goalLocation.y) {
			//bestNode가 goal임. startNode에서 bestNode까지의 경로를 생성함.
			found = true;
			int i = bestNode->id;
			while (i != -1) {
				path.push_front(&nodePool[i].location);
				i = nodePool[i].parent;
			}
			return true;
		}

		//현재의 노드인 bestNode에서 갈 수 있는 모든 경로들을 고려한다.
		int neighbors[4][2] = { {0, -1}, {-1, 0}, {1, 0}, {0, 1} };
		for (int k=0; k<=4; k++) { // 각 이웃노드들에 대해서.
			int nextLocX = bestNode->location.x + neighbors[k][0];
			int nextLocY = bestNode->location.y + neighbors[k][1];
			if (nextLocX < 0 || nextLocX >= mapWidth || nextLocY < 0 || nextLocY >= mapHeight) continue; //테두리를 벗어남.
			if (map[nextLocY*mapWidth+nextLocX] > 0) continue; //막혀있는 셀임.

			//Node의 모든 successor 노드들에 대해서 반복함.
			NodeLocation newLocation;
			newLocation.x = nextLocX;
			newLocation.y = nextLocY;
			float newCost = bestNode->cost + traverseCost(bestNode->location, newLocation);

			//새 노드가 이미 존재하는지를 검사한다.
			int actualNodeIndex = findNodeByLocation(newLocation);
			if (actualNodeIndex >= 0) {
				Node* actualNode = &nodePool[actualNodeIndex];
				if (actualNode->onOpen || actualNode->onClosed) { //openList나 closedList에 이미 있음.
					if (actualNode->cost <= newCost) { //이미 있는 경로가 더 나으므로 이 노드를 무시함.
						TRACE("\t (%d,%d)를 무시함; 새 g= %.1f; 기존 g= %.1f;\n", newLocation.x, newLocation.y, newCost, actualNode->cost);
						continue;
					} else { //기존의 node를 제거해야 함.
						releaseNode(actualNode->id); //nodePool에서 제거함.
					}
				}
			}

			//더 나은 경우이므로, 새 노드를 만들고 이를 저장한다.
			nodeIndex = allocateNode();
			if (nodeIndex < 0) {
				TRACE("ERROR: 새 노드를 할당할 수 없습니다!\n");
				return false;
			}
			Node* newNode = &nodePool[nodeIndex];
			newNode->location.x = newLocation.x;
			newNode->location.y = newLocation.y;
			newNode->parent = bestNode->id; //부모노드를 설정함.
			newNode->cost = newCost;
			newNode->total = newCost + estimateCost(newNode->location, goalLocation);
			newNode->onOpen = true;
			newNode->onClosed = false;
			//새로운 노드를 openList에 추가함.
			pushPriorityQueue(openList, newNode);
			TRACE("\t (%d,%d)를 추가함; f= %.1f, g= %.1f; Q크기=%d;\n", newNode->location.x, newNode->location.y, newNode->total, newCost, openList.size());

		}//} //for

		//bestNode에 대한 탐색이 끝났으므로 closedList에 넣는다.
		bestNode->onClosed = true;

	} //while

	return found;
}
