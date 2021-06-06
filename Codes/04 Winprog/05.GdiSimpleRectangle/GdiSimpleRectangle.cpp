// GdiSimpleRectangle.cpp : 단순한 윈도우 응용 프로그램 프레임워크.
//

#include <windows.h>  // 이 헤더파일은 Win32 API 관련 구조와 함수 선언들을 포함함.


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
	if (! RegisterClass(&wc))
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


// WIN32 함수를 사용한 2차원 그리기의 예.
LRESULT OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	RECT rc; GetClientRect(hWnd, &rc);  // 그리기 영역의 크기를 얻음

	FillRect(ps.hdc, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));

	HPEN hMyRedPen = CreatePen(PS_SOLID, 3, RGB(255,0,0)); // 펜을 생성함
	HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyRedPen); // 펜을 선택함

	Rectangle(ps.hdc, rc.left + 100, rc.top + 100, rc.right - 100, rc.bottom - 100); // 사각형을 그림

	SelectObject(ps.hdc, hOldPen); // 원래 객체를 복원함
	DeleteObject(hMyRedPen);



	// Draw Text Example

	//SetTextAlign(ps.hdc, TA_CENTER); // 텍스트를 가운데 정렬로 지정한다
	//SetTextColor(ps.hdc, RGB(255, 0, 0)); // 텍스트의 색을 붉은색으로 지정한다
	//SetBkColor(ps.hdc, RGB(255, 255, 0)); // 텍스트 배경의 색을 파란색으로 지정한다
	//SetBkMode(ps.hdc, TRANSPARENT); //투명한 배경색을 사용한다. 그래서 문자를 출력한 후에도 배경이 지워지지 않는다.

	const TCHAR str[] = "안녕하세요!";
	TextOut(ps.hdc, (rc.right-rc.left)/2-40, (rc.bottom-rc.top)/2-10, str, lstrlen(str));

	//서식이 있는 텍스트의 경우에는 DrawText 함수를 사용하면 됨. 여러 줄을 출력할 수 있음.
	//RECT rt = { rc.left + 100, rc.top + 100, rc.right - 100, rc.bottom - 100 }; //// 텍스트를 가운데 정렬하고 한 줄이 꽉찰 경우 줄바꿈한다
	//DrawText(ps.hdc, str, -1, &rt, DT_CENTER | DT_WORDBREAK);



	EndPaint(hWnd, &ps);

	return 0;
}


// 윈도우가 받는 이벤트들을 처리하는 윈도우 프로시져.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		// 왼쪽 마우스 버튼이 눌려지면, 메시지 박스를 표시함.
		MessageBox(0, "Hello, World", "Hello", MB_OK);
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
		OnPaint(hWnd);
		break;

	default:
		// 그 외의 메시지들은 디폴트 윈도우 프로시져로 forward함.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
