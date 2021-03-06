// ListSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <functional> //greater
#include <list> //list
#include <time.h> //time

using namespace std;

void print_list(list<int> r, const char* head)
{
	cout << '\t' << head;
	for (list <int>::iterator iter = r.begin(); iter != r.end(); iter++)
		cout << " " << *iter;
	cout << endl;
}

void test_list_1()
{
	cout << "test_list_1" << endl;

	list<int> r0; //빈 리스트를 생성함. r0 = ().
	print_list(r0, "r0 = ");

	list<int> r1(3); //값이 0인 세 원소의 리스트를 생성함. r1 = (0,0,0).
	print_list(r1, "r1 = ");

	list<int> r2(5, 2); //값이 2인 5개의 원소를 가진 리스트를 생성함. r2 = (2,2,2,2,2).
	print_list(r2, "r2 = ");

	list<int> r3(r2); //r2를 복사하여 새 리스트를 생성함. r3 = (2,2,2,2,2).
	print_list(r3, "r3 = ");
}

void test_list_2()
{
	cout << "test_list_2" << endl;

	list <int> r1;

	r1.push_back(1);
	r1.push_back(2);

	list <int>::iterator iter = r1.begin();
	cout << '\t' << "첫번째 원소: " << *iter << endl; //첫번째 원소: 1.
	*iter = 4;

	list <int>::const_iterator citer = r1.begin();
	cout << '\t' << "첫번째 원소: " << *citer << endl; //첫번째 원소: 4.

	// 상수 iterator에 값을 지정하면 컴파일 에러가 발생함. 예: *citer = 4.
}

void test_list_3()
{
	cout << "test_list_3" << endl;

	list<int> r1;

	r1.push_back(2);
	r1.push_back(1);
	r1.push_back(3);

	print_list(r1, "r1 = "); //r1 = (2,1,3).

	list<int>::iterator iter = r1.end();
	iter--;
	cout << '\t' << "마지막 원소: " << *iter << endl; //마지막 원소: 3.

	iter--;
	*iter = 4; //마지막에서 두번째 원소를 바꿈.
	print_list(r1, "r1 = "); //r1 = (2,4,3).
}

void test_list_4()
{
	cout << "test_list_4" << endl;

	list <int> r1;

	r1.push_front(1); //리스트의 시작부분에 추가함.
	cout << '\t' << "첫번째 원소: " << r1.front() << endl; //첫번째 원소: 1.

	r1.push_front(2);
	cout << '\t' << "첫번째 원소: " << r1.front() << endl; //첫번째 원소: 2.

	r1.pop_front(); //리스트의 첫번째 원소를 삭제함.
	if (r1.size() != 0)
		cout << '\t' << "첫번째 원소: " << r1.front() << endl; //첫번째 원소: 1.
}

void test_list_5()
{
	cout << "test_list_5" << endl;

	list <int> r1;

	r1.push_back(1); //리스트의 마지막에 추가함.
	cout << '\t' << "마지막 원소: " << r1.back() << endl; //마지막 원소: 1.

	r1.push_back(2);
	cout << '\t' << "마지막 원소: " << r1.back() << endl; //마지막 원소: 2.

	r1.pop_back(); //리스트의 마지막 원소를 삭제함.
	cout << '\t' << "마지막 원소: " << r1.back() << endl; //마지막 원소: 1.
}

void test_list_6()
{
	cout << "test_list_6" << endl;

	list <int> c1;
	c1.push_back(2);
	c1.push_back(1);
	c1.push_back(3);
	print_list(c1, "c1 = ");
	//c1 = 2 1 3

	list <int>::iterator iter;
	iter = c1.begin();
	iter++;
	c1.insert(iter, 8); //where value
	print_list(c1, "c1 = ");
	//c1 = 2 8 1 3

	iter = c1.begin();
	iter++;
	iter++;
	c1.insert(iter, 2, 9); //where count(size_type) value
	print_list(c1, "c1 = ");
	//c1 = 2 8 9 9 1 3

	list <int> c2;
	c2.push_back(4);
	c2.push_back(5);
	c2.push_back(6);
	c1.insert(++c1.begin(), c2.begin(), --c2.end()); //where, first(iterator), last(iterator)
	print_list(c1, "c1 = ");
	//c1 = 2 4 5 8 9 9 1 3
}

void test_list_7()
{
	cout << "test_list_7" << endl;

	srand((unsigned int)time(NULL));

	list <int> r1;
	for (int i = 0; i < 8; i++) { r1.push_back((rand() % 10)); }
	print_list(r1, "정렬전 리스트 = ");

	r1.sort(); //정렬함. 디폴트는 오름차순으로, less<int>()를 한것과 같음.
	print_list(r1, "정렬후 리스트 = ");

	r1.sort(greater<int>()); //내림차순으로 정렬함.
	print_list(r1, "정렬후 리스트 = ");
}


int main()
{
	test_list_1();
	test_list_2();
	test_list_3();
	test_list_4();
	test_list_5();
	test_list_6();
	test_list_7();

	return 0;
}
