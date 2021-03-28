// GdiSimpleRectangle.cpp : �ܼ��� ������ ���� ���α׷� �����ӿ�ũ.
//
#include <windows.h>  // �� ��������� Win32 API ���� ������ �Լ� ������� ������.
#include "Stack.h"
#include <time.h>
HWND hWndMain = 0;  // ���� ������ �ڵ�.

bool initWin(HINSTANCE instanceHandle, int show);
int runWin();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


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
		"Hello", "Hello", WS_OVERLAPPEDWINDOW,
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

string inAndOut(int x, int y) {
	pair <int, int> tmp = getlaststack(); // ������ stack�� x,y��ǥ�� ������ �´�.
	if (x >= tmp.first && x <= tmp.first + 200 && y >= tmp.second && y <= tmp.second + 50) { // ���� �ֱ� ������ ���ʿ� �����Ŷ��
			return "POP";// POP�� �������ش�.
	}
	return "PUSH"; // �ƴϸ� PUSH�� �������ش�.
}

bool push = false;
bool pop = false;
string result;
// �����찡 �޴� �̺�Ʈ���� ó���ϴ� ������ ���ν���.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	init(hWnd);
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		result = inAndOut(LOWORD(lParam), HIWORD(lParam)); //���콺�� ���� �ֱ� ������ ���ʿ� ���ȴ��� Ȯ���Ѵ�.
		if (result == "PUSH") {
			push = true; 
			InvalidateRect(hWnd, NULL, false); // ȭ���� �ٽ� �׷��ش�.
		}
		else {
			pop = true;
			pair <int, int> tmp = getlaststack();
			RECT rect; rect.left = tmp.first-1; 
			rect.right = tmp.first + 200 + 1; 
			rect.top = tmp.second - 1;
			rect.bottom = tmp.second + 50 + 1;
			stackPop(); // ���ÿ��� ���ش�. 
			InvalidateRect(hWnd, &rect, true); // ���� �ֱ��� ���ø� �����ش�. 
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
		// 2���� �׸����� ��.
		if (push == true) {
			if (drawStack(hWnd) == -1) { // ���� ������ �� �������� 
				::MessageBox(0, "the number of box is 6 , it's limit", "Fatal Error", MB_OK);//�޼����� ����ش�.
				::MessageBeep(MB_OK);
			}
		}
		else if (pop == true) {
			setlaststack(getlaststack().second + 50); // pop�� �������Ƿ� laststack�� �ٽ� �������ش�. 
		}
		push = false; pop = false;
		break;

	default:
		// �� ���� �޽������� ����Ʈ ������ ���ν����� forward��.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
