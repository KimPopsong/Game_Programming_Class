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
	{ 21, "�ƶ�" },
	{ 30, "������" },
	{ 31, "������" },
	{ 40, "������" },
	{ 50, "���ر�" },
	{ 48, "������" },
	{ 20, "�ӻ��" },
	{ 32, "������" },
	{ 46, "�赿ȯ" },
	{ 33, "������" },
	{ 36, "����" },
	{ 37, "������" },
}; // �л� ����ü �迭

int position, screenleft, screentop, screenright, screenbottom = 0;
int drawleft, drawtextleft;

pair <int, int> laststack; // ���� �ֱٿ� �� ������ x,y��ǥ�� ���´�.
stack <student> st; // �л����� ������ ���� �����̴�. 

void init(HWND hwnd) {
	if (screenbottom != 0) // init�Լ��� �� �ѹ� �Ҹ��� ���� ���� ���ؼ��̴�.
		return;
	RECT rc; GetClientRect(hwnd, &rc); screenleft = rc.left; screentop = rc.top; screenright = rc.right; screenbottom = rc.bottom; //screen ����� ����ͼ� ������ screen ���ڿ� �־��ش�.
	drawleft = (screenright - screenleft) / 2 - 100; // stack�� ���� x��ǥ
	drawtextleft = drawleft + 100 - 30; // text�� �׸� x��ǥ
	laststack.first = drawleft; // laststack�� ���� �ֱ� ���� x,y��ǥ�� �־��ش�.
	laststack.second = screenbottom;
	srand(time(NULL));
}

pair <int, int> getlaststack() {
	return laststack;
}

LRESULT drawStack(HWND hwnd) {
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);
	HPEN hMyPen = CreatePen(PS_SOLID, 3, RGB(rand()%255, rand()%255, rand()%255)); // ���� ������
	HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyPen); // ���� ������
	if (st.size() == 6)
		return -1;
	laststack.second = laststack.second - 50 - padding; // ���� ���̿� ������ ����ֱ� �����̴�.
	Rectangle(ps.hdc, drawleft , laststack.second, drawleft + 200, screentop + laststack.second + 50);
	st.push(Students[position++ % 12]); // stuent����ü�� ���ư��鼭 ���ʴ�� ���ÿ� Ǫ�����ش�.
	string score = to_string(st.top().score); // Int���� score�� string������ ��ȯ
	string nameAndScore = st.top().name + " " + score; // name�� score�� ���ļ� string���� ��ħ
	TextOut(ps.hdc, drawtextleft, laststack.second+50 - 25 - 10, nameAndScore.c_str(), nameAndScore.size()); // �� string�� �ؽ�Ʈ�� ������ ��ġ�� ���

	return 0;
}

void setlaststack(int y) { //���� �ֱ��� ���� x,y ��ǥ�� �������ش�.
	laststack.second = y + padding;
} 
void stackPop() { // stack���� �������� stackpop�� ���ش�. 
	if (st.size() >=1)
		st.pop();
}