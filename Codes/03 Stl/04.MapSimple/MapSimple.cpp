// MapSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <map> //map

using namespace std;

void test_map_1()
{
	cout << "test_map_1" << endl;

	map<const char*, int> m1;
	m1["january"] = 31;
	m1["february"] = 28;
	m1["april"] = 30;
	cout << '\t' << "february" << " = " << m1["february"] << endl;
	cout << '\t' << "march" << " = " << m1["march"] << endl;

	map<const char*, int>::iterator p = m1.find("january");
	cout << '\t' << p->first << " = " << p->second << endl;
}

struct MyStringCompare {
	bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) < 0; }
};

void test_map_2()
{
	cout << "test_map_2" << endl;

	map<const char*, int, MyStringCompare> m1;
	m1["january"] = 31;
	m1["february"] = 28;
	m1["april"] = 30;
	cout << '\t' << "m1 = ";
	for (map<const char*, int, MyStringCompare>::iterator iter = m1.begin(); iter != m1.end(); iter++)
		cout << iter->first << ' ';
	cout << endl;
}

void test_multimap_1()
{
	cout << "test_multimap_1" << endl;

	multimap<int, const char*> m;
	m.insert(pair<int, const char* const>(2, "two"));
	m.insert(pair<int, const char* const>(3, "three"));
	m.insert(pair<int, const char* const>(2, "zwei"));

	cout << '\t' << "#elements with key 2 = " << m.count(2) << endl;
	multimap<int, const char*>::const_iterator iter = m.find(2);
	cout << '\t' << "the first element with key 2 = " << iter->second << endl;

	for (multimap<int, const char*>::iterator p = m.begin(); p != m.end(); ++p)
		printf("\t[%d, %s]\n", p->first, p->second);
}


int main()
{
	test_map_1();
	test_map_2();
	test_multimap_1();

	return 0;
}
