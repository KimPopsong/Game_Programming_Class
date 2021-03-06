// StringSimple.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include <string> //string
#include <algorithm> //sort,reverse

using namespace std;

void test_string_1()
{
	cout << "test_string_1" << endl;

	string s;
	const char* my_str = "My friend";
	s += my_str;

	string t("Your friend");

	const char* t_cstyle = t.c_str();
	cout << "\t c-style = " << t_cstyle << "," << strlen(t_cstyle) << endl; //"Your friend,11"

	printf("\t [%s] %s [%s]\n", s.c_str(), (s < t ? "<" : ">="), t.c_str()); //"[My friend] < [Your friend]"

	t = s;

	if (t == "Your friend") cout << "\t string matched" << endl;
	else cout << "\t string mismatched" << endl; //"string mismatched"
}

void test_string_2()
{
	cout << "test_string_2" << endl;

	string s = "My friend";
	printf("\t length = %d or %d\n", s.length(), s.size()); //9 or 9

	cout << "\t chars = ";
	for (unsigned int i = 0; i < s.length(); i++) {
		cout << '(' << s[i] << ')'; //"(M)(y)( )(f)(r)(i)(e)(n)(d)"
	}
	cout << endl;

	cout << "\t chars = ";
	for (string::iterator iter = s.begin(); iter != s.end(); iter++) {
		cout << '(' << *iter << ')'; //"(M)(y)( )(f)(r)(i)(e)(n)(d)"
	}
	cout << endl;
}

void test_string_3()
{
	cout << "test_string_3" << endl;

	string s = "one cat two cat three cat and more.";

	int num = 0;
	for (int i = s.find("cat", 0); i != string::npos; i = s.find("cat", i)) {
		num++;
		i++;
	}
	cout << "\t num cat appearances = " << num << endl; //num cat appearances = 3

	string my_string = "My friend";
	string first_ten_of_alphabet = my_string.substr(3, 4);
	printf("\t substr(3, 4) is [%s]\n", first_ten_of_alphabet.c_str()); //[frie]
}

void test_string_4()
{
	cout << "test_string_4" << endl;

	string s("My friend");
	string t = s;

	reverse(s.begin(), s.end());
	printf("\t reverse = [%s]\n", s.c_str());

	sort(t.begin(), t.end());
	printf("\t sort = [%s]\n", t.c_str());
}


int main()
{
	test_string_1();
	test_string_2();
	test_string_3();
	test_string_4();

	return 0;
}
