// StackQueueSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <queue> //queue
#include <stack> //stack

using namespace std;

typedef struct MyStruct {
	int key;
	char value;
} MyStruct;

struct MyStructCompare {
	bool operator()(const MyStruct s1, const MyStruct s2) const { return s1.key < s2.key; }
};

void test_stack_1()
{
	cout << "test_stack_1" << endl;

	//두번째 인자를 생략하면 디폴트 deque기반 컨테이너를 사용하여 stack을 생성함.
	stack<MyStruct> s1;
	MyStruct p1;

	for (int i = 1; i < 10; i++) {
		p1.key = i; p1.value = 'A' + (i*i % 26);
		s1.push(p1);
	}
	//출력.
	cout << '\t' << "s1 = ";
	while (!s1.empty()) {
		p1 = s1.top();
		cout << '(' << p1.key << ',' << p1.value << ')' << ' ';
		s1.pop();
	}
	cout << endl;

	//vector 기반 컨테이너를 사용하여 stack을 생성함.
	vector<MyStruct> v1;
	p1.key = 3; p1.value = 'D';
	v1.push_back(p1);
	p1.key = 2; p1.value = 'C';
	v1.push_back(p1);
	p1.key = 4; p1.value = 'B';
	v1.push_back(p1);
	stack <MyStruct, vector<MyStruct> > s2(v1); //v1을 초기값으로 사용함.
	//출력.
	cout << '\t' << "s2 = ";
	while (!s2.empty()) {
		p1 = s2.top();
		cout << '(' << p1.key << ',' << p1.value << ')' << ' ';
		s2.pop();
	}
	cout << endl;
}

void test_queue_1()
{
	cout << "test_queue_1" << endl;

	queue<MyStruct> q1;
	MyStruct p1;
	p1.key = 3; p1.value = 'D';
	q1.push(p1);
	p1.key = 2; p1.value = 'C';
	q1.push(p1);
	p1.key = 4; p1.value = 'B';
	q1.push(p1);
	//출력.
	cout << '\t' << "q1 = ";
	while (!q1.empty()) {
		p1 = q1.front();
		cout << '(' << p1.key << ',' << p1.value << ')' << ' ';
		q1.pop();
	}
	cout << endl;
}


int main()
{
	test_stack_1();

	test_queue_1();

	return 0;
}
