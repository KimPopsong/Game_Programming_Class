// SetSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <set> //set
#include <algorithm> //set_union,set_intersection
#include <iterator>

using namespace std;

void print_set(set<int> v, const char* head)
{
	cout << '\t' << head;
	for (set<int>::iterator iter = v.begin(); iter != v.end(); iter++)
		cout << *iter << " ";
	cout << endl;
}

void test_set_1()
{
	cout << "test_set_1" << endl;

	set<int> s1;
	int A[8] = { 4, 2, 1, 2, 1, 2, 4, 1 };
	for (int i = 0; i < 8; i++)
		s1.insert(A[i]);
	print_set(s1, "s1 = ");

	set<int> s2;
	int B[5] = { 5, 2, 3, 3, 1 };
	for (int i = 0; i < 5; i++)
		s2.insert(B[i]);
	print_set(s2, "s2 = ");

	set<int> s3;
	set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(s3, s3.begin()));
	print_set(s3, "s3 = "); //합집합

	set<int> s4;
	set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(s4, s4.begin()));
	print_set(s4, "s4 = "); //교집합
}

void print_multiset(multiset<int> v, const char* head)
{
	cout << '\t' << head;
	for (multiset<int>::iterator iter = v.begin(); iter != v.end(); iter++)
		cout << *iter << " ";
	cout << endl;
}

void test_multiset_1()
{
	cout << "test_multiset_1" << endl;

	multiset<int, less<int> > s1;
	int A[8] = { 4, 2, 1, 2, 1, 2, 4, 2 };
	for (int i = 0; i < 8; i++)
		s1.insert(A[i]);
	print_multiset(s1, "s1 = ");

	s1.insert(s1.begin(), 3);
	print_multiset(s1, "s1 = ");

	s1.erase(2);
	print_multiset(s1, "s1 = ");
}


int main()
{
	test_set_1();
	test_multiset_1();

	return 0;
}
