// DirectSoundPlayWav - DirectSound로 소리를 재생함.
//

#include "d2dHelper.h"
#include "SoundManager.h"
#include <gdiplus.h>

const int SCREEN_WIDTH = 640; //스크린 너비.
const int SCREEN_HEIGHT = 480; //스크린 높이.

CSoundManager* soundManager = NULL;

bool init(HWND hWnd)
{
	soundManager = new CSoundManager;
	if (! soundManager->init(hWnd))
		return FALSE;

	// 사운드 파일을 추가함.
	int id;
	if (! soundManager->add("drum1.wav", &id)) //id=0부터 시작함.
		return FALSE;
	if (! soundManager->add("drum2.wav", &id))
		return FALSE;
	if (! soundManager->add("drum3.wav", &id))
		return FALSE;
	if (! soundManager->add("drum4.wav", &id))
		return FALSE;

	soundManager->play(3, FALSE);
	//soundManager->play(1,TRUE);

	return true;
}

void release()
{
	SAFE_DELETE(soundManager);
}

bool render(float timeDelta)
{
	if ( ::GetAsyncKeyState('1') & 0x8000f )	soundManager->play(0, FALSE);
	if ( ::GetAsyncKeyState('2') & 0x8000f )	soundManager->play(1, FALSE);
	if ( ::GetAsyncKeyState('3') & 0x8000f )	soundManager->play(2, FALSE);
	if ( ::GetAsyncKeyState('4') & 0x8000f )	soundManager->play(3, FALSE);

	if ( ::GetAsyncKeyState('Q') & 0x8000f )	soundManager->play(0, TRUE);
	if ( ::GetAsyncKeyState('W') & 0x8000f )	soundManager->play(1, TRUE);
	if ( ::GetAsyncKeyState('E') & 0x8000f )	soundManager->play(2, TRUE);
	if ( ::GetAsyncKeyState('R') & 0x8000f )	soundManager->play(3, TRUE);

	if ( ::GetAsyncKeyState('A') & 0x8000f )	soundManager->stop(0);
	if ( ::GetAsyncKeyState('S') & 0x8000f )	soundManager->stop(1);
	if ( ::GetAsyncKeyState('D') & 0x8000f )	soundManager->stop(2);
	if ( ::GetAsyncKeyState('F') & 0x8000f )	soundManager->stop(3);

	return true;
}

LRESULT OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(hWnd, &ps);
	RECT rect;
	::GetClientRect(hWnd, &rect);
	::FillRect(hDC, &rect, ::GetSysColorBrush(COLOR_WINDOW));

	::DrawText(hDC, "1-4: play once clip #1-#4\n"
				"q,w,e,r: play repeatedly clip #1-#4\n"
				"a,s,d,f: stop playing clip #1-#4",
		-1, &rect, DT_LEFT);

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
	HWND hWnd = d2d::initWin32(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT);
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
