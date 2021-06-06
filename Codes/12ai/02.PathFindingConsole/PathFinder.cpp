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
	// �־��� �� ������ �ʱ�ȭ�Ѵ�.

	// ����: map�� ��(row)�켱 ������. �� (x,y)���ҵ��� ������ (0,0),(1,0),(2,0),...��.
	// ���� _map�� �ʱ�ȭ�� �Ŀ� ȣ���Լ����� free�Ǿ ��.

	bool shoudAllocate = false;
	if (! valid) { //���� �����̰ų�, ������ release()�� ȣ��Ǿ���.
		shoudAllocate = true;
	} else if (mapWidth != _mapWidth || mapHeight != _mapHeight) {
		release();
		shoudAllocate = true;
	} else {
		//�޸𸮸� �Ҵ��� �ʿ� ����.
	}

	if (shoudAllocate) {
		//�޸𸮸� ���� �Ҵ���.
		mapWidth = _mapWidth;
		mapHeight = _mapHeight;
		numMapTiles = mapWidth*mapHeight;
		nodePool = (Node*)malloc(sizeof(Node)*numMapTiles);
		if (nodePool == NULL) {
			TRACE("ERROR: ��� Ǯ �޸𸮸� �Ҵ��� �� �����ϴ�!\n");
			return false;
		}
		map = (BYTE*)malloc(sizeof(BYTE)*numMapTiles);
		if (map == NULL) {
			TRACE("ERROR: �� �޸𸮸� �Ҵ��� �� �����ϴ�!\n");
			return false;
		}
	}

	// �� �����͸� ������.
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
// public �Լ���.

bool PathFinder::search(int start_x, int start_y, int goal_x, int goal_y)
{
	int min_x, min_y, max_x, max_y; //�ʿ��� Ž���� ����.

	min_x = 0;
	min_y = 0;
	max_x = mapWidth-1;
	max_y = mapHeight-1;
	return searchRegion(start_x, start_y, goal_x, goal_y, min_x, min_y, max_x, max_y);
}

bool PathFinder::search(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y)
{
	//���ѵ� ���� �������� ��ã�⸦ �õ���.
	return searchRegion(start_x, start_y, goal_x, goal_y, min_x, min_y, max_x, max_y);
}


//
// ��� ���

float PathFinder::estimateCost(NodeLocation startLocation, NodeLocation goalLocation) {
	//start���� goal������ ���� ���.
	return (float)(abs(startLocation.x-goalLocation.x) + abs(startLocation.y-goalLocation.y));
}

float PathFinder::traverseCost(NodeLocation startLocation, NodeLocation destLocation) {
	//start���� dest������ ���� ���.
	return (float)(abs(startLocation.x-destLocation.x) + abs(startLocation.y-destLocation.y));
}


//
// ����Ʈ ���� �Լ���.

Node* PathFinder::popPriorityQueue(PriorityQueue& pqueue)
{
	//Total time = O(log n)

	//���� �տ� �ִ� ��尡 total cost�� ���� ���� ����̹Ƿ� �� ��带 ����.
	Node* node = pqueue.front();

	//pop_heap�� ������� ��带 ���� ������ �ű��, ���� ������ ������ ���ҵ��� ������.
	std::pop_heap(pqueue.begin(), pqueue.end(), NodeTotalGreater());
	//pop_back()�� ������ ���Ҹ� ������.
	pqueue.pop_back();

	return(node);
}

void PathFinder::pushPriorityQueue(PriorityQueue& pqueue, Node* node)
{
	//Total time = O(log n)

	//��带 ������ ���� �߰���.
	pqueue.push_back(node);

	// ������ ���Ҹ� ������ �ǵ��� ������ ��ġ�� �ű�.
	// std::push_heap�� ���Ű� �����Ƿ� ������� ����.
	std::make_heap(pqueue.begin(), pqueue.end(), NodeTotalGreater());
}


//
// ��� Ǯ 'nodePool'�� �����ϴ� �Լ���.

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
// ��ã�� ����.

bool PathFinder::searchRegion(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y)
{
	int nodeIndex;

	NodeLocation startLocation, goalLocation;
	startLocation.x = start_x;
	startLocation.y = start_y;
	goalLocation.x = goal_x;
	goalLocation.y = goal_y;

	clearNodePool(); //��� pool�� clear��.
	path.clear(); //ã�� path�� clear��.

	openList.clear(); //openList�� empty�� �����.

	//���� ��带 �ʱ�ȭ�Ѵ�.
	nodeIndex = allocateNode();
	if (nodeIndex < 0) {
		TRACE("ERROR: �� ��带 �Ҵ��� �� �����ϴ�!\n");
		return false;
	}
	Node* startNode = &nodePool[nodeIndex];
	startNode->location = startLocation;
	startNode->cost = 0; //�� node������ ����� 0.
	float costToGoal = estimateCost(startLocation, goalLocation);
	startNode->total = costToGoal;
	startNode->parent = -1; //���� ��������� �θ� ��尡 ����.
	//startNode�� openList�� �߰���.
	startNode->onOpen = true;
	startNode->onClosed = false;
	pushPriorityQueue(openList, startNode);

	bool found = false;

	while (! openList.empty() ) { //openList�� ������� �ʴ� �� �����.

		Node* bestNode = popPriorityQueue(openList); //������ TotalCost�� ������ node�� openList���� pop��.
		TRACE("(%d,%d)�� �湮��; f= %.1f; g= %.1f;\n", bestNode->location.x, bestNode->location.y, bestNode->total, bestNode->cost);

		if (bestNode->location.x == goalLocation.x && bestNode->location.y == goalLocation.y) {
			//bestNode�� goal��. startNode���� bestNode������ ��θ� ������.
			found = true;
			int i = bestNode->id;
			while (i != -1) {
				path.push_front(&nodePool[i].location);
				i = nodePool[i].parent;
			}
			return true;
		}

		//������ ����� bestNode���� �� �� �ִ� ��� ��ε��� ����Ѵ�.
		int neighbors[4][2] = { {0, -1}, {-1, 0}, {1, 0}, {0, 1} };
		for (int k=0; k<=4; k++) { // �� �̿����鿡 ���ؼ�.
			int nextLocX = bestNode->location.x + neighbors[k][0];
			int nextLocY = bestNode->location.y + neighbors[k][1];
			if (nextLocX < 0 || nextLocX >= mapWidth || nextLocY < 0 || nextLocY >= mapHeight) continue; //�׵θ��� ���.
			if (map[nextLocY*mapWidth+nextLocX] > 0) continue; //�����ִ� ����.

			//Node�� ��� successor ���鿡 ���ؼ� �ݺ���.
			NodeLocation newLocation;
			newLocation.x = nextLocX;
			newLocation.y = nextLocY;
			float newCost = bestNode->cost + traverseCost(bestNode->location, newLocation);

			//�� ��尡 �̹� �����ϴ����� �˻��Ѵ�.
			int actualNodeIndex = findNodeByLocation(newLocation);
			if (actualNodeIndex >= 0) {
				Node* actualNode = &nodePool[actualNodeIndex];
				if (actualNode->onOpen || actualNode->onClosed) { //openList�� closedList�� �̹� ����.
					if (actualNode->cost <= newCost) { //�̹� �ִ� ��ΰ� �� �����Ƿ� �� ��带 ������.
						TRACE("\t (%d,%d)�� ������; �� g= %.1f; ���� g= %.1f;\n", newLocation.x, newLocation.y, newCost, actualNode->cost);
						continue;
					} else { //������ node�� �����ؾ� ��.
						releaseNode(actualNode->id); //nodePool���� ������.
					}
				}
			}

			//�� ���� ����̹Ƿ�, �� ��带 ����� �̸� �����Ѵ�.
			nodeIndex = allocateNode();
			if (nodeIndex < 0) {
				TRACE("ERROR: �� ��带 �Ҵ��� �� �����ϴ�!\n");
				return false;
			}
			Node* newNode = &nodePool[nodeIndex];
			newNode->location.x = newLocation.x;
			newNode->location.y = newLocation.y;
			newNode->parent = bestNode->id; //�θ��带 ������.
			newNode->cost = newCost;
			newNode->total = newCost + estimateCost(newNode->location, goalLocation);
			newNode->onOpen = true;
			newNode->onClosed = false;
			//���ο� ��带 openList�� �߰���.
			pushPriorityQueue(openList, newNode);
			TRACE("\t (%d,%d)�� �߰���; f= %.1f, g= %.1f; Qũ��=%d;\n", newNode->location.x, newNode->location.y, newNode->total, newCost, openList.size());

		}//} //for

		//bestNode�� ���� Ž���� �������Ƿ� closedList�� �ִ´�.
		bestNode->onClosed = true;

	} //while

	return found;
}
