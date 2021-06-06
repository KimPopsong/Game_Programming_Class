// PathFinder - A* path finding 알고리즘 구현 클래스.

#pragma once
#include <windows.h>
#include <vector>
#include <queue>


typedef struct NodeLocation {
	int x;
	int y;
} NodeLocation;

typedef struct PATHNODE {
	WORD xi;
	WORD zi;
} PATHNODE;

typedef struct Node {
	int id;			//항상 nodePool에서의 index와 동일하게 해야 함. 즉 0,1,2....
	bool allocated;

	NodeLocation location;	// node 위치
	int parent;		// parent node의 인덱스 (-1은 시작 노드임을 의미함)
	float cost;		// start에서 이 노드까지의 cost
	float total;		// 총 비용 (cost + heuristic estimate)
	bool onOpen;		// Open list에 있는 경우 참임.
	bool onClosed;		// Closed list에 있는 경우 참임.
} Node;


class NodeTotalGreater 
{
public:
	// 우선순위 큐를 정렬하기 위한 함수.
	bool operator()(Node* first, Node* second) const {
		return (first->total > second->total);
	}
};


// 우선순위 큐의 구현.
typedef std::vector<Node*> PriorityQueue;


class PathFinder
{

private:
	BYTE* map; //인덱싱은 map[y][x]임.
	int mapWidth; //x size.
	int mapHeight; //y size.
	bool valid;

	int numMapTiles; //nodePool의 전체 node수로 항상 mapWidth*mapHeight임.
	Node* nodePool;

	std::deque<NodeLocation*> path; //검색된 path임. 첫번째가 destination node이고, 마지막이 start node임.

	PriorityQueue openList; //open list임. 아직 탐색하지 않은 노드들을 가짐.

public:
	PathFinder();
	~PathFinder();
	bool init(BYTE* _map, int _mapWidth, int _mapHeight);
	bool release();

public:
	bool isValid() { return valid; }
	bool search(int start_x, int start_y, int goal_x, int goal_y);
	bool search(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y);

	std::deque<NodeLocation*> getPath() { return path; }

private:
	// 비용 계산 함수들.
	float estimateCost(NodeLocation startLocation, NodeLocation goalLocation);
	float traverseCost(NodeLocation startLocation, NodeLocation destLocation);
	// 리스트 관리 함수들.
	Node* popPriorityQueue(PriorityQueue& pqueue);
	void pushPriorityQueue(PriorityQueue& pqueue, Node* node);
	// 노드풀 관리 함수들.
	void clearNodePool();
	int allocateNode();
	void releaseNode(int index);
	int findNodeByLocation(NodeLocation loc);

	bool searchRegion(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y);

};
