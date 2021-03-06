// DequeSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <deque> //deque

using namespace std;

struct MyStruct {
	int myInt;
};

void print_deque(deque<MyStruct> dq, const char* head)
{
	cout << '\t' << head;
	MyStruct m;
	for (unsigned i = 0; i < dq.size(); i++) {
		m = dq[i];
		cout << m.myInt << " ";
	}
	cout << endl;
}

void test_deque_1()
{
	cout << "test_deque_1" << endl;

	MyStruct m;
	deque<MyStruct> dq;
	m.myInt = 4;
	dq.push_front(m);
	print_deque(dq, "dq = ");

	m.myInt = 3;
	dq.push_back(m);
	print_deque(dq, "dq = ");

	m.myInt = 5;
	dq.push_front(m);
	print_deque(dq, "dq = ");

	dq.pop_front();
	print_deque(dq, "dq = ");

	dq.pop_back();
	print_deque(dq, "dq = ");
}


int main()
{
	test_deque_1();

	return 0;
}
