// VectorSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <vector> //vector
#include <functional> //greater
#include <algorithm> //sort
#include <time.h> //time

using namespace std;

void print_vector(vector<int> v, const char* head)
{
	cout << '\t' << head;
	for (vector <int>::iterator iter = v.begin(); iter != v.end(); iter++)
		cout << " " << *iter;
	cout << endl;
}

void test_vector_1()
{
	cout << "test_vector_1" << endl;

	vector <int> v0; //빈 벡터를 생성함. v1 = ().
	print_vector(v0, "v0 = ");

	vector <int> v1(3); //값이 모두 0인 3개의 원소를 가진 벡터를 생성함. v2 = (0,0,0).
	print_vector(v1, "v1 = ");

	vector <int> v2(5, 2); //값이 모두 2인 5개의 원소를 가진 벡터를 생성함. v3 = (2,2,2,2,2).
	print_vector(v2, "v2 = ");

	vector <int> v4(v2); //v2를 복사하여 새 벡터를 생성함. v4 = (2,2,2,2,2).
	print_vector(v4, "v4 = ");

	vector <int> v5(v4.begin() + 1, v4.begin() + 3); //v4의 [First,Last) 범위를 복사하여 새 벡터를 생성함. v5 = (2,2).
	print_vector(v5, "v5 = ");
}

void test_vector_2()
{
	cout << "test_vector_2" << endl;

	vector <int> v1;
	vector <int>::size_type i;

	v1.push_back(1);
	i = v1.size();
	cout << '\t' << "벡터 길이: " << v1.size() << endl; //벡터 길이: 1.

	v1.push_back(2);
	i = v1.size();
	cout << '\t' << "벡터 길이: " << i << endl; //벡터 길이: 2.
}

void test_vector_3()
{
	cout << "test_vector_3" << endl;

	vector <int> v1;

	v1.push_back(8);
	if (v1.size() != 0)
		cout << '\t' << "마지막 원소: " << v1.back() << endl; //마지막 원소: 8.

	v1.push_back(9);
	if (v1.size() != 0)
		cout << '\t' << "마지막 원소: " << v1.back() << endl; //마지막 원소: 9.

	if (v1.size() != 0)
		cout << '\t' << "첫 원소: " << v1.front() << endl; //첫 원소: 8.
}

void test_vector_4()
{
	cout << "test_vector_4" << endl;

	vector <int> v1;

	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);

	int& i = v1[1];
	cout << '\t' << "두 번째 원소: " << i << endl; //두 번째 원소: 2.
	int &j = v1.at(2);
	cout << '\t' << "세 번째 원소: " << j << endl; //세 번째 원소: 3.
}

void test_vector_5()
{
	cout << "test_vector_5" << endl;

	vector <int> v1;

	v1.push_back(1);
	cout << '\t' << "마지막 원소: " << v1.back() << endl; //마지막 원소: 1.

	v1.push_back(2);
	cout << '\t' << "마지막 원소: " << v1.back() << endl; //마지막 원소: 2.

	v1.pop_back();
	cout << '\t' << "마지막 원소: " << v1.back() << endl; //마지막 원소: 1.
}

void test_vector_6()
{
	cout << "test_vector_6" << endl;

	vector <int> v1;

	v1.push_back(1);
	v1.push_back(2);

	const int &i = v1.at(0);
	cout << '\t' << "첫 번째 원소: " << i << endl; //첫 번쨰 원소: 1.

	int &j = v1.at(1);
	cout << '\t' << "두 번째 원소: " << j << endl; //두 번째 원소: 2.
}

void test_vector_7()
{
	cout << "test_vector_7" << endl;

	vector<int> v;

	v.push_back(1);
	v.push_back(2);

	vector<int>::pointer ptr = &v[0];
	cout << '\t' << "시작주소의 원소: " << *ptr << endl; //시작주소의 원소: 1.

	ptr++;
	cout << '\t' << "증가된 시작주소의 원소: " << *ptr << endl; //증가된 시작주소의 원소: 2.

	*ptr = 4;
	cout << '\t' << "할당한 뒤의 원소: " << *ptr << endl; //할당한 뒤의 원소: 4.
}

bool comp(int x, int y) { return x < y; }

class compObj {
public:
	bool operator()(int x, int y) { return x > y; }
};

void test_vector_8()
{
	cout << "test_vector_8" << endl;

	srand((unsigned int)time(NULL));

	vector <int> v1;
	for (int i = 0; i < 8; i++) { v1.push_back((rand() % 10)); }
	print_vector(v1, "정렬전 벡터 = ");

	sort(v1.begin(), v1.end()); //정렬함. 디폴트는 오름차순으로, less<int>()를 한것과 같음.
	print_vector(v1, "정렬후 벡터 = ");

	sort(v1.begin(), v1.end(), greater<int>()); //내림차순으로 정렬함.
	print_vector(v1, "정렬후 벡터 = ");

	sort(v1.begin(), v1.end(), comp);
	print_vector(v1, "정렬후 벡터 = "); //오름차순으로 정렬함.

	sort(v1.begin(), v1.end(), compObj());
	print_vector(v1, "정렬후 벡터 = "); //내림차순으로 정렬함.
}


int main()
{
	test_vector_1();
	test_vector_2();
	test_vector_3();
	test_vector_4();
	test_vector_5();
	test_vector_6();
	test_vector_7();
	test_vector_8();

	return 0;
}
