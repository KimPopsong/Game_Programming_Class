// MouseSimple - DirectInput으로 마우스의 입력을 제어함.
//

#include "d2dHelper.h"
#include "InputMouse.h"
#include <string.h> //strcpy
#include <gdiplus.h>

const int SCREEN_WIDTH = 640; //스크린 너비.
const int SCREEN_HEIGHT = 480; //스크린 높이.

InputMouse* inputMouse = NULL;

HWND hWnd = NULL;
char strbuf[800]; //출력할 문자열이 충분히 들어갈 수 있도록 넉넉하게 둘 것 (한글도 고려).
POINT pos;

bool init(HWND hWnd)
{
	inputMouse = new InputMouse(hWnd);
	inputMouse->setScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	inputMouse->getCursorPos(&pos);

	return true;
}

void release()
{
	SAFE_DELETE(inputMouse);
}

bool render(float timeDelta)
{
	inputMouse->update();  

	POINT pos_new;
	inputMouse->getCursorPos(&pos_new);
	if (pos_new.x != pos.x || pos_new.y != pos.y) {
		pos = pos_new;

		// 마우스 위치 정보를 표시한다.
		inputMouse->getString(strbuf);

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

	char str[256];
	wsprintf(str, "현재의 마우스 위치: %s", strbuf);
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
	case WM_SIZE:
		if (inputMouse) {
			RECT rect;
			::GetClientRect(hWnd, &rect);
			int screenWidth = rect.right - rect.left;
			int screenHeight = rect.bottom - rect.top;
			inputMouse->setScreenSize(screenWidth, screenHeight);
		}
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
