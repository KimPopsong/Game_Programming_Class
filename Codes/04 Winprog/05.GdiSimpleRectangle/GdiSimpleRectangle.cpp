// GdiSimpleRectangle.cpp : �ܼ��� ������ ���� ���α׷� �����ӿ�ũ.
//

#include <windows.h>  // �� ��������� Win32 API ���� ������ �Լ� ������� ������.


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
	if (! RegisterClass(&wc))
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


// WIN32 �Լ��� ����� 2���� �׸����� ��.
LRESULT OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	RECT rc; GetClientRect(hWnd, &rc);  // �׸��� ������ ũ�⸦ ����

	FillRect(ps.hdc, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));

	HPEN hMyRedPen = CreatePen(PS_SOLID, 3, RGB(255,0,0)); // ���� ������
	HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyRedPen); // ���� ������

	Rectangle(ps.hdc, rc.left + 100, rc.top + 100, rc.right - 100, rc.bottom - 100); // �簢���� �׸�

	SelectObject(ps.hdc, hOldPen); // ���� ��ü�� ������
	DeleteObject(hMyRedPen);



	// Draw Text Example

	//SetTextAlign(ps.hdc, TA_CENTER); // �ؽ�Ʈ�� ��� ���ķ� �����Ѵ�
	//SetTextColor(ps.hdc, RGB(255, 0, 0)); // �ؽ�Ʈ�� ���� ���������� �����Ѵ�
	//SetBkColor(ps.hdc, RGB(255, 255, 0)); // �ؽ�Ʈ ����� ���� �Ķ������� �����Ѵ�
	//SetBkMode(ps.hdc, TRANSPARENT); //������ ������ ����Ѵ�. �׷��� ���ڸ� ����� �Ŀ��� ����� �������� �ʴ´�.

	const TCHAR str[] = "�ȳ��ϼ���!";
	TextOut(ps.hdc, (rc.right-rc.left)/2-40, (rc.bottom-rc.top)/2-10, str, lstrlen(str));

	//������ �ִ� �ؽ�Ʈ�� ��쿡�� DrawText �Լ��� ����ϸ� ��. ���� ���� ����� �� ����.
	//RECT rt = { rc.left + 100, rc.top + 100, rc.right - 100, rc.bottom - 100 }; //// �ؽ�Ʈ�� ��� �����ϰ� �� ���� ���� ��� �ٹٲ��Ѵ�
	//DrawText(ps.hdc, str, -1, &rt, DT_CENTER | DT_WORDBREAK);



	EndPaint(hWnd, &ps);

	return 0;
}


// �����찡 �޴� �̺�Ʈ���� ó���ϴ� ������ ���ν���.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		// ���� ���콺 ��ư�� ��������, �޽��� �ڽ��� ǥ����.
		MessageBox(0, "Hello, World", "Hello", MB_OK);
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
		OnPaint(hWnd);
		break;

	default:
		// �� ���� �޽������� ����Ʈ ������ ���ν����� forward��.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
