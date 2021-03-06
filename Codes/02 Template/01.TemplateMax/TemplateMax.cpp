// TemplateMax.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
using namespace std;

// 'maximum' returns the maximum of the two elements

template <typename T>
T maximum(T a, T b)
{
	return a > b ? a : b;
}

int main()
{
	cout << "maximum(10, 15) = " << maximum(10, 15) << endl; // maximum(10, 15) = 15
	cout << "maximum('k', 's') = " << maximum('k', 's') << endl;   // maximum('k', 's') = s
	cout << "maximum(10.1, 15.2) = " << maximum(10.1, 15.2) << endl; //maximum(10.1, 15.2) = 15.2

    return 0;
}

