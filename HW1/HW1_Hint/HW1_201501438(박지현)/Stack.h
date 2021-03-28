#include <windows.h>
#include <string>
#include <stack>
#pragma once
using namespace std;

struct student {
	int score;
	string name;
};

LRESULT drawStack(HWND hwnd);
void init(HWND hwnd);

pair <int, int> getlaststack();

void setlaststack(int y);
void stackPop();