#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include "Stack.h"
#include <stack>
#include <string>
#include <time.h>

#define padding 5
using namespace std;

student Students[12] = {
	{ 21, "아라리" },
	{ 30, "이진용" },
	{ 31, "이희찬" },
	{ 40, "송현수" },
	{ 50, "오준근" },
	{ 48, "신현성" },
	{ 20, "임상빈" },
	{ 32, "이진용" },
	{ 46, "김동환" },
	{ 33, "김진삼" },
	{ 36, "김사랑" },
	{ 37, "구혜선" },
}; // 학생 구조체 배열

int position, screenleft, screentop, screenright, screenbottom = 0;
int drawleft, drawtextleft;

pair <int, int> laststack; // 가장 최근에 들어간 스택의 x,y좌표를 갖는다.
stack <student> st; // 학생들의 정보를 갖는 스택이다. 

void init(HWND hwnd) {
	if (screenbottom != 0) // init함수가 또 한번 불리는 것을 막기 위해서이다.
		return;
	RECT rc; GetClientRect(hwnd, &rc); screenleft = rc.left; screentop = rc.top; screenright = rc.right; screenbottom = rc.bottom; //screen 사이즈를 갖고와서 각각의 screen 인자에 넣어준다.
	drawleft = (screenright - screenleft) / 2 - 100; // stack이 쌓일 x좌표
	drawtextleft = drawleft + 100 - 30; // text를 그릴 x좌표
	laststack.first = drawleft; // laststack에 가장 최근 스택 x,y좌표를 넣어준다.
	laststack.second = screenbottom;
	srand(time(NULL));
}

pair <int, int> getlaststack() {
	return laststack;
}

LRESULT drawStack(HWND hwnd) {
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);
	HPEN hMyPen = CreatePen(PS_SOLID, 3, RGB(rand()%255, rand()%255, rand()%255)); // 펜을 생성함
	HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyPen); // 펜을 선택함
	if (st.size() == 6)
		return -1;
	laststack.second = laststack.second - 50 - padding; // 스택 사이에 간격을 띄워주기 위함이다.
	Rectangle(ps.hdc, drawleft , laststack.second, drawleft + 200, screentop + laststack.second + 50);
	st.push(Students[position++ % 12]); // stuent구조체를 돌아가면서 차례대로 스택에 푸쉬해준다.
	string score = to_string(st.top().score); // Int형인 score를 string형으로 변환
	string nameAndScore = st.top().name + " " + score; // name과 score를 합쳐서 string으로 합침
	TextOut(ps.hdc, drawtextleft, laststack.second+50 - 25 - 10, nameAndScore.c_str(), nameAndScore.size()); // 그 string형 텍스트를 적절한 위치에 출력

	return 0;
}

void setlaststack(int y) { //가장 최근의 스택 x,y 좌표를 갱신해준다.
	laststack.second = y + padding;
} 
void stackPop() { // stack에서 빠졌으면 stackpop을 해준다. 
	if (st.size() >=1)
		st.pop();
}