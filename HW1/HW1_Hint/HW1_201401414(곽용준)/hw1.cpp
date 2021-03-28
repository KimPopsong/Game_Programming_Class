#include <windows.h>  // �� ��������� Win32 API ���� ������ �Լ� ������� ������.
#include <windowsx.h>
#include <string>
#include <time.h>
#include <vector>
#include <iterator>
#include <random>
using namespace std;

HWND hWndMain = 0;  // ���� ������ �ڵ�.

bool initWin(HINSTANCE instanceHandle, int show);
int runWin();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct student {
	string name;
	int score;
};
//�л� ����ü

string name[10] = { "������", "�켼��", "�豤ǥ", "�̿���" , "����ȣ", "�����", "������", "�����","������" , "��μ�" };

vector<student> stack;
int sp = -1;
vector<student>::iterator it;
//���ð� ����������
bool isFull() {
	return sp == 5;
}
bool isEmpty() {
	return sp == -1;
} 
//�����Լ�

int recx = 550;
int recy = 400; //�׷��� �ڽ��� ��ǥ ����

int xpos = 0;
int ypos = 0; //���콺 ��ǥ�� ����

int makeDelete = 0; //�׸��� �� ���� ��� ����
// ���� ���α׷� ���� �Լ�.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	if (!initWin(hInstance, nCmdShow)) {
		MessageBox(0, "initWin - Failed", "Error", MB_OK);
		return FALSE;
	}

	return runWin();
}


// �ʱ�ȭ�� ���������� ����Ǹ� true�� �ǵ���.
bool initWin(HINSTANCE hInstance, int nCmdShow)
{
	// WNDCLASS ����ü�� ���� ä���� �����ϰ��� �� �������� Ư���� ������.
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "Hello";

	// ������ Ŭ���� description�� �����.
	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass - Failed", 0, 0);
		return false;
	}

	// ��ϵ� ������ Ŭ���� description�� ����Ͽ� �����츦 ������.
	// ������ �������� HWND�� �ǵ���.
	hWndMain = CreateWindow(
		"Hello", "HW1_201401414 ������", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (!hWndMain) {
		MessageBox(0, "CreateWindow - Failed", 0, 0);
		return false;
	}

	// ������ �����츦 show�ϰ� update��.
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);
	return true;
}


// �޽��� ������ �����ϴ� �Լ�.
int runWin()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// �����찡 ������ �� �޽��� ������ �����ϰ� WM_QUIT �޽����� �� ������ �޽��� ������ �ݺ���.

	// GetMessage �Լ��� WM_QUIT �޽����� ���� ���� 0�� �ǵ���. �̶� ������ ������� ��.
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);  //�����찡 ������ ����Ű �ڵ带 ���ڸ޽����� �ؼ���
		DispatchMessage(&msg);  //�޽����� window procedure�� ����
	}
	return (int)msg.wParam;
}


// WIN32 �Լ��� ����� 2���� �׸����� ��.
LRESULT OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);
	RECT rect;
	GetClientRect(hWnd, &rect);
	FillRect(hDC, &rect, ::GetSysColorBrush(COLOR_WINDOW));
	EndPaint(hWnd, &ps);
	return 0;
}

LRESULT drawRectangle(HWND hwnd, HDC hdc)
{

	char charName[10];
	char charScore[10];


	HBRUSH MyBrush, OldBrush;
	COLORREF color;

	
	it = stack.begin(); //iterator �ʱ�ȭ
	int count = 0;
	if (makeDelete == 1) { //�ڽ� �Է¸���϶��� �ڽ��� �׸��� ������
		for (int i = 0; i < sp + 1; i++) {
			string name = it->name;
			int score = it->score;

			string scoreString = to_string((long long int)score);  // int -> string ��ȯ

			sprintf_s(charName, "%s", name.c_str());         //string -> char   ��ȯ name
			sprintf_s(charScore, "%s", scoreString.c_str());  //string -> char   ��ȯ score
				///////////////////////////////////////////////////////////

			switch (count) {
			case 0:
				color = RGB(165, 0, 38);
				break;
			case 1:
				color = RGB(0, 48, 39);
				break;
			case 2:
				color = RGB(244, 109, 0);
				break;
			case 3:
				color = RGB(90, 174, 97);
				break;
			case 4:
				color = RGB(254, 224, 200);
				break;
			case 5:
				color = RGB(50, 0, 0);
				break;
			}
			//count���� �̿��Ͽ� �� �ڽ��� ���� ����

			MyBrush = CreateHatchBrush(HS_HORIZONTAL, color);

			OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);


			Rectangle(hdc, recx, recy, recx + 200, recy + 50);

			TextOut(hdc, recx + 20, recy + 10, charName, strlen(charName));
			TextOut(hdc, recx + 20, recy + 30, charScore, strlen(charScore));

			recy -= 50;
			//������ �ڽ��� y��ǥ �����Ͽ� �׸�
			it++;
			count++;
			//iterator, count�� �����Ͽ� ���� �ڽ� �׸���� �Ѿ
			SelectObject(hdc, OldBrush);
		}
	}
	return 0;
}

// �����찡 �޴� �̺�Ʈ���� ó���ϴ� ������ ���ν���.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		// ���� ���콺 ��ư�� �������� ��ǥ�� ����
		xpos = GET_X_LPARAM(lParam);
		ypos = GET_Y_LPARAM(lParam);

		if (isEmpty()) {
			makeDelete = 1; //�������

			student tempObj;
			srand((unsigned)time(NULL));       
			tempObj.name = name[rand() % 9];
			tempObj.score = rand() % 30 + 20;

			recy = 400;
			stack.push_back(tempObj);
			sp++;  // ������ �����ؼ� ���Ϳ� ����

			InvalidateRect(hWnd, NULL, FALSE); 
			::MessageBox(0, "you made a box", "Good Job", MB_OK); //�׸��� ���� �� �޽��� �ڽ� ���
		}
	
		else {
			if (((recx <= xpos) && (xpos <= recx + 200) && (recy+50 <= ypos) && (ypos <= recy+100)) && (recy <= 600)) { //�ڽ� ���� ����
				makeDelete = 0;

				RECT temp;                    //���� ��ǥ�� �ӽ÷� �־�� �ռ��� �Ű�����

				temp.left = recx;
				temp.top = recy +50;
				temp.right = recx + 200;
				temp.bottom = recy + 100;


				recy += 50;
				//y��ǥ ����
				stack.pop_back();
				sp--;
				//�ڽ� ����� �� ���ÿ��� pop
				InvalidateRect(hWnd, &temp, TRUE);

				::MessageBox(0, "Delete Box", "Delete", MB_OK); //�ڽ� ���Ž� �޽��� �ڽ� ���
			}
			else { //�ڽ� �߰�
				if (isFull()) { //���ð˻� �ǽ�
				::MessageBox(0, "Stack is Full", "Fatal Error", MB_OK);
				::MessageBeep(MB_OK); //�� á�� ��� �������� �Բ� �޽��� �ڽ� ���
				return 0;
				}
				else {
					makeDelete = 1; //�������

					student tempObj;
					srand((unsigned)time(NULL));        // ������ �����ؼ� ���Ϳ� ����
					tempObj.name = name[rand() % 9];
					tempObj.score = rand() % 30 + 20;

					stack.push_back(tempObj);
					sp++;

					recy = 400;
					//y��ǥ �ʱ�ȭ�Ͽ� ó������ �׸� �� �ֵ��� ����
					InvalidateRect(hWnd, NULL, TRUE);

					::MessageBox(0, "you made a box", "Good Job", MB_OK);
				}
			}
		}
		return 0;
	case WM_KEYDOWN:
		// ESC Ű�� ��������, �����츦 destroy��.
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWndMain);
		return 0;
	case WM_DESTROY:
		// destroy �޽����� ������ quit �޽����� ������ �޽��� ������ ������� ��.
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		drawRectangle(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;

	default:
		// �� ���� �޽������� ����Ʈ ������ ���ν����� forward��.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
