// TemplateStack.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include "Stack.h"
using namespace std;


int main()
{
	Stack stackInt;
	int i = 1;
	cout << endl << "stackInt에 원소들을 넣습니다:" << endl;
	while (stackInt.push(i)) { cout << i << ' '; i += 1; }
	cout << endl << "stackInt에서 원소들을 뺍니다:" << endl;
	while (stackInt.pop(i)) { cout << i << ' '; }
	return 0;
}
