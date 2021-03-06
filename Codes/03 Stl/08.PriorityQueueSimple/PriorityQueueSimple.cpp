// PriorityQueueSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <functional> //greater
#include <queue> //queue

using namespace std;

typedef struct MyStruct {
	int key;
	char value;
} MyStruct;

struct MyStructCompare {
	bool operator()(const MyStruct s1, const MyStruct s2) const { return s1.key < s2.key; }
};

void print_pqueue(priority_queue<int> q, char* head)
{
	cout << '\t' << head;
	while (!q.empty()) {
		cout << q.top() << " ";
		q.pop();
	}
	cout << endl;
}

void test_pqueue_1()
{
	cout << "test_pqueue_1" << endl;

	//디폴트 컨테이너인 vector기반 priority_queue를 선언함.
	priority_queue <int> q1;
	//출력.
	cout << '\t' << "q1 = ";
	while (!q1.empty()) {
		cout << q1.top() << " ";
		q1.pop();
	}
	cout << endl;

	//deque 기반 priority_queue를 선언함.
	priority_queue <int, deque<int> > q2;
	q2.push(5);
	q2.push(9);
	q2.push(7);
	//출력.
	cout << '\t' << "q2 = ";
	while (!q2.empty()) {
		cout << q2.top() << " ";
		q2.pop();
	}
	cout << endl;
	cout << '\t' << "q2.size = " << q1.size() << endl;

	//정렬을 위한 인자를 주어 생성함.
	priority_queue <int, vector<int>, greater<int> > q3;
	q3.push(2);
	q3.push(1);
	q3.push(3);
	//출력.
	cout << '\t' << "q3 = ";
	while (!q3.empty()) {
		cout << q3.top() << " ";
		q3.pop();
	}
	cout << endl;

	//다른 컨테이너 q1의 데이터를 복사하여 priority_queue를 생성함.
	q1.push(4);
	q1.push(5);
	priority_queue <int> q4(q1);
	//출력.
	cout << '\t' << "q4 = ";
	while (!q4.empty()) {
		cout << q4.top() << " ";
		q4.pop();
	}
	cout << endl;

	//한 벡터 v5를 생성함.
	vector <int> v1;
	v1.push_back(6);
	v1.push_back(8);
	v1.push_back(7);
	//출력.
	cout << '\t' << "v1 = ";
	for (vector <int>::iterator iter = v1.begin(); iter != v1.end(); iter++)
		cout << *iter << " ";
	cout << endl;
	// 벡터의 범위 v1[_First, _Last)의 데이터를 복사하여 priority_queue를 생성함. 
	priority_queue <int> q5(v1.begin(), v1.begin() + 2);
	//출력.
	cout << '\t' << "q5 = ";
	while (!q5.empty()) {
		cout << q5.top() << " ";
		q5.pop();
	}
	cout << endl;

	// 벡터의 범위 v1[_First, _Last)의 데이터를 복사하여 priority_queue를 생성하며 비교연산도 지정함. 
	priority_queue <int, vector<int>, greater<int> > q6(v1.begin(), v1.begin() + 2);
	//출력.
	cout << '\t' << "q6 = ";
	while (!q6.empty()) {
		cout << q6.top() << " ";
		q6.pop();
	}
	cout << endl;
}

void test_pqueue_2()
{
	cout << "test_pqueue_2" << endl;

	MyStruct obj;
	priority_queue<MyStruct, vector<MyStruct>, MyStructCompare> q1;

	for (int i = 1; i < 10; i++) {
		obj.key = rand() % 10;
		obj.value = 'A' + (rand() % 26);
		q1.push(obj);
	}
	//출력.
	cout << '\t' << "q6 = ";
	while (!q1.empty()) {
		obj = q1.top();
		cout << '(' << obj.key << ',' << obj.value << ')' << " ";
		q1.pop();
	}
	cout << endl;
}


int main()
{
	test_pqueue_1();
	test_pqueue_2();

	return 0;
}
