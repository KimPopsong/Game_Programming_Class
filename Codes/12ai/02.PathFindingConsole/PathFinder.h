// PathFinder - A* path finding �˰��� ���� Ŭ����.

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
	int id;			//�׻� nodePool������ index�� �����ϰ� �ؾ� ��. �� 0,1,2....
	bool allocated;

	NodeLocation location;	// node ��ġ
	int parent;		// parent node�� �ε��� (-1�� ���� ������� �ǹ���)
	float cost;		// start���� �� �������� cost
	float total;		// �� ��� (cost + heuristic estimate)
	bool onOpen;		// Open list�� �ִ� ��� ����.
	bool onClosed;		// Closed list�� �ִ� ��� ����.
} Node;


class NodeTotalGreater 
{
public:
	// �켱���� ť�� �����ϱ� ���� �Լ�.
	bool operator()(Node* first, Node* second) const {
		return (first->total > second->total);
	}
};


// �켱���� ť�� ����.
typedef std::vector<Node*> PriorityQueue;


class PathFinder
{

private:
	BYTE* map; //�ε����� map[y][x]��.
	int mapWidth; //x size.
	int mapHeight; //y size.
	bool valid;

	int numMapTiles; //nodePool�� ��ü node���� �׻� mapWidth*mapHeight��.
	Node* nodePool;

	std::deque<NodeLocation*> path; //�˻��� path��. ù��°�� destination node�̰�, �������� start node��.

	PriorityQueue openList; //open list��. ���� Ž������ ���� ������ ����.

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
	// ��� ��� �Լ���.
	float estimateCost(NodeLocation startLocation, NodeLocation goalLocation);
	float traverseCost(NodeLocation startLocation, NodeLocation destLocation);
	// ����Ʈ ���� �Լ���.
	Node* popPriorityQueue(PriorityQueue& pqueue);
	void pushPriorityQueue(PriorityQueue& pqueue, Node* node);
	// ���Ǯ ���� �Լ���.
	void clearNodePool();
	int allocateNode();
	void releaseNode(int index);
	int findNodeByLocation(NodeLocation loc);

	bool searchRegion(int start_x, int start_y, int goal_x, int goal_y, int min_x, int min_y, int max_x, int max_y);

};
