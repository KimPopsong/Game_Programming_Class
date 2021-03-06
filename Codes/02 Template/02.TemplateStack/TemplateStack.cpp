// TemplateStack.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include "Stack.h"
using namespace std;


int main()
{
	Stack<float> fstack(5);
	float f = 1.1F;
	cout << "fstack에 원소들을 넣습니다:" << endl;
	while (fstack.push(f)) { cout << f << ' '; f += 1.1f; }
	cout << endl << "fstack에서 원소들을 뺍니다:" << endl;
	while (fstack.pop(f)) { cout << f << ' '; }

	Stack<int> istack;
	int i = 1;
	cout << endl << "istack에 원소들을 넣습니다:" << endl;
	while (istack.push(i)) { cout << i << ' '; i += 1; }
	cout << endl << "istack에서 원소들을 뺍니다:" << endl;
	while (istack.pop(i)) { cout << i << ' '; }
	cout << endl;

	return 0;
}

