#pragma once
#include <d2d1.h>
#include <dwrite.h>
#include <Windows.h>

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
	int doMessageLoop(bool (*renderFunction)(float timeDelta));

	// d2d 관련 함수
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources(HWND,D2D1_SIZE_U);
	void DiscardDeviceResources();
	void DiscardDeviceIndependentResources();
}

// 디버깅 편의용 WIN32 함수.
#if defined(_DEBUG) && defined(WIN32) && !defined(_AFX) && !defined(_AFXDLL) && !defined(TRACE)
#define TRACE	TRACE_WIN32
void TRACE_WIN32(LPCTSTR lpszFormat, ...);
#define TRACE_WIN32_REQUIRE_IMPL
#endif

//@Author: Jong-Seung Park.

