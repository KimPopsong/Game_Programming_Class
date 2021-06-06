#pragma once
#include <windows.h>

// 자원 안전 반환 매크로.
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_FREE(p) { if (p) { free(p); (p)=NULL; } }

namespace d2d
{
	// WIN32 관련 함수.
	HWND initWin32(HINSTANCE hInstance, int width, int height);
	bool releaseWin32(HINSTANCE hInstance);
	int doMessageLoop( bool (*renderFunction)(float timeDelta));
}
