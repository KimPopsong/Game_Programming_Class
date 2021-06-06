// KeyboardSimple - DirectInput으로 키보드의 입력을 제어함.
//

#include "d2dHelper.h"
#include "InputKeyboard.h"
#include <gdiplus.h>

const int SCREEN_WIDTH = 640; //스크린 너비.
const int SCREEN_HEIGHT = 480; //스크린 높이.

InputKeyboard* inputKeyboard = NULL;

HWND hWnd = NULL;
DWORD key = 0;

bool init(HWND hWnd)
{
	inputKeyboard = new InputKeyboard(hWnd);

	return true;
}

void release()
{
	SAFE_DELETE(inputKeyboard);
}

bool render(float timeDelta)
{
	inputKeyboard->update();

	key = 0;
	if (inputKeyboard->keyPressed( DIK_RIGHT ))	key = 1;
	if (inputKeyboard->keyPressed( DIK_LEFT  ))	key = 2;
	if (inputKeyboard->keyPressed( DIK_UP    ))	key = 3;
	if (inputKeyboard->keyPressed( DIK_DOWN  ))	key = 4;
	if (key) {
		::InvalidateRect(hWnd, NULL, FALSE);
	}

	return true;
}

LRESULT OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(hWnd, &ps);
	RECT rect;
	::GetClientRect(hWnd, &rect);
	::FillRect(hDC, &rect, ::GetSysColorBrush(COLOR_WINDOW));

	const char* keyNames[] = { "DIK_RIGHT", "DIK_LEFT", "DIK_UP", "DIK_DOWN" };
	char str[256];
	if (key == 0) {
		wsprintf(str, "화살표 키를 누르세요!\n");
	} else {
		wsprintf(str, "%s가 입력되었습니다.", keyNames[key-1]);
	}
	::DrawText(hDC, str, -1, &rect, DT_LEFT);

	::EndPaint(hWnd, &ps);
  
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_PAINT:
		OnPaint(hWnd);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			::DestroyWindow(hWnd);
		}
		break;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	hWnd = d2d::initWin32(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (hWnd == 0) {
		d2d::releaseWin32(hInstance);
		return 0;
	}

	if (! init(hWnd)) {
		release();
		d2d::releaseWin32(hInstance);
		return 0;
	}

	d2d::doMessageLoop(render);

	release();
	d2d::releaseWin32(hInstance);

	return 0;
}
