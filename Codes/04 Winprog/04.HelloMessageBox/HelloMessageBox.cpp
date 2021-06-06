// HelloMessageBox.cpp : ������ ���� ���α׷� �����ӿ�ũ.
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


// �����찡 �޴� �̺�Ʈ���� ó���ϴ� ������ ���ν���.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		// ���� ���콺 ��ư�� ��������, �޽��� �ڽ��� ǥ����.
		MessageBox(0, "Hello, World", "Hello", MB_OK);
	}
	return 0;
	case WM_RBUTTONDOWN:
	{
		// ������ ���콺 ��ư�� ��������, �޽��� �ڽ��� ǥ����.
		int ret = MessageBox(0, "������ �߻��߽��ϴ�. �ٽ� �õ��ұ��?", "����", MB_ICONQUESTION | MB_RETRYCANCEL);

		if (ret == IDRETRY) MessageBox(0, "�ٽ� �õ��� �����߽��ϴ�.", "����", MB_ICONINFORMATION | MB_OK);
		else if (ret == IDCANCEL) MessageBox(0, "��Ҹ� �����߽��ϴ�.", "����", MB_ICONWARNING | MB_OK);
		else MessageBox(0, "�����ΰ� �߸��Ǿ����ϴ�.", "����", MB_ICONERROR | MB_OK);
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

	default:
		// �� ���� �޽������� ����Ʈ ������ ���ν����� forward��.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}