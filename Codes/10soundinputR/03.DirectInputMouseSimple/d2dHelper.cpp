#include "d2dHelper.h"

HWND d2d::initWin32(HINSTANCE hInstance, int width, int height)
{
	HWND hWnd; //반환할 윈도우 핸들.
	WNDCLASSEX wc; //윈도우 클래스 객체.
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	// 윈도우 클래스 속성을 초기화함.
	wc.cbSize = sizeof(WNDCLASSEX); //WNDCLASSEX 구조체의 크기.
	wc.style = CS_HREDRAW | CS_VREDRAW; //윈도우 스타일.
	wc.lpfnWndProc = (WNDPROC)WndProc; //윈도우 프로시져 주소.
	wc.cbClsExtra = 0; //클래스 추가 정보.
	wc.cbWndExtra = 0; //윈도우 추가 정보.
	wc.hInstance = hInstance; //응용 인스턴스의 핸들.
	wc.hIcon = ::LoadIcon(NULL, IDI_APPLICATION); //윈도우 아이콘.
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW); //마우스 커서.
	wc.hbrBackground = NULL; //배경 색.
	wc.lpszMenuName = NULL; //메뉴.
	wc.lpszClassName = "WinAppClass"; //윈도우 클래스 이름.
	wc.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION); //최소화된 윈도우 아이콘.

	// 윈도우 클래스를 등록함.
	if (! ::RegisterClassEx(&wc)) {
		::MessageBox(NULL, "RegisterClassEx() - Failed", "ERROR", NULL);
		return false;
	}

	// 창의 크기를 조절함.
	RECT screenrect;
	DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	::SetRect(&screenrect, 0, 0, width, height);
	::AdjustWindowRect(&screenrect, dwWindowStyle, false);
	
	// 새 창을 생성함.
	hWnd = ::CreateWindowEx(NULL, //확장된 스타일.
		"WinAppClass", //윈도우 클래스.
		"My Win Application", //윈도우 이름.
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, //윈도우 스타일.
		CW_USEDEFAULT, CW_USEDEFAULT, //X,Y좌표.
		screenrect.right - screenrect.left, screenrect.bottom - screenrect.top, //윈도우 크기.
		NULL, //부모 윈도우의 핸들.
		NULL, //메뉴.
		hInstance, //응용 인스턴스의 핸들.
		NULL); //윈도우의 포인터.
	if (! hWnd) {
		::MessageBox(NULL, "CreateWindowEx() - Failed", "ERROR", NULL);
		return 0;
	}

	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);

	return hWnd;
}

bool d2d::releaseWin32(HINSTANCE hInstance)
{
	// 윈도우 클래스를 unregister한다.
	::UnregisterClass("WinAppClass", hInstance);
	return true;
}

int d2d::doMessageLoop(bool (*renderFunction)(float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)::timeGetTime(); 

	while (msg.message != WM_QUIT) {
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			if (! renderFunction(timeDelta)) {
				return 0;
			}

			lastTime = currTime;
		}
	}
	return (int)(msg.wParam);
}
