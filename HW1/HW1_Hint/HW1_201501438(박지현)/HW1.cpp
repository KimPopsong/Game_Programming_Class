// GdiSimpleRectangle.cpp : 단순한 윈도우 응용 프로그램 프레임워크.
//
#include <windows.h>  // 이 헤더파일은 Win32 API 관련 구조와 함수 선언들을 포함함.
#include "Stack.h"
#include <time.h>
HWND hWndMain = 0;  // 메인 윈도우 핸들.

bool initWin(HINSTANCE instanceHandle, int show);
int runWin();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// 응용 프로그램 진입 함수.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	if (!initWin(hInstance, nCmdShow)) {
		MessageBox(0, "initWin - Failed", "Error", MB_OK);
		return FALSE;
	}

	return runWin();
}


// 초기화가 성공적으로 수행되면 true를 되돌림.
bool initWin(HINSTANCE hInstance, int nCmdShow)
{
	// WNDCLASS 구조체에 값을 채워서 생성하고자 할 윈도우의 특성을 지정함.
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

	// 윈도우 클래스 description을 등록함.
	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass - Failed", 0, 0);
		return false;
	}

	// 등록된 윈도우 클래스 description에 기반하여 윈도우를 생성함.
	// 생성된 윈도우의 HWND를 되돌림.
	hWndMain = CreateWindow(
		"Hello", "Hello", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (!hWndMain) {
		MessageBox(0, "CreateWindow - Failed", 0, 0);
		return false;
	}

	// 생성된 윈도우를 show하고 update함.
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

	return true;
}


// 메시지 루프를 포함하는 함수.
int runWin()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 윈도우가 생성된 후 메시지 루프에 진입하고 WM_QUIT 메시지가 올 때까지 메시지 루프를 반복함.

	// GetMessage 함수는 WM_QUIT 메시지를 받을 때만 0을 되돌림. 이때 루프를 벗어나도록 함.
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);  //윈도우가 생성한 가상키 코드를 문자메시지로 해석함
		DispatchMessage(&msg);  //메시지를 window procedure로 보냄
	}
	return (int)msg.wParam;
}

string inAndOut(int x, int y) {
	pair <int, int> tmp = getlaststack(); // 마지막 stack의 x,y좌표를 가지고 온다.
	if (x >= tmp.first && x <= tmp.first + 200 && y >= tmp.second && y <= tmp.second + 50) { // 가장 최근 스택의 안쪽에 눌린거라면
			return "POP";// POP을 리턴해준다.
	}
	return "PUSH"; // 아니면 PUSH를 리턴해준다.
}

bool push = false;
bool pop = false;
string result;
// 윈도우가 받는 이벤트들을 처리하는 윈도우 프로시져.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	init(hWnd);
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		result = inAndOut(LOWORD(lParam), HIWORD(lParam)); //마우스가 가장 최근 스택의 안쪽에 눌렸는지 확인한다.
		if (result == "PUSH") {
			push = true; 
			InvalidateRect(hWnd, NULL, false); // 화면을 다시 그려준다.
		}
		else {
			pop = true;
			pair <int, int> tmp = getlaststack();
			RECT rect; rect.left = tmp.first-1; 
			rect.right = tmp.first + 200 + 1; 
			rect.top = tmp.second - 1;
			rect.bottom = tmp.second + 50 + 1;
			stackPop(); // 스택에서 빼준다. 
			InvalidateRect(hWnd, &rect, true); // 가장 최근의 스택만 지워준다. 
		}
		return 0;
	case WM_KEYDOWN:
		// ESC 키가 눌려지면, 윈도우를 destroy함.
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWndMain);
		return 0;
	case WM_DESTROY:
		// destroy 메시지를 받으면 quit 메시지를 보내서 메시지 루프를 벗어나도록 함.
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		// 2차원 그리기의 예.
		if (push == true) {
			if (drawStack(hWnd) == -1) { // 만약 스택이 꽉 차있으면 
				::MessageBox(0, "the number of box is 6 , it's limit", "Fatal Error", MB_OK);//메세지를 띄워준다.
				::MessageBeep(MB_OK);
			}
		}
		else if (pop == true) {
			setlaststack(getlaststack().second + 50); // pop을 해줬으므로 laststack를 다시 셋팅해준다. 
		}
		push = false; pop = false;
		break;

	default:
		// 그 외의 메시지들은 디폴트 윈도우 프로시져로 forward함.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
